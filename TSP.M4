I received this from Frank Whaley <few@autodesk.com>; he agreed with
spreading the shar round. The shell archive attached may be corrupt
since I didn't use any protection when I transferred it with my old-stylish
modem; please tell me about this.
                                                             Chris

-------------------------------------------------------------------------

I disagree with only one point -- that interrupt-handling in C is unusable.  
In the US, a 25MHz 386SX is almost the minimum configuration.  The attached 
code has been used by many people to build high-speed communication or 
data acquisition software.

Frank Whaley
Senior Programmer, Autodesk
few@autodesk.com
-----
#! /bin/sh
# This is a shell archive, meaning:
# 1. Remove everything above the #! /bin/sh line.
# 2. Save the resulting text in a file.
# 3. Execute the file with /bin/sh (not csh) to create:
#       makefile
#       com.h
#       com.c
#       handler.c
#       term.c
# This archive created: Sun Feb  9 18:24:20 1992
export PATH; PATH=/bin:/usr/bin:$PATH
if test -f 'makefile'
then
        echo shar: "will not over-write existing file 'makefile'"
else
sed 's/^X//' << \SHAR_EOF > 'makefile'
X#
X#      makefile for dumb terminal emulator
X#
X
X# set PORT to COM1 or COM2
XPORT   = COM1
X
X# set BAUD to baudrate value from com.h
XBAUD   = Baud2400
X
X# set BITS to flags from com.h
XBITS   = ParityNone+Stop1+Data8
X
X# set XMITINT to -DTXR_INT to include transmit interrupt code
XXMITINT        = 
X
X
XCFLAGS = -DPORT=$(PORT) -DBAUD=$(BAUD) -DBITS=$(BITS) $(XMITINT)
X
XOBJECTS        = term.obj handler.obj com.obj
X
Xterm:  term.exe
X
Xterm.exe:      $(OBJECTS)
X       $(CC) $(OBJECTS)
X
Xhandler.obj:   com.h
Xcom.obj:       com.h
SHAR_EOF
fi
if test -f 'com.h'
then
        echo shar: "will not over-write existing file 'com.h'"
