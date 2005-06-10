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

#include "database.h"
#include <math.h>

CDatabase::CDatabase(int indexanzahl, int index1size, char * filepath, char * filename)
{
    tempkey.ptr = (unsigned char *) malloc(128);
    dbDatapointer = NULL;
    dbIndexpointer = NULL;
    for (int i = 0 ; i < 129 ; i++)
    {
        keys0[i] = (DBkey *) malloc(sizeof(struct DBkey));
    }
    statinfo = (struct stat *) malloc(sizeof(struct stat));
    dbBlocksize = dbBLOCKSIZE;
    dbFilepath = new char[strlen(filepath) + 1];
    strcpy(dbFilepath, filepath);
    dbFilename = new char[strlen(filename) + 1];
    strcpy(dbFilename, filename);
    sprintf(temppath, "%s/%s", dbFilepath, dbFilename);
    CREATEDB(temppath, indexanzahl, index1size);
    switchID = 0;
    debugFNR = 0;
}

CDatabase::~ CDatabase()
{
    for (int i = 0 ; i < 129 ; i++)
    {
        free(keys0[i]);
    }
    free(statinfo);
    if (dbIndexpointer)
    {
        delete dbIndexpointer;
        dbIndexpointer = NULL;
    }
    if (dbDatapointer)
    {
        delete dbDatapointer;
        dbDatapointer = NULL;
    }
    if (dbFilename)
    {
        delete[] dbFilename;
        dbFilename = NULL;
    }
    if (dbFilepath)
    {
        delete[] dbFilepath;
        dbFilepath = NULL;
    }
    free(tempkey.ptr);
}

void CDatabase::deleteKey(int indexID, DatabaseDatum key)
{
    int cellid, status = 0;
    DatabaseDatum data;
    data.size = 0;
    data.ptr = NULL;
    KEYDELETE( & key, indexID, cellid, status);
    if (!status)
    {
        CELLFREE(cellid);
    }
}

DatabaseDatum CDatabase::fetchKey(int indexID, DatabaseDatum key)
{
    int cellid, status = 0;
    DatabaseDatum data;
    data.size = 0;
    data.ptr = NULL;
    KEYFETCH( & key, indexID, cellid, status);
    if (!status)
    {
        (void) CELLREAD(cellid, & data);
    }
    return data;
}

DatabaseDatum CDatabase::getPrevKey(int indexID, DatabaseDatum key)
{
    int cellid = 0, status = 0;
    DatabaseDatum data;
    data.size = 0;
    data.ptr = NULL;
    KEYPREV( & key, indexID, cellid, status);
    if (!status)
    {
        (void) CELLREAD(cellid, & data);
    }
    return data;
}

DatabaseDatum CDatabase::getNextKey(int indexID, DatabaseDatum key)
{
    int cellid = 0, status = 0;
    DatabaseDatum data;
    data.size = 0;
    data.ptr = NULL;
    KEYNEXT( & key, indexID, cellid, status);
    if (!status)
    {
        (void) CELLREAD(cellid, & data);
    }
    else
    {
        data.ptr = NULL;
    }
    return data;
}

int CDatabase::storeFile(int indexID, DatabaseDatum key, const char * filename, bool updateok)
{
    int cellid = 0, status = 0;
    struct stat * stb = (struct stat *) malloc(sizeof(struct stat));
    DatabaseDatum data;
    if (!stat(filename, stb))
    {
        KEYFETCH( & key, indexID, cellid, status);
        if (status || updateok)
        {
            wxFile * fpi = new wxFile(filename, wxFile::read);
            if (fpi)
            {
                if (fpi -> IsOpened())
                {
                    int bytes = 0;
                    data.size = stb -> st_size;
                    data.ptr = (unsigned char *) malloc(data.size + 4);
                    do
                    {
                        bytes = fpi -> Read(data.ptr + bytes, data.size - bytes);
                    }
                    while (!fpi -> Eof());
                    if (status)
                    {
                        cellid = CELLWRITE(0, & data);
                        KEYINSERT( & key, indexID, cellid, status);
                    }
                    else if(updateok)
                    {
                        DatabaseDatum dataTemp;
                        dataTemp.size = 0;
                        dataTemp.ptr = NULL;
                        (void) CELLREAD(cellid, & dataTemp);
                        if (dataTemp.size != data.size)
                        {
                            printf("storeFile->updateok: new size\n");
                            // satz loeschen und neuen anfordern
                            //CELLFREE(cellid); to add
                            cellid = CELLWRITE(0, & data);
                            KEYCHANGEID( & key, indexID, cellid, status);
                        }
                        else
                        {
                            printf("storeFile->updateok: old size\n");
                            memcpy(dataTemp.ptr, data.ptr, dataTemp.size);
                            (void) CELLWRITE(cellid, & dataTemp);
                        }
                    }
                    free(data.ptr);
                }
                else
                {
                    status = - 3;
                }
                delete fpi;
            }
        }
        else
        {
            status = - 2;
            cellid = - cellid;
        }
    }
    else
    {
        status = - 1;
    }
    free(stb);
    return cellid;
}

