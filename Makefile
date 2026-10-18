# tesseraOS: composes the tessera kernel with other components.
# Each component is a submodule; this repo only wires them together.
KERNEL := kernel/tessera

all:
	$(MAKE) -C $(KERNEL)
	@echo "kernel image: $(KERNEL)/build/BOOTX64.EFI"

clean:
	$(MAKE) -C $(KERNEL) clean

.PHONY: all clean

SHARD := lib/shard
TESS  := kernel/tessera
INT   := build/integration

# Proves the toolchain path works end to end: gcc -> elfpack -> shardelf -> loaders -> run.
integration:
	$(MAKE) -C $(SHARD) build/libshard.a build/elfpack build/shardelf
	mkdir -p $(INT)
	gcc -std=c11 -O1 -fPIE -static-pie -nostdlib -fno-stack-protector -Wl,-e,shard_main \
	    -o $(INT)/prog.elf tests/integration/prog.c
	$(SHARD)/build/elfpack $(INT)/prog.elf $(INT)/prog.shard
	$(SHARD)/build/shardelf $(INT)/prog.shard $(INT)/prog2.so
	gcc -std=c11 -Wall -Wextra -Werror -O2 -I$(SHARD)/include -I$(TESS)/include \
	    -o $(INT)/run tests/integration/run.c $(TESS)/src/memory/bitmap_alloc/bitmap_alloc.c \
	    $(SHARD)/build/libshard.a
	$(INT)/run $(INT)

.PHONY: integration
