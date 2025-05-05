# No files with these names in top-level directory
.PHONY: docs test debug opt profile optprof clean coverage sanitize

GITVERSION := $(shell git describe --tags --abbrev=40 --always)
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
CC = gcc
GCC_FLAGS1 = -fPIC -Wl,-Bsymbolic-functions -c
GCC_FLAGS2 = -shared -Wl,-Bsymbolic-functions,-soname,vplanetlib.so
endif
ifeq ($(UNAME_S),Darwin)
CC = clang
GCC_FLAGS1 = -fPIC -c
GCC_FLAGS2 = -shared -Wl,-install_name,vplanetlib.so
endif

default:
	-python setup.py clean --all
	-python setup.py develop

legacy:
	-$(CC) -o bin/vplanet src/*.c -lm -DGITVERSION=\"$(GITVERSION)\"
	@echo ""
	@echo "=========================================================================================================="
	@echo 'To add vplanet to your $$PATH, please run the appropriate command for your shell type:'
	@echo '( You can see your shell by typing: echo $$0 )'
	@echo 'bash:    export PATH=$$PATH:$(CURDIR)/bin'
	@echo 'tsch:    set path=($$path $(CURDIR)/bin)'
	@echo 'csh :    set path=($$path $(CURDIR)/bin)'
	@echo 'or permanently add the VPLanet directory to the $$PATH by editing the appropriate environment file. e.g.:'
	@echo 'bash:    echo '"'"'export PATH=$$PATH:$(CURDIR)/bin'"'"' >> ~/.bashrc'
	@echo "=========================================================================================================="

debug:
	-$(CC) -g -D DEBUG -o bin/vplanet src/*.c -lm -DGITVERSION=\"$(GITVERSION)\"

debug_no_AE:
	-$(CC) -g -o bin/vplanet src/*.c -lm -DGITVERSION=\"$(GITVERSION)\"

opt:
	-$(CC) -o bin/vplanet src/*.c -lm -O3 -DGITVERSION=\"$(GITVERSION)\"
	@echo ""
	@echo "=========================================================================================================="
	@echo 'To add vplanet to your $$PATH, please run the appropriate command for your shell type:'
	@echo '( You can see your shell by typing: echo $$0 )'
	@echo 'bash:    export PATH=$$PATH:$(CURDIR)/bin'
	@echo 'tsch:    set path=($$path $(CURDIR)/bin)'
	@echo 'csh :    set path=($$path $(CURDIR)/bin)'
	@echo 'or permanently add the VPLanet directory to the $$PATH by editing the appropriate environment file. e.g.:'
	@echo 'bash:    echo '"'"'export PATH=$$PATH:$(CURDIR)/bin'"'"' >> ~/.bashrc'
	@echo "=========================================================================================================="

cpp:
	g++ -o bin/vplanet src/*.c -lm -O3 -fopenmp -fpermissive -w -DGITVERSION=\"$(GITVERSION)\"

warnings:
	-$(CC)-g -D DEBUG -Wunused-but-set-variable -Wunused-variable -Wfloat-equal -o bin/vplanet src/*.c -lm -DGITVERSION=\"$(GITVERSION)\"

parallel:
	$(CC) -o bin/vplanet src/*.c -lm -O3 -fopenmp -DGITVERSION=\"$(GITVERSION)\"

profile:
	-$(CC) -pg -o bin/vplanet src/*.c -lm -DGITVERSION=\"$(GITVERSION)\"

optprof:
	-$(CC) -pg -o bin/vplanet src/*.c -lm -O3 -DGITVERSION=\"$(GITVERSION)\"

sanitize:
	-$(CC) -g -fsanitize=address -o bin/vplanet src/*.c -lm -DGITVERSION=\"$(GITVERSION)\"

test:
	-$(CC) -o bin/vplanet src/*.c -lm -O3 -DGITVERSION=\"$(GITVERSION)\"
	-pytest --tb=short

coverage:
	-rm -f ./gcov/*.gcda ./gcov/*.gcno ./.coverage
	-mkdir -p ./gcov
	-cd gcov && $(CC) -coverage -o ./../bin/vplanet ./../src/*.c -lm
	-python -m pytest --tb=short tests --junitxml=./junit/test-results.xml
	-lcov --capture --directory ./gcov --output-file ./.coverage
	-genhtml ./.coverage --output-directory ./gcov/html

docs:
	-make -C docs html && echo 'Documentation available at `docs/.build/html/index.html`.'

shared:
	-$(CC) ${GCC_FLAGS1} src/*.c
	-$(CC) ${GCC_FLAGS2} -o bin/vplanetlib.so *.o -lc

clean:
	rm -f bin/vplanet
	rm -rf gcov
	rm -rf .pytest_cache
	rm -f src/*.o
	rm -f bin/vplanetlib.so