int CDatabase::storeData(int indexID, DatabaseDatum key, DatabaseDatum data)
{
    int cellid, status = 0;
    KEYFETCH( & key, indexID, cellid, status);
    if (status)
    {
        cellid = CELLWRITE(0, & data);
        KEYINSERT( & key, indexID, cellid, status);
    }
    return status;
}

int CDatabase::updateData(int indexID, DatabaseDatum key, DatabaseDatum data)
{
    DatabaseDatum dataTemp;
    dataTemp.size = 0;
    dataTemp.ptr = NULL;
    int cellid, status = 0;
    KEYFETCH( & key, indexID, cellid, status);
    if (status)
    {
        //create
        cellid = CELLWRITE(0, & data);
        KEYINSERT( & key, indexID, cellid, status);
    }
    else
    {
        //update
        (void) CELLREAD(cellid, & dataTemp);
        if (dataTemp.size != data.size)
        {
            // satz loeschen und neuen anfordern
            //CELLFREE to add
            cellid = CELLWRITE(0, & data);
            KEYCHANGEID( & key, indexID, cellid, status);
        }
        else
        {
            memcpy(dataTemp.ptr, data.ptr, dataTemp.size);
            (void) CELLWRITE(cellid, & dataTemp);
        }
    }
    return status;
}

void CDatabase::BLKREAD(int blocknumber, unsigned char * blocktoread)
{
    long byteposition;
    int bytes, s;
    byteposition = (blocknumber - 1) * dbBlocksize;
    s = SEEKi(byteposition);
    if (byteposition == s)
    {
        bytes = dbIndexpointer -> Read(blocktoread, dbBlocksize);
        if (bytes != dbBlocksize)
        {
            printf(" pos =% ld error1 on read/ BLKREAD\n", byteposition);
        }
#ifdef SECURE
        DECODE(blocktoread, dbBlocksize, byteposition);
#endif
    }
    else
    {
        printf("% d:% d !=% ld error2 on seek/ BLKREAD\n", blocknumber, s, byteposition);
    }
}

void CDatabase::BLKWRITE(int blocknumber, unsigned char * blocktowrite)
{
    long byteposition;
    int s, bytes;
    byteposition = (blocknumber - 1) * dbBlocksize;
    s = SEEKi(byteposition);
    if (byteposition == s)
    {
#ifdef SECURE
        ENCODE(blocktowrite, dbBlocksize, byteposition);
#endif
        bytes = dbIndexpointer -> Write(blocktowrite, dbBlocksize);
        if (bytes == dbBlocksize)
        {
            if (dbIndexsize <= byteposition)
            {
                (void) getIndexsize();
            }
        }
        else
        {
            printf(" error on write/ BLKWRITE\n");
        }
    }
    else
    {
        printf(" error on seek/ BLKWRITE\n");
    }
}

void CDatabase::CREATEDB(char * dateiname, int indexanzahl, int index1size)
{
    struct stat * statbuffer = (struct stat *) malloc(sizeof(struct stat));
    char tempname[1025];
    if (indexanzahl > 64)
    {
        indexanzahl = 64;
    }
    else if(indexanzahl < 1)
    {
        indexanzahl = 1;
    }
    sprintf(tempname, "%sd", dateiname);
    if (!stat(tempname, statbuffer))
    {
        dbDatapointer = new wxFile(tempname, wxFile::read_write);
    }
    else
    {
        dbDatapointer = new wxFile(tempname, wxFile::write);
        delete dbDatapointer;
        dbDatapointer = new wxFile(tempname, wxFile::read_write);
    }
    sprintf(tempname, "%si", dateiname);
    if (!stat(tempname, statbuffer))
    {
        dbIndexpointer = new wxFile(tempname, wxFile::read_write);
    }
    else
    {
        dbIndexpointer = new wxFile(tempname, wxFile::write);
        delete dbIndexpointer;
        dbIndexpointer = new wxFile(tempname, wxFile::read_write);
    }
    if (dbIndexpointer -> IsOpened())
    {
        if (dbDatapointer -> IsOpened())
        {
            (void) getIndexsize();
            if (dbIndexsize < dbBLOCKSIZE)
            {
    /* header initialisieren */
 /*
 headeraufbau:
 0001..0004: Blocknummer erster Freier Block der Datei
 0005..0006: Indexanzahl in dieser Indexdatei
 0007..0008: Blocktyp(=1: Fileheader)
 0009..0010: Blockgroesse (dbBlocksize)
 0011..0012: Sperrkennzeichen u.a. fuer Rollback
 0013..0016: Dateigroesse aktuell (in Bloecken exklusive Rollback)
 0257..0260: Blocknummer Indexheader von Index 1
 0261..0264: Blocknummer Indexheader von Index 2
 ...
 0509..0512: Blocknummer Indexheader von Index 64
    */
                memset(tempblock, 0, dbBLOCKSIZE);
                dbheader.FirstFreeBlock = 2;
                dbheader.FirstFreeCell = 0;
                dbheader.FreedCellBytes = 0;
                dbheader.IndexNumber = indexanzahl;
                dbheader.BlockType = 1;
                dbheader.BlockSize = dbBLOCKSIZE;
                for (int i = 0 ; i < 65 ; i++)
                {
                    dbheader.IndexFirstBlock[i] = 0;
                }
                memcpy(tempblock, & dbheader, sizeof(DBheader));
                BLKWRITE(1, tempblock);
                ADDINDEX(1, index1size);
            }
            else
            {
                BLKREAD(1, tempblock);
                memcpy( & dbheader, tempblock, sizeof(DBheader));
            }
        }
    }
    delete statbuffer;
}

