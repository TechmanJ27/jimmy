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
    - The main point being that instead of having to type in reasons of the punishment/warning, instead Jimmy uses rule
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
    - D20 fight!
    - Sending random cats/dogs/foxes pictures!
    - Getting profile picture of a member!
    - See how many message did a member send and automatically give role once they meet a certain requirement!
    - And more!

## Building

Only tested on GNU/Linux system

### Prerequisite

#### SQLite `3.37.0+`

Please make sure that you have SQLite version 3.37.0 or higher installed on your system accessible by CMake build system.

#### Concord `v3.X.X`

Please check https://github.com/Cogmasters/concord for more information.

example:
```shell
git clone https://github.com/Cogmasters/concord.git
cd concord
git checkout v3.0.0
CFLAGS="-DCCORD_SIGINTCATCH" make
sudo make install
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

Contribution are accepted but however I did not expect those so the codebase and workflow might not be contribution
friendly.

## License

This project is licensed under **Apache License 2.0**.

The full text of the license can be obtained at
http://www.apache.org/licenses/LICENSE-2.0
or in the [**LICENSE**](LICENSE) file included in this repository.