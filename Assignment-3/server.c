#include "decode.c"
#include "encode.c"

#define LEN 2000

/*
	General sockaddr_in struct
	struct sockaddr_in{
	  short   sin_family; 
	  u_short sin_port;
	  struct  in_addr sin_addr;
	  char    sin_zero[8]; 
	};
*/

char * close_message= "close_connection";
char * receive_message= "Message Received !!\n";
char * server_close_message = "Connection Closed !!";
char * acknowledgement_message = "ACK";

// function for clearing old buffer in message
void initialise(char *p,char *q)
{
	int i;
	for(i=0; i<LEN; i++)
	{
		p[i] = '\0';
		q[i] = '\0';
	}
	q[i]='\0';
}

// handles connection for each client
void clientController(int sockfd,struct sockaddr_in *client){
	char message[LEN]; // stores the message that the client sends to the server.
	char *ip = inet_ntoa(client->sin_addr); // ip of the client.
	int port = client->sin_port; // port number of the client.
	char buffer[LEN+1]; // for message_type+message

	// endless loop for communicating any number of messages
	// closes when the client wants
	while(1) {
		initialise(message,buffer); // clear old buffer
		int st = read(sockfd,message,LEN); // reading message from client
		write(1,"\n",1);
		write(1,receive_message,strlen(receive_message));

		printf("%s:%d - Coded Message:   ",ip,port);
		fflush(stdout); // flush printf buffer
		write(1,message+1,st-1); // printing coded message from client to terminal
		write(1,"\n",1);

		char *decoded_message = base64ToASCII(message+1); // decoding client message leaving message type

		printf("%s:%d - Decoded Message: ",ip,port);
		fflush(stdout);
		write(1,decoded_message,strlen(decoded_message)); // printing decoded message to terminal
		write(1,"\n",1);

		// if client wants to close the connection
		if(message[0]=='3'){
			printf("%s:%d - ",ip,port);
			fflush(stdout);
			write(1,server_close_message,strlen(server_close_message)); // connection close message
			write(1,"\n",1);
			break;
		}

		char * ack = asciiToBase64(acknowledgement_message); // encoding acknowledgement message
		snprintf(buffer,sizeof(buffer),"%c%s",'2',ack); // prepending type to encoded ack message
		write(sockfd,buffer,strlen(buffer)); // write to client through socket
	}
	close(sockfd); // free socket resources
	exit(0);
}

int main(int argc, char *argv[]){

	struct sockaddr_in client, server; // storing server,client internet addresses
	int sock = socket(AF_INET,SOCK_STREAM,0); 
	// sock variable stores the value returned by the socket 
	// system call. The socket() system call creates a new socket. It takes three arguments. 
	// The first is the address domain of the socket. AF_INET is used for Internet domain 
	// (for any two hosts on the Internet). Other common domain is UNIX domain.
	// The second argument is the type of socket. "Stream socket" (SOCK_STREAM) in which characters are read in 
	// a continuous stream, and a Datagram Socket (SOCK_DGRAM) in which messages are read in chunks.
	// The third argument is the protocol. '0' means (OS will choose the most appropriate protocol). It 
	// will choose TCP for stream sockets and UDP for datagram sockets. 
	// If the socket call fails, it returns -1.

	// shows customised error when socket system call fails by catching error directly from stderr
	if(sock==-1)
	{
		perror("SOCKET FAILURE");
		exit(-1);
	} 

	server.sin_family = AF_INET;  //code for the address family.
	server.sin_addr.s_addr = INADDR_ANY; //this field specifies address of the host. 
	// INADDR_ANY : the socket accepts connections to all the IPs of the machine.

	server.sin_port = htons(atoi(argv[1])); // // converts port number integer to network byte order 
	bzero(&server.sin_zero,8); // intialising sin_zero
	socklen_t len = sizeof(struct sockaddr_in); // len is the length of the socket address.
	// The bind() system call binds a socket to an address, in this case the address of the current host 
	// and port number on which the server will run. 
	// This can fail for a number of reasons, the most obvious being that this socket is already in use 
	// on this machine. Error is displayed on stderr and then program is aborted. 
	if(bind(sock,(struct sockaddr *) &server, len)==-1){
		perror("BIND FAILURE");
		exit(-1);
	} 
	
	// The listen system call allows the process to listen on the socket for connections. 
	// The first argument is the socket file descriptor, and the second is the size of the backlog queue, 
	// i.e., the number of connections that can be waiting while the process is handling a particular connection. 
	// The maximum size permitted by most systems is 5.

	if(listen(sock,5)==-1){
		perror("LISTEN FAILURE");
		exit(-1);
	} 

	// server keeps waiting for connections 
	while(1){
		int csocket = accept(sock,(struct sockaddr *) &client,&len); 
		// The accept() system call causes the process to block until a client connects to the server. 
		// Thus, it wakes up the process when a connection from a client has been successfully established. 
		// It returns a new file descriptor, and all communication on this connection should be done using
		//  the new file descriptor. The second argument is a reference pointer to the address of the client
		// on the other end of the connection.
 
 		// shows customised error when accept system call fails by catching error directly from stderr
		if(csocket==-1){
			perror("ACCEPT FAILURE");
			exit(-1);
		}

		// creating child process so that all clients can run independently
		int fork_flag=fork(); 
 
 		// child process used to communicate with the current client with whom the connection was accepted.
		if(fork_flag==0){
			close(sock);
			clientController(csocket,&client);
			break;
		} 
		else if(fork_flag==-1){
			printf("FORKING ERROR\n");
			break;
		} // This shows that the creation of a child process was unsuccessful.
		else{
			close(csocket); // returned to parent process. The parent process closes csocket. 
			// Server will return to the accept statement to wait for the next connection. 
			// And so due to this the concurrent server requirement is fulfilled.
		}
	}
	return 0;
}