int CDatabase::GETEFB()
{
    int result;
    result = dbheader.FirstFreeBlock;
    if ((dbheader.FirstFreeBlock * dbBlocksize) > dbIndexsize)
    {
        memset(tempblock, 0, dbBLOCKSIZE);
        BLKWRITE(dbheader.FirstFreeBlock, tempblock);
        dbheader.FirstFreeBlock = 1 + (dbIndexsize/ dbBlocksize);
    }
    else
    {
        BLKREAD(result, tempblock);
        memcpy( & dbheader.FirstFreeBlock, tempblock, sizeof(dbheader.FirstFreeBlock));
    }
    memcpy(tempblock, & dbheader, sizeof(DBheader));
    BLKWRITE(1, tempblock);
    return(result);
}

void CDatabase::PUTEFB(int blocktoput)
{
    memset(tempblock, 0, dbBLOCKSIZE);
    memcpy(tempblock, & dbheader.FirstFreeBlock, sizeof(dbheader.FirstFreeBlock));
    BLKWRITE(blocktoput, tempblock);
    dbheader.FirstFreeBlock = blocktoput;
    memcpy(tempblock, & dbheader, sizeof(DBheader));
    BLKWRITE(1, tempblock);
}

int CDatabase::CELLWRITE(int cellid, DatabaseDatum * data)
{
    DBdatacellHeader datacellheader;
    int Brutto, allbytes;
    long cellposition;
    unsigned char * databuffer;
    if (!cellid)
    {
        cellid = 1 + (getDatasize() >> 2);
    }
    cellposition = (cellid - 1) <<2;
    if (cellposition < getDatasize())
    {
        dbDatapointer -> Seek(cellposition, wxFromStart);
        allbytes = dbDatapointer -> Read( & datacellheader, sizeof(DBdatacellHeader));
        Brutto = (data -> size + 3) >> 2;
        Brutto <<= 2;
        if (data -> size > Brutto)
        {
            data -> size = Brutto;
        }
        datacellheader .SizeNetto = data -> size;
    }
    else
    {
        datacellheader.SizeNetto = data -> size;
        Brutto = (data -> size + 3) >> 2;
        Brutto <<= 2;
    }
    dbDatapointer -> Seek(cellposition, wxFromStart);
    allbytes = dbDatapointer -> Write( & datacellheader, sizeof(DBdatacellHeader));
    databuffer = (unsigned char *) malloc(Brutto);
    memset(databuffer, 0, Brutto);
    memcpy(databuffer, data -> ptr, datacellheader.SizeNetto);
    ENCODE(databuffer, data -> size, cellposition + data -> size);
    allbytes = dbDatapointer -> Write(databuffer, Brutto);
    free(databuffer);
    return cellid;
}

void CDatabase::CELLFREE(int cellid)
{
    long cellposition;
    DBdatacellHeader datacellheader;
    int allbytes;
    cellposition = (cellid - 1) <<2;
    dbDatapointer -> Seek(cellposition, wxFromStart);
    allbytes = dbDatapointer -> Read( & datacellheader, sizeof(DBdatacellHeader));
    allbytes = dbDatapointer -> Write( & dbheader.FirstFreeCell, sizeof(dbheader.FirstFreeCell));
    allbytes = (datacellheader.SizeNetto + 3) >> 2;
    allbytes <<= 2;
    dbheader.FreedCellBytes += allbytes;
    dbheader.FirstFreeBlock = cellid;
    memcpy(tempblock, & dbheader, sizeof(DBheader));
    BLKWRITE(1, tempblock);
}

int CDatabase::CELLREAD(int cellid, DatabaseDatum * data)
{
    long cellposition;
    DBdatacellHeader datacellheader;
    int allbytes;
    if (data -> ptr)
    {
        free(data -> ptr);
    }
    cellposition = (cellid - 1) <<2;
    dbDatapointer -> Seek(cellposition, wxFromStart);
    allbytes = dbDatapointer -> Read( & datacellheader, sizeof(DBdatacellHeader));
    data -> size = datacellheader .SizeNetto;
    data -> ptr = (unsigned char *) malloc(data -> size + 1);
    allbytes = dbDatapointer -> Read(data -> ptr, data -> size);
    DECODE(data -> ptr, data -> size, cellposition + data -> size);
    return allbytes;
}

