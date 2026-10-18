/* Test program for the toolchain path: gcc -> ELF -> elfpack -> SHARD -> shardelf -> ELF.
 * The initialised volatile pointer needs a load-time relocation, so every stage is exercised.
 * Returns 42 when the relocation was applied correctly. */
static const char msg[] = "shard";
static const char *volatile cursor = msg;

int shard_main(void)
{
    return cursor[0] == 's' ? 42 : 0;
}
