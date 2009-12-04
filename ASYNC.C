#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <mem.h>
#include <alloc.h>
#include <conio.h>

#include "async.h"

int     fifo_enabled = 0;

int     UART_ports[] = {0x3f8, 0x2f8, 0x3e8, 0x2e8};
int     UART_interrupts[] = {0xC, 0xB, 0xC, 0xB};
int     UART_onmask[] = {0xEF, 0xF7, 0xEF, 0xF7};
int     UART_offmask[] = {0x10, 0x08, 0x10, 0x08};

struct  async_portS async_port[NUMBEROFPORTS];

int     open_port(int comport, long baudrate, int parity, int stopbits, int wordlength)
{
    int     lcr = 0;
    int     mcr;

    if (async_port[comport].port_open) {
        return (0);
    }

    // Set port open status
    async_port[comport].port_open = 1;

    // Set handshaking to none, and clear all handshaking flags
    async_port[comport].handshaking = HSHAKE_NONE;
    async_port[comport].xonxoff_flowstatus_rx = 0;
    async_port[comport].rtscts_flowstatus_rx = 0;
    async_port[comport].dtrdsr_flowstatus_rx = 0;
    async_port[comport].xonxoff_flowstatus_tx = 0;
    async_port[comport].rtscts_flowstatus_tx = 0;
    async_port[comport].dtrdsr_flowstatus_tx = 0;

    // Set up transmit buffer
    async_port[comport].txbuf = malloc(TXBUFSIZE);
    memset(async_port[comport].txbuf, 0, TXBUFSIZE);
    async_port[comport].txhead = 0;
    async_port[comport].txtail = 0;
    async_port[comport].txbuflength = 0;

    // Set up receive buffer
    async_port[comport].rxbuf = malloc(RXBUFSIZE);
    memset(async_port[comport].rxbuf, 0, RXBUFSIZE);
    async_port[comport].rxhead = 0;
    async_port[comport].rxtail = 0;
    async_port[comport].rxbuflength = 0;

    // Set up interrupt handler
    init_ISR(comport);

    // Set UART Baud Rate -- Have the function at hand, why not use it?
    set_baudrate(comport, baudrate);

    // Set UART LCR -- Faster to do it in one shot, than call set_this and set_that.
    lcr = (wordlength-5) + ((stopbits-1)*4) + parity;
    outportb(UART_ports[comport]+LCR, lcr);

    // Setup MCR
    mcr = inportb(UART_ports[comport]+MCR);
    mcr |= MCR_DTR;
    mcr |= MCR_RTS;
    outportb(UART_ports[comport]+MCR, mcr);

    // Find out what kind of UART this is, set up fifo if its a 16550A
    async_port[comport].uart_type = async_detect_uart(comport);
    if (async_port[comport].uart_type == UART_16550A) {
        init_fifo(comport, 1);
    }

    return (1);
}

int     close_port(int comport, int rts, int dtr)
{
    int     mcr;

    if (!async_port[comport].port_open) {
        return (0);
    }

    deinit_ISR(comport);

    // Reset MCR
    mcr = inport(UART_ports[comport]+MCR);
    if (!dtr) {
        mcr ^= MCR_DTR;
    }
    if (!rts) {
        mcr ^= MCR_RTS;
    }
    outportb(UART_ports[comport]+MCR, mcr);

    // Disable Fifo
    if (fifo_enabled) {
        outportb(UART_ports[comport]+2, 0);
    }

    // Free transmit buffer
    free(async_port[comport].txbuf);
    async_port[comport].txhead = 0;
    async_port[comport].txtail = 0;

    // Free receive buffer
    free(async_port[comport].rxbuf);
    async_port[comport].rxhead = 0;
    async_port[comport].rxtail = 0;

    // Set port open status to not open
    async_port[comport].port_open = 0;

    return (1);
}
