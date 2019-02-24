album:album.o JPG.o LCD.o BMP.o
	arm-linux-gcc -o album album.o JPG.o LCD.o BMP.o -I../lib/jpg/libjpeg -L../lib/jpg/libjpeg -ljpeg -lpthread
	rm album.o JPG.o LCD.o BMP.o
album.o:album.c
	arm-linux-gcc -c album.c
JPG.o: ../lib/jpg/JPG.c
	arm-linux-gcc -c ../lib/jpg/JPG.c -I../lib/jpg/libjpeg -L../lib/jpg/libjpeg -ljpeg
LCD.o: ../lib/lcd/LCD.c
	arm-linux-gcc -c ../lib/lcd/LCD.c
BMP.o: ../lib/bmp/BMP.c
	arm-linux-gcc -c ../lib/bmp/BMP.c
