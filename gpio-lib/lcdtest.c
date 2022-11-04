#include <stdio.h>
#include "1602minidrv.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>	//Usado para UART
#include <fcntl.h>	//Usado para UART
#include <termios.h>	//Usado para UART

//#include <time.h>

// COMANDOS DE REQUISIÇÃO
#define SITUACAO_ATUAL_NODE 0x03
#define SOLICITA_ENTRADA_ANALOGICA 0x04
#define SOLICITA_ENTRADA_DIGITAL 0x05
#define ACENDE_LED 0x06

// COMANDOS DE RESPOSTA
#define NODE_COM_PROBLEMA 0x1F
#define NODE_FUNCIONANDO 0x00
#define MEDIDA_ENTRADA_ANALOGICA 0x01
#define ESTADO_ENTRADA_DIGITAL 0x02

// ENDEREÇO DO SENSOR

//#define GPIO_PIN0_ADDRESS 0x28
//#define SENSOR_NUM(i) (GPIO_PIN0_ADDRESS + (i*4))

#define PERIPHS_GPIO_BASEADDR 0x60000300

#define SENSOR_NUM(i) i

//Funcao que envia os dados na UART
void uart_tx(char* tx_string, int uart0_filestream){
	if (uart0_filestream != -1){//Se abriu o arquivo da UART
		int written_bits_length = write(uart0_filestream, tx_string, strlen(tx_string)); //Filestream,mensagem enviada,tamanho da mensagem
		if(written_bits_length == -1){
			printf("uart_tx() nao funcionou\n");
		}
		else
			printf("uart_tx() funcionou e escreveu %i bits\n", written_bits_length);
	}	
	else{
		printf("\nFalha na abertura do arquivo");
	}
}

void uart_rx(int uart0_filestream, unsigned char *respostaNode){
    
	int rx_length; 
    int count = 0;

	int i;

    //----- CHECK FOR ANY RX BYTES -----
	if (uart0_filestream != -1)
	{		
		//Variável que vai receber os bits do buffer RX
		char c;

		//Passa o bit do RX para a variável c, e depois de c para a string de resposta
		for(count = 0; count < sizeof(respostaNode); i++){
			rx_length = read(uart0_filestream, &c, 1);  //Filestream, buffer to store in, number of bytes to read (max)
			respostaNode[i] = c;
			if(c == '\n')
				break;
		}

		if (rx_length < 0)
		{
            printf("An error occured (will occur if there are no bytes)\n");	
		}
		else if (rx_length == 0)
		{
            printf("No data waiting\n");
		}
		else
		{
			//Bytes received
			printf("%i bytes read : %s\n", rx_length, respostaNode);
		}
	}
}

void sendNodeFuncionando(){
	_sendChar('N');
	_sendChar('O');
	_sendChar('D');
	_sendChar('E');
	_sendChar(' ');
	_sendChar('O');
	_sendChar('K');
}

void sendNodeProblema(){
	_sendChar('N');
	_sendChar('O');
	_sendChar('D');
	_sendChar('E');
	_sendChar(' ');
	_sendChar('N');
	_sendChar('O');
	_sendChar('T');
	_sendChar(' ');
	_sendChar('O');
	_sendChar('K');
}

void sendEntradaAnalogica(char *valorAnalogico){
	_sendChar(valorAnalogico[1]);
}

void sendEntradaDigital(char *estado){
	_sendChar(estado[1]);
}

void sendErro(){
	_sendChar('E');
	_sendChar('R');
	_sendChar('R');
	_sendChar('O');
}

//CONFIGURE THE UART
//The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
//	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
//	CSIZE:- CS5, CS6, CS7, CS8
//	CLOCAL - Ignore modem status lines
//	CREAD - Enable receiver
//	IGNPAR = Ignore characters with parity errors
//	ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
//	PARENB - Parity enable
//	PARODD - Odd parity (else even)
void uart_config(int uart0_filestream){
    struct termios options;
	tcgetattr(uart0_filestream, &options);
	options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);
	printf("Uart configurada!\n");
}

