.PHONY: docs test

default:
	-gcc -o vplanet src/*.c -lm

debug:
	-gcc -g -D DEBUG -o vplanet src/*.c -lm -Wno-div-by-zero

opt:
	-gcc -o vplanet src/*.c -lm -O3

profile:
	-gcc -pg -o vplanet src/*.c -lm

optprof:
	-gcc -pg -o vplanet src/*.c -lm -O3

test:
	-gcc -o vplanet src/*.c -lm
	nosetests -s -v -w tests

docs:
	-make -C docs html && echo 'Documentation available at `docs/.build/html/index.html`.'
