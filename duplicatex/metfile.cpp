 /*
 **** Duplicatex v1.0
 **** Find and markup duplicate files and/or delete it.
 ****
 **** Copyright (C) 2005 Frank Schaefer (sf@mulinux.org)
 ****
 **** This program is free software; you can redistribute it and/or
 **** modify it under the terms of the GNU General Public License
 **** as published by the Free Software Foundation; either version 2
 **** of the License, or (at your option) any later version.
 ****
 **** This program is distributed in the hope that it will be useful,
 **** but WITHOUT ANY WARRANTY; without even the implied warranty of
 **** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **** GNU General Public License for more details.
 ****
 **** You should have received a copy of the GNU General Public License
 **** along with this program; if not, write to the Free Software
 **** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
    */

#include <wx/wx.h>
#include "metfile.h"
#include "md4hash.h"

CMetfile::CMetfile()
{
    gaplist = NULL;
    mf_filename = NULL;
    mf_partfile = NULL;
    memset(mf_filehash, 0, 33);
    mf_metfile = NULL;
    parts = 0;
    parts_hashtable = NULL;
}

CMetfile::CMetfile(char * filepath, char * filename)
{
    gaplist = NULL;
    mf_filename = NULL;
    mf_partfile = NULL;
    memset(mf_filehash, 0, 33);
    mf_metfile = NULL;
    parts = 0;
    parts_hashtable = NULL;
    debug = 0;
    mf_metfile = Open(filepath, filename);
    if (mf_metfile -> IsOpened())
    {
        if (debug)
        {
            printf("metfile: Filename=%s\n", filename);
        }
        Read();
    }
}

wxFile * CMetfile::Open(char * filepath, char * filename)
{
    wxFile * mffp;
    if (mf_filename)
    {
        free(mf_filename);
        mf_filename = NULL;
    }
    if (mf_partfile)
    {
        free(mf_partfile);
        mf_partfile = NULL;
    }
    memset(mf_filehash, 0, 33);
    char fn[strlen(filepath) + strlen(filename) + 1];
    sprintf(fn, "%s%s", filepath, filename);
    mffp = new wxFile(fn, wxFile::read);
    return mffp;
}

void CMetfile::Create(const char * fullfilename)
{
    if (mf_filename)
    {
        free(mf_filename);
        mf_filename = NULL;
    }
    if (mf_partfile)
    {
        free(mf_partfile);
        mf_partfile = NULL;
    }
    memset(mf_filehash, 0, 33);
    parts_hashtable = NULL;
    if (mf_metfile)
    {
        delete mf_metfile;
    }
    mf_metfile = new wxFile(fullfilename, wxFile::write);
    version = 0xe0;
}

