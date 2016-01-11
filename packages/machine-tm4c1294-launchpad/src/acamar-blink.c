#include <stdint.h>
#include <stddef.h>
#include "tm4c1294ncpdt.h"

#include "rtos-acamar.h"

volatile uint32_t ui32Loop;

extern void debug_println(const char *msg);

void
fn_a(void)
{
    for (;;)
    {
        rtos_yield_to(1);
        debug_println("A");

        GPIO_PORTN_DATA_R |= 0x01;

        for(ui32Loop = 0; ui32Loop < 20000; ui32Loop++)
        {
        }

    }
}

void
fn_b(void)
{
    for (;;)
    {
        rtos_yield_to(0);
        debug_println("B");

        GPIO_PORTN_DATA_R &= ~(0x01);

        for(ui32Loop = 0; ui32Loop < 20000; ui32Loop++)
        {
        }
    }
}

int
main(void)
{
    SYSCTL_RCGCGPIO_R = SYSCTL_RCGCGPIO_R12;

    ui32Loop = SYSCTL_RCGCGPIO_R;

    GPIO_PORTN_DIR_R = 0x01;
    GPIO_PORTN_DEN_R = 0x01;

    rtos_start();
    for (;;) ;
}
