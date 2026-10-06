# Jimmy

A Discord bot that will conquer the world one day...

[![GitHub license](https://img.shields.io/github/license/TheMonHub/jimmy.svg)](LICENSE)

## Features

Actually! Jimmy is a multipurpose open source Discord bot written in C by TheMonHub using
[Concord](https://github.com/Cogmasters/concord).

- Moderation
    - Setting the Rules and sending it neatly into the channel like `#rules` for an example
    - Banning a member with an appeal system
    - Unbanning a member
    - Kicking a member
    - Timing out a member
    - Warning a member
    - The main point being that instead of having to type in reasons of the punishment/warning, instead Jimmy uses the rule
      number for more consistent and predictable punishment/warning
    - Sending a DM to the target to inform about their punishment/warning
- Logging
    - Logging message changes including the medias
    - Logging nickname and profile picture changes
    - Logging join and leave
    - Logging moderation action such as ban, unban, kick, timeout, warn that are made through Jimmy
- Fun
    - Coin flip!
    - Dice rolling!
    - Sending random cats/dogs/foxes pictures!
    - Getting a profile picture of a member!
    - And more to come!

## Building

Only tested on a GNU/Linux system

### Prerequisite

#### SQLite `3.37.0+`

Please make sure that you have SQLite version 3.37.0 or higher installed on your system accessible by CMake build system.

#### Concord `v2.4.0`

Please check https://github.com/Cogmasters/concord for more information.

example:
```shell
git clone https://github.com/Cogmasters/concord.git
cd concord
git checkout v2.4.0
CFLAGS="-DCCORD_SIGINTCATCH" make
sudo make install
```

After that, you must create a pkgconf file for CMake to find the Concord library.
The location of the file is `/usr/share/pkgconfig/concord.pc` with the following content:
```text
prefix=/usr/local
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${exec_prefix}/include

Name: concord
URL: https://github.com/Cogmasters/concord
Version: 2.4.0
Description: A Discord API wrapper library made in C
Libs: -L${libdir} -ldiscord
Cflags: -I${includedir}/concord
```

### Actually Building Jimmy

You can use the CMake usual commands as follows:

```shell
cmake -S . -B build
cmake --build build
```

## Usage

Coming soon...

## Contributing

Contributions are accepted, but however, I did not expect those, so the codebase and workflow might not be contribution-friendly.

## To-Do
- [ ] Add support for member logging (Profile picture, nickname).
- [ ] Add an appeal system.

## License

This project is licensed under **Apache License 2.0**.

The full text of the license can be obtained at
http://www.apache.org/licenses/LICENSE-2.0
or in the [**LICENSE**](LICENSE) file included in this repository.