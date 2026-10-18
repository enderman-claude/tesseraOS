# tesseraOS: composes the tessera kernel with other components.
# Each component is a submodule; this repo only wires them together.
KERNEL := kernel/tessera

all:
	$(MAKE) -C $(KERNEL)
	@echo "kernel image: $(KERNEL)/build/BOOTX64.EFI"

clean:
	$(MAKE) -C $(KERNEL) clean

.PHONY: all clean
