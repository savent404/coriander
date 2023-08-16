# Development requires

## Build dependency

see the [zephyr application README](README.original.md) to setup zephyr build env.

## How to build

```
west init -m https://github.com/savent404/coriander --mr main coriander-ws
cd coriander-ws
west build -b <board> app -- -DCMAKE_EXPORT_COMPILE_COMMANDS=ON <OTHER OPTIONS>
ninja -C build
```

`<board>` supported:
- td2kw

If you want to build a firmware with debug feature, add `-DOVERLAY_CONFIG=debug.conf` in `<OTHER OPTIONS>`

## How to flash to mcu

```
ninja -C build flash
```

## Unit test framework
```
CC=clang CXX=clang++ cmake -Bbuild-ut app -G Ninja -DUNIT_TEST=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
ninja -C build-ut
./build-ut/ut
```