CROSS_COMPILE  := 
CC             := $(TUPLE)gcc
OBJCPY         := $(TUPLE)objcopy
CFLAGS         := -Wall -Wextra -std=c99 -MMD -O2 -g
LDFLAGS        := 
OUT            := eld
FILES          := 

FILES   := $(FILES) main.o
FILES   := $(FILES) util.o
FILES   := $(FILES) sections.o
FILES   := $(FILES) elf.o
FILES   := $(FILES) elf_load.o

.PHONY: all rebuild clean run

all: $(OUT)

rebuild: clean
	$(MAKE) all

clean:
	rm -f $(FILES) $(OUT) $(OUT).debug *.d

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

%.o: %.S
	$(CC) $(CFLAGS) $< -c -o $@

$(OUT): $(FILES)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(OUT).debug: $(OUT)
	$(OBJCPY) --only-keep-debug $^ $@

# For ease of running and debugging

run: $(OUT)
	./$(OUT) ~/devel/testing/gcc_bug_mrp/main.o ~/devel/testing/gcc_bug_mrp/entry.o

-include *.d
