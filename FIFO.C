#include <dos.h>

#include "async.h"

extern  int fifo_enabled;
extern  int UART_ports[];
extern  struct async_portS async_port[NUMBEROFPORTS];

// NEEDS WORK

int     init_fifo(int comport, int enable)
{
    if ((!async_port[comport].port_open) || (async_port[comport].uart_type != UART_16550A)) {
        return (0);
    }

    fifo_enabled = enable;
    outportb(UART_ports[comport]+2, (enable?1:0));

    return (1);
}
