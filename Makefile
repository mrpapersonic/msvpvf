CC_FLAGS=-Wall -O2
LD_FLAGS=

src/%.o : src/%.c
	$(CC) -c $(CC_FLAGS) $< -o $@

msvpvf: src/main.o
	$(CC) -o $@ $< $(CC_FLAGS) $(LD_FLAGS)

# GUI is windows-only, please use cross-compiler!
gui: src/gui.o
	$(CC) -o $@ $< $(CC_FLAGS) $(LD_FLAGS) -mwindows

clean:
	rm -f src/*.o *.exe msvpvf gui
