// Handshaking

#include <stdio.h>
#include <dos.h>

#include "async.h"

extern  struct async_portS async_port[NUMBEROFPORTS];
extern  int UART_ports[];

void    set_handshaking(int comport, int handshaking, int enable)
{
    if (enable) {
        async_port[comport].handshaking |= handshaking;
    } else {
        async_port[comport].handshaking ^= handshaking;
    }
}

int     async_xonxoff_enabled(int comport)
{
    return (async_port[comport].handshaking&HSHAKE_XONXOFF);
}

int     async_rtscts_enabled(int comport)
{
    return (async_port[comport].handshaking&HSHAKE_RTSCTS);
}

int     async_dtrdsr_enabled(int comport)
{
    return (async_port[comport].handshaking&HSHAKE_DTRDSR);
}

int     async_clear_to_send(int comport)
{
    // In order of what seems to be priority to me.

    if ((async_port[comport].handshaking&HSHAKE_DTRDSR) && (async_port[comport].dtrdsr_flowstatus_tx)) {
        return (0);
    }

    if ((async_port[comport].handshaking&HSHAKE_RTSCTS) && (async_port[comport].rtscts_flowstatus_tx)) {
        return (0);
    }

    if ((async_port[comport].handshaking&HSHAKE_XONXOFF) && (async_port[comport].xonxoff_flowstatus_tx)) {
        return (0);
    }

    return (1);
}

void    async_xonxoff_on(int comport)
{
    if (async_port[comport].handshaking&HSHAKE_XONXOFF) {

        // Toggle xonxoff_flowstatus_rx.
        async_port[comport].xonxoff_flowstatus_rx = 0;

        // Place XON signal before anything else in the tx buffer.
        async_port[comport].txtail--;
        async_port[comport].txbuflength++;
        if (async_port[comport].txtail < 0) {
            async_port[comport].txtail = TXBUFSIZE;
        }

        async_port[comport].txbuf[async_port[comport].txtail] = _XON;
        async_tx_enable(comport, 1);
    }
}

void    async_rtscts_on(int comport)
{
    if (async_port[comport].handshaking&HSHAKE_RTSCTS) {
    }
}

void    async_dtrdsr_on(int comport)
{
    if (async_port[comport].handshaking&HSHAKE_DTRDSR) {
    }
}

void    async_xonxoff_off(int comport)
{
    if (async_port[comport].handshaking&HSHAKE_XONXOFF) {

        // Toggle xonxoff_flowstatus_rx.
        async_port[comport].xonxoff_flowstatus_rx = 1;

        // Place XOFF signal before anything else in the tx buffer.
        async_port[comport].txtail--;
        async_port[comport].txbuflength++;
        if (async_port[comport].txtail < 0) {
            async_port[comport].txtail = TXBUFSIZE;
        }
        async_port[comport].txbuf[async_port[comport].txtail] = _XOFF;
        async_tx_enable(comport, 1);
    }
}

void    async_rtscts_off(int comport)
{
    if (async_port[comport].handshaking&HSHAKE_RTSCTS) {
    }
}

void    async_dtrdsr_off(int comport)
{
    if (async_port[comport].handshaking&HSHAKE_DTRDSR) {
    }
}

void    async_check_flowcontrol(int comport)
{
    if (async_port[comport].handshaking) {

        // Ok to receive -- Below lower limit.
        if (async_port[comport].rxbuflength <= LOWRXBUF) {
            // If XONXOFF stopped then start it back up.
            if (async_port[comport].xonxoff_flowstatus_rx) {
                async_xonxoff_on(comport);
            }

            // If RTSCTS stopped then start it back up.
            if (async_port[comport].rtscts_flowstatus_rx) {
                async_rtscts_on(comport);
            }

            // If DTRDSR stopped then start it back up.  This isn't right I don't think.
            if (async_port[comport].dtrdsr_flowstatus_rx) {
                async_dtrdsr_on(comport);
            }
        } else

        // Not ok to receive -- Above critical limit.
        if (async_port[comport].rxbuflength >= HIGHRXBUF) {
            // If XONXOFF flowing then stop it.
            if (!async_port[comport].xonxoff_flowstatus_rx) {
                async_xonxoff_off(comport);
            }

            // If RTSCTS flowing then stop it.
            if (!async_port[comport].rtscts_flowstatus_rx) {
                async_rtscts_off(comport);
            }

            // If DTRDSR flowing then stop it.  This isn't right either.
            // I have this feeling that I'm a bit confused on flow control.
            if (!async_port[comport].dtrdsr_flowstatus_rx) {
                async_dtrdsr_off(comport);
            }
        }
    }
}
