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
#include "inc/hw_ints.h"
#include "inc/hw_pwm.h"
#include "driverlib/gpio.h"
#include "drivers/pinout.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/emac.h"
#include "driverlib/pwm.h"
#include "utils/locator.h"
#include "utils/lwiplib.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "drivers/buttons.h"
#include "httpd.h"

#include "rtos-kochab.h"

// Interrupt priorities
#define SYSTICK_INT_PRIORITY    0x80
#define ETHERNET_INT_PRIORITY   0xC0

// Bunch of definitions to facilitate SSI

#define SSI_INDEX_CPM  0

static const char *g_pcConfigSSITags[] =
{
    "cpm"        // SSI_INDEX_CPM
};

#define NUM_CONFIG_SSI_TAGS     (sizeof(g_pcConfigSSITags) / sizeof (char *))

static int32_t SSIHandler(int32_t iIndex, char *pcInsert, int32_t iInsertLen);

// Debug functions

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
uint32_t g_ui32IPAddress;

uint32_t g_cpmTotal;
uint32_t g_cpmGood;

void
ConfigureUART(void)
{
    // Enable the GPIO Peripheral used by the UART.
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Enable UART0
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Configure GPIO Pins for UART mode.
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 115200, g_ui32SysClock);
}

bool tick_irq(void) {
    rtos_timer_tick();
    return true;
}


void
fatal(const RtosErrorId error_id) {
    UARTprintf("eChronos: Internal OS error: %i\n", error_id);
    for (;;) {}
}


static int32_t
SSIHandler(int32_t iIndex, char *pcInsert, int32_t iInsertLen) {
    // Which SSI tag have we been passed?
    switch(iIndex)
    {
        case SSI_INDEX_CPM:
            usnprintf(pcInsert, iInsertLen, "%d", g_cpmGood);
            break;

        default:
            usnprintf(pcInsert, iInsertLen, "??");
            break;
    }

    // Tell the server how many characters our insert string contains.
    return ustrlen(pcInsert);
}

void
DisplayIPAddress(uint32_t ui32Addr)
{
    char pcBuf[16];

    // Convert the IP Address into a string.
    usprintf(pcBuf, "%d.%d.%d.%d", ui32Addr & 0xff, (ui32Addr >> 8) & 0xff,
            (ui32Addr >> 16) & 0xff, (ui32Addr >> 24) & 0xff);

    // Display the string.
    UARTprintf(pcBuf);
}

void
lwIPHostTimerHandler(void)
{
    uint32_t ui32NewIPAddress;

    // Get the current IP address.
    ui32NewIPAddress = lwIPLocalIPAddrGet();


    // See if the IP address has changed.
    if(ui32NewIPAddress != g_ui32IPAddress)
    {
        // See if there is an IP address assigned.
        if(ui32NewIPAddress == 0xffffffff)
        {
            // Indicate that there is no link.
            UARTprintf("Waiting for link.\n");
        }
        else if(ui32NewIPAddress == 0)
        {
            // There is no IP address, so indicate that the DHCP process is
            // running.
            UARTprintf("Waiting for IP address.\n");
        }
        else
        {
            // Display the new IP address.
            UARTprintf("IP Address: ");
            DisplayIPAddress(ui32NewIPAddress);
            UARTprintf("\n");
            UARTprintf("Open a browser and enter the IP address.\n");
        }

        // Save the new IP address.
        g_ui32IPAddress = ui32NewIPAddress;
    }

    // If there is not an IP address.
    if((ui32NewIPAddress == 0) || (ui32NewIPAddress == 0xffffffff))
    {
       // Do nothing and keep waiting.
    }
}

void radiation_event_irq(void) {
    UARTprintf("*");
    ++g_cpmTotal;
    GPIOIntClear(GPIO_PORTK_BASE, GPIO_PIN_7);
}

