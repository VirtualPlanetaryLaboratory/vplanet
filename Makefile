default:
	-gcc -o vplanet *.c -lm

debug:
	-gcc -g -o vplanet *.c -lm -Wno-div-by-zero

opt:
	-gcc -O2 -g -o vplanet *.c -lm

prof: 
	-gcc -o vplanet *.c -lm -Wno-div-by-zero -pg