/* Integration check: loads each form of the test program with the OS loaders and runs it.
 * Pages come from tessera's bitmap allocator (kernel/tessera/src/memory/bitmap_alloc). */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include "shard/shard_open.h"
#include "shard/shard_load.h"
#include "elf/elf_open.h"
#include "elf/elf_load.h"
#include "tessera/memory/bitmap_alloc.h"

#define PAGE 4096
#define PAGES 64
#define IMAGE_PAGES 8   /* each image gets a contiguous block of this many pages */
#define IMAGE_BYTES (IMAGE_PAGES * PAGE)

typedef int (*entry_fn)(void);

static uint8_t *arena;
static uint8_t bitmap[PAGES / 8];
static int failures;

static uint8_t *read_file(const char *path, size_t *len)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *b = malloc((size_t)n);
    *len = b ? fread(b, 1, (size_t)n, f) : 0;
    fclose(f);
    return b;
}

/* Allocates IMAGE_PAGES consecutive pages. The allocator is first-fit, so consecutive
 * calls on a free run return consecutive pages; anything else is treated as failure. */
static uint8_t *take_image_pages(void)
{
    int first = -1;
    for (int i = 0; i < IMAGE_PAGES; i++) {
        int p = bitmap_alloc_page(bitmap, PAGES);
        if (p < 0 || (i > 0 && p != first + i)) return NULL;
        if (i == 0) first = p;
    }
    return arena + (size_t)first * PAGE;
}

static void report(const char *what, int ok)
{
    printf("%s %s\n", ok ? "ok  " : "FAIL", what);
    if (!ok) failures++;
}

static void run_shard(const char *path, const char *what)
{
    size_t n = 0;
    uint8_t *b = read_file(path, &n);
    struct shard_image img;
    if (!b || shard_open(b, n, &img) != SHARD_OK) { report(what, 0); return; }
    uint8_t *dst = take_image_pages();
    uint64_t entry = 0;
    if (!dst || shard_load(&img, dst, IMAGE_BYTES, (uint64_t)(uintptr_t)dst, &entry) != SHARD_OK) { report(what, 0); return; }
    report(what, ((entry_fn)(uintptr_t)entry)() == 42);
}

static void run_elf(const char *path, const char *what)
{
    size_t n = 0;
    uint8_t *b = read_file(path, &n);
    struct elf_image img;
    if (!b || elf_open(b, n, &img) != 0) { report(what, 0); return; }
    uint8_t *dst = take_image_pages();
    uint64_t entry = 0, span = 0;
    if (!dst || elf_load(&img, dst, IMAGE_BYTES, (uint64_t)(uintptr_t)dst, &entry, &span) != 0) { report(what, 0); return; }
    report(what, ((entry_fn)(uintptr_t)entry)() == 42);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <dir>\n", argv[0]);
        return 2;
    }
    arena = mmap(NULL, PAGES * PAGE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (arena == MAP_FAILED) {
        printf("FAIL cannot map arena\n");
        return 1;
    }
    char path[512];
    snprintf(path, sizeof path, "%s/prog.elf", argv[1]);
    run_elf(path, "gcc ELF runs through elf_load");
    snprintf(path, sizeof path, "%s/prog.shard", argv[1]);
    run_shard(path, "elfpack SHARD runs through shard_load");
    snprintf(path, sizeof path, "%s/prog2.so", argv[1]);
    run_elf(path, "shardelf ELF runs through elf_load");
    printf("%s\n", failures ? "FAILED" : "PASSED");
    return failures ? 1 : 0;
}
