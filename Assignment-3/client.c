#include "encode.c"
#include "decode.c"

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
char * server_message = "Message from server ";
char * terminate_message= "Connection Closed !!\n";
char * ask = "\nType your message - ";
char * acknowledgement_message = "ACK";

// function for clearing old buffer in message
void initialise(char *p,char *q){
	int i;
	for(i=0; i<LEN; i++)
	{
		p[i] = '\0';
		q[i] = '\0';
	}
	q[i]='\0';
}

int main(int argc, char *argv[]){

	struct sockaddr_in server; // structure containing an internet address.
	
	char response[2]; // stores response from user
	// 'y' : send more messages whereas 'n' : close the established connection.

	char *status = "Do you want to send message? - enter 'y' else 'n' and press enter: " ;
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

	char buffer[LEN+1]; // for message_type+message 

	// shows customised error when socket system call fails by catching error directly from stderr
	if(sock==-1){
		perror("SOCKET FAILURE");
		exit(-1);
	} 

	server.sin_family = AF_INET; //code for the address family.

	// gets ip address of server from the terminal arguments and convert it into network bytes
	if(inet_aton(argv[1],&server.sin_addr)==0){
		write(1,"error",strlen("error"));
	} 

	int pt = atoi(argv[2]); // gets port number from the terminal.
	server.sin_port = htons(pt); // converts port number integer to network byte order 
	socklen_t len = sizeof(struct sockaddr_in);  // len is the length of the socket address.

	// Establish a connection to the server. 
	// It takes three arguments, the socket file descriptor, the address of the host to 
	// which it wants to connect
	// This function returns 0 on success and -1 if it fails. 
	if(connect(sock,(struct sockaddr *) &server, len)==-1){
		perror("CONNECT ERROR");
		exit(-1);
	} 

	char message[LEN]; // This is used to store the message that the user would want to send to the server.

	// endless loop for communicating any number of messages
	// closes when the client wants
	while(1){
		
		write(1,status,strlen(status)); // For getting y or n from the user as defined earlier in the code.
		read(0,response,2);

		if(response[0]=='n'){ // user wants to close the connection.
			char * encoded_message = asciiToBase64(close_message); // Encode the closing message
			snprintf(buffer,sizeof(buffer),"%c%s",'3',encoded_message); // prepending the type of message to encoded message
			write(sock,buffer,strlen(buffer)); // writing close message to socket for server to receive
			write(1,terminate_message,strlen(terminate_message)); //  Connection close message printed on client console
			break; // stop communication
		}

		write(1,ask,strlen(ask)); // ask message from client
		initialise(message,buffer); // this initialisation is necessary so that message string doesn't 
		// contain characters from previous messages
		int i=0;
		while(1){ // reading client message character by character
			read(0,response,1); // reading character from console             
			if(response[0]=='\n'||i==LEN-1) // stop reading on newline character or max message size
				break;
			message[i++] = response[0]; // storing characters into message
		}

		char *encoded_message= asciiToBase64(message); // encode the message to Base64
		snprintf(buffer,sizeof(buffer),"%c%s",'1',encoded_message); // prepending type of message
		write(sock,buffer,strlen(buffer)); // writing full message to socket
		
		initialise(message,buffer);
		int received = read(sock,message,30); // read message from server
		strcpy(message+1,base64ToASCII(message+1)); // decoding the received message

		if(message[0]!='2'){ // if message is not an acknowledgement
			write(1,"Acknowledgement not received !!\nResend the message !!\n",
				strlen("Acknowledgement not received !!\nResend the message !!\n")); 
			continue; 
		}

		write(1,server_message,strlen(server_message));
		printf("%s:%d - ",argv[1],pt); // prints ip, port number of the server onto the terminal.
		fflush(stdout); // It is used to clear the buffer. 
		write(1,message+1,strlen(message)-1); // print ack message from server
		write(1,"\n\n",2);
	}

	close(sock); // release socket resources

return 0;
}
