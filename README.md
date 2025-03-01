## OPEMU for Linux

- This is an adaptation of the original Opcode Emulator from [xnu-amd's repo](https://github.com/sinetek/xnu-amd/tree/master/osfmk/OPEMU) for Linux.

- For the kernel module, and a few other things (fpins, SSE4.1...), I took some code from [Meowthra's AVX+ OPEMU](https://www.insanelymac.com/forum/topic/338919-opcode-emulator-opemu-for-linux-64-bit/), which looks pretty good, but does not fit my needs (SSSE3, SSE4.1, & SSE4.2).

- POPCNT and CRC32 are all merit of [Andy Vandijck](https://www.insanelymac.com/forum/topic/281450-mavericks-kernel-testing-on-amd-formerly-mountain-lion-kernel-testing-on-amd/?page=211#comment-1982883) instead. 

- PMAXUD and PMINUD code was taken from [blogdron's fork](https://github.com/blogdron/opemu-linux).

- AVX and AVX2 code was taken from [rafael2k's fork](https://github.com/rafael2k/opemu-linux).

- Fix for modern Linux kernels was taken from [Spacefish's fork](https://github.com/Spacefish/opemu-linux).

- Udis86 disassembler library was updated to [canihavesomecoffee's fork](https://github.com/canihavesomecoffee/udis86).

## What's working

- SSE3, SSSE3, SSE4.2 (mostly). CRC32 doesn't pass [tests](https://github.com/htot/crc32c) though.
- most SSE4.1 instructions are missing, I tried to add a few but it's not working well yet.
- some AVX, AVX2 instructions are missing or not working well yet.
- some instructions with memory operands may not work for some conditions due to page faults.
- some opcodes can't be disassembled due to page faults.
