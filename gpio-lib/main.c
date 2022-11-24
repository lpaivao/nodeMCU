#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdbool.h>

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
#define PERIPHS_GPIO_BASEADDR 0x60000300
#define SENSOR_NUM(i) i


extern void memory_map(void);
extern void init_lcd(void);
extern void clear_lcd(void);
extern void write_char(unsigned char c);


void escolhaDigital(char *respostaNode);
void menu();
void uart_rx(char *respostaNode);
void uart_tx(char *respostaNode);
void serialFlush ();

// Inicialização do LCD
void lcd(){
	memory_map();
	init_lcd();
	clear_lcd();
}

// Mostrar uma string no LCD
void print_lcd(unsigned char c[]){
        int len = strlen(c);
        
        for(int i=0;i<len;i++){
                write_char(c[i]);
        }
}

int uart0_filestream = -1;

// Funcao que recebe os dados no rx
void uart_rx(char *respostaNode)
{
	if (uart0_filestream != -1)
	{
		read(uart0_filestream, respostaNode, 2); // Filestream, buffer to store in, number of bytes to read (max)
	}
	else
	{
		printf("\nFalha na abertura do arquivo\n");
	}
}

// Funcao que envia os dados pelo tx
void uart_tx(char *tx_string)
{
	if (uart0_filestream != -1)
	{
		write(uart0_filestream, tx_string, 2);
	}
	else
	{
		printf("\nFalha na abertura do arquivo\n");
	}
}

// Limpa entrada da comunicação serial 
void serialFlush ()
{
  tcflush (uart0_filestream, TCIOFLUSH) ;
}

// Configuração da comunicação UART
bool uart_config(){

    uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart0_filestream == -1)
    {
        printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
		return false;
    }

    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

	return true;
}

// Inicio das mensagens para o LCD
void sendNodeFuncionando()
{
	print_lcd("NODE OK");
}
void sendNodeProblema()
{
	print_lcd("NODE NOT OK");
}
void sendEntradaAnalogica(char *valorAnalogico)
{
	char buffer[20];
	sprintf(buffer, "%d", *valorAnalogico);
	print_lcd("Analogico: ");
	print_lcd(buffer);
}
void sendEntradaDigital(char *estado)
{
	print_lcd("Digital: ");
	write_char(*estado + '0');
}
void sendErro()
{
	print_lcd("ERRO");
}
// Fim das mensagens para o LCD

// Submenu para escolha do sensor digital
// D0 = 16
// D1 = 5
void escolhaDigital(char *respostaNode)
{
	char requisicaoRaspDigital[2];
	int digital;
	printf("Digite a entrada digital desejada:\n");
	printf("0 - Digital 0 (D0)\n");
	printf("1 - Digital 1 (D1)\n");
	scanf("%i", &digital);
	switch (digital)
	{
	case 0:
		serialFlush();
		requisicaoRaspDigital[0] = SOLICITA_ENTRADA_DIGITAL;
		requisicaoRaspDigital[1] = SENSOR_NUM(0);
		uart_tx(requisicaoRaspDigital);

		usleep(100000); // delay 0.1 segundos

		uart_rx(respostaNode);

		if ((respostaNode[0] == ESTADO_ENTRADA_DIGITAL))
		{
			sendEntradaDigital(&respostaNode[1]);
		}
		else
			sendErro();
		break;
	case 1:
		serialFlush();
		requisicaoRaspDigital[0] = SOLICITA_ENTRADA_DIGITAL;
		requisicaoRaspDigital[1] = SENSOR_NUM(1);

		uart_tx(requisicaoRaspDigital);

		usleep(100000); // delay 0.1 segundos

		uart_rx(respostaNode);

		if ((respostaNode[0] == ESTADO_ENTRADA_DIGITAL))
		{
			sendEntradaDigital(&respostaNode[1]);
		}
		else
			sendErro();
		break;
	default:
		printf("Digite uma opcao valida\n");
		break;
	}
}

// Menu principal
void menu()
{
	
	if (uart0_filestream != -1)
	{	
		int opcao;
		bool estado_led = false;

		do{

			char respostaNode[2]; //Recebe resposta do Node
			char requisicaoRasp[2];
			
			printf("Digite a opcao:\n");
			printf("1 - Situação atual do node\n");
			printf("2 - Valor da entrada analogica\n");
			printf("3 - Valor de uma das entradas digitais\n");
			printf("4 - Muda estado do led\n");
			printf("5 - Sair\n");
			scanf("%i", &opcao);
		
        	clear_lcd(); //clear display

		switch (opcao)
		{
		// Opcao de situacao do NodeMCU
		case 1:
			serialFlush();
			requisicaoRasp[0] = SITUACAO_ATUAL_NODE;
			requisicaoRasp[1] = 'G';
			uart_tx(requisicaoRasp);
			usleep(100000); // delay 0.1 segundos
			uart_rx(respostaNode);
			if (respostaNode[0] == NODE_FUNCIONANDO)
			{
				sendNodeFuncionando();
			}
			else if (respostaNode[0] == NODE_COM_PROBLEMA)
			{
				sendNodeProblema();
			}
			else
				sendErro();
			break;
		// Opcao de medida da entrada analogica
		case 2:
			serialFlush();
			requisicaoRasp[0] = SOLICITA_ENTRADA_ANALOGICA;
			requisicaoRasp[1] = SENSOR_NUM(0);

			uart_tx(requisicaoRasp);
			usleep(100000); // delay 0.1 segundos
			uart_rx(respostaNode);

			if (respostaNode[0] == MEDIDA_ENTRADA_ANALOGICA)
			{	
				sendEntradaAnalogica(&respostaNode[1]);
			}
			else
				sendErro();
			break;
		// Opcao de medida da entrada digital, que leva a um submenu
		case 3:
			serialFlush();
			escolhaDigital(respostaNode);
			break;
		// Opcao de toggle do led da NodeMCU
		case 4:
			serialFlush();
			requisicaoRasp[0] = ACENDE_LED;
			requisicaoRasp[1] = estado_led;			
			uart_tx(requisicaoRasp);
			estado_led?print_lcd("Led Desligado"):print_lcd("Led Ligado");
			estado_led = !estado_led;
			break;
		case 5:
			break;
		default:
			printf("Digite uma opcao valida\n");
			break;
		}
		fflush(stdin);
		} while(opcao != 5);
	}

}
int main()
{   
	lcd();
    if(uart_config() != false)
		menu(uart0_filestream);	

	close(uart0_filestream);
	return 0;
}
