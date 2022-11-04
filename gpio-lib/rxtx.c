#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include "1602minidrv.h"
#include "../basicOTA/commands.h"

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
    // Inicialização do driver de LCD
    _lcdStartup();
    _clearDisplay();
    _turnOnCursorOff();
    _setMemoryMode();

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

    // cfsetspeed(&tty, B9600);

    char command[3][2] = {
        {READ_DIGITAL, ADDR_0},
        {READ_DIGITAL, ADDR_1},
        {READ_ANALOG, ADDR_0}};

    int rx_length = 0;
    char *respostaNode = (char *)malloc(2 * sizeof(char));
    while (1)
    {
        for (int i = 0; i < 3; i++)
        {
            write(serial_port, command[i], sizeof(command[i]));
            usleep(SLEEP_TIME_U);
        }
        rx_length = read(serial_port, respostaNode, 2);

        if (rx_length == 2)
        {
            _printStr("RX OK");
        }

        sleep(SLEEP_TIME_S);
        _clearDisplay();
    }

    // Save tty settings, also checking for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0)
    {
        printf("[ERROR] %i from tcsetattr: %s\n", errno, strerror(errno));
    }

    return 0;
}