void uart_start(int uart0_filestream){

	 //-------------------------
	//----- SETUP USART 0 -----
	//-------------------------
	//At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD (ie the alt0 function) respectively
	uart0_filestream = -1;
	
	//OPEN THE UART
	//The flags (defined in fcntl.h):
	//	Access modes (use 1 of these):
	//	O_RDONLY - Open for reading only.
	//	O_RDWR - Open for reading and writing.
	//	O_WRONLY - Open for writing only.
	//
	//	O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status
	//											if there is no input immediately available (instead of blocking). Likewise, write requests can also return
	//											immediately with a failure status if the output can't be written immediately.
	//
	//	O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.
	
    uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
	if (uart0_filestream == -1)
	{
		//ERROR - CAN'T OPEN SERIAL PORT
		printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
	}
	else{
		printf("Uart Aberta!\n");
		printf("Configurando Uart...\n");
		uart_config(uart0_filestream);
		
	}
		
}


void escolhaDigital(int uart0_filestream, char *respostaNode){

	char *strReqDig;
	strReqDig = (char *) malloc(2);

    int digital;
    printf("Digite a entrada digital desejada:\n");
    printf("0 - Digital 0 (D0)\n");
    printf("1 - Digital 1 (D1)\n");

    scanf("%i", &digital);

    switch (digital)
    {
    case 0:
		strReqDig[0] = ESTADO_ENTRADA_DIGITAL;
		strReqDig[1] = SENSOR_NUM(0);
        uart_tx(strReqDig, uart0_filestream);
		usleep(100000); //delay 0.1 segundos
		uart_rx(uart0_filestream, respostaNode);
		if(respostaNode[0] != NULL || respostaNode[0] != '\0'){
			sendEntradaDigital(respostaNode);
		}
		else
			sendErro();
        break;
    case 1:
		strReqDig[0] = ESTADO_ENTRADA_DIGITAL;
		strReqDig[1] = SENSOR_NUM(1);
        uart_tx(strReqDig, uart0_filestream);
		usleep(100000); //delay 0.1 segundos
		uart_rx(uart0_filestream, respostaNode);
		if(respostaNode[0] != NULL || respostaNode[0] != '\0'){
				sendEntradaDigital(respostaNode);
		}
		else
			sendErro();
        break;
    default:
        printf("Digite uma opcao valida\n");
        break;
    }

	free(strReqDig);
}

void menu(){

	int uart0_filestream;
	uart_start(uart0_filestream);
	printf("Uart iniciada...\n");

	if(uart0_filestream =! -1){
	printf("uart0_filestream passou...\n");

	char *respostaNode;
	respostaNode = (char *) malloc(2);

	char *strReq;
	strReq = (char *) malloc(2);

    int opcao;
    printf("Digite a opcao:\n");
    printf("1 - Situação atual do node:\n");
    printf("2 - Valor da entrada analogica:\n");
    printf("3 - Valor de uma das entradas digitais:\n");
    printf("4 - Acendimento do led:\n");

    scanf("%i", &opcao);

	_clearDisplay();

    switch(opcao){
        case 1:
			strReq[0] = MEDIDA_ENTRADA_ANALOGICA;
            uart_tx(strReq, uart0_filestream);
			usleep(100000); //delay 0.1 segundos
			uart_rx(uart0_filestream, respostaNode);
			if(respostaNode[0] == NODE_FUNCIONANDO){
				sendNodeFuncionando();
			}
			else if(respostaNode[0] == NODE_COM_PROBLEMA){
				sendNodeProblema();
			}
			else
				sendErro();	
            break;
        case 2:
			strReq[0] = MEDIDA_ENTRADA_ANALOGICA;
			strReq[1] = SENSOR_NUM(0);
            uart_tx(strReq, uart0_filestream);
			usleep(100000); //delay 0.1 segundos
			uart_rx(uart0_filestream, respostaNode);
			if(respostaNode[0] != NULL || respostaNode[0] != '\0'){
				sendEntradaAnalogica(respostaNode);
			}
			else
				sendErro();
            break;
        case 3:
            escolhaDigital(uart0_filestream, respostaNode);
            break;
        case 4:
            break;
        default:
            printf("Digite uma opcao valida\n");
            break;
    }
	free(respostaNode);
	free(strReq);
	}
	close(uart0_filestream);
}

int main()
{
    _lcdStartup();
    _clearDisplay();
    _turnOnCursorOn();
    _setMemoryMode();

	while(1){
		menu();
	}

	return 0;
    
}

