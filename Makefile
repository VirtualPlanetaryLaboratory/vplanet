# No files with these names in the top-level directory
.PHONY: docs test debug opt profile optprof clean coverage sanitize

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
	@cd src && vplanet -h > /dev/null 2>&1

debug:
	-gcc -g -D DEBUG -o vplanet src/*.c -lm
	@cd src && vplanet -h > /dev/null 2>&1

debug_no_AE:
	-gcc -g -o vplanet src/*.c -lm
	@cd src && vplanet -h > /dev/null 2>&1

opt:
	-gcc -o vplanet src/*.c -lm -O3
	@cd src && vplanet -h > /dev/null 2>&1

profile:
	-gcc -pg -o vplanet src/*.c -lm
	@cd src && vplanet -h > /dev/null 2>&1

optprof:
	-gcc -pg -o vplanet src/*.c -lm -O3
	@cd src && vplanet -h > /dev/null 2>&1

test:
	-gcc -o vplanet src/*.c -lm
	@cd src && vplanet -h > /dev/null 2>&1
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
	rm -f *.o
	rm -f vplanetlib.so
