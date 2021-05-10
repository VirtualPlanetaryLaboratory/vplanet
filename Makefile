# No files with these names in top-level directory
.PHONY: docs test debug opt profile optprof clean coverage sanitize

GITVERSION := $(shell git describe --tags --abbrev=40 --always)
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
	-gcc -o vplanet src/*.c -lm -DGITVERSION=\"$(GITVERSION)\"
	@echo ""
	@echo "=========================================================================================================="
	@echo 'To add vplanet to your $$PATH, please run the appropriate command for your shell type:'
	@echo '( You can see your shell by typing: echo $$0 )'
	@echo 'bash:    export PATH=$$PATH:$(CURDIR)'
	@echo 'tsch:    set path=($$path $(CURDIR))'
	@echo 'csh :    set path=($$path $(CURDIR))'
	@echo 'or permanently add the VPLanet directory to the $$PATH by editing the appropriate environment file. e.g.:'
	@echo 'bash:    echo '"'"'export PATH=$$PATH:$(CURDIR)'"'"' >> ~/.bashrc'
	@echo "=========================================================================================================="

debug:
	-gcc -g -D DEBUG -o vplanet src/*.c -lm -DGITVERSION=\"$(GITVERSION)\"

debug_no_AE:
	-gcc -g -o vplanet src/*.c -lm -DGITVERSION=\"$(GITVERSION)\"

opt:
	-gcc -o vplanet src/*.c -lm -O3 -DGITVERSION=\"$(GITVERSION)\"
	@echo ""
	@echo "=========================================================================================================="
	@echo 'To add vplanet to your $$PATH, please run the appropriate command for your shell type:'
	@echo '( You can see your shell by typing: echo $$0 )'
	@echo 'bash:    export PATH=$$PATH:$(CURDIR)'
	@echo 'tsch:    set path=($$path $(CURDIR))'
	@echo 'csh :    set path=($$path $(CURDIR))'
	@echo 'or permanently add the VPLanet directory to the $$PATH by editing the appropriate environment file. e.g.:'
	@echo 'bash:    echo '"'"'export PATH=$$PATH:$(CURDIR)'"'"' >> ~/.bashrc'
	@echo "=========================================================================================================="

cpp:
	g++ -o vplanet src/*.c -lm -O3 -fopenmp -fpermissive -w -DGITVERSION=\"$(GITVERSION)\"

parallel:
	gcc -o vplanet src/*.c -lm -O3 -fopenmp -DGITVERSION=\"$(GITVERSION)\"

profile:
	-gcc -pg -o vplanet src/*.c -lm -DGITVERSION=\"$(GITVERSION)\"

optprof:
	-gcc -pg -o vplanet src/*.c -lm -O3 -DGITVERSION=\"$(GITVERSION)\"

sanitize:
	-gcc -g -fsanitize=address -o vplanet src/*.c -lm -DGITVERSION=\"$(GITVERSION)\"

test:
	-gcc -o vplanet src/*.c -lm -O3 -DGITVERSION=\"$(GITVERSION)\"
	-pytest

coverage:
	-mkdir -p gcov && cd gcov && gcc -coverage -o ../vplanet ../src/*.c -lm
	-pytest
	-lcov --capture --directory gcov --output-file .coverage && genhtml .coverage --output-directory gcov/html

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
