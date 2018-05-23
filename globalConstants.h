#ifndef GLOBALCONSTANTS_H
#define GLOBALCONSTANTS_H

#define TOTAL_INODES 64
#define TOTAL_DATABLOCKS 64
#define DATABLOCK_SIZE 2//should be 128 but 2 for debugging purposes
#define FILETABLE_SIZE 20
#define MAX_CONTENT_LIMIT 100


#include <sys/time.h>
#include <pthread.h>

int currentshellpid;

//INODE RELATED
      typedef struct inode
      {

      	int id;
      	char name[32];
      	char type[10];//file or directory
      	time_t accesstime;
      	time_t createtime;
      	time_t modifytime;
      	int datablocksarray[5];
      	int noOfDatablocks;
      	//if inode is a directory you can have only a maximum of 5 directory entries
        int inodeList[5];
      	int noOfInodes;
      	int parent;
      	int fdcount;//for no of processes accessing this
        int n_link;
      }inode;

      int createInode(char * name, char * type);
      int deleteInode(inode * entry);

//DATABLOCK RELATED
      typedef struct datablock
      {

      	int id;
      	char data[DATABLOCK_SIZE];
      	int currsize;
      }datablock;

      char *readDataBlocks(inode *file);
      datablock *writeDataBlock(char* content);
      int unlinkDataBlock(inode *file);

//FILETABLE RELATED

      typedef struct filetable
      {
        int used;
      	int filedescriptor;//same as inode no for easier access
      	int who;
      	int currfilepointer;
      }filetable;
      //pid and filedescriptor are the primary key

      //filetable structure


      int addEntry(inode *file,int who);
      int removeEntry(inode *file,int who);
      int flushfiletable(inode *file);
      filetable *getEntry(int fd, int who);

//SUPERBLOCK RELATED

      //there are totally 64 inodes and 64 datablocks only
      typedef struct superblock
      {
          int no_of_free_inodes;
          int no_of_free_datablocks;
          int inodelist[TOTAL_INODES];
          inode inodes[TOTAL_INODES];
          int datablocklist[TOTAL_DATABLOCKS];
          datablock datablocks[TOTAL_DATABLOCKS];

      }superblock;

      //superblock of the fs

      //functions related to superblock
      int superblockInit();
      inode *getInode();
      datablock *getDatablock();

//DUMPFS related
int diskWrite(char *disk);


//AUTOMOUNT related
int superblockDiskRead(char *disk);
int filetableDiskRead(char *disk);

//Locations for disk write/read
#define L_SUPERBLOCK 0
#define L_FILETABLE (64*(sizeof(inode)+sizeof(datablock)+2*sizeof(int))+2*(sizeof(int)) + 100)//20 is extra hole size


int automount();
int dumpfs();

#endif
