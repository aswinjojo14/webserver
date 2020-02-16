/*


This is a very simple HTTP server. Default port is 10000 and ROOT for the server is your current working directory..

You can provide command line arguments like:- $./a.aout -p [port] -r [path]

for ex.
$./a.out -p 50000 -r /home/
to start a server at port 50000 with root directory as "/home"

$./a.out -r /home/aswin
starts the server at port 10000 with ROOT as /home/shadyabhi

*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>        	

//describes the the information obtained for a directory, file or device

#include<sys/socket.h>     

 	//This POSIX header file contains declarations for the types used by system-level calls to obtain file status or time information

#include<arpa/inet.h>     	 

/* The in_port_t and in_addr_t types shall be defined as described in <netinet/in.h>*/

#include<netdb.h>
#include<signal.h>

//The signal.h header defines a variable type sig_atomic_t, two function calls, and several macros to handle different signals reported during a program's execution.

#include<fcntl.h>       	//File opertions

#define CONNMAX 1000
#define BYTES 1024

char *ROOT;
int listenfd, clients[CONNMAX];
void error(char *);
void startServer(char *);
void respond(int);

int main(int argc, char* argv[])
{
    struct sockaddr_in clientaddr;

    //structure for handling internet addresses

    socklen_t addrlen;  	
   
 //socklen + an unsigned opaque integral type of length 32 bit
    char c;    
    
    //Default Values PATH = ~/ and PORT=10000
 
   char PORT[6];
    ROOT = getenv("PWD");   	

  //Getenv= return value of string PWD= print Working Directory
  
  strcpy(PORT,"10000");

    int slot=0;

    //Parsing the command line arguments

    while ((c = getopt (argc, argv, "p:r:")) != -1)
   	 switch (c)
   	 {
   		 case 'r':
   			 ROOT = malloc(strlen(optarg));
      //optarg=optional argument
   
			 strcpy(ROOT,optarg);
   			 break;
   		 case 'p':
   			 strcpy(PORT,optarg);
   			 break;
   		 case '?':
   			 fprintf(stderr,"Wrong arguments given!!!\n");
   			 exit(1);
   		 default:
   			 exit(1);
   	 }
    
    printf("Server started at port no. %s%s%s with root directory as %s%s%s\n","\033[92m",PORT,"\033[0m","\033[92m",ROOT,"\033[0m");

    // Setting all elements to -1: signifies there is no client connected
 
    int i;
    for (i=0; i<CONNMAX; i++)
   	 clients[i]=-1;
    startServer(PORT);

    // ACCEPT connections

    while (1)
    {
   	 addrlen = sizeof(clientaddr);
   	 clients[slot] = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);

   	 if (clients[slot]<0)
   		 error ("accept() error");
   	 else
   	 {
   		 if ( fork()==0 ) 

//Fork system call use for creates a new process, which is called child process, which runs concurrently with process (which process called system call fork) and this process is called parent process.
   

		 {
   			 respond(slot);
   			 exit(0);
   		 }
   	 }

   	 while (clients[slot]!=-1)
   	 
   		 slot = (slot+1)%CONNMAX;
    }

    return 0;
}

//start server

