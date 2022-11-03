# MI - Sistemas Digitais (TEC499) - Problema 1

## Grupo
- Sérgio Pugliesi
- Lucas de Paiva
- Hugo Ribeiro

## Proposta - Temporizador
Com o objetivo de promover o aprendizado da linguagem Assembly, suas instruções e a construção de uma biblioteca, foi solicitado
o desenvolvimento de um aplicativo de temporização (timer) na *Raspberry Pi Zero W*, que apresente a contagem num
display LCD. O tempo inicial deverá ser configurado diretamente no código. Além disso,
deverão ser usados 2 botões (push buttons) de controle: um para iniciar/parar a contagem e outro para reiniciar
a partir do tempo definido.

Com a pretensão de desenvolver uma biblioteca para uso futuro em conjunto com um
programa em linguagem C, a função para enviar mensagem para o display deve estar
separada como uma biblioteca (.o), e permitir no mínimo as seguinte operações:
- Limpar display.
- Escrever caractere.
- Posicionar cursor (linha e coluna).

## Produto desenvolvido
O kit de desenvolvimento utilizado apenas disponibilizava a *Raspberry Pi Zero W*. Porém, o produto foi desenvolvido para o funcionamento tanto na *Raspberry Pi Zero W* quanto na *Raspberry Pi 2*. Portanto, no arquivo de *makefile* há os códigos compatíveis para ambos os micro-controladores, sendo documentado no último tópico deste README. 
Esses micro-controladores possuem arquitetura ARMv6, portanto, o código Assembly foi desenvolvido nessa arquitetura correspondente.

Os seguintes requisitos do problema proposto foram cumpridos:
- O tempo inicial é configurado diretamente no código.
- O código está todo em Assembly

A biblioteca possui as funcionalidades:
- Limpar display
- Escrever caractere
- Posicionar cursor (linha e coluna)

#### O seguinte requisito não foi cumprido:
- Utilizar 2 botões de controle. 

Em vez disso, foram utilizados os 3 botões conectados na protoboard.
- B1 - Botão de reset
- B2 - Botão de pause
- B3 - Botão de continue

## Funcionamento
- O temporizador faz a contagem em segundos no display num intervalo de 00 (tempo inicial) a 99 (tempo final). Após o último dígito o temporizador reinicia voltando para o 00 e retomando a contagem.
- Os botões de pause/continue funcionam, porém precisam ser apertados por ao menos 1 segundo antes de serem soltos para o funcionamento adequado.
- O botão de restart funciona, voltando o contador ao tempo inicial.

## Softwares Utilizados
- Visual Code Studio: Editor de código-fonte utilizado como ambiente de desenvolvimento, no qual foi usado juntamente com a extensão *Arm Assembly* v1.7.4 do autor *dan-c-underwood*, disponibilizada pelo próprio marketplace do software.

## Recursos Utilizados
<div id="image11" style="display: inline_block" align="center">
		<img src="/protoboard.jpg"/><br>
		<p>
		Kit de desenvolvimento
		</p>
	</div>

- Raspberry Pi Zero W
- Display LCD Hitachi HD44780U
- GPIO Extension Board
- 3 Push Buttons

### Raspberry Pi Zero W

#### Arquitetura
Como dito anteriormente, a Raspberry possui arquitetura ARMv6, e código em assembly foi escrito para essa arquitetura.
- [Documentação ARMv6](https://developer.arm.com/documentation/ddi0419/c/)

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

#### Pinos Utilizados

- GPIO 05 - Input -> Push Button 1 (reset)
- GPIO 19 - Input -> Push Button 2 (pause)
- GPIO 26 - Input -> Push Button 3 (continue)

- GPIO 12 - Output -> D4 (LCD)
- GPIO 16 - Output -> D5 (LCD)
- GPIO 20 - Output -> D6 (LCD)
- GPIO 21 - Output -> D7 (LCD)

- GPIO 01 - Output -> E (LCD)
- GPIO 25 - Output -> RS (LCD)

### Display LCD 16x2 Hitachi 44780

<div id="image11" style="display: inline_block" align="center">
		<img src="/display.jpg"/><br>
		<p>
		Display
		</p>
	</div>
  
- [Documentação Hitachi 44780](https://www.sparkfun.com/datasheets/LCD/HD44780.pdf)

#### Pinos Utilizados

- D4, D5, D6, D7
- E (Enable)
- RS


## Tipos de instrução utilizadas
### Instruções aritméticas
- ADD - Addition: Faz uma adição entre dois operandos e coloca o resultado em um registrador de destino 
- SUB - Subtraction: Faz uma subtração entre dois operandos e coloca o resultado em um registrador de destino
- MUL - Multiplication: Faz uma multiplicação entre dois operandos e coloca o resultado em um registrador de destino
### Instruções de transferência de dados
- MOV - Move: Move um endereço/valor para um registrador
- LDR - Load: Carrega dados da memória para um registrador
- STR - Store: Armazena dados de um registrador na memória
### Instruções lógicas
- AND - Faz uma operação lógica do tipo AND entre dois valores.
- ORR - Faz uma operação lógica do tipo OR entre dois valores.
- BIC - Bit Clear: Faz uma operação lógica do tipo AND entre dois valores, o primeiro normal e o segundo valor negado (NOT).
- LSL - Logical Shift Left: "Empurra" os bits pra esquerda com 0's. Equivale a multiplicar por 2^n, com *n* sendo o número de deslocamentos
- LSR - Logical Shift Right: "Empurra" os bits pra direita com 0's. Equivale a dividir por 2^n, com *n* sendo o número de deslocamentos
### Instruções de desvio
- BL  - Branch and Link: O programa faz um desvio nas instruções e guarda o valor do pc (program counter) no lr (link register)
- BX  - Branch Exchange: O programa faz um desvio para o endereço de memória de um registrador alvo.

## Instalação, configuração de ambiente e execução
### Arquivos

- *funcs.s* - Declaração de macros e constantes básicos para o mapeamento de memória e configuração de registradores de GPIO.

- *lcd_instructions.s* - Declaração de comandos e constantes para comunicação com o LCD (HD44780).

- *lcdtest.s* - Programa de teste de comunicação com o LCD, neste teste, ele lê o estado do pino GPIO5, imprime um contador de segundos, zerando ao segurar-se o botão do pino GPIO5.

- *inputest.s* - Programa de teste de entrada, lê o registrador de nível dos pinos e filtra para o pino específico a ser testado.

- *1602minidrv.s* - Biblioteca para o display 16x2 com as operações solicitadas no problema.

- *1602minidrv.h* - Cabeçalho (header) de todas as funções da biblioteca solicitada.

### Build

#### Raspberry Pi Zero W

```console

make raspzero
sudo ./lcdtest

```

#### Raspberry Pi 2

```console

make raspdois
sudo ./lcdtest

```

#### Clean
Remove todos os arquivos *.o* gerados, deixando apenas os executveis.
```console

# Para limpeza
make clean

```

### Conclusão
Os objetivos de aprendizado foram alcançados. A compreensão da linguagem Assembly da arquitetura correspondente (ARMv6) e o uso correto de suas instruções, além da construção da biblioteca solicitada para ser usado posteriormente em linguagem *C* se mostraram tarefas desafiadoras, mas o projeto foi concluído com êxito. O produto desenvolvido atende aos requisitos básicos para o funcionamento de um dispositivo de contagem, podendo ser posteriormente acrescentado melhorias como mensagens para interação do usuário, e a utilização de apenas um botão para as funções de *pause/continue*.
