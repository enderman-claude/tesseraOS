# tesseraOS

A full operating system assembled from original and reused components.

## Components
- `kernel/tessera`: our x86_64 UEFI kernel (submodule, see its docs/).

## Build
    git submodule update --init --recursive
    make

Planned: userland init from enderman-claude/oxinit once tessera has a syscall layer.
