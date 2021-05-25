# OpenTibiaBR - OTClient

[![Discord Channel](https://img.shields.io/discord/528117503952551936.svg?style=flat-square&logo=discord)](https://discord.gg/3NxYnyV)
[![GitHub issues](https://img.shields.io/github/issues/opentibiabr/otclient)](https://github.com/opentibiabr/otclient/issues)
[![GitHub pull request](https://img.shields.io/github/issues-pr/opentibiabr/otclient)](https://github.com/opentibiabr/otclient/pulls)
[![Contributors](https://img.shields.io/github/contributors/opentibiabr/otclient.svg?style=flat-square)](https://github.com/opentibiabr/otclient/graphs/contributors)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://github.com/opentibiabr/otclient/blob/develop/LICENSE)

![GitHub repo size](https://img.shields.io/github/repo-size/opentibiabr/otclient)

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/77c808ed38c0461f92a3487ada1fc57f)](https://www.codacy.com/gh/opentibiabr/otclient/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=opentibiabr/otclient&amp;utm_campaign=Badge_Grade)
[![CodeQL](https://github.com/opentibiabr/otclient/actions/workflows/analysis-codeql.yml/badge.svg)](https://github.com/opentibiabr/otclient/actions/workflows/analysis-codeql.yml)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=opentibiabr_otclient&metric=alert_status)](https://sonarcloud.io/dashboard?id=opentibiabr_otclient)

## Builds

[![Build Status](https://github.com/opentibiabr/otclient/actions/workflows/build-macos.yml/badge.svg)](https://github.com/opentibiabr/otclient/actions/workflows/build-macos.yml)
[![Build Status](https://github.com/opentibiabr/otclient/actions/workflows/build-ubuntu.yml/badge.svg)](https://github.com/opentibiabr/otclient/actions/workflows/build-ubuntu.yml)
[![Build Status](https://github.com/opentibiabr/otclient/actions/workflows/build-windows.yml/badge.svg)](https://github.com/opentibiabr/otclient/actions/workflows/build-windows.yml)

### What is otclient?

Otclient is an alternative Tibia client for usage with otserv. It aims to be complete and flexible,
for that it uses LUA scripting for all game interface functionality and configurations files with a syntax
similar to CSS for the client interface design. Otclient works with a modular system, this means
that each functionality is a separated module, giving the possibility to users modify and customize
anything easily. Users can also create new mods and extend game interface for their own purposes.
Otclient is written in C++17 and heavily scripted in lua.

### Where do I download?

Compiled for MacOS, Ubuntu and Windows can be found here:
* [MacOS](https://github.com/opentibiabr/otclient/actions/workflows/build-macos.yml)
* [Ubuntu](https://github.com/opentibiabr/otclient/actions/workflows/build-ubuntu.yml)
* [Windows](https://github.com/opentibiabr/otclient/actions/workflows/build-windows.yml)

Compatible Dat and Spr with [otservbr-global](https://github.com/opentibiabr/otservbr-global) and [canary](https://github.com/opentibiabr/canary):
* [Client 10](https://github.com/opentibiabr/tools/blob/master/Tibia%20Client%2010.zip)

**NOTE:** You will need to download spr/dat files on your own and place them in `data/things/1264/` (i.e: `data/things/1264/Tibia.spr`)

### Compiling

In short, if you need to compile OTClient, follow these tutorials:
* [Compiling on Windows](https://github.com/edubart/otclient/wiki/Compiling-on-Windows)
* [Compiling on Linux](https://github.com/edubart/otclient/wiki/Compiling-on-Linux)
* [Compiling on OS X](https://github.com/edubart/otclient/wiki/Compiling-on-Mac-OS-X)

Forum OTServBR:
* [Compiling on Windows](https://forums.otserv.com.br/index.php?/forums/topic/169297-windowsvc2019-compilando-sources-otclient-vcpkg/)

### Build and run with Docker

To build the image:

```sh
docker build -t opentibiabr/otclient .
```

To run the built image:

```sh
# Disable access control for the X server.
xhost +

# Run the container image with the required bindings to the host devices and volumes.
docker run -it --rm \
  --env DISPLAY \
  --volume /tmp/.X11-unix:/tmp/.X11-unix \
  --device /dev/dri \
  --device /dev/snd opentibiabr/otclient /bin/bash

# Enable access control for the X server.
xhost -
```

### Need help?

Try to ask questions in our [discord](https://discord.gg/3NxYnyV)

### Bugs

Have found a bug? Please create an issue in our [bug tracker](https://github.com/opentibiabr/otclient/issues)

### Contributing

We encourage you to contribute to otclient! You can make pull requests of any improvement in [pull requests](https://github.com/opentibiabr/otclient/pulls)

### Contact

[![Discord Channel](https://img.shields.io/discord/528117503952551936.svg?label=discord)](https://discord.gg/3NxYnyV)

### License

Otclient is made available under the MIT License, thus this means that you are free
to do whatever you want, commercial, non-commercial, closed or open.
