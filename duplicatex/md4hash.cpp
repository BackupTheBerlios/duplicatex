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

#include "md4hash.h"

//ok:20050413

int zerobytedupsum = 0, zerobytecounter = 0, zerobytemaxcount = 0;
unsigned int MD4data[16];
unsigned char * MD4bytes;
unsigned char zerobytelast = 0;

unsigned int MD4y[] =
{
    3, 7, 11, 19, 3, 7, 11, 19, 3, 7, 11, 19, 3, 7, 11, 19,
    3, 5, 9, 13, 3, 5, 9, 13, 3, 5, 9, 13, 3, 5, 9, 13,
    3, 9, 11, 15, 3, 9, 11, 15, 3, 9, 11, 15, 3, 9, 11, 15
};

unsigned int MD4z[] =
{
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15,
    0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15
};

Cmd4hash::Cmd4hash()
{
    hashtable = NULL;
    blockhash = new unsigned int[4];
    for (int i = 0 ; i < 4 ; i++)
    {
        blockhash[i] = 0;
    }
}

Cmd4hash::~ Cmd4hash()
{
    if (hashtable)
    {
        delete[] hashtable;
    }
    delete[] blockhash;
}

void Cmd4hash::hashFile(char * filename, bool fakecheck)
{
    isfake = false;
    wxFile * fp = new wxFile(filename, wxFile::read);
    if (fp -> IsOpened())
    {
        long bytes, filelength;
        filelength = fp -> Length();
        if (hashtable)
        {
            delete[] hashtable;
        }
        hashtable = new unsigned char[16 + filelength/ 608000];
        hashtableindex = 0;
        do
        {
            if (filelength <= 9728000)
            {
                bytes = hashFilepart(fp, filelength, fakecheck);
            }
            else
            {
                bytes = hashFilepart(fp, 9728000, fakecheck);
            }
            filelength -= bytes;
            if (fakecheck)
            {
                if (zerobytemaxcount < 2048)
                {
                    memcpy( & hashtable[hashtableindex << 4], blockhash, 16);
                }
                else
                {
                    isfake = true;
                    memset( & hashtable[hashtableindex << 4], 170, 16);
                    memset( & hashtable[hashtableindex << 4] + 8, 204, 16);
                    int i = 6;
                    unsigned char * h = & hashtable[hashtableindex <<4];
                    while (i && zerobytemaxcount)
                    {
                        h[i] = zerobytemaxcount & 255;
                        zerobytemaxcount >>= 8;
                        --i;
                    }
                    i = 12;
                    while ((i > 8) && zerobytedupsum)
                    {
                        h[i] = zerobytedupsum & 255;
                        zerobytedupsum >>= 8;
                        --i;
                    }
                }
            }
            else
            {
                memcpy( & hashtable[hashtableindex << 4], blockhash, 16);
            }
            hashtableindex++;
        }
        while (filelength > 0);
        fp -> Close();
        if (hashtableindex > 1)
        {
            bytes = hashBuffer(hashtable, 16 * hashtableindex);
        }
    }
}

void Cmd4hash::calc()
{
    unsigned int i = 0, a, b, c, d, t;
    unsigned int * y;
    unsigned int * z;
    y = MD4y;
    z = MD4z;
    a = blockhash[0];
    b = blockhash[1];
    c = blockhash[2];
    d = blockhash[3];
    do
    {
        switch (i++>> 4)
        {
        case 0:
            a += ((b & c) | ((~ b) & d));
            break;
        case 1:
            a += ((b & c) | (b & d) | (c & d)) + 0x5A827999;
            break;
        case 2:
            a += (b ^ c ^ d) + 0x6ED9EBA1;
            break;
        }
        a += MD4data[ * z++];
        t = a << * y;
        t |= a >> (32 - * y++);
        a = d;
        d = c;
        c = b;
        b = t;
    }
    while (i < 48);
    blockhash[0] += a;
    blockhash[1] += b;
    blockhash[2] += c;
    blockhash[3] += d;
}

unsigned long Cmd4hash::hashBuffer(unsigned char * buffer, unsigned int length)
{
    blockhash[0] = 0x67452301;
    blockhash[1] = 0xEFCDAB89;
    blockhash[2] = 0x98BADCFE;
    blockhash[3] = 0x10325476;
    unsigned int bytes, pos = 0, start = 0;
    unsigned long length1, length2;
    MD4bytes = (unsigned char *) MD4data;
    do
    {
        bytes = length - pos;
        if (bytes < 64)
        {
            if (bytes)
            {
                memcpy(MD4bytes, & buffer[start], bytes);
                pos += bytes;
            }
            memset( & MD4bytes[bytes], 0, 64 - bytes);
            MD4bytes[bytes++] = 0x80;
            if (bytes > 56)
            {
                calc();
                bytes = 0;
            }
            length1 = length <<3;
            length2 = length >> 29;
            memset( & MD4bytes[bytes], 0, 64 - bytes);
            memcpy( & MD4bytes[56], & length1, 4);
            memcpy( & MD4bytes[60], & length2, 4);
            calc();
        }
        else
        {
            bytes = 64;
            memcpy(MD4bytes, & buffer[start], 64);
            pos += 64;
            calc();
        }
    }
    while (bytes == 64);
    return pos;
}

void zerocheck(unsigned char * buffer, int length)
{
    int i = 0;
    if (length)
    {
        while (i < length)
        {
            if (buffer[i] == zerobytelast)
            {
                if (zerobytecounter > 63)
                {
                    zerobytedupsum++;
                }
                zerobytecounter++;
                if (zerobytemaxcount < zerobytecounter)
                {
                    zerobytemaxcount = zerobytecounter;
                }
            }
            else
            {
                zerobytelast = buffer[i];
                zerobytecounter = 0;
            }
            i++;
        }
    }
}

unsigned long Cmd4hash::hashFilepart(wxFile * fp, unsigned int length, bool fakecheck)
{
    blockhash[0] = 0x67452301;
    blockhash[1] = 0xEFCDAB89;
    blockhash[2] = 0x98BADCFE;
    blockhash[3] = 0x10325476;
    unsigned long bytes, pos = 0;
    unsigned long length1, length2;
    zerobytedupsum = 0;
    zerobytecounter = 0;
    zerobytemaxcount = 0;
    zerobytelast = 0;
    MD4bytes = (unsigned char *) MD4data;
    do
    {
        bytes = length - pos;
        if (bytes < 64)
        {
            if (bytes)
            {
                bytes = fp -> Read(MD4bytes, bytes);
                if (fakecheck)
                {
                    zerocheck(MD4bytes, bytes);
                }
                pos += bytes;
            }
            memset( & MD4bytes[bytes], 0, 64 - bytes);
            MD4bytes[bytes++] = 0x80;
            if (bytes > 56)
            {
                calc();
                bytes = 0;
            }
            length1 = length <<3;
            length2 = length >> 29;
            memset( & MD4bytes[bytes], 0, 64 - bytes);
            memcpy( & MD4bytes[56], & length1, 4);
            memcpy( & MD4bytes[60], & length2, 4);
            calc();
        }
        else
        {
            bytes = fp -> Read(MD4bytes, 64);
            if (fakecheck)
            {
                zerocheck(MD4bytes, bytes);
            }
            pos += bytes;
            calc();
        }
    }
    while (bytes == 64);
    return pos;
}

