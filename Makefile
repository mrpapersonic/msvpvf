CC_FLAGS=-Wall -O2 -fstack-protector -fdata-sections -ffunction-sections
LD_FLAGS=

ifeq ($(UNAME_S),Darwin)  # macOS is the odd one...
	LD_FLAGS+=-Wl,-dead_strip
else
	LD_FLAGS+=-Wl,--gc-sections
endif

src/%.o : src/%.c
	$(CC) -c $(CC_FLAGS) $< -o $@

msvpvf: src/main.o src/common.o
	$(CC) $(CC_FLAGS) -o $@ $^  $(LD_FLAGS)

# GUI is windows-only, please use cross-compiler!
gui: src/gui.o src/common.o
	$(CC) $(CC_FLAGS) -o $@ $^ $(LD_FLAGS) -mwindows

clean:
	rm -f src/*.o *.exe msvpvf gui
