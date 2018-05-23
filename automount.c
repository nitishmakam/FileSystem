#include "sfs.h"
#include "utility.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/time.h>
#include<unistd.h>
#include "globalConstants.h"

extern filetable FileTable[5];
extern superblock sfssuperblock;
extern inode *currdirectory;
int superblockDiskRead(char *disk)
{
    FILE *file = fopen(disk, "rb");
    if(file == NULL)
    {
        return 0;
    }
    if(fseek( file , L_SUPERBLOCK , SEEK_SET) != 0 )
    {
        return 0;
    }
    if(fread(&sfssuperblock , sizeof(superblock), 1 , file) != 1)
    {
        return 0;
    }
    fclose(file);
    currdirectory=&sfssuperblock.inodes[0];
    return 1;
}

int filetableDiskRead(char *disk)
{
    FILE *file = fopen(disk, "rb");
    if(file == NULL)
    {
        return 0;
    }
    if(fseek( file , L_FILETABLE , SEEK_SET) != 0 )
    {
        return 0;
    }
    if(fread(FileTable, sizeof(filetable)*5 , 1 , file) != 1)
    {
        return 0;
    }
    fclose(file);
    return 1;
}

int automount()
{
  char *disk="PersistantDisk.txt";
  if(!superblockDiskRead(disk))
  {
    return 0;
  }
  if(!filetableDiskRead(disk))
  {
    return 0;
  }
  return 1;
}
