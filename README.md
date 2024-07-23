# WoutCloud
A server application written in C.

## Overview
WoutCloud is a basic multi-client server application under active development designed to run on Linux systems. 
The server allows multiple clients to connect, send messages, and broadcast messages to all connected clients.
<br>
This project is licensed under the GNU General Public License Version 3.

## Features
- Accepts multiple client connections using threads.
- Broadcasts messages from one client to all other connected clients.
- Handles basic client-server communication over TCP sockets.
- Uses mutexes to handle concurrent access to shared resources.
- new features coming soon...

## How to run
1) Clone this repository
```git clone https://github.com/WouterJacobs/WoutCloud.git```
2) go into the /src folder and compile the code
```gcc -o server main.c``` or ```gcc -pthread -o server main.c```
3) Run the program
```./server```
4) Connect to the server using a tool to your liking, in this case Netcat
```nc {ip address of the host machine} 8080```
