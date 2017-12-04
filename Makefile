.PHONY: docs test

default:
	-gcc -o vplanet src/*.c -lm

debug:
	-gcc -g -D DEBUG -o vplanet src/*.c -lm -Wno-div-by-zero

opt:
	-gcc -o vplanet src/*.c -lm -O3

profile:
	-gcc -pg -o vplanet src/*.c -lm

test:
	@echo 'Compiling vplanet...'
	-gcc -o vplanet src/*.c -lm
	@echo 'Running vtest...'
	@if nosetests -s -v -w tests; then\
		tests/certificate --pass;\
	else\
		tests/certificate --fail;\
	fi

docs:
	-make -C docs html && echo 'Documentation available at `docs/.build/html/index.html`.'
