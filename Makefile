CC_FLAGS=-Wall -O2 -fstack-protector -fdata-sections -ffunction-sections
LD_FLAGS=-Wl,--gc-sections

src/%.o : src/%.c
	$(CC) -c $(CC_FLAGS) $< -o $@

msvpvf: src/main.o src/common.o
	$(CC) $(CC_FLAGS) -o $@ $^  $(LD_FLAGS)

# GUI is windows-only, please use cross-compiler!
gui: src/gui.o src/common.o
	$(CC) $(CC_FLAGS) -o $@ $^ $(LD_FLAGS) -mwindows

clean:
	rm -f src/*.o *.exe msvpvf gui
