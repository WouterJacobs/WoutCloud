# WoutCloud
A server application written in C.
<br>
## Description
WoutCloud is a personal C project designed to explore network programming concepts, including HTTP protocols and
server implementation. It leverages Winsock2 APIs for Windows systems to establish a basic server framework.
This project is first and foremost intended for my own educational purposes and development.

## Features
- handling of a basic http request.
- accepts incoming connection from a client.
- new features coming soon...

## Getting started
### prerequisites
- Microsoft OS.
- C compiler
- CMake
### Cloning the repository of WoutCloud.
    git clone https://github.com/WouterJacobs/WoutCloud.git
### Building the C project.
There are two recommended ways to build the project:

**1. Using CMake (recommended):**

Open a terminal or command prompt in the project directory.

a. **For Clion:**

If you're using Clion, you don't need to run any manual CMake commands.
Simply open the project directory in Clion and it will automatically detect the
CMakeLists.txt file and configure the project for you.

b. **For other IDEs:**

Specify the generator for your preferred build system using the `-G` option with `cmake`:

- **For Visual Studio (replace version number if needed):**

```bash
cmake -G "Visual Studio 16 2019" .
```

- **For MinGW Makefiles:**

```bash
cmake -G "MinGW Makefiles" .
```

- **For Unix Makefiles (Linux, macOS):**

```bash
cmake -G "Unix Makefiles" .
```

Once you've chosen the appropriate generator, run the build command specific to your system (`cmake --build .` for most). Refer to CMake documentation or your IDE's instructions for the exact build command.

**2. Manual Build (alternative)**
