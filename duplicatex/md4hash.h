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

#ifndef MD4HASH_H
#define MD4HASH_H

#include <wx/wx.h>
#include <wx/file.h>

class Cmd4hash
{
    public:
    Cmd4hash();
    ~ Cmd4hash();
bool	IsFake() {
return isfake;
};
    void calc();
    unsigned long hashBuffer(unsigned char *buffer, unsigned int length);
    unsigned long hashFilepart(wxFile * fp, unsigned int length,bool fakecheck);
    void hashFile(char * filename,bool IsPartfile);
    unsigned int * blockhash;
    unsigned char * hashtable;
    int hashtableindex;
    private:
	bool isfake;
};

#endif

