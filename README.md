## OPEMU for Linux


- [Meowthra's AVX+ OPEMU](https://www.insanelymac.com/forum/topic/338919-opcode-emulator-opemu-for-linux-64-bit/) code for AVX

### load module

sudo insmod opemu.ko

### unload module

sudo rmmod opemu

### view module status

lsmod | grep opemu

### view debug log

sudo dmesg

sudo dmesg | grep 'OPEMU'
