#include <dos.h>

#include "async.h"

extern  int UART_ports[];
extern  int UART_interrupts[];
extern  int UART_onmask[];
extern  int UART_offmask[];

extern  struct async_portS async_port[NUMBEROFPORTS];

void    set_UART_port(int comport, int value)
{
    UART_ports[comport] = value;
}

void    set_UART_int(int comport, int value)
{
    UART_interrupts[comport] = value;
}

void    set_UART_onmask(int comport, int value)
{
    UART_onmask[comport] = value;
}

void    set_UART_offmask(int comport, int value)
{
    UART_offmask[comport] = value;
}

int     set_parity(int comport, int parity)
{
    int     lcr;

    if (!async_port[comport].port_open) {
        return (0);
    }

    lcr = inportb(UART_ports[comport]+LCR);
    lcr &= 199;
    lcr |= parity;
    outportb(UART_ports[comport]+LCR, lcr);

    return (1);
}

int     set_stopbits(int comport, int stopbits)
{
    int     lcr;

    if (!async_port[comport].port_open) {
        return (0);
    }

    lcr = inportb(UART_ports[comport]+LCR);
    lcr &= 251;
    lcr |= (stopbits-1)*4;
    outportb(UART_ports[comport]+LCR, lcr);

    return (1);
}

int     set_wordlength(int comport, int wordlength)
{
    int     lcr;

    if (!async_port[comport].port_open) {
        return (0);
    }

    lcr = inportb(UART_ports[comport]+LCR);
    lcr &= 252;
    lcr |= (wordlength-5);
    outportb(UART_ports[comport]+LCR, lcr);

    return (1);
}

int     set_baudrate(int comport, long baudrate)
{
    int     divisor, lcr;

    if (!async_port[comport].port_open) {
        return (0);
    }

    // Set UART Baud Rate
    lcr = inportb(UART_ports[comport]+LCR);
    divisor = (115200L/baudrate);
    outportb(UART_ports[comport]+LCR, 0x80);
    outport(UART_ports[comport], divisor);
    outportb(UART_ports[comport]+LCR, lcr);

    return (1);
}
