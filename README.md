# rosbot_ros2_firmware
Micro-ROS powered firmware for STM32F4 microcontroller in ROSbot 2. Embedded, low-level firmware for STM32 microcontroller inside ROSbot. Developed using [Mbed Os](https://os.mbed.com/mbed-os/).

```
______  _____  _____  _             _           __
| ___ \|  _  |/  ___|| |           | |         / _|
| |_/ /| | | |\ `--. | |__    ___  | |_       | |_ __      __
|    / | | | | `--. \| '_ \  / _ \ | __|      |  _|\ \ /\ / /
| |\ \ \ \_/ //\__/ /| |_) || (_) || |_       | |   \ V  V /
\_| \_| \___/ \____/ |_.__/  \___/  \__|      |_|    \_/\_/
```
**Firmware version:**

[![rosbot_ros2_firmware](https://img.shields.io/github/v/release/husarion/rosbot_ros2_firmware)](https://github.com/husarion/rosbot_ros2_firmware/releases/latest)

## Prerequisites
You need to install following tools:
* [Visual Studio Code IDE](https://code.visualstudio.com/)

### Required Visual Studio Code extensions
* [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) (`ms-vscode-remote.remote-containers`)

## Clone the repository
```bash
git clone https://github.com/husarion/rosbot_ros2_firmware
```

## Loading dependencies
Unfortunately, PlatformIO has a [bug](https://github.com/platformio/platform-ststm32/issues/491) that impacts libraries configuration. We use git submodules instead.

> **Git submodules**
> https://git-scm.com/book/en/v2/Git-Tools-Submodules

To import dependencies, in project's root directory run command:

```bash
git submodule update --init --recursive
```

## Open workspace in Devcontainer
Click the left bottom green button in VS Code and select the `Reopen in Container` option.

## Speed up build process
https://docs.platformio.org/en/latest/frameworks/mbed.html#ignoring-particular-components

In directory `~/.platformio/packages/framework-mbed/features` inside the devcontainer create file called `.mbedignore` with the following content:

```
cellular/*
cryptocell/*
deprecated_warnings/*
lorawan/*
lwipstack/*
nanostack/*
netsocket/*
nfc/*
unsupported/*
```

## Build firmware
Use `PlatformIO: Build` task or type
```bash
pio run
```

## Uploading firmware

### Uploading firmware using ST-Link
Use `PlatformIO: Upload` task or type
```bash
pio run --target upload
```

### Uploading firmware using `core2-flasher`
You will find `firmware.elf` in `./pio/build/core2`.


To flash firmware using `core2-flasher` run:
```
core2-flasher .pio/build/core2/firmware.hex
```

### Uploading firmware using `stm32loader`
https://github.com/husarion/stm32loader

This tool allows you to upload firmware using RPi connector.

If you have the bootloader the first two sectors are write protected. Before uploading new firmware you must unlock them (this will erase the bootloader):

```bash
$ sudo stm32loader -c <your_sbc> -u -W
```

To upload new firmware run:
```bash
$ sudo stm32loader -c <your_sbc> -e -v -w firmware.bin
```

where `<your_sbc>` :
* `tinker` for Asus Tinker Board
* `upboard` for Upboard
* `rpi` for Raspberry Pi

You will find `firmware.bin` in `./pio/build/core2`.

## Micro-ROS interface
To start [Micro-ROS](https://micro.ros.org/) communication run:

```bash
docker run -it --rm -v /dev:/dev -v /dev/shm:/dev/shm --privileged --net=host microros/micro-ros-agent:humble serial -D <SBC_port_name>  serial -b 576000
```

`<SBC_port_name>`:
- `/dev/ttyS1` for Asus Tinker Board,
- `/dev/ttyAMA0` for Raspberry Pi
- `/dev/ttyS4` for UpBoard

The baudrate should be adjusted for SBC you use. The default value for this firmware is `576000`.

## ROS2 communication
Look [ROS_API.md](./ROS_API.md)

## Development
The easiest way to change the firmware is to work inside a devcontainer.
In VSCode use option `Reopen in Container` from extention:
- [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) (ms-vscode-remote.remote-containers)

## Creating a release

Release is created automatically after PR with bump command is merged to the master branch. Available bump commands are: `bump::major`, `bump::minor` and `bump::patch` (simply add them to the description of your PR).

## Versioning

The project uses [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/husarion/rosbot-firmware-new/tags).

## Changelog

See [CHANGELOG.md](CHANGELOG.md).

## Starting with Mbed OS

Documentation:
* [MBED OS Documentation](https://os.mbed.com/docs/v5.14/)
* [MBED OS API Doxygen](https://os.mbed.com/docs/v5.14/mbed-os-api-doxy/modules.html)


## Important dev links (mbed + platformio)
* https://github.com/platformio/platform-ststm32/tree/develop/examples/mbed-legacy-examples/mbed-rtos
* https://github.com/platformio/platform-ststm32/blob/develop/boards/olimex_e407.json
* https://github.com/platformio/platform-ststm32/blob/develop/boards/black_f407zg.json
* https://github.com/ARMmbed/mbed-os/tree/mbed-os-5.15.6/targets/TARGET_STM/TARGET_STM32F4/TARGET_STM32F407xG
* https://docs.platformio.org/en/latest/frameworks/mbed.html
* https://docs.platformio.org/en/latest/platforms/creating_board.html


## Troubleshooting

If you can't build even released versions, reinstall platformio:

```bash
sudo su
pip3 uninstall platformio -y
sudo rm -rf .pio/
sudo rm -rf ~/.platformio/

pip install platformio==6.1.11
python3 -m venv /root/.platformio/penv/bin/activate

pio run
```