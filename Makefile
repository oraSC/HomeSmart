desktop:desktop.o desktop2.o JPG.o LCD.o BMP.o album.o music.o garage.o camera.o chat.o chat2.o font.o  mysocket.o serial.o cJSON.o 
	arm-linux-gcc -o desktop desktop.o JPG.o LCD.o BMP.o album.o music.o  garage.o camera.o chat.o mysocket.o serial.o font.o cJSON.o -I./lib/jpg/libjpeg -L./lib/camera/ -L./lib/jpg/libjpeg -lapi_v4l2_arm1 -ljpeg -lpthread -lm
	arm-linux-gcc -o desktop2 desktop2.o JPG.o LCD.o BMP.o album.o music.o  garage.o camera.o chat2.o mysocket.o serial.o font.o cJSON.o -I./lib/jpg/libjpeg -L./lib/camera/ -L./lib/jpg/libjpeg -lapi_v4l2_arm1 -ljpeg -lpthread -lm
	
	rm desktop.o desktop2.o JPG.o LCD.o BMP.o album.o mysocket.o music.o garage.o camera.o chat.o chat2.o font.o serial.o cJSON.o
	cp desktop desktop2 /mnt/hgfs/share/HomeSmart/
desktop.o:desktop.c
	arm-linux-gcc -c desktop.c
desktop2.o:desktop2.c
	arm-linux-gcc -c desktop2.c
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
chat.o:./app/chat.c
	arm-linux-gcc -c ./app/chat.c
chat2.o:./app/chat2.c
	arm-linux-gcc -c ./app/chat2.c
mysocket.o:./lib/socket/mysocket.c
	arm-linux-gcc -c ./lib/socket/mysocket.c
serial.o:./lib/serial/serial.c
	arm-linux-gcc -c ./lib/serial/serial.c
font.o:./lib/font/font.c
	arm-linux-gcc -c ./lib/font/font.c
cJSON.o:./lib/cJSON/cJSON.c
	arm-linux-gcc -c ./lib/cJSON/cJSON.c
