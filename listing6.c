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

char *processfile(char *filename)
{
char * buffer = 0;
long length;
char thefile[200];
char paramname[200];
char thevalue[200];
int filemode;
filemode=1;
fprintf(stdout,"********* inside process file ************\n");
char *newfile;
newfile=malloc(500);
int ques=strpos(filename,"?");
if (ques > 0) strncpy(newfile,filename,ques);
else strcpy(newfile,filename);
char arg[100] = {0};
char value[100] = {0};
sscanf(filename, "%*[^=]%*c%[^&]%*[^=]%*c%s", arg, value);
printf("filename is %s\n",newfile);
printf("arg is %s\n value is %s\n", arg, value);
newfile++;
if (newfile[strlen(newfile)-1] == '\n') newfile[strlen(newfile)-1]= 0;
FILE *f;
if (access(newfile, X_OK) != -1) {
char command[400];
strcpy(command,"./");
strcat(command,newfile);
strcat(command, " ");
strcat(command,arg);
strcat(command," " );
strcat(command,value);
f = popen(command,"r");
filemode=0;
}
else
{
f = fopen (newfile, "rb");
}
if (f)
{
fprintf(stdout,"Reading file %s\n",newfile);

  if (filemode) fseek (f, 0, SEEK_END);
  if (filemode)length = ftell (f);
  if (filemode)fseek (f, 0, SEEK_SET);
  if (filemode) buffer = malloc (length);
  else buffer=malloc(50000);
  if (buffer)
  {
    fread (buffer, 1, length, f);
  }
  fclose (f);
}
return buffer;
}


char * WebProcess(char *Sentence)
{
//implement an interpreter pattern!
char *Command = malloc (5000 * sizeof(char));
char *Result = malloc(10000 * sizeof(char));
char Parameter1[100];
char Parameter2[100];
sscanf(Sentence,"%s %s %s",Command,Parameter1,Parameter2);
if (strcmp(Command,"GET") ==0)
 {
  //retrieve the file and send details back to client
  char *filecontents = processfile(Parameter1);
  char *okaymessage="HTTP/1.1 200 OK\n\n";
  strcpy(Result,okaymessage);
  strcat(Result,filecontents);
  return Result;
 }
else if (strcmp(Command,"POST") ==0)
{
strcpy(Result,"NOT IMPLEMENTED");
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
  sa.sin_port = htons(8888);
  sa.sin_addr.s_addr = inet_addr("0.0.0.0");
//  sa.sin_addr.s_addr = htonl(INADDR_ANY);
   printf("bind addr = %s",inet_ntoa(sa.sin_addr));
  
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
      char *response = "\n";
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
        //Send the message back to client
        char *outputmsg = WebProcess(client_message);
        write(new_socket , outputmsg , strlen(outputmsg));
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

int strpos(char *haystack, char *needle)
{
   char *p = strstr(haystack, needle);
   if (p)
      return p - haystack;
   return -1;   // Not found = -1.
}

