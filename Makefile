.PHONY: all clean install

CC=g++
CFLAGS=-c -Wall

HOME_PATH = ${HOME}

# sources 
SOURCES_PATH=./src/*.cpp

# name of executable file   
EXECUTABLE=math

all: ${EXECUTABLE}

clean: 
	rm -rf *.o *.a ${EXECUTABLE}

${EXECUTABLE}:
	$(CC) $(SOURCES_PATH) -o ${EXECUTABLE}

install: 
	sudo install ./${EXECUTABLE} /bin