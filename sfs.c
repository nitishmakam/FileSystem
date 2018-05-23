#include "globalConstants.h"
#include "sfs.h"
#include "utility.h"

extern inode * currdirectory;
extern inode * root;
extern filetable FileTable[5];
extern int currentshellpid;
extern superblock sfssuperblock;

int mkfs(int mount)
{
  if(mount == 1)
  {
    if(automount())
    {
      printf("Mounted the previous filesystem\n");
      return 1;
    }
    else
    {
      printf("Failed to mount the previous filesystem\n");
      return 0;
    }
  }
  else
  {
    if(superblockInit())
    {
      printf("Supeblock Init Sucessfull\n");
    }
    else
    {
      printf("Superblock Init failed\n");
      return 0;
    }
  }
  return 1;
}

int sfscreate(char * name)
{
    return createInode(name,"file");
}


int sfsdelete(char *name)
{
  inode *file=getInodeFromCurrDirectory(name,"file");
  flushfiletable(file);
  if(deleteInode(file))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int sfsopen(char * name,int who)
{
    inode * file= getInodeFromCurrDirectory(name,"file");
    if(addEntry(file,who)==1)
    {
      file->fdcount++;
      return 1;
    }
    else
    {
      printf("couldnt add the entry into the filetable for some reason! :(\n");
      return 0;
    }


}

int sfsclose(char * name , int who)
{
  inode * file= getInodeFromCurrDirectory(name,"file");
  return removeEntry(file,who);
}


int sfschangedir(char * name)
{
  if(name==NULL)
  {
    //cd into root
    currdirectory=root;
    printf("Current directory is now %s\n",currdirectory->name);
    return 1;
  }
  else if(strcmp(name,"..")==0 && strcmp(currdirectory->name,"root")==0)
    return 0; //Cannot change to parent directory of the root

  else if(strcmp(name,"..")==0)
  {
    //go to parent directory
    currdirectory=&sfssuperblock.inodes[currdirectory->parent];
    printf("Current directory is now %s\n",currdirectory->name);
    return  1;
  }
  else
  {
    inode *curr=getInodeFromCurrDirectory(name,"directory");
    if(curr==NULL)
    {
      return 0;
    }
    else
    {
      currdirectory=curr;
      printf("Current directory is now %s\n",currdirectory->name);
      return 1;
    }
  }
}

int sfsreaddir()
{
  for(int i=0;i<5;i++)
  {
    if(currdirectory->inodeList[i]!=-1)
    {
      if(strcmp(sfssuperblock.inodes[currdirectory->inodeList[i]].type,"file")==0)
      {
        printf("%s -- %c\n",sfssuperblock.inodes[currdirectory->inodeList[i]].name,'F');
      }
      else if(strcmp(sfssuperblock.inodes[currdirectory->inodeList[i]].type,"directory")==0)
      {
        printf("%s -- %c\n",sfssuperblock.inodes[currdirectory->inodeList[i]].name,'D');
      }
    }
  }
  return 1;
}

char *sfsread(char *filename,int who,int nbytes)
{
  inode *file = getInodeFromCurrDirectory(filename,"file");
  if(file == NULL)
  {
    printf("cant get this file from the current directory!\n");
    return NULL;
  }
  else
  {
    int fd= file->id;
    filetable * entry=getEntry(fd,who);
    if(entry==NULL)
    {
      printf("entry doesnt exist!\n");
    }
    int currfilepointer=entry->currfilepointer;
    int filesize=getSizeOfFile(file);
    if(nbytes>filesize)
    {
      printf("cant read %d characters from this file!\n",nbytes);
      return NULL;
    }
    char *content = readDataBlocks(file);
    char *toreturn=(char *)malloc(sizeof(char)*nbytes);
    for(int i=0;i<nbytes;i++)
    {
      toreturn[i]=content[currfilepointer+i];
    }
    return toreturn;
  }

}

int sfswrite(char *filename,int who,char *content)
{
  inode *file=getInodeFromCurrDirectory(filename,"file");
  if(file==NULL)
  {
    printf("File doesn't exist!\n");
    return 0;
  }
  if(file->noOfDatablocks==5)
  {
    printf("max file size exceeded!\n");
    return 0;
  }
  int length2=strlen(content);
  filetable * entry=getEntry(file->id,who);
  int currfilepointer=entry->currfilepointer;
  char *blockcontent = (char *)malloc(sizeof(char) * strlen(content));
  strcpy(blockcontent, content);
  if(file->datablocksarray!=NULL)
  {
    int filesize = getSizeOfFile(file);
    blockcontent = readDataBlocks(file);
    if(!unlinkDataBlock(file))
    {
      printf("couldnt unlink all the datablocks!\n");
      return 0;
    }
    if(currfilepointer==filesize)
    {
      blockcontent=realloc(blockcontent,filesize+length2);
      strcat(blockcontent,content);
    }
    else if(filesize>(currfilepointer+length2))
    {
      for(int i=0;i<length2-1;i++)
      {
        blockcontent[currfilepointer+i]=content[i];
      }
    }
    else
    {
      blockcontent=realloc(blockcontent,currfilepointer+length2);
      strcpy(&blockcontent[currfilepointer],content);
    }
  }
  for(int i=0;i<5;i++)
  {
    if(sfssuperblock.datablocks[file->datablocksarray[i]].id!=0)
    {
      printf("unlinking error!\n");
      return 0;
    }
  }
  int contentlen=strlen(blockcontent);
  int noofblocksreq;
  if(((contentlen-1)%DATABLOCK_SIZE)==0)
  {
    noofblocksreq=(contentlen-1)/DATABLOCK_SIZE;
  }
  else
  {
    noofblocksreq=(int)(floor(((double)(contentlen-1))/DATABLOCK_SIZE))+1;//dont know how to get floor so rewrite correctly
  }
  char * currblockcontent=(char *)malloc(sizeof(char)*DATABLOCK_SIZE);
  strncpy(currblockcontent,blockcontent,DATABLOCK_SIZE);
  printf("block 1 contains --%s-- \n",currblockcontent);
  file->noOfDatablocks=noofblocksreq;
  datablock *temp=writeDataBlock(currblockcontent);
  file->datablocksarray[0]=temp->id;
  strcpy(sfssuperblock.datablocks[file->datablocksarray[0]].data,temp->data);
  sfssuperblock.datablocks[file->datablocksarray[0]].currsize=temp->currsize;
  for(int i=1;i<noofblocksreq-1;i++)
  {
    strncpy(currblockcontent,&blockcontent[i*DATABLOCK_SIZE],DATABLOCK_SIZE);
    printf("block %d contains --%s-- \n",i+1,currblockcontent);
    datablock *temp=writeDataBlock(currblockcontent);
    file->datablocksarray[i]=temp->id;
    strcpy(sfssuperblock.datablocks[file->datablocksarray[i]].data,temp->data);
    sfssuperblock.datablocks[file->datablocksarray[i]].currsize=temp->currsize;
  }
  //last datablock to be written
  if(noofblocksreq>=2)
  {
    strcpy(currblockcontent,&blockcontent[(noofblocksreq-1)*DATABLOCK_SIZE]);
    printf("block %d contains --%s-- \n",noofblocksreq,currblockcontent);
    datablock *temp=writeDataBlock(currblockcontent);
    file->datablocksarray[noofblocksreq-1]=temp->id;
    strcpy(sfssuperblock.datablocks[file->datablocksarray[noofblocksreq-1]].data,temp->data);
    sfssuperblock.datablocks[file->datablocksarray[noofblocksreq-1]].currsize=temp->currsize;
  }
    entry->currfilepointer=contentlen-1;
    printf("the current filepointer is set to --%d--\n",entry->currfilepointer);
    printf("total no of datblocks in the file now are --%d--\n",file->noOfDatablocks);
    return 1;
}

int sfsmkdir(char * name)
{
    return createInode(name,"directory");
}

int sfsrmdir(char * name)
{

  inode *file=getInodeFromCurrDirectory(name,"directory");
  return deleteInode(file);
}

int sfslseek(char *name,int who,int offset)
{
  inode * file=getInodeFromCurrDirectory(name,"file");
  if(file ==NULL)
  {
    return 0;
  }
  else
  {
    filetable * entry=NULL;
    entry =getEntry(file->id,who);
    if(entry==NULL)
    {
      return 0;//not present in the filetable so cant sfslseek
    }
    else
    {
      entry->currfilepointer=offset;
      printf("set currfilepointer of file %s to --%d--\n",name,entry->currfilepointer);
      return 1;
    }
  }
  return 0;
}
