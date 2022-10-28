# ROSbot firmware CHANGELOG

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/) and this project adheres to [Semantic Versioning](http://semver.org/).

## [0.1.0] - 2022-10-27

### Changed
- Separated ros1 and ros2 firmware. Moved from https://github.com/husarion/rosbot-stm32-firmware to https://github.com/husarion/rosbot_ros2_firmware
- Changed communication interface to [Micro-ROS](https://micro.ros.org/).
- Created compatibility with [rosbot_hardware_interfaces](https://github.com/husarion/rosbot_hardware_interfaces).
- Added communication blinking LEDs.

### Fixed
- Supported communication with ROS2 `humble` version.