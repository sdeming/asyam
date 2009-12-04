#include "async.h"

extern  int UART_ports[];

void    interrupt far pUART_ISR1()                   // ISR for COM1
{
    int     iir;
    int     done = 0;

    while (!done) {
        iir = inportb(UART_ports[COM1]+IIR);

        if (iir&1) {
            done = 1;
        } else {
            iir&=6;
            iir>>=1;

            switch (iir) {
                case 3:                     // In order of priority.  Does it matter?
                    ISRstatus(COM1);
                    break;
                case 2:
                    ISRrx(COM1);
                    break;
                case 1:
                    ISRtx(COM1);
                    break;
                case 0:
                    ISRmodem(COM1);
                    break;
            }
        }
    }

    outport(0x20, 0x20);
}

void    interrupt far pUART_ISR2()                   // ISR for COM2
{
    int     iir;
    int     done = 0;

    while (!done) {
        iir = inportb(UART_ports[COM2]+IIR);

        if (iir&1) {
            done = 1;
        } else {
            iir&=6;
            iir>>=1;

            switch (iir) {
                case 3:                     // In order of priority.  Does it matter?
                    ISRstatus(COM2);
                    break;
                case 2:
                    ISRrx(COM2);
                    break;
                case 1:
                    ISRtx(COM2);
                    break;
                case 0:
                    ISRmodem(COM2);
                    break;
            }
        }
    }

    outport(0x20, 0x20);
}

void    interrupt far pUART_ISR3()                   // ISR for COM3
{
    int     iir;
    int     done = 0;

    while (!done) {
        iir = inportb(UART_ports[COM3]+IIR);

        if (iir&1) {
            done = 1;
        } else {
            iir&=6;
            iir>>=1;

            switch (iir) {
                case 3:                     // In order of priority.  Does it matter?
                    ISRstatus(COM3);
                    break;
                case 2:
                    ISRrx(COM3);
                    break;
                case 1:
                    ISRtx(COM3);
                    break;
                case 0:
                    ISRmodem(COM3);
                    break;
            }
        }
    }

    outport(0x20, 0x20);
}

void    interrupt far pUART_ISR4()                   // ISR for COM4
{
    int     iir;
    int     done = 0;

    while (!done) {
        iir = inportb(UART_ports[COM4]+IIR);

        if (iir&1) {
            done = 1;
        } else {
            iir&=6;
            iir>>=1;

            switch (iir) {
                case 3:                     // In order of priority.  Does it matter?
                    ISRstatus(COM4);
                    break;
                case 2:
                    ISRrx(COM4);
                    break;
                case 1:
                    ISRtx(COM4);
                    break;
                case 0:
                    ISRmodem(COM4);
                    break;
            }
        }
    }

    outport(0x20, 0x20);
}
