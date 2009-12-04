#include <stdlib.h>
#include <dos.h>

#ifdef  DOSX286
#include <phapi.h>
#endif

#include "async.h"

extern  int UART_ports[];
extern  int UART_interrupts[];
extern  int UART_onmask[];
extern  int UART_offmask[];

extern  struct async_portS async_port[4];
extern  int MSRStatus[NUMBEROFPORTS];
extern  int LSRStatus[NUMBEROFPORTS];

// Prototypes for ISR's
void    interrupt UART_ISR1(void);
void    interrupt UART_ISR2(void);
void    interrupt UART_ISR3(void);
void    interrupt UART_ISR4(void);

// Prototypes for protected mode ISR's.
#ifdef  DOSX286
void    interrupt pUART_ISR1(void);
void    interrupt pUART_ISR2(void);
void    interrupt pUART_ISR3(void);
void    interrupt pUART_ISR4(void);
#endif

// Prototypes for ISR utilities.
static  void near ISRmodem(int comport);
static  void near ISRtx(int comport);
static  void near ISRrx(int comport);
static  void near ISRstatus(int comport);

// Flag determining if atexit() function is installed.
static  int atexit_installed = 0;

// Prototype for exit function
static  void ISR_exit(void);

static  void ISR_exit()
{
    int     i;

    for (i=0; i<NUMBEROFPORTS; i++) {
        if (async_port[i].port_open) {
            deinit_ISR(i);
        }
    }
}

void    async_tx_enable(int comport, int enable)
{
    int     ier;

    ier = inportb(UART_ports[comport]+IER);

    if (enable) {
        outportb(UART_ports[comport]+IER, ier|0x03);
    } else {
        outportb(UART_ports[comport]+IER, ier&0x0D);
    }
}

void    init_ISR(int comport)
{
    int     icu, mcr;

    if (!atexit_installed) {
        atexit(ISR_exit);
        atexit_installed = 1;
    }

    _disable();

    // Get old interrupt vector
#ifndef DOSX286
    async_port[comport].oldISR = _dos_getvect(UART_interrupts[comport]);
#endif

    // Set new interrupt vector
    switch (comport) {
        case COM1:
#ifndef DOSX286
            _dos_setvect(UART_interrupts[comport], UART_ISR1);
#else
            DosSetRealProtVec(UART_interrupts[comport], (PIHANDLER) pUART_ISR1, DosProtToReal((void *) UART_ISR1), (PIHANDLER far *) async_port[comport].oldISRp, (REALPTR far *) async_port[comport].oldISR);
//            DosSetPassToProtVec(UART_interrupts[comport], (PIHANDLER) UART_ISR1, (PIHANDLER far *) async_port[comport].oldISRp, (REALPTR far *) async_port[comport].oldISR);
#endif
            break;
        case COM2:
#ifndef DOSX286
            _dos_setvect(UART_interrupts[comport], UART_ISR2);
#else
            DosSetRealProtVec(UART_interrupts[comport], (PIHANDLER) pUART_ISR2, DosProtToReal((void *) UART_ISR2), (PIHANDLER far *) async_port[comport].oldISRp, (REALPTR far *) async_port[comport].oldISR);
#endif
            break;
        case COM3:
#ifndef DOSX286
            _dos_setvect(UART_interrupts[comport], UART_ISR3);
#else
            DosSetRealProtVec(UART_interrupts[comport], (PIHANDLER) pUART_ISR3, DosProtToReal((void *) UART_ISR3), (PIHANDLER far *) async_port[comport].oldISRp, (REALPTR far *) async_port[comport].oldISR);
#endif
            break;
        case COM4:
#ifndef DOSX286
            _dos_setvect(UART_interrupts[comport], UART_ISR4);
#else
            DosSetRealProtVec(UART_interrupts[comport], (PIHANDLER) pUART_ISR4, DosProtToReal((void *) UART_ISR4), (PIHANDLER far *) async_port[comport].oldISRp, (REALPTR far *) async_port[comport].oldISR);
#endif
            break;
    }

    // Set OUT2 bit to enable interrupts
    mcr = inportb(UART_ports[comport]+MCR);
    mcr |= MCR_OUT2;
    outportb(UART_ports[comport]+MCR, mcr);

    // Enable interrupts we want to start off with.  LSC and MSC.
    outportb(UART_ports[comport]+IER, 0x0D);

    // Unmask the interrupts in the ICU
    icu = inportb(0x21);
    icu &= UART_onmask[comport];
    outportb(0x21, icu);

    _enable();
}

