# MI - Sistemas Digitais (TEC499) - Problema 2

## Grupo
- [Sérgio Pugliesi](github.com/ShinJaca)
- [Lucas de Paiva](github.com/lpaivao)

## Proposta - Projeto de sensor analógico/digital em microcontrolador utilizando comunicação serial
Foi solicitado a implementação de um protótipo de sistema de sensoriamento genérico. Na fase de protótipo do projeto foi utilizada uma plataforma baseada na NodeMCU para confecção das unidades de sensoriamento. Elas são muito flexíveis e versáteis, sendo ideais para a criação de um ecossistema de Internet das Coisas (IoT). Foram utilizado um sensor analógico e dois sensores digitais durante o desenvolvimento, mas o sistema deve ser modular, permitindo a substituição na versão de produção.

O sistema será comandado por um Single Board Computer (SBC), e deve ser capaz de controlar o acionamento de um conjunto variável de sensores, assim como monitorar o seu funcionamento, de forma automatizada. Cada operação de leitura ou monitoramento deve ser representada por um código. Dessa forma, o sistema embarcado na NodeMCU deve ser capaz de interpretá-los e realizá-los de maneira adequada, por meio de uma comunicação UART.

## Diagrama
<div id="image11" style="display: inline_block" align="center">
		<img src="/imagens/diagrama.png"/><br>
		<p>
		Diagrama Geral
		</p>
	</div>
 

<div id="image11" style="display: inline_block" align="center">
		<img src="/imagens/diagramadet.png"/><br>
		<p>
		Diagrama Detalhado
		</p>
	</div>
	
## Kit de Desenvolvimento
<div id="image11" style="display: inline_block" align="center">
		<img src="/imagens/kitdev.jpg"/><br>
		<p>
		Kit de Desenvolvimento
		</p>
	</div>
	
## NodeMCU
O NodeMCU é uma plataforma open source da família ESP8266 criado para ser utilizado no desenvolvimento de projetos IoT. Esta placa foi iniciada em 2014 e é bem interessante, pois ao contrário de alguns módulos desta família que necessitam de um conversor USB serial externo para que haja troca de informações entre computador e o módulo, o NodeMCU já vem com um conversor USB serial integrado.

Esta plataforma é composta basicamente por um chip controlador (ESP8266 ESP-12E), uma porta micro USB para alimentação e programação, conversor USB serial integrado e já possui WiFi nativo.

<div id="image11" style="display: inline_block" align="center">
		<img src="/imagens/nodemcu.jpg"/><br>
		<p>
		NodeMCU
		</p>
	</div>
  
