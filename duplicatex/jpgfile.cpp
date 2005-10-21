#include "jpgfile.h"

Cjpgfile::Cjpgfile(char * fp, char * fn)
{
    endmark = 0;
    jpgWidth = 0;
    jpgHeight = 0;
    jpgNoAscii = 0;
    buffer = (char *) malloc(1025);
    picbuf = NULL;
    sprintf(buffer, "%s%s", fp, fn);
    statbuffer = (struct stat *) malloc(sizeof(struct stat) * 2);
    if (!stat(buffer, statbuffer))
    {
        filename = (char *) malloc(strlen(fn) + 1);
        strcpy(filename, fn);
        filesize = statbuffer -> st_size;
        fpJpg = new wxFile(buffer, wxFile::read);
        if (fpJpg)
        {
            openpointer = true;
            //ok
        }
        else
        {
            openpointer = false;
        }
    }
    else
    {
        filename = NULL;
        filesize = 0;
        openpointer = false;
    }
}

Cjpgfile::~ Cjpgfile()
{
    if (fpJpg)
    {
        fpJpg -> Close();
        delete fpJpg;
    }
    free(buffer);
    if (filename)
    {
        free(filename);
        filename = NULL;
    }
    if (picbuf)
    {
        free(picbuf);
        picbuf = NULL;
    }
}

void Cjpgfile::ReadInfos()
{
    endmark = 0;
    if (openpointer)
    {
        if ((filesize > 32) && (filesize < 4096000))
        {
            if (picbuf)
            {
                free(picbuf);
            }
            picbuf = (unsigned char *) malloc(filesize + 1);
            int l, i, b, b_last = 0;
            int bytes = 0, block = 0;
            int reading = 0;
            fpJpg -> Seek(0, wxFromStart);
            do
            {
                block = filesize - bytes;
                if (block > 1024)
                {
                    block = 1024;
                }
                bytes += fpJpg -> Read(picbuf + bytes, block);
            }
            while (!fpJpg -> Eof());
            //printf("load(%d): %s\n", bytes, filename);
            i = 0;
            while (i < bytes)
            {
                l = 0;
                b = picbuf[i];
                switch (b)
                {
                case 0x00:
                    if (!reading)
                    {
                        b = b_last;
                    }
                    else
                    {
                        reading = 1;
                    }
                    i++;
                    jpgNoAscii++;
                    break;
                case 0xff:
                    if (reading)
                    {
                        reading = 2;
                    }
                    i++;
                    jpgNoAscii++;
                    break;
                default:
                    if (reading == 1)
                    {
                        //printf("%02x ", b);
                        i++;
                    }
                    else
                    {
                        reading = 0;
                        if (b_last == 0xff)
                        {
                            if ((i + 2) < bytes)
                            {
                                l = picbuf[i + 1] * 256 + picbuf[i + 2];
                            }
                            else
                            {
                                l = 0;
                            }
                            //printf("0xff %02x l=%d\n", b, l);
                            switch (b)
                            {
                            case 0xd0:
                            case 0xd1:
                            case 0xd2:
                            case 0xd3:
                            case 0xd4:
                            case 0xd5:
                            case 0xd6:
                            case 0xd7:
                                //RST0
                                l = 0;
                                break;
                            case 0xd8:
                                //SOI
                                l = 0;
                                break;
                            case 0xe0:
                                //app0
 /*
 printf("0xe0: l=%02x%02x fim=%02x%02x%02x%02x%02x "
 , picbuf[i + 1], picbuf[i + 2], picbuf[i + 3], picbuf[i + 4], picbuf[i + 5], picbuf[i + 6], picbuf[i + 7]);
 printf("mar=%02x mir=%02x den=%02x xden=%02x%02x yden=%02x%02x\n"
 , picbuf[i + 8], picbuf[i + 9], picbuf[i + 10], picbuf[i + 11], picbuf[i + 12], picbuf[i + 13], picbuf[i + 14]);
    */
                                break;
                            case 0xe1:
                            case 0xe2:
                            case 0xe3:
                            case 0xe4:
                            case 0xe5:
                            case 0xe6:
                            case 0xe7:
                            case 0xe8:
                            case 0xe9:
                            case 0xea:
                            case 0xeb:
                            case 0xec:
                            case 0xed:
                            case 0xee:
                            case 0xef:
                                break;
                            case 0xdb:
                                break;
                            case 0xdd:
                                break;
                            case 0xc0:
                                //SOF0
 /*
 printf("0xc0(SOF0): l=%02x%02x p=%02x h=%02x%02x w=%02x%02x\n"
 , picbuf[i + 1], picbuf[i + 2], picbuf[i + 3], picbuf[i + 4]
 , picbuf[i + 5], picbuf[i + 6], picbuf[i + 7]);
    */
                                jpgHeight = picbuf[i + 4] * 256 + picbuf[i + 5];
                                jpgWidth = picbuf[i + 6] * 256 + picbuf[i + 7];
                                jpgNoAscii += 64;
                                break;
                            case 0xc2:
                                //SOF2
 /*
 printf("0xc2(SOF2): l=%02x%02x p=%02x h=%02x%02x w=%02x%02x\n"
 , picbuf[i + 1], picbuf[i + 2], picbuf[i + 3], picbuf[i + 4]
 , picbuf[i + 5], picbuf[i + 6], picbuf[i + 7]);
    */
                                jpgHeight = picbuf[i + 4] * 256 + picbuf[i + 5];
                                jpgWidth = picbuf[i + 6] * 256 + picbuf[i + 7];
                                jpgNoAscii += 128;
                                break;
                            case 0xc4:
                                break;
                            case 0xda:
                                //start of scan
                                reading = 1;
                                break;
                            case 0xd9:
                                //EOI
                                l = 0;
                                bytes = - bytes;
                                break;
                            case 0xfe:
                                //COM
                                //kommentarblock
                                break;
                            default:
 /*
 printf("\n errorFF(%d)=%02x ", i, b);
    */
                                break;
                            }
                            i += l;
                        }
                        else
                        {
 /*
 printf("\n errorXX(%d)=%02x ", i, b);
    */
                        }
                        i++;
                    }
                    break;
                }
                b_last = b;
            }
            if (picbuf[filesize - 2] == 0xff)
            {
                if (picbuf[filesize - 1] == 0xd9)
                {
                    endmark++;
                }
            }
        }
    }
}

unsigned char * Cjpgfile::getPicbuf()
{
    return picbuf;
}

unsigned long Cjpgfile::getFilesize()
{
    return filesize;
}

int Cjpgfile::getWidth()
{
    return jpgWidth;
}

int Cjpgfile::getHeight()
{
    return jpgHeight;
}

bool Cjpgfile::isAscii()
{
    if (jpgNoAscii > 2)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool Cjpgfile::hasEndmark()
{
    if (endmark)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int Cjpgfile::Read(unsigned char * buffer, int bytestoread)
{
    return fpJpg -> Read(buffer, bytestoread);
}

void Cjpgfile::Seek(unsigned int bytes, wxSeekMode mode)
{
    fpJpg -> Seek(bytes, mode);
}

bool Cjpgfile::Eof()
{
    return fpJpg -> Eof();
}

