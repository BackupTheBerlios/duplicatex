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

#include <stdio.h> //FILE
#include <unistd.h> //NULL

#ifndef METFILETAG_H
#define METFILETAG_H

#include <wx/wx.h>
#include <wx/file.h>

extern int debug;

class CMetfileTag
{
public:
	CMetfileTag();
	CMetfileTag(wxFile *sourcefp);
	~CMetfileTag();
	void Read(wxFile *sourcefp);
	void Write(wxFile *sourcefp);
	void WriteString(wxFile *sourcefp,unsigned char specialtype,short length,const char *stringtoset);
	void WriteValue(wxFile *sourcefp,unsigned char specialtype,short length,int valuetoset);
	void SetString(char *stringtoset);
	void SetValue(int valuetoset);
	void Copy(wxFile *sourcefp,wxFile *targetfp);
public:
    unsigned char type;
    unsigned char specialtagtype;
    unsigned short length;
    unsigned short stringlength;
    unsigned int value;
    unsigned char *name;
    unsigned char *string;
};

#endif
