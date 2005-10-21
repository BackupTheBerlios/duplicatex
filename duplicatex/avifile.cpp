#include "avifile.h"

Cavifile::Cavifile(char * fp, char * fn)
{
    aviKBitrate = 0;
    aviMicroSecPerFrame = 0;
    aviFramesPerSec = 0;
    aviMaxBitsPerSec = 0;
    aviReserved1 = 0;
    aviFlags = 0;
    aviTotalFrames = 0;
    aviInitialFrames = 0;
    aviStreams = 0;
    aviSuggestedBuffersize = 0;
    aviWidth = 0;
    aviHeight = 0;
    aviScale = 0;
    aviRate = 0;
    aviStart = 0;
    aviLength = 0;
    aviVIDScodec = 0;
    aviVIDSdwFlags = 0;
    aviVIDSdwReserved1 = 0;
    aviVIDSdwInitialFrames = 0;
    aviVIDSdwScale = 0;
    aviVIDSdwRate = 0;
    aviVIDSdwStart = 0;
    aviVIDSdwLength = 0;
    aviVIDSdwSuggestedBuffersize = 0;
    aviVIDSdwQuality = 0;
    aviVIDSdwSampleSize = 0;
    aviVIDSbiSize = 0;
    aviVIDSbiWidth = 0;
    aviVIDSbiHeight = 0;
    aviVIDSbiPlanes = 0;
    aviVIDSbiBitCount = 0;
    aviVIDSbiCompression = 0;
    aviVIDSbiSizeImage = 0;
    aviVIDSbiXPelsPerMeter = 0;
    aviVIDSbiYPelsPerMeter = 0;
    aviVIDSbiClrUsed = 0;
    aviVIDSbiClrImportant = 0;
    aviAUDScodec = 0;
    aviAUDSdwFlags = 0;
    aviAUDSdwReserved1 = 0;
    aviAUDSdwInitialFrames = 0;
    aviAUDSdwScale = 0;
    aviAUDSdwRate = 0;
    aviAUDSdwStart = 0;
    aviAUDSdwLength = 0;
    aviAUDSdwSuggestedBuffersize = 0;
    aviAUDSdwQuality = 0;
    aviAUDSdwSampleSize = 0;
    aviAUDSweFormatTag = 0;
    aviAUDSweChannels = 0;
    aviAUDSweSamplesPerSec = 0;
    aviAUDSweAvgBytesPerSec = 0;
    aviAUDSweBlockAlign = 0;
    aviAUDSweBitsPerSample = 0;
    aviAUDSweExSize = 0;
    buffer = (char *) malloc(1025);
    sprintf(buffer, "%s%s", fp, fn);
    statbuffer = (struct stat *) malloc(sizeof(struct stat) * 2);
    if (!stat(buffer, statbuffer))
    {
        filename = (char *) malloc(strlen(fn) + 1);
        strcpy(filename, fn);
        filesize = statbuffer -> st_size;
        fpAvi = new wxFile(buffer, wxFile::read);
        if (fpAvi)
        {
            openpointer = true;
            printf("avifile(%d):%s\n", (int) statbuffer -> st_size, buffer);
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

Cavifile::~ Cavifile()
{
    if (fpAvi)
    {
        fpAvi -> Close();
        delete fpAvi;
    }
    free(buffer);
    if (filename)
    {
        free(filename);
        filename = NULL;
    }
}

void Cavifile::ReadInfos()
{
    if (openpointer)
    {
        unsigned int streamcount;
        unsigned int tagname, tag2name;
        unsigned int tagsize;
        unsigned int codecType_last = 0;
        tagname = getNextUint32();
        printf("tagname=%08x\n", tagname);
        if (tagname == 0x52494646)
        {
            //52494646==RIFF;riffsize:
            tagsize = getNextUint32i();
            tagname = getNextUint32();
            if (strncasecmp(buffer, "AVI ", 4))
            {
                if (strncasecmp(buffer, "AVIX", 4))
                {
                    printf("this is not an avi-file(RIFF(%u)-tag=%08x): %s\n"
                    , tagsize, tagname, filename);
                    tagname = 0;
                }
            }
            if (tagname)
            {
                streamcount = 0;
                int count = 0;
                do
                {
                    tagname = getNextUint32();
                    tagsize = getNextUint32i();
                    readedBytes = 0;
                    switch (tagname)
                    {
                    case 0x4a554e4b:
                        //JUNK
                        {
                            //printf("JUNK: %d bytes skipped\n", tagsize);
                            Seek(tagsize, wxFromCurrent);
                        }
                        break;
                    case 0x4c495354:
                        //LIST
                        tag2name = getNextUint32();
                        switch (tag2name)
                        {
                        case 0x6d6f7669:
                            //movi
                            if (tagsize > readedBytes)
                            {
                                Seek(tagsize - readedBytes, wxFromCurrent);
                            }
                            break;
                        case 0x6864726c:
                            //hdrl
                            break;
                        case 0x6f646d6c:
                            //odml
                            break;
                        case 0x7374726c:
                            //strl
                            break;
                        default:
                    printf("tag1(%08x):%08x(%u/%lu)\n", tagname,tagsize, tagsize, filesize);
                        printf("tag2(%d/%d):%08x(%s)\n", readedBytes, tagsize, tag2name, buffer);
                            printf("getchar2\n");
                            //getchar();
                            break;
                        }
                        break;
                    case 0x61766968:
                        //avih
                        aviMicroSecPerFrame = getNextUint32i();
                        if (aviMicroSecPerFrame)
                        {
                            aviFramesPerSec = 1000000/ aviMicroSecPerFrame;
                        }
                        else
                        {
                            aviFramesPerSec = 0;
                        }
                        aviMaxBitsPerSec = getNextUint32i() <<3;
                        aviReserved1 = getNextUint32i();
                        aviFlags = getNextUint32i();
                        aviTotalFrames = getNextUint32i();
                        aviInitialFrames = getNextUint32i();
                        aviStreams = getNextUint32i();
                        aviSuggestedBuffersize = getNextUint32i();
                        aviWidth = getNextUint32i();
                        aviHeight = getNextUint32i();
                        aviScale = getNextUint32i();
                        aviRate = getNextUint32i();
                        aviStart = getNextUint32i();
                        aviLength = getNextUint32i();
    /* ***** divx3.11(ok): ***** */
                        if (aviTotalFrames)
                        {
                            aviKBitrate = aviFramesPerSec * filesize/ aviTotalFrames;
                            aviKBitrate >>= 7;
                        }
                        printf("avi:%ux%ux%ub%u\n", aviWidth, aviHeight, aviFramesPerSec, aviKBitrate);
                        if (tagsize > 56)
                        {
                            Seek(tagsize - 56, wxFromCurrent);
                        }
                        break;
                    case 0x646d6c68:
                        //dmlh
                        if (tagsize & 1)
                        {
                            tagsize++;
                        }
                        if (tagsize > readedBytes)
                        {
                            Seek(tagsize - readedBytes, wxFromCurrent);
                        }
                        break;
                    case 0x69647831:
                        //idx1
                        {
                            if (tagsize & 1)
                            {
                                tagsize++;
                            }
                            if (tagsize > readedBytes)
                            {
                                Seek(tagsize - readedBytes, wxFromCurrent);
                            }
                        }
                        break;
                    case 0x73747268:
                        //strh
                        {
                            streamcount++;
                            tag2name = getNextUint32();
                            switch (tag2name)
                            {
                            case 0x61756473:
                                //auds (audiocodec)
                                codecType_last = 0x61756473;
                                aviAUDScodec = getNextUint32();
                                aviAUDSdwFlags = getNextUint32i();
                                aviAUDSdwReserved1 = getNextUint32i();
                                aviAUDSdwInitialFrames = getNextUint32i();
                                aviAUDSdwScale = getNextUint32i();
                                aviAUDSdwRate = getNextUint32i();
                                aviAUDSdwStart = getNextUint32i();
                                aviAUDSdwLength = getNextUint32i();
                                aviAUDSdwSuggestedBuffersize = getNextUint32i();
                                aviAUDSdwQuality = getNextUint32i();
                                aviAUDSdwSampleSize = getNextUint32i();
                                break;
                            case 0x76696473:
                                //vids (videocodec)
                                codecType_last = 0x76696473;
                                aviVIDScodec = getNextUint32();
                                //codec: DIV3==0x44495633
                                aviVIDSdwFlags = getNextUint32i();
                                aviVIDSdwReserved1 = getNextUint32i();
                                aviVIDSdwInitialFrames = getNextUint32i();
                                aviVIDSdwScale = getNextUint32i();
                                aviVIDSdwRate = getNextUint32i();
                                aviVIDSdwStart = getNextUint32i();
                                aviVIDSdwLength = getNextUint32i();
                                aviVIDSdwSuggestedBuffersize = getNextUint32i();
                                aviVIDSdwQuality = getNextUint32i();
                                aviVIDSdwSampleSize = getNextUint32i();
                                break;
                            default:
                            printf("tagsize=%u readedBytes=%u\n", tagsize, readedBytes);
                                printf("getchar2\n");
                                //getchar();
                                break;
                            }
                            if (tagsize > readedBytes)
                            {
                                Seek(tagsize - readedBytes, wxFromCurrent);
                            }
                        }
                        break;
                    case 0x73747266:
                        //strf
                        {
                            switch (codecType_last)
                            {
                            case 0x76696473:
                                //vids(aktueller codec=videocodec): BITMAPINFO-Struktur
                                aviVIDSbiSize = getNextUint32i();
                                aviVIDSbiWidth = getNextUint32i();
                                aviVIDSbiHeight = getNextUint32i();
                                aviVIDSbiPlanes = getNextUint16i();
                                aviVIDSbiBitCount = getNextUint16i();
                                aviVIDSbiCompression = getNextUint32();
                                //Compression:DIV3==0x44495633
                                aviVIDSbiSizeImage = getNextUint32i();
                                aviVIDSbiXPelsPerMeter = getNextUint32i();
                                aviVIDSbiYPelsPerMeter = getNextUint32i();
                                aviVIDSbiClrUsed = getNextUint32i();
                                aviVIDSbiClrImportant = getNextUint32i();
                                break;
                            case 0x61756473:
                                //audss(aktueller codec=audiocodec):
                                aviAUDSweFormatTag = getNextUint16i();
                                aviAUDSweChannels = getNextUint16i();
                                aviAUDSweSamplesPerSec = getNextUint32i();
                                aviAUDSweAvgBytesPerSec = getNextUint32i();
                                aviAUDSweBlockAlign = getNextUint16i();
                                if (tagsize == 14)
                                {
                                    aviAUDSweBitsPerSample = 8;
                                }
                                else
                                {
                                    aviAUDSweBitsPerSample = getNextUint16i();
                                }
                                if (tagsize > 16)
                                {
                                    aviAUDSweExSize = getNextUint16i();
                                }
                                else
                                {
                                    aviAUDSweExSize = 0;
                                }
                                switch (aviAUDSweFormatTag)
                                {
                                case 0x00:
                                    printf(" UNKNOWN\n");
                                    break;
                                case 0x01:
                                    printf(" PCM_U8\n");
                                    break;
                                case 0x02:
                                    printf(" ADPCM\n");
                                    break;
                                case 0x03:
                                    printf(" IEEE_FLOAT\n");
                                    break;
                                case 0x04:
                                    printf(" VSELP\n");
                                    break;
                                case 0x05:
                                    printf(" CVSD\n");
                                    break;
                                case 0x06:
                                    printf(" ALAW\n");
                                    break;
                                case 0x07:
                                    printf(" MULAW\n");
                                    break;
                                case 0x08:
                                    printf(" DTS\n");
                                    break;
                                case 0x10:
                                    printf(" OKI_ADPCM\n");
                                    break;
                                case 0x11:
                                    printf(" DVI_ADPCM\n");
                                    break;
                                case 0x50:
                                    printf(" MP2\n");
                                    break;
                                case 0x55:
                                    printf(" MP3\n");
                                    break;
                                case 0x2000:
                                    printf(" AC3\n");
                                    break;
                                default:
                                printf("(strf) aviAUDSweFormatTag     = %04x ", aviAUDSweFormatTag);
                                    printf(" unknown (take a look at mmreg.h\n");
                                    break;
                                }
                                break;
                            default:
                                printf("getchar3(%08x)\n", codecType_last);
                                //getchar();
                                break;
                            }
                            if (tagsize > readedBytes)
                            {
                                Seek(tagsize - readedBytes, wxFromCurrent);
                            }
                        }
                        break;
                    default:
                        //Seek(tagsize - 8, wxFromCurrent);
                        printf("getchar1\n");
                        //getchar();
                        break;
                    }
                    count++;
                }
                while (!Eof());
printf("Reading finished!\n");
            }
        }
    }
}

unsigned long Cavifile::getFilesize()
{
    return filesize;
}

int Cavifile::Read(unsigned char * buffer, int bytestoread)
{
    return fpAvi -> Read(buffer, bytestoread);
}

void Cavifile::Seek(unsigned int bytes, wxSeekMode mode)
{
    fpAvi -> Seek(bytes, mode);
}

bool Cavifile::Eof()
{
    return fpAvi -> Eof();
}

unsigned int Cavifile::getNextUint16()
{
    unsigned int uint16 = 0;
    int count = 0;
    if (openpointer)
    {
        memset(buffer, 0, 3);
        while ((count < 2) && (!fpAvi -> Eof()))
        {
            (void) fpAvi -> Read(buffer + count, 1);
            uint16 = (uint16 << 8) | buffer[count++];
        }
    readedBytes+=2;
    }
    return uint16;
}

unsigned int Cavifile::getNextUint16i()
{
    unsigned int uint16 = 0;
    int count = 0;
    if (openpointer)
    {
        memset(buffer, 0, 3);
        while ((count < 2) && (!fpAvi -> Eof()))
        {
            (void) fpAvi -> Read(buffer + count, 1);
            uint16 = (uint16 >> 8) | (buffer[count++] << 8);
        }
    readedBytes+=2;
    }
    return uint16;
}

unsigned int Cavifile::getNextUint32()
{
    unsigned int uint32 = 0;
    int count = 0;
    if (openpointer)
    {
        memset(buffer, 0, 5);
        while ((count < 4) && (!fpAvi -> Eof()))
        {
            (void) fpAvi -> Read(buffer + count, 1);
            uint32 = (uint32 << 8) | buffer[count++];
        }
    readedBytes+=4;
    }
    return uint32;
}

unsigned int Cavifile::getNextUint32i()
{
    unsigned int uint32 = 0;
    int count = 0;
    if (openpointer)
    {
        memset(buffer, 0, 5);
        while ((count < 4) && (!fpAvi -> Eof()))
        {
            (void) fpAvi -> Read(buffer + count, 1);
            uint32 = (uint32 >> 8) | (buffer[count++] << 24);
        }
    readedBytes+=4;
    }
    return uint32;
}

