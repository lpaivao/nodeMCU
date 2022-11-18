#include <stdio.h>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART
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
#define DESLIGA_LED 0X07
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

// Printar uma string no LCD
void print_lcd(unsigned char c[]){
        int len = strlen(c);
        
        for(int i=0;i<len;i++){
                write_char(c[i]);
        }
}

int uart0_filestream = -1;

void uart_rx(char *respostaNode)
{
	int count = 0;
	//----- CHECK FOR ANY RX BYTES -----
	if (uart0_filestream != -1)
	{
		// Passa o bit do RX para a variável c, e depois de c para a string de resposta
		//  for(count = 0; count < sizeof(respostaNode); count++){
		int rx_length = read(uart0_filestream, respostaNode, 2); // Filestream, buffer to store in, number of bytes to read (max)
		// respostaNode[i] = c;
		// if(c == '\n')
		// break;
		// }
		if (rx_length < 0)
		{
			printf("An error occured\n");
		}
		else if (rx_length == 0)
		{
			printf("No data waiting\n");
		}
		else
		{
			// Bytes received
			printf("%i bytes read : %s\n", rx_length, respostaNode);
		}
	}
}

// Funcao que envia os dados na UART
void uart_tx(char *tx_string)
{
	if (uart0_filestream != -1)
	{																					 // Se abriu o arquivo da UART
		int written_bits_length = write(uart0_filestream, tx_string, 2); // Filestream,mensagem enviada,tamanho da mensagem
		if (written_bits_length == -1)
		{
			printf("uart_tx() nao funcionou\n");
		}
		else
			printf("uart_tx() funcionou e escreveu %i bytes\n", written_bits_length);
	}
	else
	{
		printf("\nFalha na abertura do arquivo\n");
	}
}

// Limpar a entrada da comunicação serial 
void serialFlush ()
{
  tcflush (uart0_filestream, TCIOFLUSH) ;
}

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
	print_lcd(valorAnalogico);
}
void sendEntradaDigital(char *estado)
{
	print_lcd("Digital: ");
	print_lcd(estado);
}
void sendErro()
{
	print_lcd("ERRO");
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
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

	return true;
}

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

		printf("tx a mandar: %s\n", requisicaoRaspDigital);
		printf("tx a mandar: %s\n", requisicaoRaspDigital);
		uart_tx(requisicaoRaspDigital);

		usleep(100000); // delay 0.1 segundos

		uart_rx(respostaNode);
		printf("rx lido: %s\n", respostaNode);

		if ((respostaNode[0] == ESTADO_ENTRADA_DIGITAL))
		{
			sendEntradaDigital(respostaNode[1]);
		}
		else
			sendErro();
		break;
	case 1:
		serialFlush();
		requisicaoRaspDigital[0] = SOLICITA_ENTRADA_DIGITAL;
		requisicaoRaspDigital[1] = SENSOR_NUM(1);

		printf("tx a mandar: %s\n", requisicaoRaspDigital);
		printf("tx a mandar: %s\n", requisicaoRaspDigital);
		uart_tx(requisicaoRaspDigital);

		usleep(100000); // delay 0.1 segundos

		uart_rx(respostaNode);
		printf("rx lido: %s\n", respostaNode);

		if ((respostaNode[0] == ESTADO_ENTRADA_DIGITAL))
		{
			sendEntradaDigital(respostaNode[1]);
		}
		else
			sendErro();
		break;
	default:
		printf("Digite uma opcao valida\n");
		break;
	}
}
void menu()
{
	
	if (uart0_filestream != -1)
	{	
		int opcao;

		do{

			char respostaNode[2]; //Recebe resposta do Node
			char requisicaoRasp[2];
			
			printf("Digite a opcao:\n");
			printf("1 - Situação atual do node\n");
			printf("2 - Valor da entrada analogica\n");
			printf("3 - Valor de uma das entradas digitais\n");
			printf("4 - Acender led\n");
			printf("5 - Desligar led\n");
			printf("6 - Sair\n");
			scanf("%i", &opcao);
		
        	clear_lcd(); //clear display

		switch (opcao)
		{
		case 1:
			serialFlush();
			requisicaoRasp[0] = SITUACAO_ATUAL_NODE;
			requisicaoRasp[1] = 'G';
			printf("tx a mandar: %s\n", requisicaoRasp);
			uart_tx(requisicaoRasp);
			usleep(100000); // delay 0.1 segundos
			uart_rx(respostaNode);
			printf("rx lido: %s\n", respostaNode);
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
		case 2:
			serialFlush();
			requisicaoRasp[0] = SOLICITA_ENTRADA_ANALOGICA;
			requisicaoRasp[1] = SENSOR_NUM(0);

			uart_tx(requisicaoRasp);
			usleep(100000); // delay 0.1 segundos
			uart_rx(respostaNode);

			if (respostaNode[0] == MEDIDA_ENTRADA_ANALOGICA)
			{	
				print_lcd("Analog: ");
				sendEntradaAnalogica(respostaNode[1]);
			}
			else
				sendErro();
			break;
		case 3:
			serialFlush();
			escolhaDigital(respostaNode);
			break;
		case 4:
			serialFlush();
			requisicaoRasp[0] = ACENDE_LED;
			requisicaoRasp[1] = SENSOR_NUM(0);
			uart_tx(requisicaoRasp);
			print_lcd("Led Ligado");
			break;
		case 5:
			serialFlush();
			requisicaoRasp[0] = ACENDE_LED;
			requisicaoRasp[1] = SENSOR_NUM(0);
			uart_tx(requisicaoRasp);
			print_lcd("Led Desligado");
			break;
		case 6:
			break;
		default:
			printf("Digite uma opcao valida\n");
			break;
		}
		} while(opcao != 6);
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
