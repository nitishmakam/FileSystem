#include "globalConstants.h"
#include "utility.h"
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

extern inode *root;
extern inode *currdirectory;
extern superblock sfssuperblock;

int createInode(char * name, char * type)
{
    inode *newInode = getInode();//from superblock
    if(newInode==NULL)
    {
        return 0;
    }
    if(currdirectory->noOfInodes==0)
    {
      currdirectory->inodeList[0]=newInode->id;
      currdirectory->noOfInodes++;
      strcpy(newInode->name,name);
      strcpy(newInode->type,type);
      newInode->createtime=time(0);
      newInode->noOfDatablocks=0;
      newInode->noOfInodes=0;
      newInode->parent=currdirectory->id;
      newInode->fdcount=0;
      return 1;
    }
    else
    {
      int i=0;
      while(currdirectory->inodeList[i]!=-1)
      {
        if(strcmp(sfssuperblock.inodes[currdirectory->inodeList[i]].name,name)==0)
        {
          return 0;
        }
        i++;
      }
      if(i<5)
      {
        currdirectory->inodeList[i]=newInode->id;
        strcpy(newInode->name,name);
        strcpy(newInode->type,type);
        newInode->createtime=time(0);
        newInode->noOfDatablocks=0;
        newInode->noOfInodes=0;
        newInode->parent=currdirectory->id;
        newInode->fdcount=0;
        //Increasing the count of the number of inodes
        currdirectory->noOfInodes+=1;
        return 1;
      }
      return 0;
    }


}

int deleteInode(inode *entry)
{
    if(entry->noOfInodes!=0)
    {
      return 0;
    }
    if(strcmp(entry->type,"file")==0)
    {
      unlinkDataBlock(entry);
    }
    for(int i=0;i<5;i++)
    {
      if(currdirectory->inodeList[i]==entry->id)
      {
        sfssuperblock.inodelist[entry->id]=0;
        sfssuperblock.no_of_free_inodes++;
        currdirectory->noOfInodes--;
        for(int k=i;k<4;k++)
        {
          currdirectory->inodeList[k]=currdirectory->inodeList[k+1];
        }
        currdirectory->inodeList[4]=-1;
        return 1;
      }
    }
    return 0;
}
