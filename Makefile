desktop:desktop.o JPG.o LCD.o BMP.o album.o music.o mysocket.o
	arm-linux-gcc -o desktop desktop.o JPG.o LCD.o BMP.o album.o music.o mysocket.o -I./lib/jpg/libjpeg -L./lib/jpg/libjpeg -ljpeg -lpthread
	rm desktop.o JPG.o LCD.o BMP.o album.o mysocket.o music.o
desktop.o:desktop.c
	arm-linux-gcc -c desktop.c
JPG.o:./lib/jpg/JPG.c
	arm-linux-gcc -c ./lib/jpg/JPG.c -I./lib/jpg/libjpeg -L./lib/jpg/libjpeg -ljpeg
LCD.o:./lib/lcd/LCD.c
	arm-linux-gcc -c ./lib/lcd/LCD.c
BMP.o:./lib/bmp/BMP.c
	arm-linux-gcc -c ./lib/bmp/BMP.c
album.o:./app/album.c
	arm-linux-gcc -c ./app/album.c
music.o:./app/music.c
	arm-linux-gcc -c ./app/music.c
mysocket.o:./lib/socket/mysocket.c
	arm-linux-gcc -c ./lib/socket/mysocket.c
