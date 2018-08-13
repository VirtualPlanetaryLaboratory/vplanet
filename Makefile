.PHONY: docs test debug opt profile optprof
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
GCC_FLAGS1 = -fPIC -Wl,-Bsymbolic-functions -c
GCC_FLAGS2 = -shared -Wl,-Bsymbolic-functions,-soname,vplanetlib.so
endif
ifeq ($(UNAME_S),Darwin)
GCC_FLAGS1 = -fPIC -c
GCC_FLAGS2 = -shared -Wl,-install_name,vplanetlib.so
endif

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
	py.test

coverage:
	-mkdir -p gcov && cd gcov && gcc -coverage -o ../vplanet ../src/*.c -lm
	-py.test
	-cd gcov && lcov --capture --directory . --output-file coverage.info && genhtml coverage.info --output-directory html

docs:
	-make -C docs html && echo 'Documentation available at `docs/.build/html/index.html`.'

shared:
	-gcc ${GCC_FLAGS1} src/*.c
	-gcc ${GCC_FLAGS2} -o vplanetlib.so *.o -lc

clean:
	rm -f vplanet
	rm -rf gcov
	rm -rf .pytest_cache
	rm *.o
	rm vplanetlib.so
