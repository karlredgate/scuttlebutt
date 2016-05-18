
# OS := $(shell uname -s)
# include Makefiles/$(OS).mk

default: test

test: sink pair scuttlebutt
	sudo ./scuttlebutt --debug

scuttlebutt: scuttlebutt.o port.o

sink: sink.o

pair: pair.o

clean:
	rm -f $(CLEANS) *.o
