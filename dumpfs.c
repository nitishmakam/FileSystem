#include "globalConstants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern filetable FileTable[5];
extern superblock sfssuperblock;
extern inode * currdirectory;
int diskWrite(char *disk)
{
    FILE *file = fopen(disk, "w+b");
    if( file == NULL)
    {
        printf("failed to open the file for persistence!\n");
        return 0;
    }
    if( fseek( file , L_SUPERBLOCK , SEEK_SET) != 0 )
    {
        printf("failed to lseek for writing superblock\n");
        return 0;
    }
    if( fwrite(&sfssuperblock, sizeof(superblock), 1 , file) != 1)
    {
        printf("failed to write the superblock!\n");
        return 0;
    }

    if( fseek( file , L_FILETABLE , SEEK_SET) != 0 )
    {
        printf("failed to lseek for writing filetable\n");
        return 0;
    }
    if(fwrite(FileTable, sizeof(filetable)*5, 1 , file) != 1)
    {
        printf("failed to write filetable!\n");
        return 0;
    }
    fclose(file);
    return 1;
}

int dumpfs()
{
    char *disk="PersistantDisk.txt";
    return diskWrite(disk);
}
