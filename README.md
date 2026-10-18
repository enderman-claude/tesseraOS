# tesseraOS

A full operating system assembled from components. The kernel is ours, and the
rest is either our own project or an external one included as a submodule.

> **Status: early stage.** The repo is wired together, but the only component
> that exists is the kernel, which is still a stub. There is no bootable system yet.

## Components

| Component | Path | Origin | Role |
|---|---|---|---|
| tessera | `kernel/tessera` | enderman-claude/tessera (ours) | x86_64 UEFI kernel |
| oxinit | planned: `init/oxinit` | enderman-claude/oxinit (ours) | PID 1 init, once there is a syscall layer |
| zstd | planned: `third_party/zstd` | facebook/zstd (external) | compression for boot payloads |

Only `tessera` is wired in today. The other entries are planned, and each will be
added as a submodule when it is needed.

## How it fits together
- The kernel builds from `kernel/tessera` into `kernel/tessera/build/BOOTX64.EFI`.
- This repo owns the top-level build and any glue between components. It does not
  contain component source code.
- Each component keeps its own docs, tests, and history.

## Clone
    git clone --recurse-submodules https://github.com/enderman-claude/tesseraOS
    # or, after a plain clone:
    git submodule update --init --recursive

## Build
Requirements: `gcc`, `ld` (binutils with `i386pep`), `python3`, `make`.

    make            # builds the kernel image
    make clean

The top-level Makefile delegates to each component. Right now it only runs the
tessera build.

## Testing
Nothing boots yet. Testing will use QEMU with OVMF once the kernel prints to the
console. The development sandbox does not have QEMU or OVMF.

## Roadmap
1. Kernel boots under UEFI and prints a line (tessera).
2. Kernel reads the memory map and exits boot services.
3. Physical memory, paging, and interrupts.
4. Syscall ABI, then oxinit as PID 1.
5. Userland tools and a first boot image.

See [`kernel/tessera/docs/ROADMAP.md`](kernel/tessera/docs/ROADMAP.md) for kernel detail.

## License
Not yet chosen.