void CDatabase::ADDINDEX(int indexnummer, int keylaenge)
{
    if (keylaenge > 120)
    {
        keylaenge = 120;
    }
    if ((dbIndexsize >= dbBlocksize) & (indexnummer > 0) & (indexnummer < 65))
    {
        if (!dbheader.IndexFirstBlock[indexnummer])
        {
            dbheader.IndexFirstBlock[indexnummer] = GETEFB();
            indexheader.FirstIndexBlock = GETEFB();
            indexheader.Keylength = keylaenge;
            indexheader.BlockType = 2;
            indexheader.EntryLength = sizeof(DBkey) - 120 + (((3 + keylaenge) >> 2) << 2);
            indexheader.KeyCount = 0;
            memset(tempblock, 0, dbBlocksize);
            memcpy(tempblock, & indexheader, sizeof(DBindexheader));
            BLKWRITE(dbheader.IndexFirstBlock[indexnummer], tempblock);
            indexblockheader.BlockType = 3;
            indexblockheader.IndexID = indexnummer;
            indexblockheader.IndexLevel = 1;
            indexblockheader.EntryLength = indexheader.EntryLength;
            indexblockheader.KeyCount = 1;
            DBkey tempKey;
            tempKey.T = 1;
            tempKey.P = 0x7fffffff;
            memset(tempKey.K, 255, indexheader.Keylength);
            memcpy(keys0[1], & tempKey, indexheader.EntryLength);
            KEYBLKWRITE(indexheader.FirstIndexBlock, & indexblockheader, keys0);
            memcpy(tempblock, & dbheader, sizeof(DBheader));
            BLKWRITE(1, tempblock);
        }
        else
        {
            printf(" ADDINDEX - error:Index existiert schon:% d\n", dbheader.IndexFirstBlock[indexnummer]);
        }
    }
}

 /* noch zu programmieren:
 ...reorgbytes (2 stueck) je key zur temporaeren verwendung
    */

 /*
 void SEARCH1(int fnr, int indexnummer, int snr, int e)
 {
 int i, dkanal;
 dkanal = kanal;
 dkanal--;
 switch (fnr)
 {
 case 1:
 switch (e)
 {
 case 1://satzlaenge festlegen & datenbereich initialisieren
 datsl = snr;
 // satzlaenge in snr
 LETS(datheader, STRING(datsl, CHR(0)));
 MIDP(datheader, 1, 4, MKL(1));
 //erster freier datensatz
 MIDP(datheader, 5, 2, MKI(datsl));
 WRITE(dkanal, 0, datheader);
 i = 1;
 LETS(datblock, STRING(datsl, CHR(0)));
 while (i * datsl < idxdl[ - glkanal])
 {
 MIDP(datblock, 1, 4, MKL(i + 1));
 //erster freier datensatz
 WRITE(dkanal, i * datsl, datblock);
 i++;
 }
 break;
 case 2://freien satz holen
 snr = getefs(dkanal);
 e = 0;
 break;
 case 3://satz freigeben
 putefs(dkanal, snr);
 e = 0;
 break;
 }
 break;
 }
 s999 = snr;
 e999 = e;
 }
    */

 /*
 void putefs(int kanal, int snr)
 {
 int efs;//erster freier satz
 unsigned char * satz;
 if (snr > 0)
 {
 GFAIS(satz, 0);
 LETS(satz, READanz(kanal, snr * datsl, datsl));
 if (ASC(MID(satz, datsl, 1)) != 255)
 {
 LETS(datheader, READanz(kanal, 0, 6));
 datsl = CVI(MID(datheader, 5, 2));
 efs = CVL(MID(datheader, 1, 4));
 MIDP(datheader, 1, 4, MKL(snr));
 WRITE(kanal, 0, datheader);
 LETS(satz, STRING(datsl, CHR(0)));
 MIDP(satz, 1, 4, MKL(efs));
 MIDP(satz, datsl, 1, CHR(255));
 WRITE(kanal, snr * datsl, satz);
 }
 FREES(satz);
 }
 }
    */

void CDatabase::KEYNEXT(DatabaseDatum * key, int indexnummer, int & snr, int & e)
{
    if ((dbIndexsize > dbBlocksize) && (indexnummer > 0) && (indexnummer < 65))
    {
        BLKREAD(dbheader.IndexFirstBlock[indexnummer], tempblock);
        memcpy( & indexheader, tempblock, sizeof(DBindexheader));
        if (dbheader.IndexFirstBlock[indexnummer])
        {
            SEARCH3(indexnummer, key, snr, e);
        }
    }
}

void CDatabase::KEYPREV(DatabaseDatum * key, int indexnummer, int & snr, int & e)
{
    if ((dbIndexsize > dbBlocksize) && (indexnummer > 0) && (indexnummer < 65))
    {
        BLKREAD(dbheader.IndexFirstBlock[indexnummer], tempblock);
        memcpy( & indexheader, tempblock, sizeof(DBindexheader));
        if (dbheader.IndexFirstBlock[indexnummer])
        {
            SEARCH3( - indexnummer, key, snr, e);
        }
    }
}

void CDatabase::KEYDELETE(DatabaseDatum * key, int indexnummer, int & snr, int & e)
{
    if ((dbIndexsize > dbBlocksize) && (indexnummer > 0) && (indexnummer < 65))
    {
        BLKREAD(dbheader.IndexFirstBlock[indexnummer], tempblock);
        memcpy( & indexheader, tempblock, sizeof(DBindexheader));
        e = CREATEtempkey(key);
        if (!e)
        {
            if (dbheader.IndexFirstBlock[indexnummer])
            {
                SEARCH5(indexnummer, & tempkey, snr, e);
            }
        }
    }
}

