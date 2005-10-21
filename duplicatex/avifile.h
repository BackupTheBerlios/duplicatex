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

#ifndef AVIFILE_H
#define AVIFILE_H

#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <wx/file.h>

class Cavifile
{
    public:
    Cavifile(char * filepath, char * filename);
    ~ Cavifile();
    unsigned long getFilesize();
    void ReadInfos();
    int Read(unsigned char * buffer, int bytestoread);
    void Seek(unsigned int bytes, wxSeekMode mode);
    bool Eof();
    /* **************** private: ******************* */
    private:
    unsigned int getNextUint16();
    unsigned int getNextUint16i();
    unsigned int getNextUint32();
    unsigned int getNextUint32i();
    /* **************** protected: ******************* */
    protected:
    char * filename;
    bool openpointer;
    char * buffer;
    struct stat * statbuffer;
    unsigned long filesize;
    unsigned int readedBytes;
    wxFile * fpAvi;
    unsigned int codecLast;
    unsigned int aviVIDScodec;
    unsigned int aviVIDSdwFlags;
    unsigned int aviVIDSdwReserved1;
    unsigned int aviVIDSdwInitialFrames;
    unsigned int aviVIDSdwScale;
    unsigned int aviVIDSdwRate;
    unsigned int aviVIDSdwStart;
    unsigned int aviVIDSdwLength;
    unsigned int aviVIDSdwSuggestedBuffersize;
    unsigned int aviVIDSdwQuality;
    unsigned int aviVIDSdwSampleSize;
    unsigned int aviVIDSbiSize;
    unsigned int aviVIDSbiWidth;
    unsigned int aviVIDSbiHeight;
    unsigned short aviVIDSbiPlanes;
    unsigned short aviVIDSbiBitCount;
    unsigned int aviVIDSbiCompression;
    unsigned int aviVIDSbiSizeImage;
    unsigned int aviVIDSbiXPelsPerMeter;
    unsigned int aviVIDSbiYPelsPerMeter;
    unsigned int aviVIDSbiClrUsed;
    unsigned int aviVIDSbiClrImportant;
    unsigned int aviAUDScodec;
    unsigned int aviAUDSdwFlags;
    unsigned int aviAUDSdwReserved1;
    unsigned int aviAUDSdwInitialFrames;
    unsigned int aviAUDSdwScale;
    unsigned int aviAUDSdwRate;
    unsigned int aviAUDSdwStart;
    unsigned int aviAUDSdwLength;
    unsigned int aviAUDSdwSuggestedBuffersize;
    unsigned int aviAUDSdwQuality;
    unsigned int aviAUDSdwSampleSize;
    unsigned short aviAUDSweFormatTag;
    unsigned short aviAUDSweChannels;
    unsigned int aviAUDSweSamplesPerSec;
    unsigned int aviAUDSweAvgBytesPerSec;
    unsigned short aviAUDSweBlockAlign;
    unsigned short aviAUDSweBitsPerSample;
    unsigned short aviAUDSweExSize;
    unsigned int aviKBitrate;
    unsigned int aviMicroSecPerFrame;
    unsigned int aviFramesPerSec;
    unsigned int aviMaxBitsPerSec;
    unsigned int aviReserved1;
    unsigned int aviFlags;
    unsigned int aviTotalFrames;
    unsigned int aviInitialFrames;
    unsigned int aviStreams;
    unsigned int aviSuggestedBuffersize;
    unsigned int aviWidth;
    unsigned int aviHeight;
    unsigned int aviScale;
    unsigned int aviRate;
    unsigned int aviStart;
    unsigned int aviLength;
};

#endif

