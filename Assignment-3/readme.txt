Client:- <executableCode> <ServerIP> <ServerPort>
Server:- <executableCode> <ServerPort>

No need to compile encode.c and decode.c as they only have functions which are finally imported
in server and client files.

Compiling the code:

gcc server -o server
gcc client -o client

-- How to run the Program:-

- Start the server which will take one command line argument as port number on which it would work.
- Start the client which will take 2 command line arguments of which the first is server ip address and the second is server the port number.

You can run both client and server on the same computer as-

Example:
server : ./server 5000
client1 : ./client 127.0.0.1 5000
client2 : ./client 127.0.0.1 5000


						Properties of the presented code

- Server can handle multiple clients simultaneously and it uses Base64 encoding and both client server will work on TCP sockets.
- At first the server will be waiting for TCP connection from client.
- Client will then connect to the server using server's TCP port that is already known to the client.
- When the connection is setup successfully, the client will receive input msg from user which is typed in the terminal by the user and encodes it using Base64 encoding.
- When the encoded message is computed, the client sends the message to the server by writing to the socket on which the server can read.
- When the message is received by the server, it prints the original message and the decoded message and then sends the ACK to the client.
- Client and server remain in the loop to send any number of messages which the client wishes.
- When the client wants to close the connection, it sends message to the server and the TCP connections on both are closed gracefully by releasing the socket resources.

						Points :

- Maximum message length can be 1999 (2000 including '\0').
- Type-1 message: Normal client message
  Type-2 message: server acknowledgement
  Type-3 message: client close connection message
- Type-1 message is completed at the first occurence of '\n'

