CC=gcc
CC_FLAGS=-static

%.o : %.c
	$(CC) -c $(CC_FLAGS) $< -o $@

msvpvf: msvpvf.o
	$(CC) -o $@ $< $(CC_FLAGS) $(LD_FLAGS)

clean:
	rm -f *.o
	rm -f msvpvf msvpvf.exe
