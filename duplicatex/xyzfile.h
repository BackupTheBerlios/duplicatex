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

#ifndef XYZFILE_H
#define XYZFILE_H

#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <wx/file.h>

class Cxyzfile
{
    public:
    Cxyzfile(char * filepath, char * filename);
    ~ Cxyzfile();
    unsigned long getFilesize();
    void ReadInfos();
    int Read(unsigned char *buffer,int bytestoread);
    void Seek(unsigned int bytes,wxSeekMode mode);
    bool Eof();
/* **************** private: ******************* */
    private:
/* **************** protected: ******************* */
    protected:
    char *filename;
    bool openpointer;
    char * buffer;
    struct stat * statbuffer;
    unsigned long filesize;
    wxFile * fpJpg;
};

#endif

