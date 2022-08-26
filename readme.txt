# load module
sudo insmod opemu.ko

# unload module
sudo rmmod opemu

# view module status
lsmod | grep opemu

# view debug log
sudo dmesg
or
sudo dmesg | grep 'OPEMU'
