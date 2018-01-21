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
FILE *myfile;
char thecommand[25];
myfile = fopen ("commands.dat","r");

if (myfile != NULL)
 {
	char * eof = fgets(thecommand,sizeof(thecommand),myfile);
	while (eof != NULL)
		{
		char *Response = Process(thecommand);
		fprintf(stdout,"%s\n",Response);
		eof = fgets(thecommand,sizeof(thecommand),myfile);
		}
  fclose(myfile);
 }
}
