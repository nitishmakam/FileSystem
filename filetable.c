#include "globalConstants.h"
#include "utility.h"
#include<stdlib.h>
#include<string.h>
#include<sys/time.h>

filetable FileTable[5];
extern int currentshellpid;

filetable* getEntry(int fd,int who)
{
  //traverse the filetable and search for the entry with file's inode id and the who and
  for(int i=0;i<5;i++)
  {
    if(FileTable[i].used==1)
    {
      if((FileTable[i].who==who)&&(FileTable[i].filedescriptor==fd))
      {
        return &FileTable[i];
      }
    }
  }
  return NULL;
}


int addEntry(inode *file,int who)
{
  if(getEntry(file->id,who)!=NULL)
  {
    //this process already has this file open!
    return 1;
  }
  int fd=file->id;
  int count=0;
  for(int i=0;i<5;i++)
  {
    if(FileTable[i].used==0)
    {
      FileTable[i].used=1;
      FileTable[i].filedescriptor=fd;
      FileTable[i].who=who;
      FileTable[i].currfilepointer=0;
      return 1;
    }
  }
  return 0;
}

int removeEntry(inode *file,int who)
{
  for(int i=0;i<5;i++)
  {
    if( (FileTable[i].filedescriptor == file->id) && (FileTable[i].who == who) )
    {
      for(int k=i;k<4;k++)
      {
        FileTable[i].who = FileTable[i + 1].who;
        FileTable[i].filedescriptor = FileTable[i + 1].filedescriptor;
        FileTable[i].used = FileTable[i + 1].used;
        FileTable[i].currfilepointer = FileTable[i + 1].currfilepointer;
      }
      FileTable[4].used = 0;
      FileTable[4].who = 0;
      FileTable[4].filedescriptor = 0;
      FileTable[4].currfilepointer = 0;
      return 1;
    }
  }
    return 0;
}


int flushfiletable(inode *file)
{
  //traverse this table and remove all the entries with the filedescriptor as this file
  int fd=file->id;
  for(int i=0;i<5;i++)
  {
    if( FileTable[i].filedescriptor == fd )
    {
      FileTable[i].used=0;
    }
  }
    return 1;
}
