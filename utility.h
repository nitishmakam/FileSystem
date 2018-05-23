#ifndef UTILITY_H
#define UTILITY_H

#include "globalConstants.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//myshell utility functions
void printSupportedCommands();

char * getCommandFromInput(char * input);

char * getOtherPartFromInput(char * input);

char ** split(char * input,int noofwords);
//filesystem utility functions

inode * getInodeFromCurrDirectory(char *name,char *type);

//filetable utility functions

void showFileTableContents();

//inode utility functions

int getSizeOfFile(inode * file);

//datablock utility functions

//superblock utility functions
void showSuperBlockStatus();
//automount utility functions

//dumpfs utility functions
#endif
