CC=clang  # clang cause gcc throws compiling errors that i don't know how to fix
CC_FLAGS=-Wall -static -O2
LD_FLAGS=-ffunction-sections

%.o : %.c
	$(CC) -c $(CC_FLAGS) $< -o $@

msvpvf: msvpvf.o
	$(CC) -o $@ $< $(CC_FLAGS) $(LD_FLAGS)

clean:
	rm -f *.o
	rm -f msvpvf msvpvf.exe
