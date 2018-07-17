.PHONY: docs test debug opt profile optprof

default:
	-gcc -o vplanet src/*.c -lm
	mv vplanet /usr/local/bin/vplanet

debug:
	-gcc -g -D DEBUG -o vplanet src/*.c -lm -Wno-div-by-zero
	mv vplanet /usr/local/bin/vplanet

opt:
	-gcc -o vplanet src/*.c -lm -O3
	mv vplanet /usr/local/bin/vplanet

profile:
	-gcc -pg -o vplanet src/*.c -lm
	mv vplanet /usr/local/bin/vplanet

optprof:
	-gcc -pg -o vplanet src/*.c -lm -O3
	mv vplanet /usr/local/bin/vplanet

test:
	-gcc -o vplanet src/*.c -lm
	mv vplanet /usr/local/bin/vplanet
	py.test -s tests

docs:
	-make -C docs html && echo 'Documentation available at `docs/.build/html/index.html`.'