void CMetfile::WriteTo()
{
    if (mf_metfile)
    {
        int b;
        if (debug)
        {
            printf("CMetfile::WriteTo: %s\n", mf_filename);
        }
        b = mf_metfile -> Write( & version, 1);
        time_t date0;
        date0 = time(NULL);
        b = mf_metfile -> Write( & date0, 4);
        b = mf_metfile -> Write(checkid, 16);
        b = mf_metfile -> Write( & parts, 2);
        if (parts)
        {
            unsigned int blocklength = parts * 16;
            b = mf_metfile -> Write(parts_hashtable, blocklength);
            if (debug)
            {
                printf("hashtable written - (%d) part\n", parts);
            }
            Cmd4hash * hash = new Cmd4hash();
            (void) hash -> hashBuffer(parts_hashtable, 16 * parts, true);
            delete hash;
        }
        if (gaplist)
        {
            SortGaplist();
            tagcount = 6 + 2 * gaplistCounter;
        }
        else
        {
            tagcount = 8;
        }
        b = mf_metfile -> Write( & tagcount, 4);
        CMetfileTag * tag = new CMetfileTag();
        if (tag)
        {
            tag -> WriteString(mf_metfile, 1, 1, mf_filename);
            tag -> WriteValue(mf_metfile, 2, 1, mf_filesize);
            tag -> WriteValue(mf_metfile, 0x18, 1, 5);
            tag -> WriteValue(mf_metfile, 0x19, 1, 5);
            tag -> WriteValue(mf_metfile, 0x23, 1, 0);
            if (mf_partfile)
            {
                tag -> WriteString(mf_metfile, 0x12, 1, mf_partfile);
            }
            if (tag -> name)
            {
                delete[] tag -> name;
            }
            tag -> name = new unsigned char[11];
            int index = 0;
            if (gaplist)
            {
                GAP * currentGap = gaplist;
                while (currentGap)
                {
                    sprintf((char *) tag -> name, "\t%d", index);
                    tag -> WriteValue(mf_metfile, 0x00, strlen((char *) tag -> name), currentGap -> start);
                    sprintf((char *) tag -> name, "\n%d", index);
                    tag -> WriteValue(mf_metfile, 0x00, strlen((char *) tag -> name), currentGap -> end);
                    currentGap = currentGap -> next;
                    index++;
                }
            }
            else
            {
                sprintf((char *) tag -> name, "0x09%d", index);
                tag -> WriteValue(mf_metfile, 0x00, strlen((char *) tag -> name), 0);
                sprintf((char *) tag -> name, "0x0a%d", index);
                tag -> WriteValue(mf_metfile, 0x00, strlen((char *) tag -> name), mf_filesize);
            }
            delete tag;
        }
    }
}

