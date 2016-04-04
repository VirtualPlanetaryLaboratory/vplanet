default:
	-gcc -o vplanet *.c -lm

debug:
	-gcc -g -o vplanet *.c -lm -Wno-div-by-zero

opt:
	-gcc -O3 -o vplanet *.c -lm

profile:
	-gcc -pg -o vplanet *.c -lm

