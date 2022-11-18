# Makefile para biblioteca de interface com LCD 1602

# Opções de compilador e montador
COMPOPTS = -Wall
COMPOUTPUT = lcdtest
ASPIZERO = -mcpu=arm1176jz-s --defsym .pimodel=0
ASPIDOIS = -mcpu=cortex-a7 --defsym .pimodel=2


# all: lcdtest clean

BASEGCC=gcc -o $@ $+
BASEAS=as -o $@ $<

LIBOBJS = lcdInit.o map.o lcdWrite.o clear.o


# LCD
# Para RaspberryPi 2
# raspdois: lcdtest2 clean
lcdtest2: lcdtest2.o 1602minidrv.o
	gcc $(COMPOPTS) -o $(COMPOUTPUT) $+
lcdtest2.o: rxtx.c
	gcc -g -c -o $@ $<
1602minidrv.o: 1602minidrv.s
	as -o $@ $< $(ASPIDOIS)

%.o : %.s
	$(BASEAS)
libdisplay.a: $(LIBOBJS)
	ar -cvq $@ $<
rxtx: rxrx.c libdisplay.a
	$(BASEGCC)
raspdois: rxtx clean


# Para RaspberryPi Zero
raspzero: lcdtestZ clean
lcdtestZ: lcdtestZ.o 1602minidrvZ.o
	gcc $(COMPOPTS) -o $(COMPOUTPUT) $+
lcdtestZ.o: rxtx.c 
	gcc -g -c -o $@ $<
1602minidrvZ.o: 1602minidrv.s
	as -o $@ $< $(ASPIZERO)

clean:
	rm -vf *.o

SERIAL_PORT=/dev/ttyAMA0

run: lcdtest
	sudo ./$< $(SERIAL_PORT)