/*
 * eChronos Real-Time Operating System
 * Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, version 3, provided that these additional
 * terms apply under section 7:
 *
 *   No right, title or interest in or to any trade mark, service mark, logo
 *   or trade name of of National ICT Australia Limited, ABN 62 102 206 173
 *   ("NICTA") or its licensors is granted. Modified versions of the Program
 *   must be plainly marked as such, and must not be distributed using
 *   "eChronos" as a trade mark or product name, or misrepresented as being
 *   the original Program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @TAG(NICTA_AGPL)
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "drivers/pinout.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#include "rtos-kochab.h"

#define debug_println(text) UARTprintf(text);UARTprintf("\n")


#define SYST_CSR_REG 0xE000E010
#define SYST_RVR_REG 0xE000E014
#define SYST_CVR_REG 0xE000E018

#define SYST_CSR_READ() (*((volatile uint32_t*)SYST_CSR_REG))
#define SYST_CSR_WRITE(x) (*((volatile uint32_t*)SYST_CSR_REG) = x)

#define SYST_RVR_READ() (*((volatile uint32_t*)SYST_RVR_REG))
#define SYST_RVR_WRITE(x) (*((volatile uint32_t*)SYST_RVR_REG) = x)

#define SYST_CVR_READ() (*((volatile uint32_t*)SYST_CVR_REG))
#define SYST_CVR_WRITE(x) (*((volatile uint32_t*)SYST_CVR_REG) = x)

#ifdef DEBUG
void __error__( char *pcFilename, uint32_t ui32Line ) {
    for(;;);
}
#endif

void nmi() { for(;;); }
void hardfault() { for(;;); }
void memmanage() { for(;;); }
void busfault() { for(;;); }
void usagefault() { for(;;); }

uint32_t g_ui32SysClock;

void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, g_ui32SysClock);
}

bool tick_irq(void) {
    rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_TICK);
    return true;
}

void
fatal(const RtosErrorId error_id)
{
    UART_printf("FATAL ERROR\n");
    for (;;)
    {
    }
}

void
fn_a(void)
{
    uint8_t count;

    UART_printf("task a: taking lock\n");
    rtos_mutex_lock(RTOS_MUTEX_ID_M0);
    if (rtos_mutex_try_lock(RTOS_MUTEX_ID_M0))
    {
        UART_printf("unexpected mutex not locked.\n");
    }
    UART_printf("task a: releasing lock\n");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M0);

    for (count = 0; count < 10; count++)
    {
        UART_printf("task a\n");
        if (count % 5 == 0)
        {
            UART_printf("unblocking b\n");
            rtos_signal_send_set(RTOS_TASK_ID_B, RTOS_SIGNAL_SET_TEST);
        }
    }

    UART_printf("A now waiting for ticks\n");
    for (;;)
    {

        (void) rtos_signal_wait_set(RTOS_SIGNAL_SET_TIMER);
        UART_printf("tick\n");
        rtos_signal_send_set(RTOS_TASK_ID_B, RTOS_SIGNAL_SET_TEST);
    }
}

void
fn_b(void)
{
    UART_printf("task b: attempting lock\n");
    rtos_mutex_lock(RTOS_MUTEX_ID_M0);
    UART_printf("task b: got lock\n");

    while (1) {
        if (rtos_signal_poll_set(RTOS_SIGNAL_SET_TEST)) {
            UART_printf("task b blocking\n");
            (void) rtos_signal_wait_set(RTOS_SIGNAL_SET_TEST);
            UART_printf("task b unblocked\n");
        }
    }
}

int
main(void)
{
    SYST_RVR_WRITE(0x0001ffff);
    SYST_CVR_WRITE(0);
    SYST_CSR_WRITE((1 << 1) | 1);

    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                SYSCTL_CFG_VCO_480), 120000000);

    ConfigureUART();

    UART_printf("Starting eChronos core (Kochab)...\n");

    rtos_start();

    /* Should never reach here, but if we do, an infinite loop is
       easier to debug than returning somewhere random. */
    for (;;) ;
}
