
CXX= g++
CXXFLAGS= -std=c++20 -pedantic -Wall -Wextra -Werror -O2 -g

PROG=ipk25chat-client

SOURCEDIR=src
BUILDDIR=bin

SRC=$(wildcard $(SOURCEDIR)/*.cpp)
OBJ=$(patsubst $(SOURCEDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRC))
DEPS=$(patsubst %.cpp,%.d,$(SRC))

.PHONY: clean

all:dir build

dir:
	mkdir -p bin

-include $(DEPS)

build:$(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $(PROG)

$(BUILDDIR)/%.o:$(SOURCEDIR)/%.cpp Makefile
	 $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(PROG) $(BUILDDIR) *.zip

zip:
	zip -r xdvorar00.zip doc/ src/ Makefile README.md CHANGELOG.md LICENSE