#include "utility.h"
#include<assert.h>

extern inode *root;
extern inode * currdirectory;
extern filetable FileTable[5];
extern superblock sfssuperblock;
extern int currentshellpid;

//myshell uttility functions
void printSupportedCommands()
{
  printf("\n");
  printf("Commands supported :\n");//write all the supported commands and its brief usage
  printf("ls     :list the current directory\n");
    printf("\tUsage : ls <dirname>\n");
  printf("cd     :change the current directory\n");
    printf("\tUsage : cd <dirname>\n");
  printf("rmdir  :delete a directory in the current directory\n");
    printf("\tUsage : rmdir <dirname>\n");
  printf("rm     :delete a file in the current directory\n");
    printf("\tUsage : rm <filename>\n");
  printf("lseek  :reset the currentfilepointer of the process opening a particular file \n");
    printf("\tUsage : lseek <filename> <offset>\n");
  printf("touch  :create a file in the current directory\n");
    printf("\tUsage : touch <filename>\n");
  printf("cat    :read a particular file in the current directory\n");
    printf("\tUsage : cat <filename>\n");
  printf("write  :write into a particular file in the current directory\n");
    printf("\tUsage : write <filename> and then enter the content\n");
  printf("read   :read nbytes from a particular file in the current directory\n");
    printf("\tUsage : read <filename> <nbytes>\n");
  printf("close  :close the file which was opened by the current terminal's process\n");
    printf("\tUsage : close <filename>\n");
  printf("showfiletable  :show contents of the filetable\n");
    printf("\tUsage : showfiletable\n");
  printf("showsuperblock  :show the current status of the superblock\n");
    printf("\tUsage : showsuperblock\n");
  printf("\n\n");
}

char ** split(char * input,int noofwords)
{
	char **result=(char **)malloc(sizeof(char *)*noofwords);
	char *token = strtok(input," ");
	int i=0;
	while (token != NULL)
	{
		result[i]=token;
		i++;
		token = strtok(NULL, " ");
	}
	if(token==NULL)
	{
		return result;
	}
}

char * getCommandFromInput(char * input)
{
    int i=0;
    int endpoint=0;
    int inputlen=strlen(input);
    for(i=0;i<inputlen;i++)
    {
        if(*(input+i)==' '||*(input +i)=='\n')
        {
            endpoint=i-1;
            break;
        }
    }
    char *command=(char *)(malloc(sizeof(char)*(endpoint+1)));
    strncpy(command,input,endpoint+1);
    return command;

}

char * getOtherPartFromInput(char * input)
{
    int i=0;
    int endpoint=0;
    int inputlen=strlen(input);
    for(i=0;i<inputlen;i++)
    {
        if(*(input+i)==' ')
        {
            endpoint=i+1;
            break;
        }
    }
    char *command=(char *)(malloc(sizeof(char)*(inputlen-endpoint)));
    for(i=endpoint;i<inputlen;i++)
    {
        *(command+(i-endpoint))=*(input+i);
    }
    return command;
}

//filesystem utility functions
inode * getInodeFromCurrDirectory(char *name,char *type)
{
  for(int i=0;i<5;i++)
  {
    if(strcmp(sfssuperblock.inodes[currdirectory->inodeList[i]].name,name)==0 && strcmp(sfssuperblock.inodes[currdirectory->inodeList[i]].type,type)==0)
    {
      return &sfssuperblock.inodes[currdirectory->inodeList[i]];
    }
  }
  return NULL;
}

//filetable utility functions

void showFileTableContents()
{
  printf("\n----------Showing all the filetable entries----------\n");
  for(int i=0;i<5;i++)
  {
    if(FileTable[i].used==1)
    {
      printf("\t-->filedescriptor:%d\tprocess_id:%d\tcurrfilepointer:%d\n",FileTable[i].filedescriptor,FileTable[i].who,FileTable[i].currfilepointer);
    }
  }
  printf("-------------------------------------------------------\n");
}


//inode utility functions

int getSizeOfFile(inode * file)
{
  int noOfDatablocks = file->noOfDatablocks;
  int size=0;
  for(int i=0;i<noOfDatablocks;i++)
  {
    size+=sfssuperblock.datablocks[file->datablocksarray[i]].currsize;
  }
  return size;
}

//datablock utility functions

//superblock utility functions
void showSuperBlockStatus()
{
  printf("\n----------------Showing superblock status------------\n");
  int free_inodes=sfssuperblock.no_of_free_inodes;
  int free_datablocks=sfssuperblock.no_of_free_datablocks;
  printf("no of free inodes : %d\n",free_inodes);
  printf("no of free datablocks : %d\n",free_datablocks);
  printf("bitmap of inodes pool\n");
  for(int i=0;i<TOTAL_INODES;i++)
  {
    printf("%d ",sfssuperblock.inodelist[i]);
  }
  printf("\nbitmap of datablocks pool\n");
  for(int i=0;i<TOTAL_DATABLOCKS;i++)
  {
    printf("%d ",sfssuperblock.datablocklist[i]);
  }
  printf("\n-------------------------------------------------------\n");
}
//automount utility functions

//dumpfs utility functions
