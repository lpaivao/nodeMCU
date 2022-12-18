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
O NodeMCU é uma plataforma open source da família ESP8266 criado para ser utilizado no desenvolvimento de projetos IoT. O *NodeMCU* já vem com um conversor USB serial integrado.

Esta plataforma é composta basicamente por um chip controlador (ESP8266 ESP-12E), uma porta micro USB para alimentação e programação, conversor USB serial integrado e já possui WiFi nativo.

<div id="image11" style="display: inline_block" align="center">
		<img src="/imagens/nodemcu.jpg"/><br>
		<p>
		NodeMCU
		</p>
	</div>
  
- [ESP8266 Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp8266-technical_reference_en.pdf)
- [ESP8266 Datasheet](https://www.espressif.com/sites/default/files/documentation/0a-esp8266ex_datasheet_en.pdf) 

### Especificações
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

### Pinos Utilizados


| Pino  	| Modo | Correspondente	|
| ------------- | ------------- | ------------- |
| GPIO 16  	| Input |  Botão 0 (D0)	|
| GPIO 05  	| Input |  Botão 1 (D1)	|
| A0  	| Input |  Potenciômetro|
| TX0  	| Output |  RX0 (SBC)	|
| RX0  	| Input |  TX0 (SBC)	|

### OTA (Over the air)
Através do NodeMCU há a possibilidade de fazer a programação da placa via OTA (Over The Air), ou seja, através do WiFi pode se enviar os códigos para a placa. Inclusive, o código do problema possui configurações para seu uso no LARSID - UEFS, com IP fixo para a rede INTELBRAS.

## SBC - Raspberry Pi Zero W

O SBC utilizado para este problema foi a Raspberry Pi Zero W.

<div id="image11" style="display: inline_block" align="center">
		<img src="/imagens/raspberry.jpg"/><br>
		<p>
		Raspberry Pi Zero W
		</p>
	</div>


### Especificações
- [Documentação Raspberry](https://www.raspberrypi.com/documentation/)
- [Documentação CPU Broadcom BCM2835 SOC](https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf)
- ARM1176JZF-S core type
- Single-core
- Clock Speed 1 GHz
- RAM 512 MB
- GPIO 40-Pins

### Pinos Utilizados


| Pino  	| Modo | Correspondente	|
| ------------- | ------------- | ------------- |
| GPIO 12  	| Output |  D4 (LCD)	|
| GPIO 16  	| Output |  D5 (LCD)	|
| GPIO 20  	| Output |  D6 (LCD)	|
| GPIO 21  	| Output |  D7 (LCD)	|
| GPIO 01 	| Output |  EN (LCD)	|
| GPIO 25  	| Output |  RS (LCD)	|
| TX0  	| Output |  RX (Node)	|
| RX0  	| Input |  TX (Node)	|

## Protocolo de comunicação - UART
O SBC precisa se comunicar com o NodeMCU através de um protocoloco de comunicação chamado UART (Universal Asynchronous Receiver / Transmitter).  O UART é muito simples e utiliza somente dois fios entre o transmissor e o receptor para transmitir e receber em ambas as direções.

Uma das grandes vantagens do UART é que ele é assíncrono – o transmissor e o receptor não compartilham um sinal de clock comum. Apesar disso simplificar significativamente o protocolo, isso também impõe alguns requisitos para o transmissor e o receptor. Uma vez que eles não compartilham um sinal de clock comum, ambas as extremidades devem transmitir ao mesmo tempo e em velocidade predefinida para poder ter a mesma temporização de bits. Além de ter a mesma taxa de bauds, ambos os lados de uma conexão UART também têm que usar a mesma estrutura de frames e parâmetros. Portanto, para este problema, as configurações da UART de ambos os dispositivos foram definidas de maneira igual. Essas configurações estão descritas na tabela abaixo:


### Configuração de parâmetros

| Parâmetro  	| Valor |
| ------------- | ------------- |
| Baud Rate  | 115200 |
| Tamanho do dado  | 8 Bits |
| Bits de paridade  | Não possui |
| Bits de parada  | 1 bit |

### Pinos RX/TX

O pino TX da Raspberry envia dados para o pino RX do NodeMCU, que recebe esses dados. De maneira análoga, o pino TX do NodeMCU envia dados para o pino RX da Raspberry.

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

## Produto Desenvolvido - Protocolo de comunicação sobre UART 
O Protocolo de comunicação desenvolvido procurou atender às restrições do problema usando as seguintes regras:
1. As mensagens de requisições sempre têm 2 bytes apenas.
2. As mensagens de resposta têm 2 bytes, exceto em caso de múltiplas amostragens (histórico).
3. Os dados de uma amostragem tem apenas 1 byte em um determinado instante.

As mensagens de requisição seguem o padrão: comando, endereço; para solicitações de medidas digitais ou analógicas, mas para o caso do controle do LED, o padrão é comando no byte 1 e estado (ligado ou desligado) para o byte 2, e ainda para o caso de solicitação de status do nodeMCU, o primeiro byte é o comando e o segundo é obrigatoriamente lido mas ignorado. A figura a seguir demonstra uma solicitação de leitura digital e uma solicitação para ligar o led.

![Exemplo de mensagens](/imagens/protocolocomando.png)


O nodeMCU contém um ADC com resolução de 10bits, portando os valores medidos podem estar numa faixa de 0 a 1023 (em decimal), portanto é necessário uma diminuição de resolução para transimssão, assumindo a perda de informação para tanto, isso é realizado dentro do nodeMCU através de uma função que mapeia os valores de 10bits para 8bits (1byte), conformando o valor para o protocolo, como ilustrado na figura a seguir.

![Fluxo de resposta de medida analógica](/imagens/medidaanalogica.png)



## Produto Desenvolvido (SBC)

O código da raspberry foi totalmente desenvolvido em linguagem C. Foi utilizada a biblioteca [Termios](https://pubs.opengroup.org/onlinepubs/7908799/xsh/termios.h.html) (termios.h) para a utilização da comunicação UART, que contém as definições usadas pela interface do terminal de entrada/saída.

- Funções principais e suas funcionalidades

| Função 	| O que faz |
| ------------- | ------------- |
| open()  |  Abre o arquivo da porta UART (*syscall*)|
| close()  |  Fecha o arquivo da porta UART (*syscall*)|
| write()  | Envia os bytes para o buffer TX (*syscall*)|
| read()   | Lê os bytes do buffer RX (*syscall*)|
| uart_tx()  | Envia os bytes para o buffer TX; utiliza write() |
| uart_rx()   | Lê os bytes do buffer RX; utiliza read() |
| usleep()  |  Delay em nanosegundos|
| menu()  | Menu que fica em loop com as opções para o usuário|

- Funcões do display LCD

| Função 	| O que faz |
| ------------- | ------------- |
| memory_map()  | Faz o mapeamento de memória |
| init_lcd()  | Inicializa o display |
| clear_lcd()  | Limpa o display |
| printLcd()  | Envia uma string ao display |

Para inicializar as funcões do display, deve se chamar primeiro as seguintes funçõs:

```c
memory_map();
init_lcd();
clear_lcd();
```

### Makefile

- Raspberry Pi Zero W

```console

make
sudo ./main

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

### Mapeamento de sensores

O mapeamento de endereços de sensores utilizado no protocolo de comunicação do problema foi feito através da lógica de que o endereço solicitado no protocolo é, na prática, o índice do vetor que contém as definições dos sensores no nodeMCU, ou seja, os sensores estão organizados em um vetor (array) no nodeMCU, e os índices desse vetor (que não representam os endereços de memória física) são interpretados como endereço para o protocolo de comunicação, por exemplo: o sensor digital de nome D2 está posicionado no índice 2 do vetor de sensores digitais, portanto a solicitação terá o 2 como endereço do comando de medidas digitais.

A figura a seguir demonstra como o mapeamento funciona.

![Mapeamento de sensores](/imagens/mapsensores.png)

### Representação dos sensores

Os sensores e o vetor de sensores na verdade são duas estruturas, a primeira descreve o sensor através de seu tipo (Digital ou Analógico), qual o pino a que esse sensor está associado e qual a função que executa a sua leitura.

Esses sensores estão organizados em duas estruturas, uma para os sensores digitais, outra para sensores analógicos, que contém a quantidade de sensores ativos naquele nodeMCU, qual o tipo desses sensores e o vetor em si.

A organização em estruturas e o uso de ponteiros para funções de leitura dão ao código uma grande flexibilidade permitindo implementação de variadas formas de comunicação com os sensores e a abstração desses detalhes para o protocolo de comunicação com o SBC.

A figura a seguir descreve as estruturas:

![Estruturas de sensores e mapas](/imagens/structs.png)

## Testes e Simulações

### Testes do menu
#### Case 1: Funcionamento do Node [OK]
Retorna o estado do node adequadamente.
#### Case 2: Medida da entrada analógica [OK]
Mostra perfeitamente os valores num intervalo de 0 a 254.
#### Case 3: Medida da entrada digital [!!!]
- D0: O valor fica fixo em 1, independente do botão ser pressionado ou não.
- D1: O valor altera perfeitamente de acordo com o pressionamento do botão .
#### Case 4: Acionamento do led [OK]
Foi implementada uma função de toggle que altera o estado do led e funciona adequadamente.
