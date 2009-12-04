#ifndef     __async_h
#define     __async_h

// Dos extender compatibility definitions.                          //add
#ifdef  DOSX286     // This is kind of a hack, but it works well.
    #define _OLDISR     void _interrupt _far (*oldISR)();        \
                        void _interrupt _far (*oldISRp)()
#endif

// Compiler compatibility definitions.

#ifdef  BORLANDC
    #ifndef _OLDISR
        #define _OLDISR     void interrupt (*oldISR)()
    #endif
#endif

#ifdef  TURBOC
    #define _dos_getvect    getvect
    #define _dos_setvect    setvect

    #define _enable         enable
    #define _disable        disable

    #define _OLDISR     void interrupt (*oldISR)()
#endif

#ifdef  POWERC
    #define _dos_getvect    getvect
    #define _dos_setvect    setvect

    #define _enable         enable
    #define _disable        disable

    #define clrscr          clrscrn

    #define _OLDISR     void interrupt (*oldISR)()

// pc_comp.c        -- Power C Compatibility functions
void    delay(long milliseconds);
#endif

#ifdef  MSC
    #define _OLDISR     void far *oldISR
#endif

#define     NUMBEROFPORTS   4       // Number of COM ports available
                                    // If this number is changed, and you plan
                                    // on opening a com port greater than 4
                                    // then you must create an ISR for it.
                                    // See UART_ISR1..4 and you shouldn't have
                                    // any problems.

#define     COM1            0       // COM port 1
#define     COM2            1       // COM port 2
#define     COM3            2       // COM port 3
#define     COM4            3       // COM port 4

#define     UART_NONE       0       // No UART attached
#define     UART_8250       1       // UART is an 8250
#define     UART_16450      2       // UART is a 16460
#define     UART_16550      3       // UART is a 16550
#define     UART_16550A     4       // UART is a 16550A

#define     HSHAKE_NONE     0x00    // Handshaking:  None
#define     HSHAKE_XONXOFF  0x01    // Handshaking:  XON/XOFF
#define     HSHAKE_RTSCTS   0x02    // Handshaking:  RTS/CTS    //add
#define     HSHAKE_DTRDSR   0x04    // Handshaking:  DTR/DSR    //add

#define     TXBUFSIZE       2048    // Transmit Buffer Size
#define     RXBUFSIZE       2048    // Receive Buffer Size

#define     LOWRXBUF        128     // Low RX buffer size for flow control      //add
#define     HIGHRXBUF       512     // High RX buffer size for flow control     //add

#define     BASE            0       // Base (No real reason to have this)
#define     IER             1       // Interrupt Enable Register
#define     IIR             2       // Interrupt Identification Register
#define     FCR             2       // 16550 FIFO Control Register
#define     LCR             3       // Line Control Register
#define     MCR             4       // Modem Control Register
#define     LSR             5       // Line Status Register
#define     MSR             6       // Modem Status Register
#define     SCRATCH         7       // Scratch Pad (Is this ever used?)

#define     MCR_DTR         0x01    // DTR
#define     MCR_RTS         0x02    // RTS
#define     MCR_OUT1        0x04    // OUT1
#define     MCR_OUT2        0x08    // OUT2
#define     MCR_LOOPBACK    0x10    // Loopback

#define     LCR_PNONE       0       // None
#define     LCR_PODD        8       // Odd
#define     LCR_PEVEN       24      // Even
#define     LCR_PMARK       40      // Mark
#define     LCR_PSPACE      56      // Space

#define     MSR_DCTS        0x01    // Delta Clear to Send changed      //add
#define     MSR_DDSR        0x02    // Delta Data Set Ready changed     //add
#define     MSR_RI_CHANGED  0x04    // Ring Indicator changed           //add
#define     MSR_DDCD        0x08    // Delta Data Carrier Detect changed//add
#define     MSR_CTS         0x10    // Clear to Send                    //add
#define     MSR_DSR         0x20    // Data Set Ready                   //add
#define     MSR_RI          0x40    // Ring Indicator                   //add
#define     MSR_RLSD        0x80    // Receive Line Signal Detect       //add

