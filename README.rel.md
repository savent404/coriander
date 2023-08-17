# Release Firmware requires

Firstly, plz setup develop env ref ![Develop doc](README.dev.md).

## Firmware build
```
west build -b <board> coriander/app
```

## Firmware download

### bootloader 
```
west flash --domain mcuboot
```

### application
```
west flash --domain app
```

### recovery image
```
west flash --domain recovery
```
