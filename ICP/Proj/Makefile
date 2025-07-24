PRO_FILE = fsm_tool.pro

# detect OS and set qmake and app paths
ifeq ($(shell uname), Darwin)
	# macOS
	QMAKE = ~/Qt/5.15.18/clang_64/bin/qmake
	APP = ./fsm_tool.app/Contents/MacOS/fsm_tool
else
	# Linux Ubuntu
	QMAKE = qmake
	APP = ./fsm_tool
endif

all:
	cd src && $(QMAKE) $(PRO_FILE) && make && cd ..

run:
	cd src && $(APP) && cd ..

doxygen:
	doxygen
	
clean:
	rm -rf doc/html && \
	cd src && \
	rm -rf fsm_tool.app fsm_tool fsm_tool.pro.user* main_window.ui.autosave && \
	make clean && \
	rm .qmake.stash && \
	rm -rf Makefile build/ && \
	cd ..

pack: clean
	zip -r xborda01-xrojak00-xdvorar00.zip . -x "*/.DS_Store" ".DS_Store" ".gitignore" ".git/*"

.PHONY: all run doxygen clean pack
