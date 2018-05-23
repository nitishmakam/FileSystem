#include "globalConstants.h"
#include "utility.h"
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>


extern superblock sfssuperblock;

char * readDataBlocks(inode *file)
{
    if(strcmp(file->type,"file") == 0)//only if it is a file
    {
        char * concatenatedResult = (char *)malloc(sizeof(char)) ;
        char  *dataBlockContent = (char *)malloc(sizeof(char)*DATABLOCK_SIZE) ;//store intermediate data
        int currdatablocklen=0;
        int count=0;
        while(file->datablocksarray[count]!=-1)
        {
            strcpy(dataBlockContent , sfssuperblock.datablocks[file->datablocksarray[count]].data);
            currdatablocklen += strlen(dataBlockContent);
            concatenatedResult=(char *)realloc(concatenatedResult,currdatablocklen);
            strcat(concatenatedResult,dataBlockContent);
            count++;
        }
        file->accesstime=time(0);
        return concatenatedResult;
    }
    return  NULL;

}


datablock* writeDataBlock(char *content)
{
      int length = strlen(content);
      datablock *newdb = getDatablock();
      strcpy(newdb -> data,content);
      newdb -> currsize = length;
      return newdb;
}

int unlinkDataBlock(inode *file)
{
  if(strcmp(file -> type, "file") == 0)
  {
    if(file->noOfDatablocks==0)
      return 1;
    int i=0;
    for(int i=0;i<5;i++)
    {
      sfssuperblock.datablocks[file->datablocksarray[i]].id = 0;
      file->datablocksarray[i]=-1;
    }
    file -> noOfDatablocks = 0;
    return 1;
  }
  else
  {
    return 0;
  }
}
