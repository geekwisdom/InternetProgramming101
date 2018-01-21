#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>

#define TRUE 1

void sigchld_handler(int s) {
  while(waitpid(-1, NULL, WNOHANG) > 0);
}

char * Process(char *Sentence)
{
//implement an interpreter pattern!
char *Command = malloc (5000 * sizeof(char));
char *Result = malloc(10000 * sizeof(char));
char Parameter1[100];
char Parameter2[100];
sscanf(Sentence,"%s %s %s",Command,Parameter1,Parameter2);
if (strcmp(Command,"ECHO") ==0)
 {
  //echo back to system
  strcpy(Result,Parameter1);
  return Result;
 }
else if (strcmp(Command,"ADD") ==0)
{
int number1=0;
int number2=0;
sscanf(Parameter1,"%d",&number1);
sscanf(Parameter2,"%d",&number2);
int Sum=number1+number2;
sprintf(Result,"Result is: %d",Sum);
return Result;
}
else if (strcmp(Command,"QUIT") ==0)
{
strcpy(Result,"GOODBYE!");
return Result;
}
else
 {
sprintf(Result,"Bad Command: %s",Sentence);
return Result;
 }
}



int main(void) {
  // create a TCP Stream socket
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) return -1;
  
  // set socket option: reuseaddr
  int on = 1;
  if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
    return -1;
  }
  
  // create an address and bind the socket to it
  struct sockaddr_in sa;
  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(1986);
  sa.sin_addr.s_addr = htonl(INADDR_ANY);
  char *bind_address = "127.0.0.1";
  if (inet_aton(bind_address, &sa.sin_addr) == 0) {
    printf("invalid bind address\n");
    close(s);
    return -1;
  }
  
  // bind socket to sa
  if (bind(s, (struct sockaddr*)&sa, sizeof(sa)) == -1) {
    printf("can't bind on this address.\n");
    close(s);
    return -1;
  }
  
  // ok, socket has been created and binded to an address, now listen on this socket now...
  if (listen(s, 511) == -1) { // this magic 511 backlog value is fron nginx
    printf("ERROR: listen: %s\n", strerror(errno));
    close(s);
    return -1;
  }
  
  // register signal handler
  struct sigaction sig_a;
  sig_a.sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sig_a.sa_mask);
  sig_a.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sig_a, NULL) == -1) {
    printf("register signal handler failed.\n");
    return -1;
  }
  
  // all right, now we listen for new connections on this socket and handle them in a infinite loop
  struct sockaddr_in peer_addr;
  socklen_t addr_len = sizeof(peer_addr);
  while(TRUE) {
    printf("trying to accept connections, pid: %d...\n", getpid());
    int new_socket = accept(s, (struct sockaddr*)&peer_addr, &addr_len);
    if (new_socket == -1) {
      perror("can't accept connection on socket");
      exit(-1);
    }
    printf("server: accepted connection from %s\n", inet_ntoa(peer_addr.sin_addr));
    pid_t pid = fork();
    if (pid == -1) {
      perror("fork");
      exit(0);
    }
    
    if (pid == 0) {
      // in child process
      close(s);
      printf("child process pid: %d\n", getpid());
      printf("Child process takes over the socket handling task...\n");
      char *response = "[message from server] Master, What is your command?: ";
      int len = strlen(response);
      int bytes_sent = send(new_socket, response, len, 0);
      if (bytes_sent == -1) {
        perror("write error on socket!\n");
        close(new_socket);
        exit(-1);
      } else if (bytes_sent == 0) {
        // ?
      } else if (bytes_sent > 0) {
        printf("Great! message was sent to client!\n");
        printf("bytes to send: %d\n", len);
        printf("actually sent: %d\n", bytes_sent);
      }
	char client_message[2000];
	int read_size=0; 
if( (read_size = recv(new_socket , client_message , 2000 , 0)) > 0 )
    {
        //Process the command
        char *outputmsg = Process(client_message);
        write(new_socket , outputmsg , strlen(outputmsg));
        write(new_socket , "\n" , strlen("\n"));
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }     
 printf("ok, nothing interesting to to, we're going to close the socket...\n");
      close(new_socket);
      printf("socket closed, exit...\n");
      exit(0);
    } else if (pid > 0) {
      // in parent
      printf("[parent] in parent process...\n");
      close(new_socket);
    }
  }
  
  return 0;
}