else
sed 's/^X//' << \SHAR_EOF > 'com.h'
X/*
X *     com.h - com port utility routines
X */
X
X/*  port numbers  */
X#define        COM1    0
X#define        COM2    1
X
X/*  funny business for MSC  */
X#ifdef __TURBOC__
X#define INTERRUPT      interrupt
X#else
X#define INTERRUPT      interrupt far
X#define inportb                inp
X#define outportb       outp
X#define getvect                _dos_getvect
X#define setvect                _dos_setvect
X#define disable                _disable
X#define enable         _enable
X#endif
X
X/*  ports and masks  */
X#define        DLH(p)  (p?0x2F9:0x3F9)         /*  Divisor Latch High          */
X#define        DLL(p)  (p?0x2F8:0x3F8)         /*  Divisor Latch Low           */
X#define        IER(p)  (p?0x2F9:0x3F9)         /*  Interrupt Enable Register   */
X#define        IIR(p)  (p?0x2FA:0x3FA)         /*  Interrupt Ident Register    */
X#define        LCR(p)  (p?0x2FB:0x3FB)         /*  Line Control Register       */
X#define        LSR(p)  (p?0x2FD:0x3FD)         /*  Line Status Register        */
X#define        MCR(p)  (p?0x2FC:0x3FC)         /*  Modem Control Register      */
X#define        MSR(p)  (p?0x2FE:0x3FE)         /*  Modem Status Register       */
X#define        RBR(p)  (p?0x2F8:0x3F8)         /*  Receiver Buffer Register    */
X#define        THR(p)  (p?0x2F8:0x3F8)         /*  Transmit Holding Register   */
X#define        IMSK(p) (p?~0x08:~0x10)         /*  interrupt enable mask       */
X#define        VEC(p)  (p?11:12)               /*  interrupt vector number     */
X
X/*  IER bits  */
X#define        MSI     0x08                    /*  Modem Status Int            */
X#define        RLSI    0x04                    /*  Rx Line Status Int          */
X#define        THREI   0x02                    /*  THRE Int                    */
X#define        DAI     0x01                    /*  Data Available Int          */
X/*  IIR bits  */
X#define        IID1    0x04                    /*  Int ID 1                    */
X#define        IID0    0x02                    /*  Int ID 0                    */
X#define        IP      0x01                    /*  Interrupt Pending           */
X/*  LCR bits  */
X#define        DLAB    0x80                    /*  Divisor Latch Access Bit    */
X#define        SB      0x40                    /*  Set Break                   */
X#define        SP      0x20                    /*  Stick Parity                */
X#define        EPS     0x10                    /*  Even Parity Select          */
X#define        PEN     0x08                    /*  Parity Enable               */
X#define        STB     0x04                    /*  Stop Bits                   */
X#define        WLS1    0x02                    /*  Word Length Select 1        */
X#define        WLS0    0x01                    /*  Word Length Select 0        */
X/*  LSR bits  */
X#define        TSRE    0x40                    /*  Tx Shift Register Empty     */
X#define        THRE    0x20                    /*  Tx Holding Register Empty   */
X#define        BI      0x10                    /*  Break Interrupt             */
X#define        FE      0x08                    /*  Framing Error               */
X#define        PE      0x04                    /*  Parity Error                */
X#define        OR      0x02                    /*  Overrun Error               */
X#define        DR      0x01                    /*  Data Ready                  */
X/*  MCR bits  */
X#define        LOOP    0x10                    /*  Loopback                    */
X#define        OUT1    0x08                    /*  Output 1                    */
X#define        OUT2    0x04                    /*  Output 2                    */
X#define        RTS     0x02                    /*  Request To Send             */
X#define        DTR     0x01                    /*  Data Terminal Ready         */
X/*  MSR bits  */
X#define        RLSD    0x80                    /*  Rx Line Signal Detect       */
X#define        RI      0x40                    /*  Ring Indicator              */
X#define        DSR     0x20                    /*  Data Set Ready              */
X#define        CTS     0x10                    /*  CTS line                    */
X#define        DRLSD   0x08                    /*  Delta Rx Line Signal Detect */
X#define        TERI    0x04                    /*  Trailing Edge Ring Indicator*/
X#define        DDSR    0x02                    /*  Delta Data Set Ready        */
X#define        DCTS    0x01                    /*  Delta Clear To Send         */
X
X/*  interrupt values  */
X#define        RSTATUS 0                       /*  Receiver Line Status        */
X#define        RDR     IID1                    /*  Received Data Ready         */
X#define        TXR     IID0                    /*  Transmit Ready              */
X#define        MSTATUS (IID0+IID1)             /*  Modem Status                */
X
X/*  8259 ports/flags  */
X#define        EOI     0x20                    /*  End of Interrupt            */
X#define        INTA00  0x20                    /*  interrupt controller        */
X#define        INTA01  0x21                    /*  interrupt mask              */
X
X/*  baud rate divisors  */
X#define        Baud50          2304
X#define        Baud75          1536
X#define        Baud110         1047
X#define        Baud134_5       857
X#define        Baud150         768
X#define        Baud300         384
X#define        Baud600         192
X#define        Baud1200        96
X#define        Baud1800        64
X#define        Baud2000        58
X#define        Baud2400        48
X#define        Baud3600        32
X#define        Baud4800        24
X#define        Baud7200        16
X#define        Baud9600        12
X#define        Baud19200       6
X#define        Baud38400       3
X
X#define        ParityNone      0       /*  pick one  */
X#define        ParityOdd       PEN
X#define        ParityEven      (PEN+EPS)
X
X#define        Stop1           0       /*  pick one  */
X#define        Stop2           STB     /*  1-1/2 bits if 5 data bits  */
X
X#define        Data5           0       /*  pick one  */
X#define        Data6           WLS0
X#define        Data7           WLS1
X#define        Data8           (WLS0+WLS1)
X
X/*  functions  */
Xextern void ComOpen(int port, int baud, int bits, int ints,
X                                               void (INTERRUPT * handler)());
Xextern void ComClose(int port);
X
X/*  END of com.h  */
SHAR_EOF
fi
if test -f 'com.c'
then
        echo shar: "will not over-write existing file 'com.c'"
