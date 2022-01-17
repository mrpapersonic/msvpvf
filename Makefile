CC=gcc
CC_FLAGS=-static -O2

%.o : %.c
	$(CC) -c $(CC_FLAGS) $< -o $@

msvpvf: msvpvf.o
	$(CC) -o $@ $< $(CC_FLAGS) $(LD_FLAGS)

clean:
	rm -f *.o
	rm -f msvpvf msvpvf.exe
