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

#ifndef DATABASE_H
#define DATABASE_H

#include <wx/wx.h>
#include <wx/file.h>
#include <sys/stat.h>

#define ATOMSIZE	16
#define BUFFERSIZE	2048
#define SECURE		1
#define dbBLOCKSIZE	512
#define dbMAXBLOCKKEYS 	169
#define dbMAXINDEXIDS	32

typedef struct DBheader
{
    //Blocktyp (1=Fileheader)
    //short BlockType;
    int BlockType;
    int IndexNumber;
    /* Indexanzahl der Indexdatei */
    //	short BlockSize;
    int FirstFreeBlock;
    int FirstFreeCell;
    int FreedCellBytes;
    int BlockSize;
    /* erster freier Block der Indexdatei */
    int IndexFirstBlock[65];
};

typedef struct DBindexheader
{
    //Blocktyp (2=Indexheader)
    //short BlockType;
    int BlockType;
    //short IndexID;
    int IndexID;
    int FirstIndexBlock;
    //short Keylength;
    int Keylength;
    //char EntryLength;
    int EntryLength;
    int KeyCount;
    int maxIndexLevel;
};

typedef struct DBindexblockHeader
{
    /* Blocktyp (3=Indexblock)*/
    short BlockType;
    char IndexID;
    char IndexLevel;
    char EntryLength;
    char KeyCount;
};

 /*
 typedef struct DBdataheader
 {
 long Datasize;
 int FirstFreeCell;
 int CellSize;
 int NumberOfEntries;
 };
    */

typedef struct DBdatacellHeader
{
    int SizeNetto;
};

typedef struct DBkey
{
    short T;
    int P;
    unsigned char K[120];
};

typedef struct DatabaseDatum
{
    unsigned char * ptr;
    int size;
};

class CDatabase
{
    public:
    CDatabase(int indexanzahl, int index1size, char * filename, char * basename);
    ~ CDatabase();
    int SEEKi(int position);
    void deleteKey(int indexID, DatabaseDatum key);
    DatabaseDatum fetchKey(int indexID, DatabaseDatum key);
    DatabaseDatum getNextKey(int indexID, DatabaseDatum key);
    DatabaseDatum getPrevKey(int indexID, DatabaseDatum key);
    int storeFile(int indexID, DatabaseDatum key, const char * filename,bool updateok);
    int storeData(int indexID, DatabaseDatum key, DatabaseDatum data);
    int updateData(int indexID, DatabaseDatum key, DatabaseDatum data);
    void BLKREAD(int blocknumber, unsigned char * blockbuffer);
    void BLKWRITE(int blocknumber, unsigned char * blockbuffer);
    void KEYBLKREAD(int blocknumber, DBindexblockHeader * ibh, DBkey ** keys);
    void KEYBLKWRITE(int blocknumber, DBindexblockHeader * ibh, DBkey ** keys);
    void ADDINDEX(int indexid, int keylength);
    int GETEFB();
    void PUTEFB(int blocktofree);
    void PUTEFC(int celltofree);
    void ENCODE(unsigned char * buffer, int size, long key);
    void DECODE(unsigned char * buffer, int size, long key);
    int CELLWRITE(int cellid, DatabaseDatum * data);
    int CELLREAD(int cellid, DatabaseDatum * data);
    void CELLFREE(int cellid);
    void SETKEY(DBkey * key, short entrytype, int blocknumber, DatabaseDatum * keydata);
    void CREATEDB(char * dateiname, int indexanzahl, int index1size);
    void KEYPREV(DatabaseDatum * key, int indexnummer, int & snr, int & status);
    void KEYNEXT(DatabaseDatum * key, int indexnummer, int & snr, int & status);
    void KEYDELETE(DatabaseDatum * key, int indexnummer, int & snr, int & status);
    void KEYFETCH(DatabaseDatum * key, int indexnummer, int & snr, int & status);
    void KEYCHANGEID(DatabaseDatum * key, int indexnummer, int & snr, int & status);
    void KEYINSERT(DatabaseDatum * key, int indexnummer, int & snr, int & status);
    void SEARCH2(int indexnummer, DatabaseDatum * key, int & snr, int & e);
    void SEARCH3(int indexnummer, DatabaseDatum * key, int & snr, int & e);
    void SEARCH4(int indexnummer, DatabaseDatum * key, int & snr, int & e);
    void SEARCH5(int indexnummer, DatabaseDatum * key, int & snr, int & e);
    int CREATEtempkey(DatabaseDatum * key);
    private:
    char * dbFilename;
    char * dbFilepath;
    long dbDatasize;
    long dbIndexsize;
    wxFile * dbIndexpointer;
    wxFile * dbDatapointer;
    short dbBlocksize;
    int getIndexsize();
    int getDatasize();
    struct stat * statinfo;
    DBheader dbheader;
    DBindexheader indexheader;
    DBindexblockHeader indexblockheader;
    DBkey * keys0[129];
    DatabaseDatum tempkey;
    int indexlevel;
    int keynumber[32];
    int blocknumber[32];
    char temppath[BUFFERSIZE];
    unsigned char keybuffer[BUFFERSIZE];
    unsigned char tempblock[dbBLOCKSIZE];
    long switchID;
    long debugFNR;
};

#endif