- [ESP8266 Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp8266-technical_reference_en.pdf)
- [ESP8266 Datasheet](https://www.espressif.com/sites/default/files/documentation/0a-esp8266ex_datasheet_en.pdf) 

#### Especificações
- Processador ESP8266-12E
- Arquitetura RISC de 32 bits
- Processador pode operar em 80MHz / 160MHz
- 4Mb de memória flash
- 64Kb para instruções
- 96Kb para dados
- WiFi nativo padrão 802.11b/g/n
- Opera em modo AP, Station ou AP + Station
- Pode ser alimentada com 5VDC através do conector micro USB– Possui 11 pinos digitais
- Possui 1 pino analógico com resolução de 10 bits
- Pinos digitais, exceto o D0 possuem interrupção, PWM, I2C e one wire
- Pinos operam em nível lógico de 3.3V
- Pinos não tolerantes a 5V
- Possui conversor USB Serial integrado
- Programável via USB ou WiFi (OTA)
- Compatível com a IDE do Arduino
- Compatível com módulos e sensores utilizados no Arduino


#### Relação de pinagem NodeMCU
<div id="image11" style="display: inline_block" align="center">
		<img src="/imagens/nodemcupins.jpg"/><br>
		<p>
		NodeMCU - Pinos
		</p>
	</div>

### OTA (Over the air)
Outro diferencial do NodeMCU é a possibilidade de fazer a programação da placa via OTA (Over The Air), ou seja, através do WiFi você pode enviar os códigos para a placa. Inclusive, o código do problema possui configurações para seu uso no LARSID - UEFS, com IP fixo para a rede INTELBRAS.

## SBC - Raspberry Pi Zero W

O SBC utilizado foi a Raspberry Pi Zero W.

<div id="image11" style="display: inline_block" align="center">
		<img src="/imagens/raspberry.jpg"/><br>
		<p>
		Raspberry Pi Zero W
		</p>
	</div>


#### Especificações
- [Documentação Raspberry](https://www.raspberrypi.com/documentation/)
- [Documentação CPU Broadcom BCM2835 SOC](https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf)
- ARM1176JZF-S core type
- Single-core
- Clock Speed 1 GHz
- RAM 512 MB
- GPIO 40-Pins

#### Pinos Utilizados

- GPIO 12 - Output -> D4 (LCD)
- GPIO 16 - Output -> D5 (LCD)
- GPIO 20 - Output -> D6 (LCD)
- GPIO 21 - Output -> D7 (LCD)
- GPIO 01 - Output -> E (LCD)
- GPIO 25 - Output -> RS (LCD)
- TXD0
- RXD0

## Protocolo de comunicação - UART
O SBC precisa se comunicar com o NodeMCU através de um protocoloco de comunicação chamado UART (Universal Asynchronous Receiver / Transmitter).  O UART é muito simples e utiliza somente dois fios entre o transmissor e o receptor para transmitir e receber em ambas as direções.

Uma das grandes vantagens do UART é que ele é assíncrono – o transmissor e o receptor não compartilham um sinal de clock comum. Apesar disso simplificar significativamente o protocolo, isso também impõe alguns requisitos para o transmissor e o receptor. Uma vez que eles não compartilham um, ambas as extremidades devem transmitir ao mesmo tempo e em velocidade predefinida para poder ter a mesma temporização de bits. As taxas de baud mais comuns utilizadas em UART atualmente são 4800, 9600, 19,2 K, 57,6 K e 115,2 K. Além de ter a mesma taxa de bauds, ambos os lados de uma conexão UART também têm que usar a mesma estrutura de frames e parâmetros. Essas configurações estão descritas na tabela abaixo:


### Configuração de parâmetros

| Parâmetro  	| Valor |
| ------------- | ------------- |
| Baud Rate  | 115200 |
| Tamanho do dado  | 8 Bits |
| Bits de paridade  | Não possui |
| Bits de parada  | 1 bit |

### Pinos RX/TX

Dessa forma, o pino TX da Raspberry envia dados para o pino RX do NodeMCU, que recebe esses dados. De maneira análoga, o pino TX do NodeMCU envia dados para o pino RX da Raspberry.

No problema, a comunicação funciona da seguinte forma: a Raspbery envia comandos de requisição para a NodeMCU através de códigos hexadecimais já estabelecidos, e o NodeMCU envia de volta comandos de resposta por um código também hexadecimal. Esses comandos de requisição são descritos nas tabelas abaixo:

- Comandos de Requisição (Raspberry -> NodeMCU)

| Código  	| Descrição do comando |
| ------------- | ------------- |
| 0x03  | Solicita a situação atual do NodeMCU |
| 0x04  | Solicita o valor da entrada analógica |
| 0x05  | Solicita o valor de uma das entradas digitais |
| 0x06  | Acendimento do led da NodeMCU |

- Comandos de Resposta (NodeMCU -> Raspberry)

| Código  	| Descrição do comando |
| ------------- | ------------- |
| 0x1F  | NodeMCU com problema |
| 0x00  | NodeMCU funcionando normalmente |
| 0x01  | Medida da entrada analógica |
| 0x02  | Estado da entrada digital |

## Ambiente de desenvolvimento

### *Arduino IDE*
A IDE foi configurada de forma a instalar e configurar todas as dependências, bibliotecas e funcionalidades para o uso do NodeMCU. Entretanto, nenhuma função própria do Arduino (com exceção das configurações para o OTA) foi utilizada. Todo o código foi desenvolvido em linguagem C.

### *Visual Studio Code* (VSCode)
Esse editor de texto foi utilizado somente para a escrita do código em C, que posteriormente seria introduzido no código do *Arduino IDE* para a compilação no sketch.


## Produto Desenvolvido (SBC)

O código da raspberry foi totalmente desenvolvido em linguagem C. Foi utilizada a biblioteca Termios (termios.h) para a utilização da comunicação UART, que contém as definições usadas pela interface do terminal de entrada/saída.

- Funções e suas funcionalidades

| Função 	| O que faz |
| ------------- | ------------- |
| open()  |  Abre o arquivo da porta UART|
| write()  | Envia os bytes para o buffer TX |
| read()   | Lê os bytes do buffer RX |
| usleep()  |  Delay em nanosegundos|
| menu()  | Menu que fica em loop com as opções para o usuário|

### Makefile

- Raspberry Pi Zero W

```console

make raspzero
sudo ./lcdtest

```

- Raspberry Pi 2

```console

make raspdois
sudo ./lcdtest

```


## Produto Desenvolvido (NodeMCU)

O código do node MCU foi totalmente desenvolvido em linguagem C, sem utilizar as funcionalidades do *ArduinoIDE* (com exceção da parte referente ao OTA). Portanto, utilizou-se da biblioteca do kit *esp8266* da IDE para o desenvolvimento.

- Caminho no Linux para as bibliotecas

```console
/home/(username)/.arduino15/packages/esp8266/hardware/esp8266/3.0.2/
```

- Funções e suas funcionalidades

| Função 	| O que faz |
| ------------- | ------------- |
| setupSensorMaps()  |  Configura o mapa de sensores |
| readDigitalSensor()  |  Retorna o pino do sensor digital selecionado |
| readAnalogSensor()  | Retorna o pino do sensor analógico |
| uart_write_char() | Envia um byte para o buffer TX |
| uart_read()   | Lê os bytes do buffer RX |

## Testes e Simulações

### SBC

### NodeMCU
