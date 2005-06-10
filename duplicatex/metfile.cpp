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
#include "metfile_tag.h"

CMetfile::CMetfile()
{
    mf_filename = NULL;
    mf_partfile = NULL;
    mf_filehash = NULL;
    mf_metfile = NULL;
    parts = 0;
    parts_hashtable = NULL;
}

CMetfile::CMetfile(char * filepath, char * filename)
{
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
    mf_filename = NULL;
    mf_partfile = NULL;
    mf_filehash = new char[33];
    parts_hashtable = NULL;
    char fn[strlen(filepath) + strlen(filename) + 1];
    sprintf(fn, "%s%s", filepath, filename);
    mffp = new wxFile(fn, wxFile::read);
    return mffp;
}

void CMetfile::Create(const char * fullfilename)
{
    mf_filename = NULL;
    mf_partfile = NULL;
    mf_filehash = new char[33];
    parts_hashtable = NULL;
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
        b = mf_metfile -> Write( & checkid, 16);
        b = mf_metfile -> Write( & parts, 2);
        if (debug)
        {
            printf("version written\n");
            printf("date written (%08x)\n", (unsigned int) date0);
            printf("filehash written: %s\n", mf_filehash);
        }
        if (parts)
        {
            unsigned int blocklength = parts * 16;
            b = mf_metfile -> Write( & parts_hashtable, blocklength);
            if (debug)
            {
                printf("hashtable written - (%d) part\n", parts);
            }
        }
        tagcount = 8;
        b = mf_metfile -> Write( & tagcount, 4);
        CMetfileTag * tag = new CMetfileTag();
        if (tag)
        {
            tag -> WriteString(mf_metfile, 1, 1, mf_filename);
            tag -> WriteValue(mf_metfile, 2, 1, mf_filesize);
            tag -> WriteValue(mf_metfile, 0x18, 1, 5);
            tag -> WriteValue(mf_metfile, 0x19, 1, 5);
            tag -> WriteValue(mf_metfile, 0x23, 1, 0);
            tag -> WriteString(mf_metfile, 0x12, 1, "005.part");
            if (tag -> name)
            {
                delete[] tag->name;
            }
            tag -> name = new unsigned char[2];
            tag -> name[0]=0x09;
            tag -> name[1]=0x30;
            tag -> WriteValue(mf_metfile, 0x00, 2, 256);
            tag -> name[0]=0x0a;
            tag -> WriteValue(mf_metfile, 0x00, 2, mf_filesize);
            delete tag;
        }
    }
}

void CMetfile::Read()
{
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
                        if (16 == mf_metfile -> Read( & checkid, 16))
                        {
                            for (int i = 0 ; i < 16 ; i++)
                            {
                                sprintf(mf_filehash + i * 2, "%02x", checkid[i]);
                            }
                            if (2 == mf_metfile -> Read( & parts, 2))
                            {
                                if (parts_hashtable)
                                {
                                    delete[] parts_hashtable;
                                    parts_hashtable = NULL;
                                }
                                if (parts)
                                {
                                    int blocklength, position = 0, bytes = 0;
                                    blocklength = 16 * parts;
                                    parts_hashtable = new unsigned char[blocklength];
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
                        if (16 == mf_metfile -> Read( & checkid, 16))
                        {
                            if (2 == mf_metfile -> Read( & parts, 2))
                            {
                                if (parts_hashtable)
                                {
                                    delete[] parts_hashtable;
                                }
                                if (parts)
                                {
                                    parts_hashtable = new unsigned char[16 * parts];
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
                            if (16 == mf_metfile -> Read( & checkid, 16))
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
                    int i = 0;
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
 /*
 switch (CTag -> name[0])
 {
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
 if (debug)
 {
 printf("\n");
 }
 break;
 }
    */
                                if (debug)
                                {
                                    printf("\n");
                                }
                                break;
                            case 1:
                                //filename
                                if (mf_filename == NULL)
                                {
                                    mf_filename = new char[strlen((char *) CTag -> string) + 1];
                                    strcpy(mf_filename, (char *) CTag -> string);
                                }
                                if (debug)
                                {
                                    printf("\n");
                                }
                                break;
                            case 2:
                                //filesize
                                mf_filesize = CTag -> value;
                                if (debug)
                                {
                                    printf("\n");
                                }
                                break;
                            case 0x12:
                                //partfile
                                if (mf_partfile == NULL)
                                {
                                    mf_partfile = new char[strlen((char *) CTag -> string) + 1];
                                    strcpy(mf_partfile, (char *) CTag -> string);
                                }
                                if (debug)
                                {
                                    printf("\n");
                                }
                                break;
                            default:
                                if (debug)
                                {
                                    printf("\n");
                                }
                                break;
                            }
                            break;
                        default:
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
}

CMetfile::~ CMetfile()
{
    if (mf_filename)
    {
        delete[] mf_filename;
        mf_filename = NULL;
    }
    if (mf_partfile)
    {
        delete[] mf_partfile;
        mf_partfile = NULL;
    }
    if (mf_filehash)
    {
        delete[] mf_filehash;
        mf_filehash = NULL;
    }
    if (mf_metfile && mf_metfile -> IsOpened())
    {
        mf_metfile -> Close();
    }
    if (parts_hashtable)
    {
        delete[] parts_hashtable;
        parts_hashtable = NULL;
    }
}

void CMetfile::SetFilename(const char * filename)
{
    if (mf_filename)
    {
        delete[] mf_filename;
    }
    mf_filename = new char[strlen(filename) + 1];
    strcpy(mf_filename, filename);
}

void CMetfile::SetFilehash(unsigned char * filehash)
{
    for (int i = 0 ; i < 16 ; i++)
    {
        checkid[i] = filehash[i];
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
        delete [] parts_hashtable;
    }
    parts_hashtable = new unsigned char[blocklength];
    memcpy(parts_hashtable, hashtable, blocklength);
}

void CMetfile::SetTagcount(int count)
{
    tagcount = count;
}

void CMetfile::ClearTagtable()
{
}

void CMetfile::AddTagToTable(int tagtype)
{
}

