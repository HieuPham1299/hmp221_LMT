## Design for IPC

- Both parent and child process need 1 more socket to communicate with one another

- Child process would then listen to one socket, using the subroutine ```processClientConnection```

- Upon client's request, the child process will foward that byte array to the parent process

- Depending on the request type (Request or File), the parent process will read/write from/to the hashmap, and foward the response byte to the child process. The child process can use this to send to the client

- Parent process used the existing subroutines (from Cut2) to serve/receive files 
