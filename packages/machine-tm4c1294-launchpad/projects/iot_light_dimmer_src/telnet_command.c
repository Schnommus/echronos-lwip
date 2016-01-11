#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"

#include "utils/uartstdio.h"
#include "utils/ustdlib.h"

#include <string.h>
#include "telnet_command.h"

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


char tstr[2000];
char *prompt = "\r\neChronos Launchpad >> ";

extern void apply_brightness();

// telnet command handler
void cmd_parser(struct telnet_state *hs)
{
    char *p = hs->cmd_buffer;

    if (strstr(p, "quit"))
    {
        hs->left = 0;
        hs->flags |= TELNET_FLAG_CLOSE_CONNECTION;
    }
    else if( strstr(p, "help") )
    {
        usprintf(tstr,"\r\n"
            "== COMMAND LIST ==\r\n"
            " 'quit' - Exit the telnet session\r\n"
            " 'help' - Show this message\r\n"
            " 'status' - Show the current state of this device\r\n"
            " 'red' - Switch to red light\r\n"
            " 'green' - Switch to green light\r\n"
            " 'set xxx' - Set LED brightness to xxx percent (integer 0-100)\r\n"
            "%s",prompt);

        hs->data_out = tstr;
        hs->left = strlen(hs->data_out);
    }
    else if( strstr(p, "status") )
    {
        usprintf(tstr,"\r\n"
            "== STATUS ==\r\n"
            " Light on - %s\r\n"
            " Current brightness - %i\r\n"
            "%s",
            system_status.power_on ? "green" : "red",
            system_status.brightness,
            prompt );

        hs->data_out = tstr;
        hs->left = strlen(hs->data_out);
    }
    else if( strstr(p, "green") )
    {
        relays_on();
        usprintf(tstr,"\r\n"
            "Switched to green light\r\n"
            "%s",prompt);

        hs->data_out = tstr;
        hs->left = strlen(hs->data_out);
    }
    else if( strstr(p, "red") )
    {
        relays_off();
        usprintf(tstr,"\r\n"
            "Switched to red light\r\n"
            "%s",prompt);

        hs->data_out = tstr;
        hs->left = strlen(hs->data_out);
    }
 
    else if( strstr(p, "set") )
    {
        // Don't have sscanf
        char *set_string_start = p + strlen("set") + 1;
        const char *next;
        int new_set = ustrtoul( set_string_start, &next, 0 );

        if( new_set < 0 || new_set > 100 ) {
            usprintf(tstr,"\r\n"
                "%i is an invalid brightness. Valid brightnesses are [0-100]:\r\n"
                "%s", new_set, prompt);
        } else {
            system_status.brightness = new_set;
            apply_brightness();
            usprintf(tstr,"\r\n"
                "New brightness set: %i\r\n"
                "%s", system_status.brightness, prompt);
        }

        hs->data_out = tstr;
        hs->left = strlen(hs->data_out);
    }
    else
    {
        usprintf(tstr,"\r\nThis command isn't implemented. Echoing: \"%s\"\r\n%s",p,prompt);
        hs->data_out = tstr;
        hs->left = strlen(hs->data_out);
    }

    hs->cmd_buffer[0] = 0;
    // copy any remaining part of command line to position 0
    //strcpy(hs->cmd_buffer,q+1);
}

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
}

static err_t telnet_poll(void *arg, struct tcp_pcb *pcb)
{
    struct telnet_state *hs;

    hs = arg;

    UARTprintf("Poll\n");
    if (hs == NULL) {
        tcp_abort(pcb);
        return ERR_ABRT;
    } else {


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

void sendopt(struct telnet_state *hs, u8_t option, u8_t value)
{
    hs->data_out[hs->left++] = TELNET_IAC;
    hs->data_out[hs->left++] = option;
    hs->data_out[hs->left++] = value;
    hs->data_out[hs->left] = 0;
}

void get_char(struct tcp_pcb *pcb, struct telnet_state *hs, char c)
{
    int l;

    if (c == '\r') return;
    if (c != '\n')
    {
        l = strlen(hs->cmd_buffer);
        hs->cmd_buffer[l++] = c;
        hs->cmd_buffer[l] = 0;
    }
    else
    {
        cmd_parser(hs);     // handle command
        if (hs->left > 0)
        {
            send_data(pcb, hs);
            tcp_sent(pcb, telnet_sent);
        }
    }
}

static err_t telnet_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    struct telnet_state *hs;

    // TODO: Changed to unsigned - make sure this is correct
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
                        get_char(pcb,hs,c);
                        hs->state = STATE_NORMAL;
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

        //! \todo should check max length here
//      strcat(hs->cmd_buffer,p->payload);

        pbuf_free(p);
/*
        if (strchr(hs->cmd_buffer,'\r'))
        {
            cmd_parser(hs);     // handle command

            send_data(pcb, hs);

            // Tell TCP that we wish be to informed of data that has been
            // successfully sent by a call to the http_sent() function.
            tcp_sent(pcb, telnet_sent);
        }
*/

    }

    if (err == ERR_OK && p == NULL) {
        UARTprintf("remote host closed connection\n");
        close_conn(pcb, hs);
    }

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
    to the http_recv() function. */
    tcp_recv(pcb, telnet_recv);

    tcp_err(pcb, conn_err);
    tcp_poll(pcb, telnet_poll, 4);

    usprintf(tstr,"\r\n        == eChronos Light Dimmer TELNET ==\r\n"
                    "This is a fantastically bright light dimmer, running on lwIP v1.4.1\r\n"
                    "Type 'help' to get a command list.\r\n\r\n%s",prompt);

    hs->data_out = tstr;
    hs->left = strlen(tstr);
    send_data(pcb, hs);

    return ERR_OK;
}

void telnet_command_init(int port)
{
    UARTprintf("Initializing telnet command server on port %i...\n", port);

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

