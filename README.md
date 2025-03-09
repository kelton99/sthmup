# C Shoot Them Up

It is a game and engine made from scratch in C for learning pourposes, based on [tutorial](https://www.parallelrealities.co.uk/tutorials/#shooter) by Pararel Reality.

## Building

Installing necessary dependencies

# ![](https://img.shields.io/badge/Debian-A81D33?style=for-the-badge&logo=debian&logoColor=white)
```bash
$ sudo apt update && sudo apt install -y build-essential make libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev libsdl2-gfx-dev
```

# ![](https://img.shields.io/badge/Arch_Linux-1793D1?style=for-the-badge&logo=arch-linux&logoColor=white)
```bash
$ sudo pacman -S base-devel make sdl2 sdl2_image sdl2_gfx sdl2_mixer sdl2_ttf
```

Clone the repository, enter it and run the compilation command 
```bash
$ git clone git@github.com:kelton99/sthmup.git
$ cd sthmup
```
For debug build
```bash
$ make
```
For release build
```bash
$ make release
```

Cleaning the build
```bash
$ make clean
```

## Usage
In the root folder use
```bash
make run
```

## How to Play

<p>Arrows Keys - Move</p>
<p>LCTRL - Shoot<p>