void CDatabase::KEYFETCH(DatabaseDatum * key, int indexnummer, int & snr, int & e)
{
    if ((dbIndexsize > dbBlocksize) && (indexnummer > 0) && (indexnummer < 65))
    {
        BLKREAD(dbheader.IndexFirstBlock[indexnummer], tempblock);
        memcpy( & indexheader, tempblock, sizeof(DBindexheader));
        e = CREATEtempkey(key);
        if (!e)
        {
            if (dbheader.IndexFirstBlock[indexnummer])
            {
                SEARCH2(indexnummer, & tempkey, snr, e);
            }
        }
    }
}

void CDatabase::KEYCHANGEID(DatabaseDatum * key, int indexnummer, int & snr, int & e)
{
    if ((dbIndexsize > dbBlocksize) && (indexnummer > 0) && (indexnummer < 65))
    {
        BLKREAD(dbheader.IndexFirstBlock[indexnummer], tempblock);
        memcpy( & indexheader, tempblock, sizeof(DBindexheader));
        e = CREATEtempkey(key);
        if (!e)
        {
            if (dbheader.IndexFirstBlock[indexnummer])
            {
                switchID = snr;
                SEARCH2(indexnummer, & tempkey, snr, e);
                switchID = 0;
            }
        }
    }
}

void CDatabase::KEYINSERT(DatabaseDatum * key, int indexnummer, int & snr, int & e)
{
    if ((dbIndexsize > dbBlocksize) && (indexnummer > 0) && (indexnummer < 65))
    {
        BLKREAD(dbheader.IndexFirstBlock[indexnummer], tempblock);
        memcpy( & indexheader, tempblock, sizeof(DBindexheader));
        e = CREATEtempkey(key);
        if (!e)
        {
            if (dbheader.IndexFirstBlock[indexnummer])
            {
                SEARCH4(indexnummer, & tempkey, snr, e);
            }
        }
    }
}

void CDatabase::SEARCH2(int indexnummer, DatabaseDatum * tempkey, int & snr, int & e)
{
    int z = 0, i = 0, v = 0;
    int bnr1 = 0, bnr2 = 0;
    e = 0;
    snr = 0;
    if ((indexnummer > 0) && (indexnummer < 65))
    {
        indexlevel = 1;
        keynumber[0] = 0;
        bnr1 = indexheader.FirstIndexBlock;
        do
        {
            blocknumber[indexlevel] = bnr1;
            keynumber[indexlevel] = 0;
            KEYBLKREAD(bnr1, & indexblockheader, keys0);
            bnr2 = 0;
            i = 0;
            while (i < indexblockheader.KeyCount)
            {
                ++i;
                v = memcmp(tempkey -> ptr, keys0[i] -> K, indexheader.Keylength);
                if (!v)
                {
                    bnr2 = keys0[i] -> P;
                    keynumber[indexlevel] = i;
                    i |= 0x4000;
                }
                else if(v < 0)
                {
                    indexblockheader.KeyCount = - indexblockheader.KeyCount;
                    i |= 0x4000;
                }
            }
            if (!bnr2)
            {
                if (i & 0x4000)
                {
                    i &= 0x3fff;
                    switch (keys0[i] -> T)
                    {
                    case 1:
                        e = 2;
                        z = e;
                        break;
                    case 2:
                        indexlevel++;
                        bnr1 = keys0[i] -> P;
                        break;
                    }
                }
                else
                {
                    e = 2;
                    z = e;
                }
            }
            else
            {
                i &= 0x3fff;
                switch (keys0[i] -> T)
                {
                case 1:
                    snr = bnr2;
                    if (switchID > 0)
                    {
                        keys0[i] -> P = switchID;
                        KEYBLKWRITE(bnr1, & indexblockheader, keys0);
                        printf("s2: keyswitchid from %d to %ld\n", snr, switchID);
                        switchID = - switchID;
                    }
                    ++z;
                    break;
                case 2:
                    indexlevel++;
                    bnr1 = bnr2;
                    break;
                }
            }
        }
        while (!z);
    }
}

