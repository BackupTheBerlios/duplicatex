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

#include "metfile_tag.h"
#include <sys/time.h>

extern int debug;

char tstring[40];

CMetfileTag::CMetfileTag()
{
    type = 0;
    length = 0;
    specialtagtype = 0;
    stringlength = 0;
    value = 0;
    name = NULL;
    string = NULL;
}

CMetfileTag::CMetfileTag(wxFile * metfile)
{
    type = 0;
    length = 0;
    specialtagtype = 0;
    stringlength = 0;
    value = 0;
    name = NULL;
    string = NULL;
    Read(metfile);
}

CMetfileTag::~ CMetfileTag()
{
    if (name)
    {
        delete[] name;
        name = NULL;
    }
    if (string)
    {
        delete[] string;
        string = NULL;
    }
}

void CMetfileTag::Write(wxFile * sourcefp)
{
    int b;
    b = sourcefp -> Write( & type, 1);
    b = sourcefp -> Write( & length, 2);
    if (length == 1)
    {
        b = sourcefp -> Write( & specialtagtype, 1);
    }
    else if(length)
    {
        b = sourcefp -> Write( & name, length);
    }
    if (length)
    {
        switch (type)
        {
        case 2:
            b = sourcefp -> Write( & stringlength, 2);
            b = sourcefp -> Write( & string, stringlength);
            break;
        case 3:
            b = sourcefp -> Write( & value, 4);
            break;
        }
    }
}

void CMetfileTag::WriteString(wxFile * sourcefp, unsigned char sptype, short length, const char * stringtoset)
{
    int b;
    type = 2;
    b = sourcefp -> Write( & type, 1);
    b = sourcefp -> Write( & length, 2);
    if (length == 1)
    {
        b = sourcefp -> Write( & sptype, 1);
    }
    else if(length)
    {
        b = sourcefp -> Write(name, length);
    }
    if (length)
    {
        stringlength = strlen(stringtoset);
        b = sourcefp -> Write( & stringlength, 2);
        b = sourcefp -> Write(stringtoset, stringlength);
    }
}

void CMetfileTag::WriteValue(wxFile * sourcefp, unsigned char sptype, short length, int valuetoset)
{
    int b;
    type = 3;
    b = sourcefp -> Write( & type, 1);
    b = sourcefp -> Write( & length, 2);
    if (length == 1)
    {
        b = sourcefp -> Write( & sptype, 1);
    }
    else if(length)
    {
        b = sourcefp -> Write( name, length);
    }
    if (length)
    {
        b = sourcefp -> Write( & valuetoset, 4);
    }
}

