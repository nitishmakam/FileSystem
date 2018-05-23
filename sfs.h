#ifndef SFS_H
#define SFS_H

#include "globalConstants.h"
#include "utility.h"
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>


//1 for sucess 0 for failure
int mkfs(int mount);
//working
int sfscreate(char * name);
int sfsmkdir(char * name);
int sfschangedir(char * name);//for cd and cd ..
int sfsreaddir();//for ls
int sfsrmdir(char *name);
int sfsdelete(char *name);


//finished
int sfsclose(char * name,int who);
int sfsopen(char * name , int who);
int sfslseek(char *name,int who,int offset);


//todo
int sfswrite(char *filename,int who,char *content);//removed mode only write from now on no append mode
char * sfsread(char * name,int who,int nbytes);




#endif