void CDatabase::SEARCH3(int indexnummer, DatabaseDatum * tempkey, int & snr, int & e)
{
    int keys0T, keys0P;
    char keys0K[128];
    int keys0Tleft, keys0Pleft;
    char keys0Kleft[128];
    int z = 0, i = 0, v = 0;
    int bnr1 = 0;
    int direction = 1;
    if (indexnummer < 0)
    {
        direction = 0;
        indexnummer = - indexnummer;
    }
    if ((indexnummer > 0) && (indexnummer < 65))
    {
        e = 0;
        snr = 0;
        indexlevel = 1;
        bnr1 = indexheader.FirstIndexBlock;
	keys0Tleft = 0;
        keys0Pleft = 0;
        do
        {
            blocknumber[indexlevel] = bnr1;
            KEYBLKREAD(bnr1, & indexblockheader, keys0);
            keys0P = 0;
            keys0T = 0;
            memset(keys0K, 0, 128);
            if (direction)
            {
                i = 0;
                while (i < indexblockheader.KeyCount)
                {
                    ++i;
                    v = memcmp(tempkey -> ptr, keys0[i] -> K, indexheader.Keylength);
                    if (v < 0)
                    {
                        keys0T = keys0[i] -> T;
                        keys0P = keys0[i] -> P;
                        memcpy(keys0K, keys0[i] -> K, indexheader.Keylength);
                        i = indexblockheader.KeyCount;
                    }
                }
            }
            else
            {
                i = 0;
                while (i < indexblockheader.KeyCount)
                {
                    ++i;
                    v = memcmp(tempkey -> ptr, keys0[i] -> K, indexheader.Keylength);
                    if (v <= 0)
                    {
                        if (keys0[i] -> T == 2)
                        {
                            keys0T = keys0[i] -> T;
                            keys0P = keys0[i] -> P;
                            memcpy(keys0K, keys0[i] -> K, indexheader.Keylength);
                        }
                        else if(keys0Pleft)
                        {
                            keys0T = keys0Tleft;
                            keys0P = keys0Pleft;
                            memcpy(keys0K, keys0Kleft, indexheader.Keylength);
                            keys0Pleft = 0;
                        }
                        i |= 0x4000;
                    }
                    else
                    {
                        keys0T = keys0[i] -> T;
                        keys0P = keys0[i] -> P;
                        memcpy(keys0K, keys0[i] -> K, indexheader.Keylength);
                        keys0Tleft = keys0[i] -> T;
                        keys0Pleft = keys0[i] -> P;
                        memcpy(keys0Kleft, keys0[i] -> K, indexheader.Keylength);
                    }
                }
            }
            i &= 0x3fff;
            if (!keys0P)
            {
                e = 1;
                z = 3;
            }
            else
            {
                switch (keys0T)
                {
                case 2:
                    indexlevel++;
                    bnr1 = keys0P;
                    break;
                default:
                    if (keys0P == 0x7fffffff)
                    {
                        e = 1;
                    }
                    else
                    {
                        e = 0;
                        snr = keys0P;
                        memcpy(tempkey -> ptr, keys0K, indexheader.Keylength);
                    }
                    z = 3;
                    break;
                }
            }
        }
        while (!z);
    }
    else
    {
        e = 3;
        snr = 0;
    }
}

void CDatabase::SETKEY(DBkey * key, short entrytype, int blocknumber, DatabaseDatum * keydata)
{
    memset(key -> K, 0, indexheader.EntryLength);
    key -> T = entrytype;
    key -> P = blocknumber;
    if (keydata -> size)
    {
        memcpy(key -> K, keydata -> ptr, keydata -> size);
    }
}

void CDatabase::SEARCH4(int indexnummer, DatabaseDatum * tempkey, int & snr, int & e)
{
    int z = 0, i = 0, v = 0;
    int bnr1 = 0, bnr2 = 0;
    int usedbytes;
    e = 0;
    indexlevel = 1;
    keynumber[0] = 0;
    BLKREAD(dbheader.IndexFirstBlock[indexnummer], tempblock);
    memcpy( & indexheader, tempblock, sizeof(DBindexheader));
    bnr1 = indexheader.FirstIndexBlock;
    do
    {
        blocknumber[indexlevel] = bnr1;
        keynumber[indexlevel] = 0;
        if (indexlevel > indexheader.maxIndexLevel)
        {
            indexheader.maxIndexLevel = indexlevel;
        }
        KEYBLKREAD(bnr1, & indexblockheader, keys0);
        bnr2 = 0;
        i = 0;
        while (i < indexblockheader.KeyCount)
        {
            ++i;
            v = memcmp(tempkey -> ptr, keys0[i] -> K, indexheader.Keylength);
            if (!v)
            {
                bnr2 = keys0[i] -> P;
                keynumber[indexlevel] = i;
                indexblockheader.KeyCount = - indexblockheader.KeyCount;
            }
            else if(v < 0)
            {
                indexblockheader.KeyCount = - indexblockheader.KeyCount;
            }
        }
        if (!bnr2)
        {
            if (indexblockheader.KeyCount < 0)
            {
                indexblockheader.KeyCount = - indexblockheader.KeyCount;
                usedbytes = sizeof(DBindexheader) + indexheader.EntryLength * indexblockheader.KeyCount;
                switch (keys0[i] -> T)
                {
                case 1:
                    //nächstgrößerer Key ist ein Dateneintrag, kein Blockeintrag:
                    if ((usedbytes + indexheader.EntryLength) <= dbBlocksize)
                    {
                        //**********Key einfuegen in diesen Block an Position <i>:
                        int n = indexblockheader.KeyCount;
                        DBkey * keys0temp;
                        keys0temp = keys0[n + 1];
                        while (i <= n)
                        {
                            keys0[n + 1] = keys0[n];
                            n--;
                        }
                        keys0[i] = keys0temp;
                        indexblockheader.KeyCount++;
                        indexheader.KeyCount++;
                        SETKEY(keys0[i], 1, snr, tempkey);
                        //**********gelesenen Block bnr1 zurückschreiben nach einfügen des Keys:
                        KEYBLKWRITE(bnr1, & indexblockheader, keys0);
                        z = 4;
                    }
                    else
                    {
                        memcpy(keys0[0], keys0[i], indexheader.EntryLength);
                        keys0[i] -> T = 2;
                        keys0[i] -> P = GETEFB();
                        KEYBLKWRITE(bnr1, & indexblockheader, keys0);
                        indexlevel++;
                        bnr1 = keys0[i] -> P;
                        indexblockheader.BlockType = 3;
                        indexblockheader.IndexID = indexnummer;
                        indexblockheader.IndexLevel = indexlevel;
                        indexblockheader.EntryLength = indexheader.EntryLength;
                        indexblockheader.KeyCount = 1;
                        memcpy(keys0[1], keys0[0], indexheader.EntryLength);
                        KEYBLKWRITE(bnr1, & indexblockheader, keys0);
                    }
                    break;
                case 2:
                    indexlevel++;
                    bnr1 = keys0[i] -> P;
                    break;
                default:
                    break;
                }
            }
            else
            {
                usedbytes = sizeof(DBindexheader) + indexheader.EntryLength * indexblockheader.KeyCount;
                if ((usedbytes + indexheader.EntryLength) <= dbBlocksize)
                {
                    indexblockheader.KeyCount++;
                    indexheader.KeyCount++;
                    SETKEY(keys0[(int)indexblockheader.KeyCount], 1, GETEFB(), tempkey);
                    KEYBLKWRITE(bnr1, & indexblockheader, keys0);
                    z = 4;
                }
                else
                {
                    switch (keys0[i] -> T)
                    {
                    case 1:
                        //*****KeyData-Entry zum KeyBlock-Entry wandeln, dabei key durch neuen key ersetzen:
                        memcpy(keys0[0], keys0[(int)indexblockheader.KeyCount], indexheader.EntryLength);
                        SETKEY(keys0[(int)indexblockheader.KeyCount], 2, GETEFB(), tempkey);
                        KEYBLKWRITE(bnr1, & indexblockheader, keys0);
                        indexlevel++;
                        bnr1 = keys0[(int)indexblockheader.KeyCount] -> P;
                        indexblockheader.BlockType = 3;
                        indexblockheader.IndexID = indexnummer;
                        indexblockheader.IndexLevel = indexlevel;
                        indexblockheader.EntryLength = indexheader.EntryLength;
                        indexblockheader.KeyCount = 1;
                        memcpy(keys0[1], keys0[0], indexheader.EntryLength);
                        KEYBLKWRITE(bnr1, & indexblockheader, keys0);
                        break;
                    case 2:
                        SETKEY(keys0[(int)indexblockheader.KeyCount], 2, bnr1, tempkey);
                        KEYBLKWRITE(bnr1, & indexblockheader, keys0);
                        bnr1 = keys0[(int)indexblockheader.KeyCount] -> P;
                        indexlevel++;
                        break;
                    default:
                        break;
                    }
                }
            }
        }
        else
        {
            switch (keys0[i] -> T)
            {
            case 1:
                e = 1;
                z = 4;
                break;
            case 2:
                bnr1 = bnr2;
                indexlevel++;
                break;
            }
        }
    }
    while (!z);
    if (!e)
    {
        memset(tempblock, 0, dbBlocksize);
        memcpy(tempblock, & indexheader, sizeof(DBindexheader));
        BLKWRITE(dbheader.IndexFirstBlock[indexnummer], tempblock);
    }
}

