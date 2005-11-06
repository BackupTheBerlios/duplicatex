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

#ifndef METFILE_H
#define METFILE_H

#include <wx/file.h> // needed for wxFile

#include <dirent.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h> // needed for free
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#include "metfile_tag.h"

extern int debug;

struct GAP {
	GAP *next;
	unsigned long start;
	unsigned long end;
};

class CMetfile 
{
public:
	CMetfile();	
	CMetfile(char *path,char *filename);	
	~CMetfile();
 	GAP *gaplist;
	wxFile *Open(char *path,char *filename);
	void WriteHashtable(FILE *fp);
	void WriteGaplist(FILE *fp);
	void Create(const char *fullfilename);
	void WriteTo();
	void Read();
	void SetFilename(const char *filename);
	void SetPartfilename(const char *filename);
	void SetFilehash(unsigned char *filehash);
	void SetFilesize(long filesize);
	void SetHashtable(unsigned short partcount,unsigned char *hashtable);
	void SetTagcount(int tagcount);
	void ClearGaplist();
	void ShowGaplist();
	void SortGaplist();
	void CopyGaplistFrom(GAP *sourceGaplist);
	void AddGap(unsigned int start,unsigned int end);
	void FillGap(unsigned int start,unsigned int end);
	char *mf_filename;
        char mf_filehash[33];
	char *mf_partfile;
	long mf_filesize;
	wxFile *mf_metfile;
	unsigned char checkid[16];
	short parts;
	unsigned char *parts_hashtable;
protected:
	
	int tagcount;
	unsigned char version;
	bool isnewstyle;
	unsigned int date;
	unsigned char hash_current[16];
	int gaplistCounter;
};

#endif
