Mike Surikov sent me these functions. The detect_UART function below is
different from the one in the file "The_Serial_Port". Just cut the lines
below, copy them to a file "anyname.c" and compile them. If you specify
a base address at the command line, info for this port is given. (E.g.
"anyname 3f8").
                                                                Chris

-----------------------------------------------------------------------

One more thing: the function "peek" may be missing from your C library
(the Borland Turbo C++ compiler has it). Use this code for the Microsoft
C compiler.

unsigned _cdecl peek(unsigned a, unsigned b)
{
   _asm
      {
      mov es,a
      mov bx,b
      mov ax,es:[bx]
      }
}

-----------------------------------------------------------------------

#include <dos.h>

int    detect_UART(unsigned baseaddr)
{
  // This function returns:
  //   -1 if no UART is installed
  //    0 - 8250
  //    1 - 16450
  //    2 - 16550 w/o SCR
  //    3 - 16550
  //    4 - 16550A w/o SCR
  //    5 - 16550A
  unsigned char x, scr=1;
  // First step: see if the LCR is there
  outp(baseaddr+3,0x1B);
  if( inp(baseaddr+3)!=0x1B ) return -1;
  outp(baseaddr+3,0x03);
  if( inp(baseaddr+3)!=0x03 ) return -1;
  // Next thing to do is look for the scratch register
  outp(baseaddr+7,0x55);
  if( inp(baseaddr+7)!=0x55 ) scr=0;
  outp(baseaddr+7,0xAA);
  if( inp(baseaddr+7)!=0xAA ) scr=0;
  // Then check if there's a FIFO
  outp(baseaddr+2,0x01);
  x=inp(baseaddr+2);
  outp(baseaddr+2,0x00); // Some old-fashioned software relies on this!
  if( !(x&0x80) ) return scr;
  if( !(x&0x40) ) return 2+scr;
  return 4+scr;
}

#include <stdio.h>
void    check_type(unsigned baseaddr)
{
  char  *uart[]={"COM1","COM2","COM3","COM4","UART"};
  char  *type[]={"not installed",
                 "8250","16450",
                 "16550 w/o SCR","16550",
                 "16550A w/o SCR","16550A"};
  int   i;
  for(i=0;i<4;i++)
    if( (unsigned)peek(0x40,2*i)==baseaddr ) break;
  if( baseaddr==0 ) i=4;
  printf(" %s with base address %03X is %-14s\n",
        uart[i], baseaddr, type[1+detect_UART(baseaddr)]);
}

#include <stdlib.h>
void    main(int argc, char *argv[])
{
  int   i;
  printf("UARTs detect routine\n");
  if( argc<2 )
    for(i=0; i<4; i++) {
      register baseaddr=peek(0x40,2*i);
      if( baseaddr!=0 ) check_type((unsigned)baseaddr);
    }
  else
    for(i=1; i<argc; i++) {
      char *end;
      check_type((unsigned)strtol(argv[i],&end,16));
    }
  printf("\n");
}





