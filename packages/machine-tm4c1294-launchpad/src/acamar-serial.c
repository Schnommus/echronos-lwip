#include <stdint.h>
#include <stdbool.h>
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

#include "rtos-acamar.h"

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

void
fn_a(void)
{
    for (;;)
    {
        rtos_yield_to(1);
        UARTprintf("In Task A...:\n");

        // Turn LED on
        LEDWrite(CLP_D1, 1);

        SysCtlDelay(g_ui32SysClock / 10 / 3);
    }
}

void
fn_b(void)
{
    for (;;)
    {
        rtos_yield_to(0);
        UARTprintf("In Task B...:\n");
        
        // Turn LED off
        LEDWrite(CLP_D1, 0);

        SysCtlDelay(g_ui32SysClock / 10 / 3);

    }
}

int
main(void)
{
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                SYSCTL_CFG_VCO_480), 120000000);

    // Configure the device pins.
    PinoutSet(false, false);

    // Enable the GPIO pins for the LED D1 (PN1).
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

    ConfigureUART();


    UARTprintf("Starting eChronos RTOS core:\n");
    rtos_start();
    for (;;) ;
}
