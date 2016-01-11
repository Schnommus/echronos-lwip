#include <stdint.h>
#include <stdbool.h>

#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/tcpip.h"

#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include <string.h>
#include "telnet_uart_echo.h"

#include "system_status.h"


#define TELNET_IAC   255
#define TELNET_WILL  251
#define TELNET_WONT  252
#define TELNET_DO    253
#define TELNET_DONT  254

//#define TIMEOUT_DISCONNECT
#define IDLE_TIMEOUT    30      // 30 seconds

struct telnet_state {
    char        *data_out;
    char        cmd_buffer[80];
    u16_t       left;
    u16_t       timeout;
    u16_t       flags;
#define TELNET_FLAG_CLOSE_CONNECTION    (1<<0)
    u16_t       state;
#define STATE_NORMAL 0
#define STATE_IAC    1
#define STATE_WILL   2
#define STATE_WONT   3
#define STATE_DO     4
#define STATE_DONT   5
#define STATE_CLOSE  6
};

extern uint32_t g_ui32SysClock;

static err_t telnet_poll(void *arg, struct tcp_pcb *pcb);

char tstr[2000];

static void conn_err(void *arg, err_t err)
{
    struct http_state *hs;
    LWIP_UNUSED_ARG(err);
    hs = arg;
    mem_free(hs);
}

static void close_conn(struct tcp_pcb *pcb, struct telnet_state *hs)
{
    tcp_arg(pcb, NULL);
    tcp_sent(pcb, NULL);
    tcp_recv(pcb, NULL);
    mem_free(hs);
    tcp_close(pcb);
}

static void send_data(struct tcp_pcb *pcb, struct telnet_state *hs)
{
    err_t err;
    u16_t len;

    /* We cannot send more data than space available in the send
    buffer. */
    if (tcp_sndbuf(pcb) < hs->left) {
        len = tcp_sndbuf(pcb);
    } else {
        len = hs->left;
    }

    do {
        err = tcp_write(pcb, hs->data_out, len, 0);
        if (err == ERR_MEM) {
            len /= 2;
        }
    } while (err == ERR_MEM && len > 1);

    if (err == ERR_OK) {
        hs->data_out += len;
        hs->left -= len;
    }

    // Make sure there's no data left
    telnet_poll( hs, pcb ); 
}

#define UART_BUFFER_SIZE 4096

char uart_buffer[UART_BUFFER_SIZE];
char uart_buffer_to_send[UART_BUFFER_SIZE];
int uart_buffer_index = 0;

void buffer_uart_interrupt() {

    uint32_t ui32Status;

    ui32Status = ROM_UARTIntStatus(UART6_BASE, true);

    // Clear the asserted interrupts.
    ROM_UARTIntClear(UART6_BASE, ui32Status);


    while( UARTCharsAvail( UART6_BASE ) ) {

        // Silently fail a buffer overflow case (only transmit what was possible)
        if( uart_buffer_index == UART_BUFFER_SIZE ) {
            UARTprintf( "External UART - buffer exceeded %i bytes before TCP poll! Truncated.", UART_BUFFER_SIZE );
            break;
        }

        char c = UARTCharGetNonBlocking( UART6_BASE );
        uart_buffer[uart_buffer_index++] = c;
    }
}


static err_t telnet_poll(void *arg, struct tcp_pcb *pcb)
{
    struct telnet_state *hs;

    hs = arg;

    if (hs == NULL) {
        tcp_abort(pcb);
        return ERR_ABRT;
    } else {
        
        // If we have any data; send it!
        if( uart_buffer_index != 0 ) {
            ROM_IntDisable(INT_UART6);

            memcpy( uart_buffer_to_send, uart_buffer, UART_BUFFER_SIZE );

            hs->left = uart_buffer_index;
            hs->data_out = uart_buffer_to_send;

            uart_buffer_index = 0;

            ROM_IntEnable(INT_UART6);
            ROM_UARTIntEnable(UART6_BASE, UART_INT_RX | UART_INT_RT);

            send_data(pcb, hs);
        }

        if (hs->flags & TELNET_FLAG_CLOSE_CONNECTION) {
            UARTprintf("closing connection in poll\n");
            close_conn(pcb, hs);
            hs->flags &= ~TELNET_FLAG_CLOSE_CONNECTION;
        }

#ifdef TIMEOUT_DISCONNECT
        // timeout and close connection if nothing has been received for N seconds
        hs->timeout++;
        if (hs->timeout > IDLE_TIMEOUT) {
            UARTprintf("client timed out - disconnecting");
            close_conn(pcb,hs);
        }
#endif
    }

    return ERR_OK;
}

static err_t telnet_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
    struct telnet_state *hs;

    LWIP_UNUSED_ARG(len);

    hs = arg;

    if (hs->left > 0) {
        send_data(pcb, hs);
    } else {
        if (hs->flags & TELNET_FLAG_CLOSE_CONNECTION) {
            UARTprintf("closing connection\n");
            close_conn(pcb, hs);
            hs->flags &= ~TELNET_FLAG_CLOSE_CONNECTION;
        }
    }

    return ERR_OK;
}

static void sendopt(struct telnet_state *hs, u8_t option, u8_t value)
{
    hs->data_out[hs->left++] = TELNET_IAC;
    hs->data_out[hs->left++] = option;
    hs->data_out[hs->left++] = value;
    hs->data_out[hs->left] = 0;
}

static void get_char(struct tcp_pcb *pcb, struct telnet_state *hs, char c)
{
    UARTCharPut( UART6_BASE, c );
}

