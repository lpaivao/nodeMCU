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
		NodeMCU
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
Outro diferencial do NodeMCU é a possibilidade de fazer a programação da placa via OTA (Over The Air), ou seja, através do WiFi você pode enviar os códigos para a placa.

## SBC - Raspberry Pi Zero W

O SBC utilizado foi a Raspberry Pi Zero W.

<div id="image11" style="display: inline_block" align="center">
		<img src="/raspberry.jpg"/><br>
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

## Protocolo de comunicação - UART