void CDatabase::SEARCH5(int indexnummer, DatabaseDatum * tempkey, int & snr, int & e)
{
    int z = 0, i = 0, v = 0;
    int bnr1 = 0, bnr2 = 0;
    e = 0;
    snr = 0;
    if ((indexnummer > 0) && (indexnummer < 65))
    {
        indexlevel = 1;
        keynumber[0] = 0;
        bnr1 = indexheader.FirstIndexBlock;
        do
        {
            blocknumber[indexlevel] = bnr1;
            keynumber[indexlevel] = 0;
            KEYBLKREAD(bnr1, & indexblockheader, keys0);
            bnr2 = 0;
            i = 0;
            while (i < indexblockheader.KeyCount)
            {
                ++i;
                v = memcmp(tempkey -> ptr, keys0[i] -> K, indexheader.Keylength);
                if (!v)
                {
                    bnr2 = keys0[i] -> P;
                    keynumber[indexlevel] = i;
                    i |= 0x4000;
                }
                else if(v < 0)
                {
                    indexblockheader.KeyCount = - indexblockheader.KeyCount;
                    i |= 0x4000;
                }
            }
            if (!bnr2)
            {
                if (i & 0x4000)
                {
                    i &= 0x3fff;
                    switch (keys0[i] -> T)
                    {
                    case 1:
                        e = 2;
                        z = e;
                        break;
                    case 2:
                        indexlevel++;
                        bnr1 = keys0[i] -> P;
                        break;
                    }
                }
                else
                {
                    e = 2;
                    z = e;
                }
            }
            else
            {
                i &= 0x3fff;
                switch (keys0[i] -> T)
                {
                case 1:
                    snr = bnr2;
                    keys0[0] -> T = 0;
                    do
                    {
                        i = keynumber[indexlevel];
                        if (keys0[0] -> T)
                        {
                            memcpy(keys0[i] -> K, keys0[0] -> K, indexheader.Keylength);
                            keys0[0] -> P = blocknumber[indexlevel];
                            keys0[0] -> T = 2;
                            KEYBLKWRITE(blocknumber[indexlevel], & indexblockheader, keys0);
                            indexlevel--;
                            if (keynumber[indexlevel])
                            {
                                KEYBLKREAD(blocknumber[indexlevel], & indexblockheader, keys0);
                            }
                            else
                            {
                                indexlevel = - indexlevel;
                            }
                        }
                        else if(i == indexblockheader.KeyCount)
                        {
                            if (indexblockheader.KeyCount > 1)
                            {
                                indexblockheader.KeyCount--;
                                memcpy(keys0[0] -> K, keys0[(int)indexblockheader.KeyCount] -> K, indexheader.Keylength);
                                keys0[0] -> P = blocknumber[indexlevel];
                                keys0[0] -> T = 2;
                                KEYBLKWRITE(blocknumber[indexlevel], & indexblockheader, keys0);
                                indexlevel--;
                                if (keynumber[indexlevel])
                                {
                                    KEYBLKREAD(blocknumber[indexlevel], & indexblockheader, keys0);
                                }
                                else
                                {
                                    indexlevel = - indexlevel;
                                }
                            }
                            else if(indexlevel > 1)
                            {
                                PUTEFB(blocknumber[indexlevel]);
                                indexlevel--;
                                if (keynumber[indexlevel])
                                {
                                    KEYBLKREAD(blocknumber[indexlevel], & indexblockheader, keys0);
                                }
                                else
                                {
                                    indexlevel = - indexlevel;
                                }
                            }
                            else
                            {
                                indexblockheader.KeyCount--;
                                KEYBLKWRITE(blocknumber[indexlevel], & indexblockheader, keys0);
                                indexlevel = - indexlevel;
                            }
                        }
                        else
                        {
                            DBkey * keys0temp;
                            keys0temp = keys0[i];
                            while (i < indexblockheader.KeyCount)
                            {
                                keys0[i] = keys0[i + 1];
                                i++;
                            }
                            keys0[indexheader.KeyCount] = keys0temp;
                            indexblockheader.KeyCount--;
                            KEYBLKWRITE(blocknumber[indexlevel], & indexblockheader, keys0);
                            indexlevel = - indexlevel;
                        }
                    }
                    while (indexlevel > 0);
                    z = 5;
                    break;
                case 2:
                    indexlevel++;
                    bnr1 = bnr2;
                    break;
                }
            }
        }
        while (!z);
    }
}

