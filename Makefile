default:
	-gcc -o vplanet *.c -lm &> makelog

debug:
	-gcc -g -o vplanet *.c -lm &> makelog