void CMetfile::Read()
{
    ClearGaplist();
    unsigned char * buffer = new unsigned char[512];
    long length = 0;
    length = mf_metfile -> Length();
    tagcount = 0;
    if (1 == mf_metfile -> Read( & version, 1))
    {
        if (debug)
        {
            printf("Version=0x%02x: Length=%lu\n", version, length);
        }
        isnewstyle = false;
        switch (version)
        {
        case 0xe0:
        case 0xe1:
            isnewstyle = (version == 0xe1);
            parts = 0;
            if (!isnewstyle)
            {
                if (debug)
                {
                    printf("...is not newstyle\n");
                }
                mf_metfile -> Seek(24);
                if (4 == mf_metfile -> Read(buffer, 4))
                {
                    if ((buffer[0] == 0) && (buffer[1] == 0) && (buffer[2] == 2) && (buffer[3] == 1))
                    {
                        isnewstyle = true;
                    }
                    mf_metfile -> Seek(1);
                    if (4 == mf_metfile -> Read( & date, 4))
                    {
                        if (16 == mf_metfile -> Read(checkid, 16))
                        {
                            for (int i = 0 ; i < 16 ; i++)
                            {
                                sprintf(mf_filehash + i * 2, "%02x", checkid[i]);
                            }
                            if (2 == mf_metfile -> Read( & parts, 2))
                            {
                                if (parts_hashtable)
                                {
                                    free(parts_hashtable);
                                    parts_hashtable = NULL;
                                }
                                if (parts)
                                {
                                    int blocklength, position = 0, bytes = 0;
                                    blocklength = 16 * parts;
                                    parts_hashtable = (unsigned char *) malloc(blocklength);
                                    while (position < blocklength)
                                    {
                                        bytes = mf_metfile -> Read(parts_hashtable + position, 16);
                                        if (!bytes)
                                        {
                                            blocklength = - blocklength;
                                        }
                                        else
                                        {
                                            position += bytes;
                                        }
                                    }
                                    if (blocklength <= 0)
                                    {
                                        blocklength = - blocklength;
                                        if (debug)
                                        {
                                            printf("\nerror: %d(%d)%d ", blocklength, position, bytes);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                tagcount = - 4;
                            }
                        }
                        else
                        {
                            tagcount = - 3;
                        }
                    }
                    else
                    {
                        tagcount = - 2;
                    }
                }
                else
                {
                    tagcount = - 1;
                }
            }
            else
            {
                if (debug)
                {
                    printf("...is newstyle...");
                }
                if (4 == mf_metfile -> Read(buffer, 4))
                {
                    if (debug)
                    {
                        printf("temp-ok %02x,%02x,%02x,%02x\n", buffer[0], buffer[1], buffer[2], buffer[3]);
                    }
                    if (! (buffer[0] | buffer[1] | buffer[2] | buffer[3]))
                    {
                        if (16 == mf_metfile -> Read(checkid, 16))
                        {
                            if (2 == mf_metfile -> Read( & parts, 2))
                            {
                                if (parts_hashtable)
                                {
                                    free(parts_hashtable);
                                    parts_hashtable = NULL;
                                }
                                if (parts)
                                {
                                    parts_hashtable = (unsigned char *) malloc(16 * parts);
                                    int i = 0;
                                    while (i < parts)
                                    {
                                        if (16 == mf_metfile -> Read( & hash_current, 16))
                                        {
                                            memcpy(parts_hashtable + i * 16, & hash_current, 16);
                                            i++;
                                        }
                                        else
                                        {
                                            i = parts;
                                            tagcount = - 13;
                                        }
                                    }
                                }
                            }
                            else
                            {
                                parts = 0;
                                tagcount = - 12;
                            }
                        }
                        else
                        {
                            tagcount = - 11;
                        }
                    }
                    else
                    {
                        mf_metfile -> Seek(2);
                        if (4 == mf_metfile -> Read( & date, 4))
                        {
                            if (16 == mf_metfile -> Read(checkid, 16))
                            {
                                for (int i = 0 ; i < 16 ; i++)
                                {
                                    sprintf(mf_filehash + i * 2, "%02x", checkid[i]);
                                }
                            }
                            else
                            {
                                tagcount = - 23;
                            }
                        }
                        else
                        {
                            tagcount = - 22;
                        }
                    }
                }
                else
                {
                    if (debug)
                    {
                        printf("temp-error(-21)\n");
                    }
                    tagcount = - 21;
                }
            }
            if (!tagcount)
            {
                if (4 == mf_metfile -> Read( & tagcount, 4))
                {
                    int gapindex = 0, gapstatus = 0, gapstart = 0, gapend = 0, i = 0;
                    while (i < tagcount)
                    {
                        if (debug)
                        {
                            printf("ctag(%dv%d): ", 1 + i, tagcount);
                        }
                        CMetfileTag * CTag = new CMetfileTag(mf_metfile);
                        if (!CTag)
                        {
                            printf("error\n");
                        }
                        if (debug)
                        {
                            printf(" T=%d L=%d SpT=0x%02x StL=%d ",
                            CTag -> type, CTag -> length, CTag -> specialtagtype, CTag -> stringlength);
                        }
                        switch (CTag -> type)
                        {
                        case 2:
                        case 3:
                            switch (CTag -> specialtagtype)
                            {
                            case 0:
                                switch (CTag -> name[0])
                                {
                                case 0x09:
                                    gapstart = CTag -> value;
                                    gapstatus |= 1;
                                    gapindex = atoi((char *) CTag -> name + 1);
                                    break;
                                case 0x0a:
                                    gapstatus |= 2;
                                    gapend = CTag -> value;
                                    if (gapindex != atoi((char *) CTag -> name + 1))
                                    {
                                        printf("gaperror: startindex(%d) != endindex(%d)\n", gapindex, atoi((char *) CTag -> name + 1));
                                    }
                                    else if(gapstatus == 3)
                                    {
                                        AddGap(gapstart, gapend);
                                    }
                                    else
                                    {
                                        printf("gaperror: gapstatus(%d)!=3\n", gapstatus);
                                    }
                                    gapstatus = 0;
                                    break;
                                case 0xfe:
                                    //ed2k-last-seen-complete
                                    time_t timet0;
                                    timet0 = (time_t) CTag -> value;
                                    if (debug)
                                    {
                                        printf(" ct=%s ", ctime( & timet0));
                                    }
                                    break;
                                default:
                                    gapstatus = 0;
                                    break;
                                }
                                break;
                            case 1:
                                gapstatus = 0;
                                //filename
                                if (mf_filename == NULL)
                                {
                                    mf_filename = (char *) malloc(strlen((char *) CTag -> string) + 1);
                                    strcpy(mf_filename, (char *) CTag -> string);
                                }
                                break;
                            case 2:
                                gapstatus = 0;
                                //filesize
                                mf_filesize = CTag -> value;
                                break;
                            case 0x12:
                                gapstatus = 0;
                                //partfile
                                if (mf_partfile == NULL)
                                {
                                    mf_partfile = (char *) malloc(strlen((char *) CTag -> string) + 1);
                                    strcpy(mf_partfile, (char *) CTag -> string);
                                }
                                break;
                            default:
                                gapstatus = 0;
                                break;
                            }
                            if (debug)
                            {
                                printf("\n");
                            }
                            break;
                        default:
                            gapstatus = 0;
                            if (debug)
                            {
                                printf("\n");
                            }
                            break;
                        }
                        delete CTag;
                        i++;
                    }
                }
                else
                {
                    tagcount = - 31;
                }
            }
            if (debug)
            {
                printf("Date=%s", asctime(localtime((time_t *) & date)));
                printf(" Parts=%d tagcount=%d\n", parts, tagcount);
            }
            break;
        default:
            break;
        }
    }
    delete[] buffer;
    SortGaplist();
}

CMetfile::~ CMetfile()
{
    ClearGaplist();
    if (mf_filename)
    {
        free(mf_filename);
        mf_filename = NULL;
    }
    if (mf_partfile)
    {
        free(mf_partfile);
        mf_partfile = NULL;
    }
    if (mf_metfile && mf_metfile -> IsOpened())
    {
        mf_metfile -> Close();
    }
    if (parts_hashtable)
    {
        free(parts_hashtable);
        parts_hashtable = NULL;
    }
}

void CMetfile::SetFilename(const char * filename)
{
    if (mf_filename)
    {
        free(mf_filename);
    }
    mf_filename = (char *) malloc(strlen(filename) + 1);
    strcpy(mf_filename, filename);
}

void CMetfile::SetPartfilename(const char * filename)
{
    if (mf_partfile)
    {
        free(mf_partfile);
    }
    if (filename)
    {
        mf_partfile = (char *) malloc(strlen(filename) + 1);
        strcpy(mf_partfile, filename);
    }
    else
    {
        mf_partfile = NULL;
    }
}

void CMetfile::SetFilehash(unsigned char * filehash)
{
    memcpy(checkid, filehash, 16);
    for (int i = 0 ; i < 16 ; i++)
    {
        sprintf(mf_filehash + i * 2, "%02x", checkid[i]);
    }
}

void CMetfile::SetFilesize(long filesize)
{
    mf_filesize = filesize;
}

void CMetfile::SetHashtable(unsigned short partcount, unsigned char * hashtable)
{
    int blocklength;
    parts = partcount;
    blocklength = 16 * parts;
    if (parts_hashtable)
    {
        free(parts_hashtable);
    }
    parts_hashtable = (unsigned char *) malloc(blocklength);
    memcpy(parts_hashtable, hashtable, blocklength);
}

void CMetfile::SetTagcount(int count)
{
    tagcount = count;
}

void CMetfile::ClearGaplist()
{
    GAP * nextGap = NULL;
    while (gaplist)
    {
        nextGap = gaplist -> next;
        free(gaplist);
        gaplist = nextGap;
    }
}

void CMetfile::ShowGaplist()
{
    GAP * nextGap = gaplist;
    while (nextGap)
    {
        printf("Gaplist:%012ld-%012ld\n", nextGap -> start, nextGap -> end);
        nextGap = nextGap -> next;
    }
}

void CMetfile::SortGaplist()
{
    int change;
    unsigned long tmp = 0;
    GAP * currentGap, * nextGap;
    gaplistCounter = 0;
    if (gaplist)
    {
        do
        {
            change = 0;
            currentGap = gaplist;
            gaplistCounter = 0;
            while (currentGap)
            {
                gaplistCounter++;
                nextGap = currentGap -> next;
                if (nextGap)
                {
                    if (nextGap -> start < currentGap -> start)
                    {
                        tmp = nextGap -> start;
                        nextGap -> start = currentGap -> start;
                        currentGap -> start = tmp;
                        tmp = nextGap -> end;
                        nextGap -> end = currentGap -> end;
                        currentGap -> end = tmp;
                        change++;
                    }
                }
                currentGap = nextGap;
            }
        }
        while (change);
    }
}

void CMetfile::AddGap(unsigned int start, unsigned int end)
{
    GAP * newGap;
    newGap = (GAP *) malloc(sizeof(struct GAP));
    memset(newGap, 0, sizeof(struct GAP));
    newGap -> next = gaplist;
    newGap -> start = start;
    newGap -> end = end;
    gaplist = newGap;
}

void CMetfile::FillGap(unsigned int start, unsigned int end)
{
    int todelete = 0;
    unsigned long newStart, newEnd;
    GAP * nextGap;
    newStart = 0;
    newEnd = 0;
    GAP * currentGap = gaplist;
    while (currentGap)
    {
        nextGap = currentGap -> next;
        if (start <= currentGap -> start)
        {
            if (end > currentGap -> start)
            {
                if (end < currentGap -> end)
                {
                    currentGap -> start = end;
                }
                else
                {
                    todelete++;
                }
            }
        }
        else if(start < currentGap -> end)
        {
            if (end < currentGap -> end)
            {
                newStart = end;
                newEnd = currentGap -> end;
            }
            currentGap -> end = start;
        }
        if (todelete)
        {
            if (gaplist == currentGap)
            {
                gaplist = currentGap -> next;
            }
            free(currentGap);
            todelete = 0;
        }
        else if(newEnd)
        {
            AddGap(newStart, newEnd);
            newEnd = 0;
        }
        currentGap = nextGap;
    }
}

void CMetfile::CopyGaplistFrom(GAP * gaplistSource)
{
    GAP * newGap;
    GAP * sourceGap = gaplistSource;
    ClearGaplist();
    while (sourceGap)
    {
        newGap = (GAP *) malloc(sizeof(struct GAP));
        newGap -> next = gaplist;
        newGap -> start = sourceGap -> start;
        newGap -> end = sourceGap -> end;
        gaplist = newGap;
        sourceGap = sourceGap -> next;
    }
}

void CMetfile::WriteHashtable(FILE * fp)
{
    int i0, i1;
    char tmp[128];
    long start, end;
    fprintf(fp, "Hashtable :\n");
    for (i0 = 0 ; i0 < parts ; i0++)
    {
        start = i0 * 9728000;
        end = start + 9727999;
        if (end >= mf_filesize)
        {
            end = mf_filesize - 1;
        }
        fprintf(fp, "%012ld-%012ld:", start + 1, end + 1);
        for (i1 = 0 ; i1 < 16 ; i1++)
        {
            sprintf(tmp + i1 * 2, "%02x", parts_hashtable[i0 * 16 + i1]);
        }
        fprintf(fp, " %s\n", tmp);
    }
}

void CMetfile::WriteGaplist(FILE * fp)
{
    int index = 0;
    GAP * nextGap = gaplist;
    fprintf(fp, "Gaplist :\n");
    while (nextGap)
    {
        fprintf(fp, "%08d: %012ld-%012ld\n", index, nextGap -> start, nextGap -> end);
        nextGap = nextGap -> next;
        index++;
    }
}