void    deinit_ISR(int comport)
{
    int     icu;

    _disable();
    // Return the old interrupt vector to normal
#ifndef DOSX286
    _dos_setvect(UART_interrupts[comport], async_port[comport].oldISR);
#else
    DosSetRealProtVec(UART_interrupts[comport], (PIHANDLER) async_port[comport].oldISRp, (REALPTR) async_port[comport].oldISR, NULL, NULL);
#endif

    // Remask the interrupts in the ICU
    icu = inportb(0x21);
    icu |= UART_offmask[comport];
    outportb(0x21, icu);

    _enable();
}

// Interrupt Service Routines

void    interrupt far UART_ISR1()                   // ISR for COM1
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

void    interrupt far UART_ISR2()                   // ISR for COM2
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

void    interrupt far UART_ISR3()                   // ISR for COM3
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

void    interrupt far UART_ISR4()                   // ISR for COM4
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

// Phar Lap PROTECTED MODE ISR's.
#ifdef  DOSX286

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

#endif

void    near ISRmodem(int comport)
{
    async_port[comport].MSRstatus = inportb(UART_ports[comport]+MSR);

    // RTS/CTS Flow Control
    if (async_port[comport].MSRstatus & MSR_DCTS) {

        // Set the flowstatus to the proper value.
        async_port[comport].rtscts_flowstatus_tx = ((async_port[comport].MSRstatus & MSR_CTS)==MSR_CTS);

        // Make sure that ISRtx gets the message.
        async_tx_enable(comport, 1);
    }

    // DTR/DSR Flow Control
    if (async_port[comport].MSRstatus * MSR_DDSR) {

        // Set the flowstatus to the proper value.
        async_port[comport].dtrdsr_flowstatus_tx = ((async_port[comport].MSRstatus & MSR_DSR)==MSR_DSR);

        // Make sure that ISRtx gets the message.
        async_tx_enable(comport, 1);
    }
}

void    near ISRtx(int comport)
{
    int     mcr;

    // Do RTS/CTS flow control here
    if (async_port[comport].handshaking&HSHAKE_RTSCTS) {
        mcr = inportb(UART_ports[comport]+MCR);
        if (!(mcr & MCR_RTS)) {
            outportb(UART_ports[comport]+MCR, mcr&MCR_RTS);
        }
    }

    // Do DTR/DSR flow control here
    if (async_port[comport].handshaking&HSHAKE_DTRDSR) {
        mcr = inportb(UART_ports[comport]+MCR);
        if (!(mcr & MCR_DTR)) {
            outportb(UART_ports[comport]+MCR, mcr&MCR_DTR);
        }
    }

    // Read from buffer and transmit.
    while (1) {
        if ((async_port[comport].txbuflength) && (async_clear_to_send(comport))) {

            // There is something waiting to be sent, send it
            outportb(UART_ports[comport], async_port[comport].txbuf[async_port[comport].txtail]);

            // Move buffer tail
            async_port[comport].txtail++;
            async_port[comport].txbuflength--;
            if (async_port[comport].txtail == TXBUFSIZE) {
                async_port[comport].txtail = 0;
            }

            async_port[comport].LSRstatus = inport(UART_ports[comport]+LSR);
            if (async_port[comport].LSRstatus & 0x01) {
                break;
            }

            if (!(async_port[comport].LSRstatus & 0x20)) {
                break;
            }

        } else {
            // There is nothing, so inhibit TX interrupts
            async_tx_enable(comport, 0);
            break;
        }
    }
}

void    near ISRrx(int comport)
{
    int     rx;
    int     done = 0;
    int     skip = 0;

    while (!done) {
        rx = inportb(UART_ports[comport]);

        // Check for XON/XOFF
        if (async_xonxoff_enabled(comport)) {
            if (rx == _XON) {
                async_port[comport].xonxoff_flowstatus_tx = 0;
                async_tx_enable(comport, 1);
                skip = 1;
            } else
            if (rx == _XOFF) {
                async_port[comport].xonxoff_flowstatus_tx = 1;
                skip = 1;
            }
        }

        if (!skip) {
            // Place character into buffer
            async_port[comport].rxbuf[async_port[comport].rxhead] = rx;

            // Increment rx buffer head
            async_port[comport].rxhead++;
            async_port[comport].rxbuflength++;
            if (async_port[comport].rxhead == RXBUFSIZE) {
                async_port[comport].rxhead = 0;
            }
        }

        skip = 0;

        // Read Line Status Register.
        async_port[comport].LSRstatus = inportb(UART_ports[comport]+LSR);

        // Check for more.
        if (!(async_port[comport].LSRstatus & 0x01)) {
            done = 1;
        }

        // Check transmit shift register.
        if (async_port[comport].LSRstatus & 0x40) {
            ISRtx(comport);
        }
    }
}

void    near ISRstatus(int comport)
{
    async_port[comport].LSRstatus = inportb(UART_ports[comport]+LSR);
}
