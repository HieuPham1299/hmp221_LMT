# HMP221 - LIGHTWEIGHT MESSAGE TRANSFER PROTOCOL

## Summary:
- HMP221-LMT is a lightweight message transfer protocol, inspired by the famous MQTT( https://mqtt.org/ ). 

- Due to its lightweight nature and minimum footprint, HMP221-LMT is especially suitable for communication between constrained devices in Internet of Things(IoT) applications.

- HMP221-LMT is built on top of TCP/IP protocol, providing reliable message transfer channels.

- HMP221-LMT overall architecture is publish-subscribe, where a client can either publish a message or subscribe to a channel to receive messages. The channel information is stored in the server's Random Access Memory (RAM). In other words, two clients are never directly connected.

- The number of connected clients are limited by the number of ports available in the server.

- The message size limit is 65536 bytes.

## System requirements:
- Ubuntu 20.04 LTS
- gcc 9.3.0

## Features completed:
- Subscribing to a channel and receiving the latest message

- Server storing the channel information in a hashmap data structure for fast access

- Inter-Process Communication between clients and the server to read/write to the hashmap in parent process.

## Bugs to be fixed:
- Currently assigning fixed port number to incomming client, needs to assign dynamic port numbers in case there are multiple connections made at the same moment -> DONE
- Add appropriate debug messages -> DONE
- Allow connections from other physical machines (currently only local host communication is working) -> DONE
- Fix errors throwing when subscribing to a non-existent channel -> DONE
- Clean up info messages after that portion is done

## Moving foward
- Make client connection persistent and receive message immediately after the channel has new message. -> DOING
- Complete publishing messages.
- Rigorous testing.
- Write supporting library for high-level languages: Java, Python, and JavaScript.

## 1. To set up server: 
The first step is to get the server up and running, otherwise, the client code will throw an exception. The Makefile first create object files and library files, then link them together to create an executable. Locate to the server folder, then type:
```
make all
```
The executable is put in `build/bin/release`. To run the executable, type:
```
./build/bin/release/server --hostname localhost:[portNo]
```

For example:

```
./build/bin/release/server --hostname localhost:8081
```

The server will be listening on port 8081 of the current machine. In case port 8081 is occupied, consider switching to another port.

-------------------------------

## 2. Publishing message from client:

Locate to the client server, then type:

```
make all
```
then,
```
./build/bin/release/client --hostname [host's IP]:[portNo] --publish [channel] [message]
```

For example,

```
./build/bin/release/client --hostname 192.168.0.1:8081 --publish Testing HelloWorld
```

Here, we are publishing the message HelloWorld into the topic Testing. Note that topic name is unique

-------------------------------

## 3. Client subscribe to a channel to receive message

To receive a message, a client has to subscribe to a channel:

```
./build/bin/release/client --hostname localhost:8000 --subscribe [channel]
```

------------------------------
## Reference:
[1] https://mqtt.org/

[2] https://www.tutorialspoint.com/unix_sockets/socket_quick_guide.htm

[3] https://cp-algorithms.com/string/string-hashing.html