static err_t telnet_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    struct telnet_state *hs;

    unsigned char *q,c;
    int len;

    hs = arg;

    if (err == ERR_OK && p != NULL) {

        /* Inform TCP that we have taken the data. */
        tcp_recved(pcb, p->tot_len);
        hs->timeout = 0;    // reset idletime ount

        q = p->payload;
        len = p->tot_len;

        while(len > 0)
        {
            c = *q++;
            --len;

            switch (hs->state) {
                case STATE_IAC:
                    if(c == TELNET_IAC) {
                        hs->state = STATE_NORMAL;
                        get_char(pcb,hs,c);
                    } else {
                        switch(c) {
                            case TELNET_WILL:
                                hs->state = STATE_WILL;
                                break;
                            case TELNET_WONT:
                                hs->state = STATE_WONT;
                                break;
                            case TELNET_DO:
                                hs->state = STATE_DO;
                                break;
                            case TELNET_DONT:
                                hs->state = STATE_DONT;
                                break;
                            default:
                                hs->state = STATE_NORMAL;
                                break;
                        }
                    }
                    break;

                case STATE_WILL:
                    /* Reply with a DONT */
                    sendopt(hs,TELNET_DONT, c);
                    hs->state = STATE_NORMAL;
                    break;
                case STATE_WONT:
                    /* Reply with a DONT */
                    sendopt(hs,TELNET_DONT, c);
                    hs->state = STATE_NORMAL;
                    break;
                case STATE_DO:
                    /* Reply with a WONT */
                    sendopt(hs,TELNET_WONT, c);
                    hs->state = STATE_NORMAL;
                    break;
                case STATE_DONT:
                    /* Reply with a WONT */
                    sendopt(hs,TELNET_WONT, c);
                    hs->state = STATE_NORMAL;
                    break;
                case STATE_NORMAL:
                    if(c == TELNET_IAC) {
                        hs->state = STATE_IAC;
                    } else {
                        get_char(pcb,hs,c);
                    }
                    break;
            }
        }

        pbuf_free(p);

    }

    if (err == ERR_OK && p == NULL) {
        UARTprintf("remote host closed connection\n");
        close_conn(pcb, hs);
    }

    // If there is an instantaneous response, send it straight away.
    telnet_poll( arg, pcb ); 

    return ERR_OK;
}

/*-----------------------------------------------------------------------------------*/
static err_t telnet_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
    struct telnet_state *hs;

    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);

    tcp_setprio(pcb, TCP_PRIO_MIN);

    /* Allocate memory for the structure that holds the state of the
    connection. */
    hs = (struct telnet_state *)mem_malloc(sizeof(struct telnet_state));

    if (hs == NULL) {
        UARTprintf("telnet_accept: Out of memory\n");
        return ERR_MEM;
    }

    UARTprintf("new connection from %d.%d.%d.%d\n", (pcb->remote_ip.addr >> 0) & 0xff, (pcb->remote_ip.addr >> 8) & 0xff, (pcb->remote_ip.addr >> 16) & 0xff, (pcb->remote_ip.addr >> 24) & 0xff);

    /* Initialize the structure. */
    hs->data_out = NULL;
    hs->cmd_buffer[0] = 0;
    hs->left = 0;
    hs->timeout = 0;
    hs->flags = 0;
    hs->state = STATE_NORMAL;

    /* Tell TCP that this is the structure we wish to be passed for our
    callbacks. */
    tcp_arg(pcb, hs);

    /* Tell TCP that we wish to be informed of incoming data by a call
    to the telnet_recv() function. */
    tcp_recv(pcb, telnet_recv);

    tcp_err(pcb, conn_err);
    tcp_poll(pcb, telnet_poll, 1);

    usprintf(tstr,"\r\n        == eChronos launchpad TELNET ==\r\n"
                    "This is eChronos running lwIP v1.4.1 on a TI TM4C129\r\n"
                    "UART1 Echo server, running at %i baud.\r\n\r\n", system_status.current_baud_rate);

    hs->data_out = tstr;
    hs->left = strlen(tstr);
    send_data(pcb, hs);

    return ERR_OK;
}

void external_uart_init() {
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART6);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);

    // Set GPIO P0 and P1 as UART pins.
    GPIOPinConfigure(GPIO_PP0_U6RX);
    GPIOPinConfigure(GPIO_PP1_U6TX);
    ROM_GPIOPinTypeUART(GPIO_PORTP_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Configure the UART.
    ROM_UARTConfigSetExpClk(UART6_BASE, g_ui32SysClock, system_status.current_baud_rate,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

    // Enable the UART interrupt
    ROM_IntEnable(INT_UART6);
    ROM_UARTIntEnable(UART6_BASE, UART_INT_RX | UART_INT_RT);
}

void telnet_uart_echo_init(int port)
{
    external_uart_init();

    UARTprintf("Initializing telnet UART echo server on port %i...\n", port);

    struct tcp_pcb *pcb;

    pcb = tcp_new();

    UARTprintf("New tcp PCB: %x\n", pcb);

    char bind_error = tcp_bind(pcb, IP_ADDR_ANY, port);

    UARTprintf("Binding PCB to port 23, returned %i\n", (int)bind_error);

    pcb = tcp_listen(pcb);

    tcp_accept(pcb, telnet_accept);

    UARTprintf("Final PCB has address: %x\n", pcb);

    UARTprintf(" -> Telnet server initialized.\n");
}

