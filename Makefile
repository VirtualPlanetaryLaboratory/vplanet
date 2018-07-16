.PHONY: docs test debug opt profile optprof

default:
	-gcc -o vplanet src/*.c -lm
	@cd src && vplanet -h > /dev/null 2>&1 || echo "\033[0;31mPlease add the vplanet directory to your PATH variable.\033[0m"

debug:
	-gcc -g -D DEBUG -o vplanet src/*.c -lm -Wno-div-by-zero
	@cd src && vplanet -h > /dev/null 2>&1 || echo "\033[0;31mPlease add the vplanet directory to your PATH variable.\033[0m"

opt:
	-gcc -o vplanet src/*.c -lm -O3
	@cd src && vplanet -h > /dev/null 2>&1 || echo "\033[0;31mPlease add the vplanet directory to your PATH variable.\033[0m"

profile:
	-gcc -pg -o vplanet src/*.c -lm
	@cd src && vplanet -h > /dev/null 2>&1 || echo "\033[0;31mPlease add the vplanet directory to your PATH variable.\033[0m"

optprof:
	-gcc -pg -o vplanet src/*.c -lm -O3
	@cd src && vplanet -h > /dev/null 2>&1 || echo "\033[0;31mPlease add the vplanet directory to your PATH variable.\033[0m"

test:
	-gcc -o vplanet src/*.c -lm
	@cd src && vplanet -h > /dev/null 2>&1 || echo "\033[0;31mPlease add the vplanet directory to your PATH variable.\033[0m"

docs:
	-make -C docs html && echo 'Documentation available at `docs/.build/html/index.html`.'
