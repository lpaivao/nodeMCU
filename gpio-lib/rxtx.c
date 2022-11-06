#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include "1602minidrv.h"
#include "../basicOTA/commands.h"

#define PC_MODE

#ifdef __TESTING__
#define ADDR_0 '0'
#define ADDR_1 '1'
#else
#define ADDR_0 0
#define ADDR_1 1
#endif

#define SLEEP_TIME_S 1
#define SLEEP_TIME_U 10

int main(int argc, char const *argv[])
{

    printf("RxTx Test program\n");
#ifndef PC_MODE
    // Inicialização do driver de LCD
    _lcdStartup();
    _clearDisplay();
    _turnOnCursorOff();
    _setMemoryMode();
#endif
    // Configurações de porta serial (usando o driver padrão so SO)
    int serial_port = open(argv[1], O_RDWR);
    if (serial_port < 0)
    {
        printf("[ERROR] %i from open: %s\n", errno, strerror(errno));
    }
    struct termios tty;
    if (tcgetattr(serial_port, &tty) != 0)
    {
        printf("[ERROR] %i from tcgetattr: %s\n", errno, strerror(errno));
    }

    struct termios
    {
        tcflag_t c_iflag;
        tcflag_t c_oflag;
        tcflag_t c_cflag;
        tcflag_t c_lflag;
        cc_t c_line;
        cc_t c_cc[NCCS];
    };
    tty.c_cflag &= ~PARENB; // disable parity bit
    tty.c_cflag &= ~CSTOPB; // one stop bit
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;                     // 8 bits per byte
    tty.c_cflag &= ~CRTSCTS;                // disabel HW flow control
    tty.c_cflag |= CREAD | CLOCAL;          // Turn on READ & ignore ctrl lines (CLOCAL = 1)
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl

    cfsetspeed(&tty, B115200);

    char command[3][2] = {
        {READ_DIGITAL, ADDR_0},
        {READ_DIGITAL, ADDR_1},
        {READ_ANALOG, ADDR_0}};

    int rx_length = 0;
    char *respostaNode = (char *)malloc(2 * sizeof(char));

    char resType = 'O';

    // Save tty settings, also checking for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0)
    {
        printf("[ERROR] %i from tcsetattr: %s\n", errno, strerror(errno));
    }

    while (1)
    {
#ifndef PC_MODE
        _clearDisplay();
#endif
        for (int i = 0; i < 3; i++)
        {
            write(serial_port, command[i], sizeof(command[i]));
            usleep(SLEEP_TIME_U);
        }

        while (read(serial_port, (void *)respostaNode, 2) == 2)
        {
            switch (respostaNode[0])
            {
            case ANALOG_READ:
                resType = 'A';
                break;
            case DIGITAL_READ:
                resType = 'D';
                break;
            default:
                break;
            }
#ifdef PC_MODE
            printf("%c %d\n", resType, respostaNode[1]);
#else
            _sendChar(resType);
            _sendChar(respostaNode[1]);
            _sendChar(' ');
#endif
        }
        sleep(SLEEP_TIME_S);
    }

    return 0;
}
