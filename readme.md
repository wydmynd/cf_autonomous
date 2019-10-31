# autonomous framework for crazyflie

once system is up, the function appMain() is called

## Build

You must have the required tools to build the [Crazyflie firmware](https://github.com/bitcraze/crazyflie-firmware).

Clone the repos with ```--recursive```. If you did not do so, pull submodules with:
```
git submodule update --init --recursive
```
Then build and bootload:
```
make -j$(nproc)
make cload
```


