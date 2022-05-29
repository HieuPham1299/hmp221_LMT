# HMP221 - LIGHTWEIGHT MESSAGE TRANSFER PROTOCOL

## Summary:
- HMP221-LMT is a lightweight message transfer protocol, inspired by the famous MQTT( https://mqtt.org/ ). 

- Due to its lightweight nature and minimum footprint, HMP221-LMT is especially suitable for communication between constrained devices.

- HMP221-LMT is built on top of TCP/IP protocol, providing reliable message transfer channels.

- HMP221-LMT overall architecture is publish-subscribe, where a client can either publish a message or subscribe to a channel to receive messages. The channel information is stored in the server's Random Access Memory (RAM). In other words, two clients are never directly connected.

- The number of connected clients are limited by the number of ports available in the server.

- The message size limit is 65536 bytes.

## Features completed:
- Subscribing to a channel and receiving the latest message

- Server storing the channel information in a hashmap data structure for fast access

- Inter-Process Communication between clients and the server to read/write to the hashmap in parent process.

## Bugs to be fixed:
- Currently assigning fixed port number to incomming client, needs to assign dynamic port numbers in case there are multiple connections made at the same moment.
- 

## Moving foward
- Complete publishing messages.
- Rigorous testing.
- Write supporting library for high-level languages: Java, Python, and JavaScript.

## 1. To set up server: 

Locate to the server folder, then type:
```
make all
```

then,
```
./build/bin/release/server --hostname localhost:[portNo]
```

For example:

```
./build/bin/release/server --hostname localhost:8081
```

The server will be running on port 8081 of the current machine. In case port 8081 is occupied, consider switching to another port.

-------------------------------

## 2. Publishing message from client:

Locate to the client server, then type:

```
make all
```
then,
```
./build/bin/release/client --hostname [host's IP]:[portNo] --publish --topic [topicName] [message]
```

For example,

```
./build/bin/release/client --hostname 192.168.0.1:8081 --publish --topic Testing HelloWorld
```

Here, we are publishing the message HelloWorld into the topic Testing. Note that topic name is unique

-------------------------------

## 3. Client subscribe to a channel to receive message

To receive a message, a client has to subscribe to a message

------------------------------
## Reference:
[1] https://mqtt.org/

[2] https://www.tutorialspoint.com/unix_sockets/socket_quick_guide.htm

[3] https://cp-algorithms.com/string/string-hashing.html

