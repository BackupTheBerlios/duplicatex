#include "xyzfile.h"

Cxyzfile::Cxyzfile(char * fp, char * fn)
{
    buffer = (char *) malloc(1025);
    sprintf(buffer, "%s%s", fp, fn);
    statbuffer = (struct stat *) malloc(sizeof(struct stat) * 2);
    if (!stat(buffer, statbuffer))
    {
        filename = (char *) malloc(strlen(fn) + 1);
        strcpy(filename, fn);
        filesize = statbuffer -> st_size;
        openpointer = true;
        printf("xyzfile(%d):%s\n", (int) statbuffer -> st_size, buffer);
        //ok
    }
    else
    {
        filename = NULL;
        filesize = 0;
        openpointer = false;
    }
}

Cxyzfile::~ Cxyzfile()
{
    free(buffer);
    if (filename)
    {
        free(filename);
        filename = NULL;
    }
}

void Cxyzfile::ReadInfos()
{
//the next one
}

unsigned long Cxyzfile::getFilesize()
{
    return filesize;
}

int Cxyzfile::Read(unsigned char * buffer, int bytestoread)
{
    return fpJpg -> Read(buffer, bytestoread);
}

void Cxyzfile::Seek(unsigned int bytes, wxSeekMode mode)
{
    fpJpg -> Seek(bytes, mode);
}

bool Cxyzfile::Eof()
{
    return fpJpg -> Eof();
}

