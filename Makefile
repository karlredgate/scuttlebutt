
# OS := $(shell uname -s)
# include Makefiles/$(OS).mk

default: scuttlebutt

scuttlebutt: scuttlebutt.o port.o

clean:
	rm -f $(CLEANS) *.o