void CMetfileTag::Read(wxFile * sourcefp)
{
    if (name)
    {
        delete[] name;
        name = NULL;
    }
    if (string)
    {
        delete[] string;
        string = NULL;
    }
    type = 0;
    length = 0;
    specialtagtype = 0;
    stringlength = 0;
    value = 0;
    if (1 == sourcefp -> Read( & type, 1))
    {
        if (2 == sourcefp -> Read( & length, 2))
        {
            name = new unsigned char[length + 1];
            name[0] = 0;
            if (length == 1)
            {
                if (1 != sourcefp -> Read( & specialtagtype, 1))
                {
                    length = 0;
                    if (debug)
                    {
                        printf("..no.specialtagtype.");
                    }
                }
                else
                {
                    if (debug)
                    {
                        switch (specialtagtype)
                        {
                        case 0x00:
                            printf("%02x_UNKNOWN..........", specialtagtype);
                            break;
                        case 0x01:
                            printf("%02x_FILENAME.........", specialtagtype);
                            break;
                        case 0x02:
                            printf("%02x_FILESIZE.........", specialtagtype);
                            break;
                        case 0x03:
                            printf("%02x_FILETYPE.........", specialtagtype);
                            break;
                        case 0x04:
                            printf("%02x_FILEFORMAT.......", specialtagtype);
                            break;
                        case 0x05:
                            printf("%02x_LASTSEENCOMPLETE.", specialtagtype);
                            break;
                        case 0x08:
                            printf("%02x_TRANSFERED.......", specialtagtype);
                            break;
                        case 0x09:
                            printf("%02x_GAPSTART.........", specialtagtype);
                            break;
                        case 0x0a:
                            printf("%02x_GAPEND...........", specialtagtype);
                            break;
                        case 0x12:
                            printf("%02x_PARTFILENAME.....", specialtagtype);
                            break;
                        case 0x13:
                            printf("%02x_OLDDLPRIORITY....", specialtagtype);
                            break;
                        case 0x14:
                            printf("%02x_STATUS...........", specialtagtype);
                            break;
                        case 0x15:
                            printf("%02x_SOURCES..........", specialtagtype);
                            break;
                        case 0x16:
                            printf("%02x_PERMISSIONS......", specialtagtype);
                            break;
                        case 0x17:
                            printf("%02x_OLDULPRIORITY....", specialtagtype);
                            break;
                        case 0x18:
                            printf("%02x_DLPRIORITY.......", specialtagtype);
                            break;
                        case 0x19:
                            printf("%02x_ULPRIORITY.......", specialtagtype);
                            break;
                        case 0x1a:
                            printf("%02x_FT_COMPRESSION...", specialtagtype);
                            break;
                        case 0x1b:
                            printf("%02x_FT_CORRUPTED.....", specialtagtype);
                            break;
                        case 0x20:
                            printf("%02x_KADLASTPUBLISHKEY", specialtagtype);
                            break;
                        case 0x21:
                            printf("%02x_KADLASTPUBLISHSRC", specialtagtype);
                            break;
                        case 0x22:
                            printf("%02x_FLAGS............", specialtagtype);
                            break;
                        case 0x23:
                            printf("%02x_DL_ACTIVE_TIME...", specialtagtype);
                            break;
                        case 0x24:
                            printf("%02x_CORRUPTEDPARTS...", specialtagtype);
                            break;
                        case 0x25:
                            printf("%02x_DL_PREVIEW.......", specialtagtype);
                            break;
                        case 0x26:
                            printf("%02x_KADLASTPUBLISHN..", specialtagtype);
                            break;
                        case 0x27:
                            printf("%02x_AICH_HASH........", specialtagtype);
                            break;
                        case 0x30:
                            printf("%02x_COMPLETESOURCES..", specialtagtype);
                            break;
                        case 0x50:
                            printf("%02x_ATTRANSFERED.....", specialtagtype);
                            break;
                        case 0x51:
                            printf("%02x_ATREQUESTED......", specialtagtype);
                            break;
                        case 0x52:
                            printf("%02x_ATACCEPTED.......", specialtagtype);
                            break;
                        case 0x53:
                            printf("%02x_CATEGORY.........", specialtagtype);
                            break;
                        case 0x54:
                            printf("%02x_ATTRANSFEREDHI...", specialtagtype);
                            break;
                        case 0xc1:
                            printf("%02x_DYNAMIC..........", specialtagtype);
                            break;
                        case 0xd0:
                            printf("%02x_MEDIAARTIST......", specialtagtype);
                            break;
                        case 0xd1:
                            printf("%02x_MEDIAALBUM.......", specialtagtype);
                            break;
                        case 0xd2:
                            printf("%02x_MEDIATITLE.......", specialtagtype);
                            break;
                        case 0xd3:
                            printf("%02x_MEDIALENGTH......", specialtagtype);
                            break;
                        case 0xd4:
                            printf("%02x_MEDIABITRATE.....", specialtagtype);
                            break;
                        case 0xd5:
                            printf("%02x_MEDIACODEC.......", specialtagtype);
                            break;
                        default:
                            printf("%02x_UNKNOWN..........", specialtagtype);
                            break;
                        }
                    }
                }
            }
            else
            {
                if (length != sourcefp -> Read(name, length))
                {
                    length = 0;
                }
                else
                {
                    name[length] = 0;
                }
                if (debug)
                {
                    printf("(%d)%s(0x", length, name);
                    for (int i = 0 ; i < length ; i++)
                    {
                        printf("%02x", name[i]);
                    }
                    printf(" ");
                }
            }
            if (length)
            {
                //string:
                if (type == 2)
                {
                    if (2 == sourcefp -> Read( & stringlength, 2))
                    {
                        string = new unsigned char[stringlength + 1];
                        if (stringlength == sourcefp -> Read(string, stringlength))
                        {
                            string[stringlength] = 0;
                            if (debug)
                            {
                                printf("s:(%d)%s ", stringlength, string);
                            }
                        }
                        else
                        {
                            delete[] string;
                            delete[] name;
                            string = NULL;
                            name = NULL;
                            type = 0;
                        }
                    }
                    else
                    {
                        delete [] name;
                        name = NULL;
                        type = 0;
                    }
                }
                //integer:
                else if(type == 3)
                {
                    if (4 == sourcefp -> Read( & value, 4))
                    {
                        if (debug)
                        {
                            printf("v:(%d) ", value);
                        }
                    }
                    else
                    {
                        delete[] name;
                        name = NULL;
                        type = 0;
                    }
                }
                else
                {
                    delete[] name;
                    name = NULL;
                    type = 0;
                }
                if (debug && type)
                {
                    if (specialtagtype)
                    {
                        //printf(" %s(0,%d) ", name, length);
                    }
                    else
                    {
                        switch (name[0])
                        {
                        case 0xfe:
                            printf("%02x_LASTSEENCOMPLETE.", name[0]);
                            //ed2k-last-seen-complete
                            time_t timet0;
                            timet0 = (time_t) value;
                            sprintf(tstring, "%s", ctime( & timet0));
                            tstring[24] = 0;
                            printf("%s ", tstring);
                            break;
                        case 0x09:
                            printf("%02x_GAPSTART.........", name[0]);
                            break;
                        case 0x0a:
                            printf("%02x_GAPEND...........", name[0]);
                            break;
                        default:
                            printf("%02x_UNKNOWN......(%d)", name[0], length);
                            for (int i = 0 ; i < length ; i++)
                            {
                                printf("%02x", name[i]);
                            }
                            printf(" ");
                            break;
                        }
                        switch (type)
                        {
                        case 2:
                            printf(" %s ", string);
                            break;
                        case 3:
                            printf(" %d ", value);
                            break;
                        }
                    }
                }
            }
            else
            {
                delete[] name;
                name = NULL;
                type = 0;
            }
        }
    }
}