void CDatabase::KEYBLKWRITE(int blocknummer, DBindexblockHeader * ibh, DBkey ** keys)
{
    //ok 20050515
    int i = 0;
    unsigned char * tb = tempblock;
    memset(tb, 0, dbBlocksize);
    memcpy(tb, ibh, sizeof(DBindexblockHeader));
    tb += sizeof(DBindexblockHeader);
    while (i < ibh -> KeyCount)
    {
        memcpy(tb, keys[++i], ibh -> EntryLength);
        tb += ibh -> EntryLength;
    }
    BLKWRITE(blocknummer, tempblock);
}

void CDatabase::KEYBLKREAD(int blocknumber, DBindexblockHeader * ibh, DBkey ** keys)
{
    //ok 20050515
    int i = 0;
    unsigned char * tb = tempblock;
    BLKREAD(blocknumber, tempblock);
    memcpy(ibh, tb, sizeof(DBindexblockHeader));
    tb += sizeof(DBindexblockHeader);
    while (i < ibh -> KeyCount)
    {
        memcpy(keys[++i], tb, ibh -> EntryLength);
        tb += ibh -> EntryLength;
    }
}

int CDatabase::getDatasize()
{
    /* ok 20050503*/
    dbDatasize = dbDatapointer -> Length();
    return dbDatasize;
}

int CDatabase::getIndexsize()
{
    /* ok 20050503*/
    dbIndexsize = dbIndexpointer -> Length();
    return dbIndexsize;
}

int CDatabase::CREATEtempkey(DatabaseDatum * key)
{
    int e = 0, i = 0;
    memset(tempkey.ptr, 0, 128);
    if (key -> size > indexheader.Keylength)
    {
        tempkey.size = indexheader.Keylength;
    }
    else
    {
        tempkey.size = key -> size;
    }
    memcpy(tempkey.ptr, key -> ptr, tempkey.size);
    if (tempkey .size > 0)
    {
        if (tempkey.ptr[0] == 255)
        {
            while (i < tempkey.size)
            {
                if (tempkey.ptr[i] != 255)
                {
                    i += 1000;
                }
                else
                {
                    i++;
                }
            }
            if (i < 1000)
            {
                e = 98;
            }
        }
    }
    else
    {
        e = 99;
    }
    return e;
}

int CDatabase::SEEKi(int position)
{
    int retvalue;
    retvalue = (int) dbIndexpointer -> Seek(position, wxFromStart);
    return retvalue;
}

void CDatabase::ENCODE(unsigned char * buffer, int size, long keylong)
{
    int byte;
    for (int i = 0 ; i < size ; i++)
    {
        byte = buffer[i] + (int)((float) sin(keylong + i) * 256);
        if (byte > 255)
        {
            byte -= 256;
        }
        buffer[i] = byte;
    }
}

void CDatabase::DECODE(unsigned char * buffer, int size, long keylong)
{
    int byte;
    for (int i = 0 ; i < size ; i++)
    {
        byte = buffer[i] - (int)((float) sin(keylong + i) * 256);
        if (byte < 0)
        {
            byte += 256;
        }
        buffer[i] = byte;
    }
}

