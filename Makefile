desktop:desktop.o JPG.o LCD.o BMP.o album.o music.o garage.o camera.o font.o  mysocket.o serial.o 
	arm-linux-gcc -o desktop desktop.o JPG.o LCD.o BMP.o album.o music.o  garage.o camera.o mysocket.o serial.o font.o -I./lib/jpg/libjpeg -L./lib/camera/ -L./lib/jpg/libjpeg -lapi_v4l2_arm1 -ljpeg -lpthread
	rm desktop.o JPG.o LCD.o BMP.o album.o mysocket.o music.o garage.o camera.o font.o serial.o
	cp desktop /mnt/hgfs/share/HomeSmart/
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
garage.o:./app/garage.c
	arm-linux-gcc -c ./app/garage.c
camera.o:./app/camera.c
	arm-linux-gcc -c ./app/camera.c 
mysocket.o:./lib/socket/mysocket.c
	arm-linux-gcc -c ./lib/socket/mysocket.c
serial.o:./lib/serial/serial.c
	arm-linux-gcc -c ./lib/serial/serial.c
font.o:./lib/font/font.c
	arm-linux-gcc -c ./lib/font/font.c
