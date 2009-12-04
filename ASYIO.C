#include <stdio.h>
#include <dos.h>
#include <stdarg.h>
#include <alloc.h>
#include <mem.h>

#include "async.h"

extern  int UART_ports[];
extern  struct async_portS async_port[NUMBEROFPORTS];

int     async_putch_timeout(int comport, char c, long timeout)
{
    long    to;

    to = timeout;
    while (async_port[comport].txbuflength>=TXBUFSIZE) {
        if (timeout) {
            to--;
            delay(1);

            if (to == 0) {
                return(0);
            }
        }
    }

    async_port[comport].txbuf[async_port[comport].txhead] = c;

    async_port[comport].txhead++;
    async_port[comport].txbuflength++;
    if (async_port[comport].txhead == TXBUFSIZE) {
        async_port[comport].txhead = 0;
    }

    async_tx_enable(comport, 1);

    return (1);
}

void    async_putch(int comport, char c)
{
    async_putch_timeout(comport, c, 0L);
}

void    async_puts(int comport, char *s)
{
    while (*s) {
        async_putch(comport, *s++);
    }
}

void    async_printf(int comport, char *fmt, ...)
{
    va_list argptr;
    char    *buf;

    buf = malloc(512);                  // Should this be on the stack or
                                        // is this a good way to do it?

    va_start(argptr, fmt);
    vsprintf(buf, fmt, argptr);
    va_end(argptr);

    async_puts(comport, buf);

    free(buf);
}

int     async_putblock_timeout(int comport, const char *block, int size, long timeout)
{
    long    to;

    while (size--) {
        to = timeout;

        // Wait for room in the TX buffer
        while (async_port[comport].txbuflength>=TXBUFSIZE) {
//test            async_tx_enable(comport, 1);
            if (timeout) {
                to--;
                delay(1);

                if (to == 0) {
                    return(0);
                }
            }
        }

        // Go ahead and put next character from block into TX buffer
        async_port[comport].txbuf[async_port[comport].txhead] = *block++;

        // Increment buffer pointers
        async_port[comport].txhead++;
        async_port[comport].txbuflength++;
        if (async_port[comport].txhead == TXBUFSIZE) {
            async_port[comport].txhead = 0;
        }
    }

    async_tx_enable(comport, 1);

    return (1);
}

void    async_putblock(int comport, const char *block, int size)
{
    async_putblock_timeout(comport, block, size, 0L);
}

int     async_ready(int comport)
{
    async_check_flowcontrol(comport);

    if (async_port[comport].rxbuflength) {
        return (1);
    } else {
        return (0);
    }
}

int     async_rx(int comport)
{
    int     c;

    if (async_port[comport].rxbuflength) {
        c = async_port[comport].rxbuf[async_port[comport].rxtail];

        async_port[comport].rxtail++;
        async_port[comport].rxbuflength--;
        if (async_port[comport].rxtail == RXBUFSIZE) {
            async_port[comport].rxtail = 0;
        }

        if (async_port[comport].handshaking) {
            async_check_flowcontrol(comport);
        }

        return(c);
    }

    return (0);
}

char    async_getch(int comport)
{
    char    c;

    c = async_rx(comport);

    return (c);
}

int     async_gets(int comport, char *s, int size)
{
    int     snatched = 0;
    int     done = 0;

    if (!size) {
        return (0);
    }

    memset(s, 0, size);

    while ((async_ready(comport)) && (!done)) {
        s[snatched] = async_rx(comport);

        if ((s[snatched] == 0x0D) || (s[snatched] == 0x0A)) {
            done = 1;
        } else
        if (snatched == size) {
            done = 1;
        }

        snatched ++;
    }

    return (snatched);
}

int     async_getblock_timeout(int comport, char *block, int size, long timeout)
{
    int     snatched = 0;
    long    to = timeout;
    int     done = 0;

    while ((size) && (!done)) {
        do {
            if (timeout) {
                delay(1);
                if (!to) {
                    done = 1;
                    return (snatched);
                }
                to--;
            }
        } while (!async_ready(comport));

        block[snatched] = async_rx(comport);
        snatched++;
    }

    return (snatched);
}

int     async_getblock(int comport, char *block, int size)
{
    int     snatched = 0;

    snatched = async_getblock_timeout(comport, block, size, 0L);

    return (snatched);
}

char    async_peek(int comport)
{
    char    c;

    if (async_port[comport].rxhead != async_port[comport].rxtail) {
        c = async_port[comport].rxbuf[async_port[comport].rxtail];
        return (c);
    }

    return (0);
}

void    async_flushrx(int comport)
{
    async_port[comport].rxhead = 0;
    async_port[comport].rxtail = 0;
    async_port[comport].rxbuflength = 0;

    if (async_port[comport].handshaking) {
        async_check_flowcontrol(comport);
    }
}

void    async_flushtx(int comport)
{
    async_port[comport].txhead = 0;
    async_port[comport].txtail = 0;
    async_port[comport].txbuflength = 0;
}
