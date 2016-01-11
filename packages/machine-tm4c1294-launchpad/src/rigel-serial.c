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

#include "rtos-rigel.h"

#define SYST_CSR_REG 0xE000E010
#define SYST_RVR_REG 0xE000E014
#define SYST_CVR_REG 0xE000E018

#define SYST_CSR_READ() (*((volatile uint32_t*)SYST_CSR_REG))
#define SYST_CSR_WRITE(x) (*((volatile uint32_t*)SYST_CSR_REG) = x)

#define SYST_RVR_READ() (*((volatile uint32_t*)SYST_RVR_REG))
#define SYST_RVR_WRITE(x) (*((volatile uint32_t*)SYST_RVR_REG) = x)

#define SYST_CVR_READ() (*((volatile uint32_t*)SYST_CVR_REG))
#define SYST_CVR_WRITE(x) (*((volatile uint32_t*)SYST_CVR_REG) = x)

uint32_t g_ui32SysClock;

void
ConfigureUART(void) {
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

void
tick_irq(void) {
    //Don't use UART inside an interrupt! Results in fatal.
    //UARTprintf("irq tick\n");
    rtos_timer_tick();
}

void
fatal(RtosErrorId error_id) {
    UARTprintf("FATAL ERROR: %d\n", error_id);
    UARTprintf("\n");
    for (;;) { }
}

void
fn_a(void) {
    volatile int i;
    uint8_t count;

    rtos_task_start(RTOS_TASK_ID_B);

    if (rtos_task_current() != RTOS_TASK_ID_A) {
        UARTprintf("task a: wrong task??\n");
        for (;;) { }
    }


    UARTprintf("task a: taking lock\n");
    rtos_mutex_lock(RTOS_MUTEX_ID_TEST);
    rtos_yield();
    if (rtos_mutex_try_lock(RTOS_MUTEX_ID_TEST)) {
        UARTprintf("task a: ERROR: unexpected mutex not locked.\n");
    }
    UARTprintf("task a: releasing lock\n");
    rtos_mutex_unlock(0);
    rtos_yield();

    for (count = 0; count < 10; count++) {
        UARTprintf("task a\n");
        if (count % 5 == 0) {
            UARTprintf("task a: unblocking b\n");
            rtos_signal_send(RTOS_TASK_ID_B, RTOS_SIGNAL_ID_TEST);
        }
        UARTprintf("task a: yield\n");
        rtos_yield();
    }

    /* Do some sleeps */
    UARTprintf("task a: sleep 10\n");
    rtos_sleep(10);
    UARTprintf("task a: sleep done - sleep 5\n");
    rtos_sleep(5);
    UARTprintf("task a: sleep done\n");

    do {
        UARTprintf("task a: remaining test - %d\n", rtos_timer_current_ticks);
        UARTprintf("\n");
        rtos_yield();
    } while (!rtos_timer_check_overflow(RTOS_TIMER_ID_TEST));



    if (!rtos_signal_poll(RTOS_SIGNAL_ID_TIMER)) {
        UARTprintf("ERROR: couldn't poll expected timer.\n");
    }

    UARTprintf("task a: sleep for 100\n");
    rtos_sleep(100);

    /* Spin for a bit - force a missed ticked */
    UARTprintf("task a: start delay\n");
    for (i = 0 ; i < 50000000; i++) { }
    UARTprintf("task a: complete delay\n");
    rtos_yield();

    UARTprintf("task a: now waiting for ticks\n");
    for (;;) {
        rtos_signal_wait(RTOS_SIGNAL_ID_TIMER);
        UARTprintf("task a: timer tick\n");
    }
}

void
fn_b(void) {
    for (;;) {
        UARTprintf("task b: sleeping for 7\n");
        rtos_sleep(7);
    }
}

int
main(void) {
    /* Set the systick reload value */
    SYST_RVR_WRITE(0x0001ffff);
    SYST_CVR_WRITE(0);
    SYST_CSR_WRITE((1 << 1) | 1);

    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                SYSCTL_CFG_VCO_480), 120000000);

    // Configure the device pins.
    PinoutSet(false, false);

    // Enable the GPIO pins for the LED D1 (PN1).
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

    ConfigureUART();


    UARTprintf("Starting eChronos RTOS core (RIGEL):\n");

    rtos_start();
    /* Should never reach here, but if we do, an infinite loop is
       easier to debug than returning somewhere random. */
    for (;;) ;
}
