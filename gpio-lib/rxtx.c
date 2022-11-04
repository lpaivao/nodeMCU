#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include "../basicOTA/commands.h"

#ifdef __TESTING__
#define ADDR_0 '0'
#define ADDR_1 '1'
#else
#define ADDR_0 0
#define ADDR_1 1
#endif

int main(int argc, char const *argv[])
{
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
    tty.c_cflag |= CS8; // 8 bits per byte

    tty.c_cflag &= ~CRTSCTS; // disabel HW flow control

    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl

    // cfsetispeed(&tty, B9600);
    // cfsetospeed(&tty, B9600);
    // cfsetspeed(&tty, B9600);

    char command[3][2] = {
        {READ_DIGITAL, ADDR_0},
        {READ_DIGITAL, ADDR_1},
        {READ_ANALOG, ADDR_0}};

    while (1)
    {
        for (int i = 0; i < 3; i++)
        {
            write(serial_port, command[i], sizeof(command[i]));
            usleep(10);
        }
    }

    // Save tty settings, also checking for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0)
    {
        printf("[ERROR] %i from tcsetattr: %s\n", errno, strerror(errno));
    }

    return 0;
}
