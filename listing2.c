#include<stdio.h>
#include<string.h>
#include<stdlib.h>

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

void main()
{
int done=0;
while (! done)
 {
  char Line[200];
  fprintf(stdout,"Master What is your command?: ");
fgets(Line,sizeof(Line),stdin);
char *Response = Process(Line);
fprintf(stdout,"%s\n",Response);
if (strcmp(Response,"GOODBYE!") == 0) done = 1;
   
 }
}

int strpos(char *haystack, char *needle)
{
   char *p = strstr(haystack, needle);
   if (p)
      return p - haystack;
   return -1;   // Not found = -1.
}
