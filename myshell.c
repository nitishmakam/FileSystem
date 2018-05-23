#include "sfs.h"
#include "utility.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include "globalConstants.h"

//purpose of this file is to mimic the shell
//and support the following commands and call the appropriate handlers
extern int currentshellpid;
extern superblock sfssuperblock;

int main()
{
	int i=0;
	int choice;
	char *disk = "PersistentDisk.txt";
	printf("Do you want to load the previous filesystem state?(1 for yes ,0 for no)\n");
	scanf("%d",&choice);
	currentshellpid=getpid();
	char *command=NULL;
	char *otherpartofinput=NULL;
	char input[50];
	char prompt[3]=">>\0"; //print this every line as the prompt of our shell
	if(!mkfs(choice))
	{
		printf("mkfs failed!\n");
		exit(0);
	}

	printf("\n\n---------------------Current Shell PID is %d---------------------\n\n",currentshellpid);
	printf("Welcome to Simple FileSystem\n");
	printSupportedCommands();


	while(1)
	{
		printf("%s",prompt);
		gets(input);
		if(strcmp(input,"quit")==0)
		{
			if(!dumpfs())
			{
				printf("couldnt save it for persistence!\n");
			}
			else
			{
				printf("Successfully saved the current state\n");
			}
			exit(0);
		}
		command=getCommandFromInput(input);
		if(strcmp(command,"touch")==0)
		{
			otherpartofinput=getOtherPartFromInput(input);
			if(!sfscreate(otherpartofinput))
			{
					printf("cannot create a file!!\n");
			}
		}
		else if(strcmp(command,"switch")==0)
		{
			char **tokens=split(input,2);
			int pid=atoi(tokens[1]);
			currentshellpid=pid;
			printf("\n\n---------------------Current Shell PID is %d---------------------\n\n",currentshellpid);
		}
		else if(strcmp(input,"ls")==0)
		{
			if(!sfsreaddir())
			{
				printf("empty!!\n");
			}
		}
		else if(strcmp(input,"showfiletable")==0)
		{
			showFileTableContents();
		}
		else if(strcmp(input,"showsuperblock")==0)
		{
			showSuperBlockStatus();
		}
		else if(strcmp(command,"mkdir")==0)
		{
			otherpartofinput=getOtherPartFromInput(input);
			if(!sfsmkdir(otherpartofinput))
			{
				printf("cant create a directory!!\n");
			}
		}
		else if(strcmp(command,"cd")==0)
		{
			otherpartofinput=getOtherPartFromInput(input);
			if(!sfschangedir(otherpartofinput))
			{
				printf("cant go inside this directory!!\n");
			}
		}
		else if(strcmp(command,"rmdir")==0)
		{
				otherpartofinput=getOtherPartFromInput(input);
				if(!sfsrmdir(otherpartofinput))
				{
					printf("directory not empty! can't delete!!\n");
				}
		}
		else if(strcmp(command,"rm")==0)
		{
				otherpartofinput=getOtherPartFromInput(input);
				if(!sfsdelete(otherpartofinput))
				{
					printf("cant delete this file!!\n");
				}
		}

		else if(strcmp(command,"close")==0)
		{
				char **tokens=split(input,2);
				char *filename=tokens[1];
				if(!sfsclose(filename,currentshellpid))
				{
					printf("cant close this file!!\n");
				}
		}

		else if(strstr(command,"write")==command)
		{
			char * content =(char *)malloc(sizeof(char)*MAX_CONTENT_LIMIT);
			printf("enter the content you want to write into this file\n");
			scanf("%[^\n]%*c",content);
			char ** tokens =split(input,2);
			char * name= tokens[1];
			strcat(content,"\n");
			if(!sfsopen(name,currentshellpid))
			{
				printf("cant open this file!\n");
			}
			showFileTableContents();
			if(!sfswrite(name,currentshellpid,content))
			{
				printf("cant write into this file!\n");
			}
			showFileTableContents();//debug line
			free(content);
		}

		else if(strstr(command,"read")==command)
		{
			char ** tokens =split(input,3);
			char * name= tokens[1];
			int nbytes =atoi(tokens[2]);
			int who=currentshellpid;
			if(!sfsopen(name,who))
			{
				printf("cant open this file!\n");
			}
			char * readstr=sfsread(name,who,nbytes);
			if(readstr==NULL)
			{
				printf("cant write into this file!\n");
			}
			printf("read string is :%s\n",readstr);
		}

		else if(strstr(command,"cat")==command)
		{
			char ** tokens =split(input,2);
			char * filename= tokens[1];
			inode *file=getInodeFromCurrDirectory(filename,"file");
		  if(file == NULL)
		  {
		    printf("file doesnt exist!\n");
		  }
			else
			{
				int who=currentshellpid;
				char *content = readDataBlocks(file);
				if(content == NULL)
				{
					printf("cant read all the content in this file!\n");
				}
				else
				{
					printf("%s",content);
				}
			}
		}
		else if(strstr(command,"lseek")==command)
		{
			char ** tokens =split(input,3);
			char *filename=tokens[1];
			int offset=atoi(tokens[2]);
			int who =currentshellpid;

			if(!sfslseek(filename,who,offset))
			{
				printf("Can't set file pointer \n");
			}
		}
		else if(i>0)
		{
			printf("wrong command ! please reenter the command!\n");
		}
		command="";
		otherpartofinput="";
		i++;
	}

	if(command!=NULL)
	{
		free(command);
	}
	if(otherpartofinput!=NULL)
	{
		free(otherpartofinput);
	}

	return 0;
}
