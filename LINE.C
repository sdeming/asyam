#include <dos.h>

#include "async.h"

extern  int UART_ports[];

int     async_LCR(int comport)
{
    return (inportb(UART_ports[comport]+LCR));
}

int     async_MCR(int comport)
{
    return (inportb(UART_ports[comport]+MCR));
}

int     async_LSR(int comport)
{
    return (inportb(UART_ports[comport]+LSR));
}

int     async_MSR(int comport)
{
    return (inportb(UART_ports[comport]+MSR));
}

int     async_DTR_status(int comport)
{
    return (inportb(UART_ports[comport]+MCR)&MCR_DTR);
}

int     async_RTS_status(int comport)
{
    return ((inportb(UART_ports[comport]+MCR)&MCR_RTS)?1:0);
}

void    async_set_DTR(int comport, int status)
{
    int dtr;

    dtr = inportb(UART_ports[comport]+MCR);

    if (status) {
        dtr |= MCR_DTR;
    } else {
        dtr ^= MCR_DTR;
    }

    outportb(UART_ports[comport]+MCR, dtr);
}

void    async_set_RTS(int comport, int status)
{
    int rts;

    rts = inportb(UART_ports[comport]+MCR);

    if (status) {
        rts |= MCR_RTS;
    } else {
        rts ^= MCR_RTS;
    }

    outportb(UART_ports[comport]+MCR, rts);
}
