# Development requires

## Build dependency

see the [zephyr application README](README.original.md) to setup zephyr build env.

## How to build

```
west init -m https://github.com/savent404/coriander --mr main coriander-ws
cd coriander-ws
west update
west build -b <board> coriander/app -- -DCONFIG_CORIANDER_MINIMAL=y <OTHER OPTIONS>
ninja -C build
```

`<board>` supported:
- atk(atk-dm407)
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

## Use Overlay options for develop purpose

There are some default overlay config to debug system.

Use overlay config as below:
```
west build -b <board> app -- -DOVERLAY_CONFIG=<overlay config>
```

overlay config list:
- perf.conf: use systemview to analyze firmware performance and timing.
- debug.conf: used to debug step-by-step

as example, it will enable debug-optimization for board:atk
```
west build -b atk app -- -DOVERLAY_CONFIG=debug.conf
```
