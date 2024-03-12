# Heartbeat firmware for periodic bluetooth updates.

When programmed to a rdl81522, allows apps to pair and periodically do things in the background on iOS.

```
cd rdl51822/s130/armgcc
make erase
flash_softdevice
CFLAGS=-Werror=array-bounds=0 make flash
```