CFLAGS=-Wall -O2 -fstack-protector -fdata-sections -ffunction-sections
LDFLAGS=

ifeq ($(shell uname -s),Darwin)  # macOS is the odd one...
	LDFLAGS+=-Wl,-dead_strip
else
	LDFLAGS+=-Wl,--gc-sections
endif

src/%.o : src/%.c
	$(CC) -c $(CC_FLAGS) $< -o $@

msvpvf: src/main.o src/common.o
	$(CC) $(CFLAGS) -o $@ $^  $(LDFLAGS)

# GUI is windows-only, please use cross-compiler!
gui: src/gui.o src/common.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -mwindows

clean:
	rm -f src/*.o *.exe msvpvf gui
