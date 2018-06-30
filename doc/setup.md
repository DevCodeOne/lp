Cloning the project
====

* `git clone https://github.com/DevCodeOne/lp --recursive`

Setting up client system
===

* Install archlinuxarm on raspberry pi, instructions for this can be found at [Instructions for the Raspberry Pi 3](https://archlinuxarm.org/platforms/armv8/broadcom/raspberry-pi-3)
* Install dependencies of this project:
    - jsoncpp
    - qrencode
    - hostapd
    - cmake

* Copy the configuration files to their specified directory
    - hostapd.conf to /etc/hostapd/hostapd.conf
    - bridge.netdev to /etc/systemd/network/bridge.netdev
    - bridge.network to /etc/systemd/network/bridge.network
    - eth0.network to /etc/systemd/network/eth0.network

Setting up host system
===

Setting up toolchain on host pc and project environment
--

Get the prebuilt crosstools-ng toolchains from [Link to toolchain](https://archlinuxarm.org/wiki/Distcc_Cross-Compiling) for armv7l-hard-float and extract it somewhere in your home directory
Now you have to prepare your own SetupToolChain.cmake for your System, since cmake wouldn't accept relative paths for the toolchain.

```
INCLUDE(CMakeForceCompiler)

set(CMAKE_SYSTEM_NAME=Linux)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

set(CMAKE_C_COMPILER {DIRECTORY_TO_EXTRACTED_TOOLCHAIN}/arm-unknown-linux-gnueabihf/bin/arm-unknown-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER {DIRECTORY_TO_EXTRACTED_TOOLCHAIN}/arm-unknown-linux-gnueabihf/bin/arm-unknown-linux-gnueabihf-g++)
set(CMAKE_SYSROOT {PROJECT_DIRECTORY}/platform)
set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT})

set(ENV{PKG_CONFIG_DIR} "")
set(ENV{PKG_CONFIG_LIBDIR} "${CMAKE_SYSROOT}/usr/lib/pkgconfig:${CMAKE_SYSROOT}/usr/share/pkgconfig:${CMAKE_SYSROOT}/opt/vc/lib/pkgconfig:${}")
set(ENV{PKG_CONFIG_SYSROOT_DIR} ${CMAKE_SYSROOT})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

```

Install cmake for building the project and doxygen for generating the documentation

Transfering the compiled binary to test on the Raspberry Pi
----

* cd in directory containing the compiled binary and execute `https://archlinuxarm.org/platforms/armv8/broadcom/raspberry-pi-3`

Synchronizing libraries with host pc and Raspberry Pi
---

* cd in project directory and execute `./update_platform.sh`