#define     LSR_DATAREADY   0x01    // Data Ready                       //add
#define     LSR_OVERRUN     0x02    // Overrun Error (OR)               //add
#define     LSR_PARITY      0x04    // Parity Error (PE)                //add
#define     LSR_FRAMING     0x08    // Framing Error (FE)               //add
#define     LSR_BREAK       0x10    // Break Interrupt (BI)             //add
#define     LSR_THRE        0x20    // Transmitter Holding Register Empty //add
#define     LSR_TSRE        0x40    // Transmitter Shift Register Empty //add
#define     LSR_16550ERR    0x80    // FE, PE, or BI in FIFO queue.  16550 Only.  //add

// A few ASCII codes
#define     _CR             0x0D    // Carriage Return (^M, Ascii 13)   //add
#define     _LF             0x0A    // Line Feed (^J, Ascii 10)         //add
#define     _ESC            0x1B    // Escape (^[, Ascii 27)            //add
#define     _BEL            0x07    // Bell (^G, Ascii 7)               //add
#define     _XON            0x11    // XON (DC3, ^Q, Ascii 17)          //add
#define     _XOFF           0x13    // XOFF (DC1, ^S, Ascii 19)         //add

struct  async_portS {
    int     port_open;

    _OLDISR;                            // See compatibility defs for info.

    volatile char *txbuf;
    volatile int txhead, txtail;
    volatile int txbuflength;

    volatile char *rxbuf;
    volatile int rxhead, rxtail;
    volatile int rxbuflength;

    volatile int xonxoff_flowstatus_rx;     //add
    volatile int rtscts_flowstatus_rx;      //add
    volatile int dtrdsr_flowstatus_rx;      //add

    volatile int xonxoff_flowstatus_tx;     //add
    volatile int rtscts_flowstatus_tx;      //add
    volatile int dtrdsr_flowstatus_tx;      //add

    int     overrun_errors;
    int     parity_errors;
    int     framing_errors;
    int     LSRstatus;
    int     MSRstatus;

    int     uart_type;

    int     handshaking;                    // Implementing     //add
};

// setuart.c        -- UART setup routines
void    set_UART_port(int comport, int value);
void    set_UART_int(int comport, int value);
void    set_UART_onmask(int comport, int value);
void    set_UART_offmask(int comport, int value);
int     set_parity(int comport, int parity);
int     set_stopbits(int comport, int stopbits);
int     set_wordlength(int comport, int wordlength);
int     set_baudrate(int comport, long baudrate);

// detect.c         -- Various detecting routines: needs work
int     async_detect_uart(int comport);
int     async_detect_irq(int comport);

// isr.c            -- ISR setup routines, and the actual ISR's themselves
void    init_ISR(int comport);
void    deinit_ISR(int comport);
void    async_tx_enable(int comport, int enable);               //add

// fifo.c           -- FIFO routines: needs work
int     init_fifo(int comport, int enable);

// async.c          -- Async setup routines
int     open_port(int comport, long baudrate, int parity, int stopbits, int wordlength);
int     close_port(int comport, int rts, int dtr);

// line.c           -- Line control/monitoring routines: needs work
int     async_LCR(int comport);
int     async_MCR(int comport);
int     async_LSR(int comport);
int     async_MSR(int comport);
int     async_DTR_status(int comport);
int     async_RTS_status(int comport);
void    async_set_DTR(int comport, int status);
void    async_set_RTS(int comport, int status);

// asyio.c          -- IO/Buffer routines: needs work
int     async_putch_timeout(int comport, char c, long timeout);
void    async_putch(int comport, char c);
void    async_puts(int comport, char *s);
void    async_printf(int comport, char *fmt, ...);
int     async_putblock_timeout(int comport, const char *block, int size, long timeout);
void    async_putblock(int comport, const char *block, int size);
int     async_ready(int comport);
char    async_getch(int comport);
int     async_gets(int comport, char *s, int size);
int     async_getblock_timeout(int comport, char *block, int size, long timeout);
int     async_getblock(int comport, char *block, int size);
char    async_peek(int comport);
void    async_flushrx(int comport);
void    async_flushtx(int comport);

// hshake.c         -- Handshaking check/set routines.          //add
void    set_handshaking(int comport, int handshaking, int enable);
int     async_xonxoff_enabled(int comport);
int     async_rtscts_enabled(int comport);
int     async_dtrdsr_enabled(int comport);
int     async_clear_to_send(int comport);
void    async_xonxoff_on(int comport);
void    async_rtscts_on(int comport);
void    async_dtrdsr_on(int comport);
void    async_xonxoff_off(int comport);
void    async_rtscts_off(int comport);
void    async_dtrdsr_off(int comport);
void    async_check_flowcontrol(int comport);

#endif
