
CXX= g++
CXXFLAGS= -std=c++20 -pedantic -Wall -Wextra -Werror -O2 -g

PROG=ipk-l4-scan

SOURCEDIR=src
BUILDDIR=bin

SRC=$(wildcard $(SOURCEDIR)/*.cpp)
OBJ=$(patsubst $(SOURCEDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRC))

.PHONY: clean

all:dir build

dir:
	mkdir -p bin

build:$(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $(PROG)

$(BUILDDIR)/%.o:$(SOURCEDIR)/%.cpp
	 $(CXX) -c $(CXXFLAGS) $^ -o $@

clean:
	rm -rf $(PROG) $(BUILDDIR) *.zip

zip:
	zip -r xdvorar00.zip etc/ src/ Makefile README.md CHANGELOG.md LICENSE