void update_cpm_task() {
    while(1) {
        UARTprintf("Waiting for timer...\n");
        rtos_signal_wait( RTOS_SIGNAL_ID_UPDATE_CPM );
        g_cpmGood = g_cpmTotal * 6;
        g_cpmTotal = 0;
        UARTprintf("Updated CPM to %d...\n", g_cpmGood);
    }
}

int
main(void)
{
    uint32_t ui32User0, ui32User1;
    uint8_t pui8MACArray[8];

    //Needed by the PHY
    SysCtlMOSCConfigSet(SYSCTL_MOSC_HIGHFREQ);

    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                SYSCTL_CFG_VCO_480), 120000000);

    PinoutSet(true, false);

    MAP_SysTickPeriodSet(g_ui32SysClock / (1000/SYS_TICK_INTERVAL) );
    MAP_SysTickEnable();
    MAP_SysTickIntEnable();

    ConfigureUART();

    // Configure interrupt source on PK7
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);

    GPIOPinTypeGPIOInput(GPIO_PORTK_BASE, GPIO_PIN_7);
    GPIOPadConfigSet(GPIO_PORTK_BASE,GPIO_PIN_7,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);

    // Interrupt setup
    GPIOIntTypeSet(GPIO_PORTK_BASE, GPIO_PIN_7, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTK_BASE, GPIO_PIN_7);
    IntEnable(INT_GPIOK);

    g_cpmTotal = 0;
    g_cpmGood = 0;

    UARTprintf( "Starting eChronos geiger server...\n" );

    // Configure the hardware MAC address for Ethernet Controller filtering of
    // incoming packets.  The MAC address will be stored in the non-volatile
    // USER0 and USER1 registers.
    MAP_FlashUserGet(&ui32User0, &ui32User1);
    if((ui32User0 == 0xffffffff) || (ui32User1 == 0xffffffff))
    {
        // Let the user know there is no MAC address
        UARTprintf("No MAC programmed!\n");

        while(1)
        {
        }
    }

    // Tell the user what we are doing just now.
    UARTprintf("Waiting for IP.\n");

    // Convert the 24/24 split MAC address from NV ram into a 32/16 split
    // MAC address needed to program the hardware registers, then program
    // the MAC address into the Ethernet Controller registers.
    pui8MACArray[0] = ((ui32User0 >>  0) & 0xff);
    pui8MACArray[1] = ((ui32User0 >>  8) & 0xff);
    pui8MACArray[2] = ((ui32User0 >> 16) & 0xff);
    pui8MACArray[3] = ((ui32User1 >>  0) & 0xff);
    pui8MACArray[4] = ((ui32User1 >>  8) & 0xff);
    pui8MACArray[5] = ((ui32User1 >> 16) & 0xff);

    lwIPInit(g_ui32SysClock, pui8MACArray, 0, 0, 0, IPADDR_USE_DHCP);

    // Setup the device locator service.
    LocatorInit();
    LocatorMACAddrSet(pui8MACArray);
    LocatorAppTitleSet("EK-TM4C1294XL enet_io");

    // Initialize a sample httpd server.
    httpd_init();

    // Set the interrupt priorities.  We set the SysTick interrupt to a higher
    // priority than the Ethernet interrupt to ensure that the file system
    // tick is processed if SysTick occurs while the Ethernet handler is being
    // processed.  This is very likely since all the TCP/IP and HTTP work is
    // done in the context of the Ethernet interrupt.
    MAP_IntPrioritySet(INT_EMAC0, ETHERNET_INT_PRIORITY);
    MAP_IntPrioritySet(FAULT_SYSTICK, SYSTICK_INT_PRIORITY);

    http_set_ssi_handler((tSSIHandler)SSIHandler, g_pcConfigSSITags, NUM_CONFIG_SSI_TAGS);

    IntMasterEnable();

    rtos_start();

    /* Should never reach here, but if we do, an infinite loop is
       easier to debug than returning somewhere random. */
    for (;;) ;
}
