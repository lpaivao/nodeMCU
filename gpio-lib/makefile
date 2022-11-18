all:main

main:main.c map.o lcd.o
	gcc -o main main.c map.o lcd.o
  
lcd.o:lcd.s
	as -g -o lcd.o lcd.s
  
map.o:map.s
	as -g -o map.o map.s