else
sed 's/^X//' << \SHAR_EOF > 'com.c'
X/*
X *     com.c - com port utility routines
X */
X
X#include <assert.h>
X#include <dos.h>
X
X#include "com.h"
X
X       /*  place to save original vectors  */
Xstatic void (INTERRUPT *oldvec[2])();
X
X/*
X-*     ComOpen - open and init com port
X */
X       void
XComOpen(
X       int port,                       /*  port number, 0 or 1  */
X       int baud,                       /*  baud rate divisor  */
X       int bits,                       /*  other parameter bits  */
X       int ints,                       /*  interrupts to handle  */
X       void (INTERRUPT *handler)()     /*  interrupt handler  */
X       )
X{
X       assert((port == COM1) || (port == COM2));
X       /*  set baud rate  */
X       outportb(LCR(port), DLAB);
X       outportb(DLL(port), baud);
X       outportb(DLH(port), baud >> 8);
X       /*  set other parameters, clear DLAB  */
X       outportb(LCR(port), bits & ~DLAB);
X       /*  no interrupts through this  */
X       outportb(IER(port), 0);
X       /*  save original interrupt vector  */
X       oldvec[port] = getvect(VEC(port));
X       /*  install new handler  */
X       setvect(VEC(port), handler);
X       /*  clear pending status  */
X       inportb(LSR(port));
X       inportb(RBR(port));
X       inportb(IIR(port));
X       inportb(MSR(port));
X       /*  init MCR with DTR, RTS & OUT1  */
X       outportb(MCR(port), inportb(MCR(port)) | (DTR+RTS+OUT1));
X       /*  set our interrupt mask  */
X       outportb(INTA01, inportb(INTA01) & IMSK(port));
X       /*  select interrupts  */
X       outportb(IER(port), ints);
X}
X
X/*
X-*     ComClose - unhook interrupt handler
X */
X       void
XComClose(int port)
X{
X       assert((port == COM1) || (port == COM2));
X       /*  turn off serial interrupts  */
X       outportb(IER(port), 0);
X       /*  turn off MCR  */
X       outportb(MCR(port), 0);
X       /*  reset interrupt mask  */
X       outportb(INTA01, inportb(INTA01) | ~IMSK(port));
X       /*  restore interrupt handler  */
X       setvect(VEC(port), oldvec[port]);
X}
X
X/*  END of com.c  */
SHAR_EOF
fi
if test -f 'handler.c'
then
        echo shar: "will not over-write existing file 'handler.c'"
