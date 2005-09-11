#include "database.h"
#include "picframe.h"
#include <math.h>

int BlockCounter = 0, BlockKeyCounter = 0;
int kc0, bl0;
int debugmode = 0;

CDatabase::CDatabase(int indexanzahl, int index1size, char * filepath, char * filename)
{
    keys0Last.T = 1;
    keys0Last.P = 0x7fffffff;
    memset(keys0Last.K, 0xff, 128);
    if (indexanzahl < 0)
    {
        rwmode = 0;
        indexanzahl = - indexanzahl;
    }
    else
    {
        rwmode = 1;
    }
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

void CDatabase::DebugBlkKeys(int position, int blocknumber1)
{
    int i, bni;
    printf("vorher(pos=%d) bnr1=%d\n", position, blocknumber1);
    for (bni = 1 ; bni <= indexlevel ; bni++)
    {
        KEYBLKREAD(blocknumber[bni], & indexblockheader, keys0);
        printf("%d: bni=%d   %d=?=%d\n"
        , bni, blocknumber[bni], keynumber[bni], indexblockheader.KeyCount);
    }
    i = 0;
    while (i < indexblockheader.KeyCount)
    {
        ++i;
        printf("(%d/%d): %s %d %d\n"
        , i, indexblockheader.KeyCount, keys0[i] -> K, keys0[i] -> P, keys0[i] -> T);
    }
}

void CDatabase::deleteKey(int indexID, DatabaseDatum key)
{
    int cellid, status = 0;
    DatabaseDatum data;
    data.size = 0;
    data.ptr = NULL;
    KEYDELETE(key, indexID, cellid, status);
    if (!status)
    {
        //CELLFREE(cellid);
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

int CDatabase::reorgIndex(int indexID)
{
    int KeyCounter;
    DatabaseDatum keyTmp;
    DatabaseDatum keyTmp2;
    DatabaseDatum dataTmp;
    keyTmp.ptr = (unsigned char *) malloc(133);
    keyTmp.size = 20;
    keyTmp2.ptr = (unsigned char *) malloc(17);
    keyTmp2.size = 16;
    dataTmp.ptr = (unsigned char *) malloc(133);
    dataTmp.size = 132;
    (void) unlink("tmp.DBIDXi");
    (void) unlink("tmp.DBIDXd");
    CDatabase * tmpDB = new CDatabase(1, 16, "./", "tmp.DBIDX");
    int x, cellid, status;
    KeyCounter = 0;
    BlockCounter = 0;
    BlockKeyCounter = 0;
    memset(keyTmp.ptr, 0, 133);
    memset(keyTmp2.ptr, 0, 17);
    do
    {
        status = 0;
        cellid = 0;
        KEYNEXT(keyTmp, indexID, cellid, status);
        if (!status)
        {
            memset(dataTmp.ptr, 0, 132);
            memcpy(dataTmp.ptr, & cellid, 4);
            memcpy(dataTmp.ptr + 4, keyTmp.ptr, keyTmp.size);
            dataTmp.size = 4 + keyTmp.size;
            x = 0;
            do
            {
                sprintf((char *) keyTmp2.ptr, "%08x%08x", rand() & 0xffffffff, rand() & 0xffffffff);
                tmpDB -> KEYFETCH( & keyTmp2, indexID, cellid, status);
                if (status)
                {
                    cellid = tmpDB -> CELLWRITE(0, & dataTmp);
                    tmpDB -> KEYINSERT( & keyTmp2, indexID, cellid, status);
                    //printf("keytmp(%d): %s key: %s\n", cellid, keyTmp2.ptr, keyTmp.ptr);
                    ++x;
                }
                x = 1;
            }
            while (!x);
            if (!status)
            {
                KeyCounter++;
                KEYDELETE(keyTmp, indexID, cellid, status);
            }
            status = 0;
        }
    }
    while (!status);
 /*
 printf("nachherALL1: %d BC=%d BKC=%d bl0=%d kc0=%d\n"
 , KeyCounter, BlockCounter, BlockKeyCounter, bl0, kc0);
    */
    KeyCounter = 0;
    BlockCounter = 0;
    BlockKeyCounter = 0;
    memset(keyTmp.ptr, 0, 133);
    memset(keyTmp2.ptr, 0, 17);
    free(dataTmp.ptr);
    dataTmp.size = 0;
    dataTmp.ptr = NULL;
    do
    {
        status = 0;
        cellid = 0;
        tmpDB -> KEYNEXT(keyTmp2, indexID, cellid, status);
        if (!status)
        {
            (void) tmpDB -> CELLREAD(cellid, & dataTmp);
            memcpy( & cellid, dataTmp.ptr, 4);
            keyTmp.size = dataTmp.size - 4;
            memcpy(keyTmp.ptr, dataTmp.ptr + 4, keyTmp.size);
            KEYFETCH( & keyTmp, indexID, cellid, status);
            if (status)
            {
                memcpy( & cellid, dataTmp.ptr, 4);
                keyTmp.size = dataTmp.size - 4;
                memcpy(keyTmp.ptr, dataTmp.ptr + 4, keyTmp.size);
                KEYINSERT( & keyTmp, indexID, cellid, status);
                memcpy( & cellid, dataTmp.ptr, 4);
 /*
 printf("keytmp(%d): %s key: %s\n", cellid, keyTmp2.ptr, keyTmp.ptr);
    */
                KeyCounter++;
            }
            status = 0;
        }
    }
    while (!status);
 /*
 printf("nachherALL2: %d BC=%d BKC=%d bl0=%d kc0=%d\n"
 , KeyCounter, BlockCounter, BlockKeyCounter, bl0, kc0);
    */
    if (dataTmp.ptr)
    {
        free(dataTmp.ptr);
    }
    free(keyTmp2.ptr);
    free(keyTmp.ptr);
    delete tmpDB;
    return KeyCounter;
}

DatabaseDatum CDatabase::getNextKey(int indexID, DatabaseDatum key)
{
    int cellid = 0, status = 0;
    DatabaseDatum data;
    data.size = 0;
    data.ptr = NULL;
    KEYNEXT(key, indexID, cellid, status);
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
        if (!rwmode)
        {
            dbDatapointer = new wxFile(tempname, wxFile::read);
        }
        else
        {
            dbDatapointer = new wxFile(tempname, wxFile::read_write);
        }
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
        if (!rwmode)
        {
            dbIndexpointer = new wxFile(tempname, wxFile::read);
        }
        else
        {
            dbIndexpointer = new wxFile(tempname, wxFile::read_write);
        }
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
	    memset(&indexheader,0,sizeof(DBindexheader));
            indexheader.BlockType = 2;
            indexheader.IndexID = indexnummer;
            indexheader.FirstIndexBlock = GETEFB();
            indexheader.Keylength = keylaenge;
            indexheader.EntryLength = sizeof(DBkey) - 128 + (((3 + keylaenge) >> 2) << 2);
            indexheader.KeyCounterAll = 0;
	    indexheader.maxIndexLevel = 0;
            memset(tempblock, 0, dbBlocksize);
            memcpy(tempblock, & indexheader, sizeof(DBindexheader));
            BLKWRITE(dbheader.IndexFirstBlock[indexnummer], tempblock);
            indexblockheader.BlockType = 3;
            indexblockheader.IndexID = indexnummer;
            indexblockheader.IndexLevel = 1;
            indexblockheader.EntryLength = indexheader.EntryLength;
            indexblockheader.KeyCount = 1;
            memcpy(keys0[1], & keys0Last, indexheader.EntryLength);
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

void CDatabase::KEYNEXT(DatabaseDatum & key, int indexnummer, int & snr, int & e)
{
    if ((dbIndexsize > dbBlocksize) && (indexnummer > 0) && (indexnummer < 65))
    {
        BLKREAD(dbheader.IndexFirstBlock[indexnummer], tempblock);
        memcpy( & indexheader, tempblock, sizeof(DBindexheader));
        if (dbheader.IndexFirstBlock[indexnummer])
        {
            if (key .size > indexheader.Keylength)
            {
                key .size = indexheader.Keylength;
            }
            SEARCH3(indexnummer, & key, snr, e);
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

void CDatabase::KEYDELETE(DatabaseDatum & key, int indexnummer, int & snr, int & e)
{
    if ((dbIndexsize > dbBlocksize) && (indexnummer > 0) && (indexnummer < 65))
    {
        if (dbheader.IndexFirstBlock[indexnummer])
        {
            e = 0;
            BLKREAD(dbheader.IndexFirstBlock[indexnummer], tempblock);
            memcpy( & indexheader, tempblock, sizeof(DBindexheader));
            memset(tempkey.ptr, 0, 128);
            memcpy(tempkey.ptr, key.ptr, key.size);
            SEARCH5(indexnummer, tempkey, snr, e);
            memcpy(key.ptr, tempkey.ptr, key.size);
        }
        else
        {
            e = 2;
        }
    }
    else
    {
        e = 1;
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

void CDatabase::SEARCH3(int indexnummer, DatabaseDatum * key, int & snr, int & e)
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
        bnr1 = indexheader.FirstIndexBlock;
        keys0Tleft = 0;
        keys0Pleft = 0;
        memset(keys0[0], 0, 128);
        //printf("key:%s -> ", key -> ptr);
        indexlevel = 1;
        keynumber[0] = 0;
        do
        {
            keynumber[indexlevel] = 0;
            blocknumber[indexlevel] = bnr1;
            KEYBLKREAD(bnr1, & indexblockheader, keys0);
            if (!bl0)
            {
                bl0 = bnr1;
                kc0 = indexblockheader.KeyCount;
            }
            BlockCounter++;
            BlockKeyCounter += indexblockheader.KeyCount;
            keys0P = 0;
            keys0T = 0;
            memset(keys0K, 0, 128);
            if (direction)
            {
                i = 0;
                while (i < indexblockheader.KeyCount)
                {
                    ++i;
                    v = memcmp(key -> ptr, keys0[i] -> K, indexheader.Keylength);
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
                    v = memcmp(key -> ptr, keys0[i] -> K, indexheader.Keylength);
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
                        memcpy(key -> ptr, keys0K, indexheader.Keylength);
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

int CDatabase::GetMaxIndexLevel(int indexnummer)
{
    BLKREAD(dbheader.IndexFirstBlock[indexnummer], tempblock);
    memcpy( & indexheader, tempblock, sizeof(DBindexheader));
    return indexheader.maxIndexLevel;
}

void CDatabase::SEARCH4(int indexnummer, DatabaseDatum * key, int & snr, int & e)
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
            v = memcmp(key -> ptr, keys0[i] -> K, indexheader.Keylength);
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
                        indexheader.KeyCounterAll++;
                        SETKEY(keys0[i], 1, snr, key);
                        //**********gelesenen Block bnr1 zurückschreiben nach einfügen des Keys:
                        KEYBLKWRITE(bnr1, & indexblockheader, keys0);
                        z = 41;
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
                    indexheader.KeyCounterAll++;
                    SETKEY(keys0[ (int) indexblockheader.KeyCount], 1, snr, key);
                    KEYBLKWRITE(bnr1, & indexblockheader, keys0);
                    z = 42;
                }
                else
                {
                    i = indexblockheader.KeyCount;
                    switch (keys0[i] -> T)
                    {
                    case 1:
                        //*****KeyData-Entry zum KeyBlock-Entry wandeln, dabei key durch neuen key ersetzen:
                        memcpy(keys0[0], keys0[i], indexheader.EntryLength);
                        SETKEY(keys0[i], 2, GETEFB(), key);
                        KEYBLKWRITE(bnr1, & indexblockheader, keys0);
                        indexlevel++;
                        bnr1 = keys0[i] -> P;
                        indexblockheader.BlockType = 3;
                        indexblockheader.IndexID = indexnummer;
                        indexblockheader.IndexLevel = indexlevel;
                        indexblockheader.EntryLength = indexheader.EntryLength;
                        indexblockheader.KeyCount = 1;
                        indexheader.KeyCounterAll++;
                        memcpy(keys0[1], keys0[0], indexheader.EntryLength);
                        KEYBLKWRITE(bnr1, & indexblockheader, keys0);
                        z = 43;
                        break;
                    case 2:
                        bnr2 = keys0[i] -> P;
                        SETKEY(keys0[i], 2, bnr2, key);
                        KEYBLKWRITE(bnr1, & indexblockheader, keys0);
                        bnr1 = bnr2;
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
            e = 1;
            z = 4;
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

void CDatabase::SEARCH5(int indexnummer, DatabaseDatum & key, int & snr, int & e)
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
                v = memcmp(key .ptr, keys0[i] -> K, indexheader.Keylength);
                if (!v)
                {
                    bnr2 = keys0[i] -> P;
                    keynumber[indexlevel] = i;
                    i |= 0x4000;
                }
                else if(v < 0)
                {
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
                        bnr1 = blocknumber[indexlevel];
                        KEYBLKREAD(bnr1, & indexblockheader, keys0);
                        if (keys0[0] -> T)
                        {
                            memcpy(keys0[i] -> K, keys0[0] -> K, indexheader.Keylength);
                            keys0[0] -> P = blocknumber[indexlevel];
                            keys0[0] -> T = 2;
                            KEYBLKWRITE(blocknumber[indexlevel], & indexblockheader, keys0);
                            indexlevel--;
                            if (!keynumber[indexlevel])
                            {
                                indexlevel = - indexlevel;
                            }
                        }
                        else if(i == indexblockheader.KeyCount)
                        {
                            if (indexblockheader.KeyCount > 1)
                            {
                                indexblockheader.KeyCount--;
                                memcpy(keys0[0] -> K, keys0[ (int) indexblockheader.KeyCount] -> K, indexheader.Keylength);
                                keys0[0] -> P = blocknumber[indexlevel];
                                keys0[0] -> T = 2;
                                KEYBLKWRITE(blocknumber[indexlevel], & indexblockheader, keys0);
                                indexlevel--;
                                if (!keynumber[indexlevel])
                                {
                                    indexlevel = - indexlevel;
                                }
                            }
                            else if(indexlevel > 1)
                            {
                                PUTEFB(bnr1);
                                indexlevel--;
                            }
                            else
                            {
                                indexblockheader.KeyCount--;
                                KEYBLKWRITE(bnr1, & indexblockheader, keys0);
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
                            keys0[ (int) indexblockheader.KeyCount] = keys0temp;
                            indexblockheader.KeyCount--;
                            KEYBLKWRITE(bnr1, & indexblockheader, keys0);
                            if (indexblockheader.KeyCount == 1)
                            {
                                if (keys0[1] -> T == 1)
                                {
                                    if (!memcmp(keys0[1] -> K, keys0Last.K, indexheader.Keylength))
                                    {
                                        int p = keys0[1] -> P;
                                        while (indexlevel > 0)
                                        {
                                            KEYBLKREAD(bnr1, & indexblockheader, keys0);
                                            i = indexblockheader.KeyCount;
                                            if ((i == 1) && (indexlevel > 1))
                                            {
                                                PUTEFB(bnr1);
                                                indexlevel--;
                                                bnr1 = blocknumber[indexlevel];
                                            }
                                            else
                                            {
                                                keys0[i] -> T = 1;
                                                keys0[i] -> P = p;
                                                KEYBLKWRITE(bnr1, & indexblockheader, keys0);
                                                indexlevel = 0;
                                            }
                                        }
                                    }
                                }
                            }
                            indexlevel = - indexlevel;
                        }
                    }
                    while (indexlevel > 0);
                    z = 5;
                    indexheader.KeyCounterAll--;
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
    if (!e)
    {
        memset(tempblock, 0, dbBlocksize);
        memcpy(tempblock, & indexheader, sizeof(DBindexheader));
        BLKWRITE(dbheader.IndexFirstBlock[indexnummer], tempblock);
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

void CDatabase::ShowFileFromDB(wxFrame * parent, const char * key)
{
    DatabaseDatum tempKey;
    DatabaseDatum tempData;
    tempKey.ptr = (unsigned char *) malloc(17);
    memset(tempKey.ptr, 0, 17);
    tempKey.size = strlen(key);
    memcpy(tempKey.ptr, key, tempKey.size);
    tempData = fetchKey(1, tempKey);
    if (tempData.ptr)
    {
        if (!memcmp(tempKey.ptr + tempKey.size - 4, ".jpg", 4))
        {
            CpicFrame * tmp;
            tmp = new CpicFrame(parent, (char *) tempData.ptr, tempData.size, - 1, 540,
            (char *) tempKey.ptr);
            tmp -> Show();
        }
        else if(!memcmp(tempKey.ptr + tempKey.size - 4, ".txt", 4))
        {
            printf("key(%d): %s\n", tempKey.size, tempKey.ptr);
        }
        free(tempData.ptr);
        tempData.ptr = NULL;
        tempData.size = 0;
    }
    free(tempKey.ptr);
}

void CDatabase::ImportFileIntoDB(wxFrame * parent, const char * fpath, const char * fname)
{
    DatabaseDatum tempKey;
    DatabaseDatum tempData;
    char datafile[256];
    char datapath[256];
    tempKey.ptr = (unsigned char *) malloc(17);
    memset(tempKey.ptr, 0, 17);
    sprintf(datapath, "%s", fpath);
    sprintf(datafile, "%s", fname);
    wxFileDialog * fdDATA = new wxFileDialog(parent, "Bitte eine Datei auswaehlen zum Importieren:",
    datapath, datafile, "*.*");
    if (fdDATA -> ShowModal() == wxID_OK)
    {
        strcpy(datapath, fdDATA -> GetDirectory() .GetData());
        strcpy(datafile, fdDATA -> GetFilename() .GetData());
        tempKey.size = strlen(datafile);
        if (tempKey.size > 16)
        {
            memcpy(tempKey.ptr, datafile + tempKey.size - 16, 17);
            tempKey.size = 16;
        }
        else
        {
            memcpy(tempKey.ptr, datafile, tempKey.size + 1);
        }
        storeFile(1, tempKey, fdDATA -> GetPath() .GetData(), false);
        tempData = fetchKey(1, tempKey);
        if (tempData.ptr)
        {
            if (!memcmp(tempKey.ptr + tempKey.size - 4, ".jpg", 4))
            {
                CpicFrame * tmp;
                tmp = new CpicFrame(parent, (char *) tempData.ptr, tempData.size, - 1, 540,
                (char *) tempKey.ptr);
                tmp -> Show();
            }
            else if(!memcmp(tempKey.ptr + tempKey.size - 4, ".txt", 4))
            {
                printf("key(%d): %s\n", tempKey.size, tempKey.ptr);
            }
            free(tempData.ptr);
            tempData.ptr = NULL;
            tempData.size = 0;
        }
    }
    free(tempKey.ptr);
    delete fdDATA;
}

