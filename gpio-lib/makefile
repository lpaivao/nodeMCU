# Makefile para biblioteca de interface com LCD 1602

# Opções de compilador e montador
COMPOPTS = -Wall
COMPOUTPUT = lcdtest
ASPIZERO = -mcpu=arm1176jz-s --defsym .pimodel=0
ASPIDOIS = -mcpu=cortex-a7 --defsym .pimodel=2


# all: lcdtest clean

BASEGCC=gcc -o $@ $+
BASEAS=as -o $@ $<

# LCD
# Para RaspberryPi 2
raspdois: lcdtest2 clean
lcdtest2: lcdtest2.o 1602minidrv.o
	gcc $(COMPOPTS) -o $(COMPOUTPUT) $+
lcdtest2.o: lcdtest.s
	as -o $@ $< $(ASPIDOIS)
1602minidrv.o: 1602minidrv.s
	as -o $@ $< $(ASPIDOIS)


# Para RaspberryPi Zero
raspzero: lcdtestZ clean
lcdtestZ: lcdtestZ.o 1602minidrvZ.o
	gcc $(COMPOPTS) -o $(COMPOUTPUT) $+
lcdtestZ.o: lcdtest.s 
	as -o $@ $< $(ASPIZERO)
1602minidrvZ.o: 1602minidrv.s
	as -o $@ $< $(ASPIZERO)


# Teste em C
cteste: cteste.o 1602minidrv.o
	gcc -o cteste cteste.o 1602minidrv.o
cteste.o: lcdtest.c 1602minidrv.h
	gcc -g -c -Wall -o $@ lcdtest.c


mteste: teste clean
teste: teste.c teste.o
	$(BASEGCC)
teste.o: teste.s
	$(BASEAS)

# Input

intest: inputtest clean
inputtest: inputtest.o
	gcc -o $@ $+
inputtest.o: inputtest.s
	as -o $@ $<

clean:
	rm -vf *.o