void startServer(char *port)
{
    struct addrinfo hints, *res, *p; 

//struct addrinfo is returned by getaddrinfo(), and contains, on success, a linked list of such structs for a specified hostname and/or service.

    // getaddrinfo for host
    memset (&hints, 0, sizeof(hints));  

 //memset=To fill a block of memory with 0.

    hints.ai_family = AF_INET; 

// This field specifies the desired address family(IPV4) for the returned addresses. Allow IPv4 or 
IPv6

    hints.ai_socktype = SOCK_STREAM;

//This field specifies the preferred socket type, for exam‐ple SOCK_STREAM or  SOCK_DGRAM.

    hints.ai_flags = AI_PASSIVE; 

//This field specifies additional options, described below.Multiple flags are specified by bitwise OR-ing them together.For wildcard IP address

    if (getaddrinfo( NULL, port, &hints, &res) != 0)

//Given node and service, which identify an Internet host and a service, getaddrinfo() returns one or more addrinfo structures, each of which contains an Internet address that can be specified in a call to bind(2) or connect(2).

    {
   	 perror ("getaddrinfo() error");
   	 exit(1);
    }

    // socket and bind

    for (p = res; p!=NULL; p=p->ai_next) //res= service
    {
   	 listenfd = socket (p->ai_family, p->ai_socktype, 0); 

//listenfd socket waiting for connection.
   
	 if (listenfd == -1) continue;
   	 if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) break;

 //The bind() function shall assign a local socket address address to a socket identified by descriptor socket that has no local socket address assigned. arguments: socket, sockaddr, address_len

    }
    if (p==NULL)
    {
   	 perror ("socket() or bind()");
   	 exit(1);
    }

    freeaddrinfo(res);

    // listen for incoming connections

    if ( listen (listenfd, 1000000) != 0 )

 //int listen(int socket, int backlog); The listen() function shall mark a connection-mode socket, specified by the socket argument, as accepting connections.The backlog argument provides a hint to the implementation which the implementation shall use to limit the number of outstanding connections in the socket's listen queue. Implementations may impose a limit on backlog and silently reduce the specified value. Normally, a larger backlog argument value shall result in a larger or equal length of the listen queue. Implementations shall support values of backlog up to SOMAXCONN, defined in <sys/socket.h>.

    {
   	 perror("listen() error");
   	 exit(1);
    }
}

//client connection
void respond(int n)
{
    char mesg[99999], *reqline[3], data_to_send[BYTES], path[99999];
    int rcvd, fd, bytes_read;

    memset( (void*)mesg, (int)'\0', 99999 );

    rcvd=recv(clients[n], mesg, 99999, 0); 

//recv - receive a message from a connected socket

    if (rcvd<0)	// receive error
   	 fprintf(stderr,("recv() error\n")); 

//stderr. The standard error stream is the default destination for error messages and other diagnostic warnings

    else if (rcvd==0)	// receive socket closed
   	 fprintf(stderr,"Client disconnected upexpectedly.\n");
    else	

// message received
    {
   	 printf("%s", mesg);
   	 reqline[0] = strtok (mesg, " \t\n"); //strtok= Splitting a string \t
   	 if ( strncmp(reqline[0], "GET\0", 4)==0 )
   	 {
   		 reqline[1] = strtok (NULL, " \t");
   		 reqline[2] = strtok (NULL, " \t\n");
   		 if ( strncmp( reqline[2], "HTTP/1.0", 8)!=0 && strncmp( reqline[2], "HTTP/1.1", 8)!=0 )
   		 {
   			 write(clients[n], "HTTP/1.0 400 Bad Request\n", 25);

//HTTP 1.0 does not officially require a Host header, but it doesn't hurt to add one, and many applications (proxies) expect to see the Host header regardless of the protocol version.
   
		 }
   		 else
   		 {
   			 if ( strncmp(reqline[1], "/\0", 2)==0 )
   				 reqline[1] = "index.html";    	

//Because if no file is specified, index.html will be opened by default (strlike it happens in APACHE...

   			 strcpy(path, ROOT);
   			 strcpy(&path[strlen(ROOT)], reqline[1]);
   			 printf("file: %s\n", path);

   			 if ( (fd=open(path, O_RDONLY))!=-1 )	//FILE FOUND
   			 {
   				 send(clients[n], "HTTP/1.0 200 OK\n\n", 17, 0);  	

//ssize_t send(int socket, const void *buffer, size_t length, int flags);The send() function shall initiate transmission of a message from the specified socket to its peer.
   				 while ( (bytes_read=read(fd, data_to_send, BYTES))>0 )

//The read() function shall attempt to read nbyte bytes from the file associated with the open file descriptor, fildes, into the buffer pointed to by buf.
   					 write (clients[n], data_to_send, bytes_read); 

//write is a system call that is used to write data out of a buffer.
   			 }
   			 else	write(clients[n], "HTTP/1.0 404 Not Found\n", 23); 
//FILE NOT FOUND
   		 }
   	 }
    }

    //Closing SOCKET
    shutdown (clients[n], SHUT_RDWR);     	//All further send and recieve operations are DISABLED...
    close(clients[n]);
    clients[n]=-1;
}