void CMetfileTag::Copy(wxFile * sourcefp, wxFile * targetfp)
{
    int mode = 1;
    if (name)
    {
        delete[] name;
        name = NULL;
    }
    if (string)
    {
        delete[] string;
        string = NULL;
    }
    type = 0;
    if (1 == sourcefp -> Read( & type, 1))
    {
        if (mode == 1)
        {
            targetfp -> Write( & type, 1);
        }
        if (debug)
        {
            printf(" type(%02x) ", type);
        }
        if (2 == sourcefp -> Read( & length, 2))
        {
            if (mode == 1)
            {
                targetfp -> Write( & length, 2);
            }
            name = new unsigned char[length + 1];
            name[0] = 0;
            if (length == 1)
            {
                if (1 != sourcefp -> Read( & specialtagtype, 1))
                {
                    length = 0;
                }
                else
                {
                    if (mode == 1)
                    {
                        if (specialtagtype == 0xc1)
                        {
                            specialtagtype = 0xc2;
                        }
                        targetfp -> Write( & specialtagtype, 1);
                    }
                    switch (specialtagtype)
                    {
                    case 0x01:
                        //printf("%02x_FILENAME.........", specialtagtype);
                        break;
                    case 0x02:
                        //printf("%02x_FILESIZE.........", specialtagtype);
                        break;
                    case 0x03:
                        printf("%02x_FILETYPE.........", specialtagtype);
                        break;
                    case 0x04:
                        printf("%02x_FILEFORMAT.......", specialtagtype);
                        break;
                    case 0x05:
                        printf("%02x_LASTSEENCOMPLETE.", specialtagtype);
                        break;
                    case 0x08:
                        printf("%02x_TRANSFERED.......", specialtagtype);
                        break;
                    case 0x12:
                        printf("%02x_PARTFILENAME.....", specialtagtype);
                        break;
                    case 0x13:
                        printf("%02x_OLDDLPRIORITY....", specialtagtype);
                        break;
                    case 0x14:
                        printf("%02x_STATUS...........", specialtagtype);
                        break;
                    case 0x15:
                        printf("%02x_SOURCES..........", specialtagtype);
                        break;
                    case 0x16:
                        printf("%02x_PERMISSIONS......", specialtagtype);
                        break;
                    case 0x17:
                        printf("%02x_OLDULPRIORITY....", specialtagtype);
                        break;
                    case 0x18:
                        printf("%02x_DLPRIORITY.......", specialtagtype);
                        break;
                    case 0x19:
                        printf("%02x_ULPRIORITY.......", specialtagtype);
                        break;
                    case 0x30:
                        printf("%02x_COMPLETESOURCES..", specialtagtype);
                        break;
                    case 0x50:
                        //printf("%02x_ATTRANSFERED.....", specialtagtype);
                        break;
                    case 0x51:
                        //printf("%02x_ATREQUESTED......", specialtagtype);
                        break;
                    case 0x52:
                        //printf("%02x_ATACCEPTED.......", specialtagtype);
                        break;
                    case 0x53:
                        //printf("%02x_CATEGORY.........", specialtagtype);
                        break;
                    case 0x54:
                        //printf("%02x_ATTRANSFEREDHI...", specialtagtype);
                        break;
                    case 0xc1:
                        //printf("%02x_DYNAMIC..........", specialtagtype);
                        break;
                    case 0xd0:
                        //printf("%02x_MEDIAARTIST......", specialtagtype);
                        break;
                    case 0xd1:
                        //printf("%02x_MEDIAALBUM.......", specialtagtype);
                        break;
                    case 0xd2:
                        //printf("%02x_MEDIATITLE.......", specialtagtype);
                        break;
                    case 0xd3:
                        //printf("%02x_MEDIALENGTH......", specialtagtype);
                        break;
                    case 0xd4:
                        //printf("%02x_MEDIABITRATE.....", specialtagtype);
                        break;
                    case 0xd5:
                        //printf("%02x_MEDIACODEC.......", specialtagtype);
                        break;
                    default:
                        //printf("%02x_UNKNOWN..........", specialtagtype);
                        break;
                    }
                }
            }
            else
            {
                if (length != sourcefp -> Read(name, length))
                {
                    if (mode == 1)
                    {
                        if (length)
                        {
                            targetfp -> Write( & name, length);
                        }
                    }
                    length = 0;
                }
                else
                {
                    if (mode == 1)
                    {
                        if (length)
                        {
                            targetfp -> Write( & name, length);
                        }
                    }
                }
                name[length] = 0;
            }
            if (specialtagtype)
            {
                printf("(%s,0,%d)", name, length);
            }
            else
            {
                switch (name[0])
                {
                case 0x09:
                    printf("%02x_GAPSTART.........", name[0]);
                    break;
                case 0x0a:
                    printf("%02x_GAPEND...........", name[0]);
                    break;
                }
                printf("(%d,%d,%d) ", name[0], name[1], length);
            }
            if (length)
            {
                //string:
                if (type == 2)
                {
                    if (2 == sourcefp -> Read( & stringlength, 2))
                    {
                        string = new unsigned char[stringlength + 1];
                        if (stringlength == sourcefp -> Read(string, stringlength))
                        {
                            string[stringlength] = 0;
                            //printf("=%s ", (char *) string);
                            if (mode == 1)
                            {
                                switch (specialtagtype)
                                {
                                case 0x12:
                                    if (stringlength > 3)
                                    {
                                        string[stringlength - 3] = 'a';
                                        string[stringlength - 2] = 'r';
                                        string[stringlength - 1] = 't';
                                    }
                                    targetfp -> Write( & stringlength, 2);
                                    targetfp -> Write(string, stringlength);
                                    break;
                                default:
                                    targetfp -> Write( & stringlength, 2);
                                    targetfp -> Write(string, stringlength);
                                    break;
                                }
                            }
                        }
                        else
                        {
                            delete[] string;
                            delete[] name;
                            string = NULL;
                            name = NULL;
                            type = 0;
                        }
                    }
                    else
                    {
                        delete[] name;
                        name = NULL;
                        type = 0;
                    }
                }
                //integer:
                else if(type == 3)
                {
                    if (4 == sourcefp -> Read( & value, 4))
                    {
                        if (mode == 1)
                        {
                            targetfp -> Write( & value, 4);
                        }
                        //printf("=%u ", value);
                    }
                    else
                    {
                        delete[] name;
                        name = NULL;
                        type = 0;
                    }
                }
                else
                {
                    delete[] name;
                    name = NULL;
                    type = 0;
                }
            }
            else
            {
                delete[] name;
                name = NULL;
                type = 0;
            }
        }
    }
}

void CMetfileTag::SetString(char * stringtoset)
{
    stringlength = strlen(stringtoset);
    if (string)
    {
        delete[] string;
        string = NULL;
    }
    string = new unsigned char[stringlength + 1];
    strcpy((char *) string, stringtoset);
}

void CMetfileTag::SetValue(int valuetoset)
{
    value = valuetoset;
}

