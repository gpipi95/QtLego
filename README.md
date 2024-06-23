# QtLego
This project is forked from [Symbitic's QtLego](https://github.com/Symbitic/QtLego)

A Qt library for connecting and controlling LEGO® devices.

**THIS LIBRARY IS STILL IN ITS INFANCY! IT CAN ONLY HANDLE BASIC CONNECTIONS AND MOTOR COMMANDS!! EXPECT BREAKING CHANGES!!!**

## Introduction

QtLego is a library for connecting to LEGO devices and querying and controlling peripherals attached to those devices. Currently only LEGO Powered UP hubs are supported, but it should be possible to add support for Mindstorms and Spike Prime hubs.

## Reference
For [lego bluetooth protocol](https://github.com/LEGO/lego-ble-wireless-protocol-docs)

## Using

### API

QLegoDevice

```c++
auto scanner = new QLegoDeviceScanner();

QObject::connect(scanner, &QLegoDeviceScanner::deviceFound, [=](QLegoDevice *device) {
  qDebug() << "Address:" << device->address();
  qDebug() << "Firmware:" << device->firmware();
  QTimer::singleShot(10000, device, &QLegoDevice::disconnect); // Disconnect after 10 seconds.
});
```
See [boostApp/boost.cpp](./boostApp/boost.cpp) for lego boost example.

## License

Copyright © Alex Shaw 2021

Source code is licensed under the [MIT](LICENSE.md) license.