else
sed 's/^X//' << \SHAR_EOF > 'handler.c'
X/*
X *     handler.c - sample com port interrupt handler
X *
X *     Demonstration of COM package
X */
X
X#include <conio.h>
X#include <dos.h>
X
X#include "com.h"
X
X#ifdef TXR_INT
X#define        INTS    MSI+RLSI+THREI+DAI
X#else
X#define        INTS    MSI+RLSI+DAI
X#endif
X
X#define        rBufSize        64      /*  receive buffer size         */
X#define        sBufSize        16      /*  send buffer size            */
X
X/*  break flag  */
Xunsigned char Break = 0;       /*  non-zero if break received  */
X
X/*  buffer jazz  */
Xstatic unsigned char rBuf[rBufSize];   /*  receive buffer      */
Xstatic int rBufCnt;
Xstatic int rIn;
Xstatic int rOut;
Xstatic unsigned char sBuf[sBufSize];   /*  send buffer         */
Xstatic int sBufCnt;
Xstatic int sIn;
Xstatic int sOut;
X
X/*
X *     serial interrupt handler
X */
X       static void INTERRUPT
Xhandler()
X{
X       register unsigned input;
X
X       /*  which interrupt ??  */
X       switch ( inportb(IIR(PORT)) & (IID1+IID0) )
X       {
X       case RSTATUS:   /*  Receiver Line Status  */
X               /*  just check for break  */
X               if ( inportb(LSR(PORT)) & BI )
X                       Break++;
X               break;
X
X       case RDR:       /*  Received Data Ready  */
X               /*  drop RTS line  */
X               outportb(MCR(PORT), inportb(MCR(PORT)) & ~RTS);
X               /*  get and buffer byte  */
X               input = inportb(RBR(PORT));
X               if ( rBufCnt < rBufSize )
X               {
X                       rBufCnt++;
X                       rBuf[rIn++] = input;
X                       if ( rIn >= rBufSize )
X                               rIn = 0;
X               }
X               break;
X
X       case TXR:       /*  Transmit Ready  */
X#ifdef TXR_INT
X               /*  anything there ??  */
X               if ( sBufCnt )
X               {
X                       /*  spit one from buffer  */
X                       outportb(THR(PORT), sBuf[sOut++]);
X                       sBufCnt--;
X                       if ( sOut >= sBufSize )
X                               sOut = 0;
X               }
X               else
X#endif
X                       /*  tell them to shut up  */
X                       outportb(IER(PORT), inportb(IER(PORT)) & ~THREI);
X               break;
X
X       case MSTATUS:   /*  Modem Status  */
X               inportb(MSR(PORT));     /*  just clear  */
X               break;
X       }
X
X       /*  signal end-of-interrupt, twice for stacked interrupts  */
X       outportb(INTA00, EOI);
X       outportb(INTA00, EOI);
X
X/*
X *  Bug-fix thanks to Steve Miller (uunet!pictel!miller) and David Kessner
X *  (david@kessner.denver.co.us).  This code solves the problem on some
X *  'budget' serial cards which lock when a byte is received in the
X *  middle of a send.  Unfortunately, it doesn't work everywhere.
X */
X#if 0
X       /*  create interrupt edge  */
X       input = inportb(IER(PORT));
X       outportb(IER(PORT), 0);
X       outportb(IER(PORT), input);
X#endif
X}
X
X/*
X-*     ComInstall - install com port handler
X */
X       void
XComInstall()
X{
X       /*  reset buffers  */
X       rBufCnt = rIn = rOut = 0;
X       sBufCnt = sIn = sOut = 0;
X       /*  open com port  */
X       ComOpen(PORT, BAUD, BITS, INTS, handler);
X}
X
X/*
X-*     ComRelease - release com port
X */
X       void
XComRelease()
X{
X       /*  unhook interrupts  */
X       ComClose(PORT);
X}
X
X/*
X-*     ComGet - get byte from com port
X */
X       int
XComGet()
X{
X       int c;
X
X       /*  just return byte if there  */
X       if ( rBufCnt )
X       {
X               disable();
X               c = rBuf[rOut++];
X               rBufCnt--;
X               if ( rOut >= rBufSize )
X                       rOut = 0;
X               enable();
X               return ( c );
X       }
X
X       /*  raise RTS again  */
X       outportb(MCR(PORT), inportb(MCR(PORT)) | RTS);
X       /*  no byte ready  */
X       return ( -1 );
X}
X
X/*
X-*     ComPut - output byte to com port
X */
X       int
XComPut(unsigned char out)
X{
X#ifdef TXR_INT
X       if ( sBufCnt < sBufSize )
X       {
X               /*  stuff into buffer  */
X               sBufCnt++;
X               sBuf[sIn++] = out;
X               if ( sIn >= sBufSize )
X                       sIn = 0;
X               /*  make sure interrupts enabled  */
X               outportb(IER(PORT), inportb(IER(PORT)) | THREI);
X               /*  good return  */
X               return ( 0 );
X       }
X
X       /*  buffer overflow  */
X       return ( -1 );
X#else
X       unsigned timer = 0xFFFF;
X
X       /*  wait for ready state  */
X       while ( (!(inportb(LSR(PORT)) & THRE) ||
X                !(inportb(MSR(PORT)) & CTS)) &&
X               timer )
X       {
X               timer--;
X       }
X       /*  timeout ??  */
X       if ( !timer )
X               return ( -1 );
X
X       outportb(THR(PORT), out);
X       return ( 0 );
X#endif
X}
X
X/*  END of handler.c  */
SHAR_EOF
fi
if test -f 'term.c'
then
        echo shar: "will not over-write existing file 'term.c'"
else
sed 's/^X//' << \SHAR_EOF > 'term.c'
X/*
X *     term.c - dumb terminal emulator
X *
X *     Demonstration of COM package
X */
X
X#include <conio.h>
X#include <dos.h>
X
X/*
X *     good old main
X */
X       void
Xmain()
X{
X       int c;
X
X       /*  install handler  */
X       ComInstall();
X       /*  loop until Ctrl-X entered  */
X       for ( ;; )
X       {
X               /*  send kbd input if there  */
X               if ( kbhit() )
X               {
X                       c = getch();
X                       /*  exit on Ctrl-X  */
X                       if ( c == 24 )
X                               break;
X                       else if ( ComPut(c) < 0 )
X                               putch(7);       /*  beep  */
X               }
X
X               /*  display host data if there  */
X               if ( (c = ComGet()) >= 0 )
X                       putch(c);
X       }
X       /*  release handler  */ 
X       ComRelease();
X}
X
X/*  END of term.c  */
SHAR_EOF
fi
exit 0
#       End of shell archive

sbustd chbl (25): 




