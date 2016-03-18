
# OS := $(shell uname -s)
# include Makefiles/$(OS).mk

default: test

test: sink scuttlebutt
	sudo ./scuttlebutt

scuttlebutt: scuttlebutt.o port.o

sink: sink.o

clean:
	rm -f $(CLEANS) *.o
