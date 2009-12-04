#include <dos.h>

#include "async.h"

extern  int UART_ports[];

int     async_detect_uart(int comport)
{
    int     x;
    int     base = UART_ports[comport];

    outp(base+3,0x1b);
    if (inp(base+3)!=0x1b) {
        return(UART_NONE);
    }

    outp(base+3,0x3);
    if (inp(base+3)!=0x3) {
        return(UART_NONE);
    }

    outp(base+7,0x55);
    if (inp(base+7)!=0x55) {
        return(UART_8250);
    }

    outp(base+7,0xAA);
    if (inp(base+7)!=0xAA) {
        return(UART_16450);
    }

    outp(base+2,1);
    x=inp(base+2);
    if ((x&0x80)==0) {
        return(UART_16450);
    }
    if ((x&0x40)==0) {
        return(UART_16550);
    }

    outp(base+2,0x0);

    return(UART_16550A);
}

int     async_detect_irq(int comport)
{
    int     mask, irq, imr, ier, lcr, mcr;
    int     base = UART_ports[comport];

    _disable();

    lcr = inp(base+LCR);                // Read LCR
    outp(base+LCR, ~0x80 & lcr);        // Clear DLAB

    ier = inp(base+IER);                // Read IER
    outp(base+IER, 0x00);               // Disable all UART interrupts

    mcr = inp(base+MCR);                // Read MCR
    outp(base+MCR, (~0x10 & mcr) | 0x0C); // Enable UART interrupt generation

    imr = inp(0x21);                    // Read the interrupt mask register
    outp(0x20, 0x0A);                   // Prepare to read the IRR

    mask = 0xFC;                        // The mask for IRQ2-7
    outp(base+IER, 0x02);               // Enable 'Transmitter Empty' interrupt

    mask &= inp(0x20);                  // Select risen interrupt request
    outp(base+IER, 0x00);               // Disable 'Transmitter Empty' interrupt

    mask &= ~inp(0x20);                 // Select fallen interrupt request
    outp(base+IER, 0x02);               // Enable 'Transmitter Empty' interrupt

    mask &= inp(0x20);                  // Select risen interrupt request
    outp(0x21, ~mask);                  // Unmask only this interrupt(s)

    outp(0x20, 0x0C);                   // Enter the poll mode

    irq = inp(0x20);                    // Accept the high level interrupt
    inp(base+LSR);                      // Read LSR to reset line status interrupt
    inp(base);                          // Read RBR to reset data ready interrupt
    inp(base+IIR);                      // Read IIR to reset transmitter empty interrupt
    inp(base+MSR);                      // Read MSR to reset modem status interrupt

    outp(base+IER, ier);                // Restore Interrupt Enable Reg
    outp(base+LCR, lcr);                // Restore Line Control Reg
    outp(base+MCR, mcr);                // Restore Modem Control Reg
    outp(0x20, 0x20);                   // End of interrupt mode
    outp(0x21, imr);                    // Restore Interrupt Mask Reg

    _enable();

    return ((irq&0x80) ? (irq&0x07) : -1);
}
