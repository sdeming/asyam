#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <fcntl.h>
#include <io.h>

#include "async.h"

extern  struct async_portS async_port[NUMBEROFPORTS];
extern  int UART_ports[];

char    *uart_names[] = {
    "NONE",
    "8250",
    "16450",
    "16550",
    "16550A",
};

int     main(int argc, char *argv[])
{
    char    ch;
    int     done = 0;
    char    s[60];
    long    speed;
    int     comport;

    int     fp;
    int     size;
    long    totsize;
    char    buf[1024];

    comport = COM1;
    if (argc > 1) {
        comport = atoi(argv[1])-1;
    }

    speed = 9600L;
    if (argc > 2) {
        speed = atol(argv[2]);
    }

    clrscr();

    printf("##############################################################################\n");
    printf("##                                                                          ##\n");
    printf("## Async Routines for the Async Minded -- DUMB TERMINAL                     ##\n");
    printf("## ===================================                                      ##\n");
    printf("## TERM.EXE <baudrate> <comport>                                            ##\n");
    printf("##                                                                          ##\n");
    printf("## If no baudrate or comport is specified, COM1 at 9600 baud is assumed.    ##\n");
    printf("##                                                                          ##\n");
    printf("##############################################################################\n");
    printf("##                                                                          ##\n");
    printf("## COM PORT: %d                           SPEED: %-6lu                      ##\n", comport+1, speed);
    printf("##     UART: %-6s                        IRQ: %d                           ##\n", uart_names[async_detect_uart(comport)], async_detect_irq(comport));
    printf("##                                                                          ##\n");
    printf("##############################################################################\n");

    open_port(comport, speed, LCR_PNONE, 1, 8);

    while (!done) {

        if (async_gets(comport, s, 59)) {
            printf(s);
        }

        if (kbhit()) {
            ch = getch();
            switch (ch) {
                case 0:
                    switch (getch()) {
                        case 59: //F1
                            set_parity(comport, LCR_PEVEN);
                            set_wordlength(comport, 7);
                            break;
                        case 60: //F2
                            set_parity(comport, LCR_PNONE);
                            set_wordlength(comport, 8);
                            break;
                        case 61: //F3 -- Toggle XONXOFF
                            set_handshaking(comport, HSHAKE_XONXOFF, !async_xonxoff_enabled(comport));
                            printf("XONXOFF: %d\n", async_xonxoff_enabled(comport));
                            break;
                        case 62: //F4
                            set_handshaking(comport, HSHAKE_RTSCTS, !async_rtscts_enabled(comport));
                            printf("RTSCTS: %d\n", async_rtscts_enabled(comport));
                            break;
                        case 63: //F5
                            set_handshaking(comport, HSHAKE_DTRDSR, !async_dtrdsr_enabled(comport));
                            printf("DTRDSR: %d\n", async_dtrdsr_enabled(comport));
                            break;
                        case 64: //F6
                            break;
                        case 65: //F7
                            break;
                        case 66: //F8
                            async_set_DTR(comport, !async_DTR_status(comport));
                            printf("DTR: %d\n", async_DTR_status(comport));
                            break;
                        case 67: //F9

                            fp = open("test", O_RDONLY);

                            do {
                                size = read(fp, buf, 1024);
                                async_putblock(comport, buf, size);
                                totsize+=size;
                            } while (size != 0);

                            printf("%lu bytes dumped.\n", buf);

                            close(fp);

                            break;
                        case 68: //F10
                            done = 1;
                            break;
                    }
                    break;
                default:
                    async_putch(comport, ch);
                    break;
            }
        }
    }

    close_port(comport, 0, 0);

    return (0);
}

