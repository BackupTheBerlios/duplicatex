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

#ifndef MINERDLG_H
#define MINERDLG_H
//#define WITHGDBM

#ifdef WITHGDBM
#include <gdbm.h>
#endif

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include "metfile.h"
#include "md4hash.h"
#include "database.h"
#include "picframe.h"
#include "htmlpage.h"
#include "avifile.h"
#include "jpgfile.h"

class CminerDlg:public wxPanel
{
    /****************************/
    public:
    CminerDlg(wxWindow * parent);
    ~ CminerDlg();
    int filterNames(char *& filename, char *& sourcename);
    int checkSubhash(unsigned char * subHash,int partid);
    void storeSubhash(int mode, unsigned char * subHash, unsigned char * mainHash, char * datas);
    void executeFiletype();
    void enterSubdir(const char * subdirname);
    void leaveSubdir();
    void sampleGetBase();
    void protoSubdirs();
    void extractFilename(char * target, char * source);
    void storeUrls(char * url1, char * url2);
    void GetDomain(char * domain, char * source);
    void checkANDstoreHTM(char * filename);
    void checkANDstoreJPG(char * filename);
    void lan00German();
    void lan01English();
    void sub_ISMimportMD4();
    void sub_METimportMD4();
    void sub_PRTcreateOVN();
    void sub_Repair_METfile(char *filename);
    void SetHostname(char * name);
    void SetLocalIP(char * ip);
    void SetStorageID(char * id);
    void ISMclearSHDB(wxCommandEvent & event);
    void ISMexportMD4(wxCommandEvent & event);
    void ButtonMATCHDOMAINclr(wxCommandEvent & event);
    void ButtonMATCHSTARTclr(wxCommandEvent & event);
    void ButtonMATCHURLclr(wxCommandEvent & event);
    void ButtonCHDD(wxCommandEvent & event);
    void ButtonCHSD(wxCommandEvent & event);
    void ButtonCHTD(wxCommandEvent & event);
    void ButtonCLEAR(wxCommandEvent & event);
    void ButtonPARSE(wxCommandEvent & event);
    void ButtonDBHANDLING(wxCommandEvent & event);
    void ExportDatabaseTXT(wxCommandEvent & event);
    void ImportDatabaseTXT(wxCommandEvent & event);
#ifdef WITHGDBM
    void ImportDBMAIN(wxCommandEvent & event);
#endif
    void ED2KhashMD4(wxCommandEvent & event);
    void ButtonProgramminfo(wxCommandEvent & event);
    void ImportJpgFile(wxCommandEvent & event);
    void ButtonMD4HASHS(wxCommandEvent & event);
    void ButtonAVIparse(wxCommandEvent & event);
    void ButtonDKTparse(wxCommandEvent & event);
    void ButtonGIFparse(wxCommandEvent & event);
    void ButtonHTMparse(wxCommandEvent & event);
    void ButtonISMparse(wxCommandEvent & event);
    void ButtonJPGparse(wxCommandEvent & event);
    void ButtonMETparse(wxCommandEvent & event);
    void ButtonMP3parse(wxCommandEvent & event);
    void ButtonMPGparse(wxCommandEvent & event);
    void ButtonPRTparse(wxCommandEvent & event);
    void ButtonSTDparse(wxCommandEvent & event);
    void ButtonWAVparse(wxCommandEvent & event);
    void ButtonWD3parse(wxCommandEvent & event);
    void ButtonWMFparse(wxCommandEvent & event);
    void ButtonWMVparse(wxCommandEvent & event);
    void ButtonRENAME(wxCommandEvent & event);
    void ButtonSTART(wxCommandEvent & event);
    void ButtonGENTFD(wxCommandEvent & event);
    void ButtonSTOP(wxCommandEvent & event);
    /* MAD=markup duples, RED=Remove .dup-Extension, DED=Delete Duples*/
    wxMenu * smPMfinddups;
    void ButtonFINDDUPS(wxCommandEvent & event);
    wxMenu * smPMhocfiles;
    wxMenu * smPMstdfiles;
    void GetFilename(char * filename, char * sourcepath);
    void ButtonHOCFILES(wxCommandEvent & event);
    void RadioboxLAN(wxCommandEvent & event);
    void CheckCOKEY(wxCommandEvent & event);
    void CheckboxMAD(wxCommandEvent & event);
    void CheckboxRED(wxCommandEvent & event);
    void CheckboxDED(wxCommandEvent & event);
    void CheckboxDHP(wxCommandEvent & event);
    void CheckboxSHP(wxCommandEvent & event);
    void CheckboxLOG(wxCommandEvent & event);
    /*----------------------------------------------------------------*/
    void CheckboxAVIparse(wxCommandEvent & event);
    void CheckboxDEBUG(wxCommandEvent & event);
    void CheckboxDKTparse(wxCommandEvent & event);
    void CheckboxDKTrenameMD4(wxCommandEvent & event);
    void CheckboxGIFexportMD4(wxCommandEvent & event);
    void CheckboxGIFimportMD4(wxCommandEvent & event);
    void CheckboxGIFparse(wxCommandEvent & event);
    void CheckboxHTMparse(wxCommandEvent & event);
    void CheckboxISMparse(wxCommandEvent & event);
    void CheckboxISMdeleteDUP(wxCommandEvent & event);
    void CheckboxISMimportMD4(wxCommandEvent & event);
    void CheckboxJPGexportMD4(wxCommandEvent & event);
    void CheckboxJPGaddRStoFN(wxCommandEvent & event);
    void CheckboxJPGaddFNtoTA(wxCommandEvent & event);
    void CheckboxJPGimportMD4(wxCommandEvent & event);
    void CheckboxJPGcreatePDS(wxCommandEvent & event);
    void CheckboxJPGparse(wxCommandEvent & event);
    void CheckboxMETimportMD4(wxCommandEvent & event);
    void CheckboxMETparse(wxCommandEvent & event);
    void CheckboxMP3parse(wxCommandEvent & event);
    void CheckboxMPGparse(wxCommandEvent & event);
    void CheckboxPRTparse(wxCommandEvent & event);
    void CheckboxSTDparse(wxCommandEvent & event);
    void CheckboxWAVparse(wxCommandEvent & event);
    void CheckboxWD3parse(wxCommandEvent & event);
    void CheckboxWD3deleteALL(wxCommandEvent & event);
    void CheckboxWMFparse(wxCommandEvent & event);
    void CheckboxWMVparse(wxCommandEvent & event);
    void CheckboxVIDexportMD4(wxCommandEvent & event);
    void CheckboxVIDimportMD4(wxCommandEvent & event);
    void CheckboxAHD(wxCommandEvent & event);
    void CheckboxAHF(wxCommandEvent & event);
    void CheckboxAHH(wxCommandEvent & event);
    void CheckboxDPY(wxCommandEvent & event);
    void CheckboxAPR(wxCommandEvent & event);
    void CheckboxDFI(wxCommandEvent & event);
    void CheckboxFCK(wxCommandEvent & event);
    void CheckboxFOO(wxCommandEvent & event);
    void CheckboxGST(wxCommandEvent & event);
    void CheckboxHLK(wxCommandEvent & event);
    void CheckboxMETcreateISM(wxCommandEvent & event);
    void CheckboxPRTcreateISM(wxCommandEvent & event);
    void CheckboxPRTcreateOVN(wxCommandEvent & event);
    void CheckboxPRTsearchMET(wxCommandEvent & event);
    void CheckboxIGX(wxCommandEvent & event);
    void CheckboxWSF(wxCommandEvent & event);
    void CheckboxSTDcreateINF(wxCommandEvent & event);
    void CheckboxSTDcreateMET(wxCommandEvent & event);
    void CheckboxCPJ(wxCommandEvent & event);
    void CheckboxCPH(wxCommandEvent & event);
    void CheckboxMETcreatePRT(wxCommandEvent & event);
    void CheckboxMETrepairPRT(wxCommandEvent & event);
    void CheckboxMD4(wxCommandEvent & event);
    void CheckboxMFL(wxCommandEvent & event);
    void CheckboxJPGwrbackMD4(wxCommandEvent & event);
    void CheckboxGIFwrbackMD4(wxCommandEvent & event);
    void CheckboxAPC(wxCommandEvent & event);
    void CheckboxSFT(wxCommandEvent & event);
    void CheckboxCreateDBentry(wxCommandEvent & event);
    void CheckboxMETcleanuTYP(wxCommandEvent & event);
    void AddHashToSourcename(char * sourcename, bool toadd);
    int AddingDirStart();
    void AddingDir();
    void AddingDirEnd();
    void CreatingFiletree();
    int CheckDatabaseISM(unsigned char *md4hash);
    char * CheckDatabase(char * keybuffer, char * databuffer, bool tostore);
    void ClearDatabaseDUPLI();
    void CreateInfofile(const char * infofilename, const char * filepath, const char * filename);
    void Create_ISMfile(char * infofilename, char * filepath, char * filename);
    void CreatePartfileMET(char * partfilename, char * filepath, char * filename);
    void Create_METfile(const char * infofilename, const char * filepath, const char * filename);
    void Repair_METfile();
    int GetFiletype(char * filename);
    void Process_AVIfile(char * filename);
    void Process_DKTfile(char * filepath, char * filename);
    void Process_DUPfile();
    void Process_GIFfile(char * filename);
    void Process_HTMfile(char * filename);
    void Process_ISMfile();
    void Process_JPGfile(char * filename);
    void Process_METfile(char * filename);
    void Process_PRTfile();
    void Process_STDfile(char * filename);
    void Process_WD3file(char * filename);
    void Process_VIDEOfile(char * filepath, char * filename);
    void Process_PRYfile(char * filename);
    bool running()
    {
        return(threadcount == 1);
        };
    void RenameNF(char * sourcename, bool tofake);
    void HashAndRename(char * sourcename, char * extension, bool tohash);
    void WriteDirfile(char * filename, bool itemhasedpointer);
    void W(const char * text);
    void Wgui(const char * text);
    void Message(int messageid, char * addon);
    void CreateThread();
    void Thread();
    void from32to16(char * source, unsigned char * target);
    int filecountTST(char * filename);
    int filetestASCII(unsigned char * buffer, int bufferlength);
    /****************************/
    public:
    wxTextCtrl * tctrl_ARC;
    wxTextCtrl * tctrl_MATCHDOMAIN;
    wxTextCtrl * tctrl_MATCHSTART;
    wxTextCtrl * tctrl_MATCHURL;
    char * DATABASEp;
    char * TARGETDIR;
    /****************************/
    protected:
    CpicFrame * picfr001;
    wxButton * button_FINDDUPS;
    wxButton * button_HOCFILES;
    wxButton * button_MATCHDOMAINclr;
    wxButton * button_MATCHSTARTclr;
    wxButton * button_MATCHURLclr;
    wxButton * button_DBHANDLING;
    wxButton * button_AVIparse;
    wxButton * button_DKTparse;
    wxButton * button_GIFparse;
    wxButton * button_HTMparse;
    wxButton * button_ISMparse;
    wxButton * button_JPGparse;
    wxButton * button_METparse;
    wxButton * button_MP3parse;
    wxButton * button_MPGparse;
    wxButton * button_PRTparse;
    wxButton * button_STDparse;
    wxButton * button_WAVparse;
    wxButton * button_WD3parse;
    wxButton * button_WMFparse;
    wxButton * button_WMVparse;
    wxButton * button_CHDD;
    wxButton * button_CHSD;
    wxButton * button_CHTD;
    wxButton * button_START;
    wxButton * button_GENTFD;
    wxButton * button_STOP;
    wxButton * button_CLEAR;
    wxButton * button_PARSE;
    wxButton * button_PINFO;
    wxNotebook * smDlgNotebook;
    wxMenu * smPMdktfiles;
    wxMenu * smPMgiffiles;
    wxMenu * smPMhtmfiles;
    wxMenu * smPMismfiles;
    wxMenu * smPMjpgfiles;
    wxMenu * smPMmd4hashs;
    wxMenu * smPMmetfiles;
    wxMenu * smPMprtfiles;
    wxMenu * smPMwd3files;
    wxMenu * smPMrename;
    wxMenu * smPMdbhandling;
    wxStaticText * stext_CHDD;
    wxStaticText * stext_CHSD;
    wxStaticText * stext_CHTD;
    wxStaticText * stext_C1;
    char * buffer;
    unsigned char * buffer0;
    char * keybuffer;
    bool p_creDBentry;
    bool p_crePageForHTM;
    bool p_crePageForJPG;
    bool p_delHtmPxxCxx;
    bool p_simHtmPxxCxx;
    bool p_debugonoff;
    bool p_METcleanuTYP;
    bool p_METcreateISM;
    bool p_METcreatePRT;
    bool p_METrepairPRT;
    bool p_METseparaTYP;
    bool p_PRTcreateISM;
    bool p_PRTsearchMET;
    bool p_STDcreateINF;
    bool p_STDcreateMET;
    bool p_PRTcreateOVN;
    bool p_doMD4hashing;
    bool p_addMD4tofnam;
    bool p_DKTrenameMD4;
    bool p_HTMLaddMD4;
    bool p_AVIparse;
    bool p_DKTparse;
    bool p_GIFparse;
    bool p_HTMparse;
    bool p_ISMparse;
    bool p_JPGparse;
    bool p_METparse;
    bool p_MP3parse;
    bool p_MPGparse;
    bool p_PRTparse;
    bool p_STDparse;
    bool p_WAVparse;
    bool p_WD3parse;
    bool p_WMFparse;
    bool p_WMVparse;
    bool p_delPRYfiles;
    bool p_useHARDLOCK;
    bool p_doMD4fakechk;
    bool p_addPICCtodir;
    bool p_doHDNsetting;
    bool p_moveLocation;
    bool p_useAPRadding;
    bool p_AVIexportMD4;
    bool p_GIFexportMD4;
    bool p_GIFwrbackMD4;
    bool p_GIFimportMD4;
    bool p_ISMdeleteDUP;
    bool p_ISMimportMD4;
    bool p_JPGimportMD4;
    bool p_JPGwrbackMD4;
    bool p_JPGexportMD4;
    bool p_JPGaddRStoFN;
    bool p_JPGaddFNtoTA;
    bool p_JPGcreatePDS;
    bool p_METimportMD4;
    bool p_MPGexportMD4;
    bool p_VIDimportMD4;
    bool p_VIDexportMD4;
    bool p_WD3deleteALL;
    bool p_WMFexportMD4;
    bool p_WMVexportMD4;
    bool p_selectvidfiles;
    bool p_usingdirfile;
    bool p_nosubfolders;
    bool p_createsample;
    bool p_foldersonly;
    bool p_ignorexfiles;
    bool p_markupduples;
    bool p_removedupext;
    bool p_deleteduples;
    bool p_logduples;
    bool p_ED2KhashMD4;
    bool p_useCOKEY;
    wxRadioBox * radiobox_LAN;
    wxCheckBox * checkbox_APC;
    wxCheckBox * checkbox_APR;
    wxCheckBox * checkbox_DFI;
    wxCheckBox * checkbox_GST;
    wxCheckBox * checkbox_FOO;
    wxCheckBox * checkbox_WSF;
    wxCheckBox * checkbox_IGX;
    wxCheckBox * checkbox_MD4;
    wxCheckBox * checkbox_AVIparse;
    wxCheckBox * checkbox_DKTparse;
    wxCheckBox * checkbox_GIFparse;
    wxCheckBox * checkbox_HTMparse;
    wxCheckBox * checkbox_ISMparse;
    wxCheckBox * checkbox_JPGparse;
    wxCheckBox * checkbox_METparse;
    wxCheckBox * checkbox_MP3parse;
    wxCheckBox * checkbox_MPGparse;
    wxCheckBox * checkbox_PRTparse;
    wxCheckBox * checkbox_STDparse;
    wxCheckBox * checkbox_WAVparse;
    wxCheckBox * checkbox_WD3parse;
    wxCheckBox * checkbox_WMFparse;
    wxCheckBox * checkbox_WMVparse;
    /****************************/
    private:
    unsigned int GetUint32(Cavifile * source, unsigned char * buffer);
    unsigned int GetUint32i(Cavifile * source, unsigned char * buffer);
    int mil;
    int lmil;
    int filetype;
    Cmd4hash * hash;
    CMetfile * mfile;
    char * fileextension;
    struct stat * statbuffer;
    long dbfilecount;
    char * databuffer;
    CDatabase * dbPI;
    CDatabase * dbSH;
    CDatabase * dbSM;
    CDatabase * dbTMP;
    CDatabase * dbTX;
    CDatabase * dbUR;
    CDatabase * dbVI;
    DatabaseDatum dbPIkey;
    DatabaseDatum dbPIdata;
    DatabaseDatum dbPIresult;
    DatabaseDatum dbSHkey;
    DatabaseDatum dbSHdata;
    DatabaseDatum dbSHresult;
    DatabaseDatum dbSMkey;
    DatabaseDatum dbSMdata;
    DatabaseDatum dbSMresult;
    DatabaseDatum dbTMPkey;
    DatabaseDatum dbTMPdata;
    DatabaseDatum dbTMPresult;
    DatabaseDatum dbTXkey;
    DatabaseDatum dbTXdata;
    DatabaseDatum dbTXresult;
    DatabaseDatum dbURkey;
    DatabaseDatum dbURdata;
    DatabaseDatum dbURresult;
    DatabaseDatum dbVIkey;
    DatabaseDatum dbVIdata;
    DatabaseDatum dbVIresult;
    long count_duplogged;
    long count_dupmarked;
    long count_dupremoved;
    long count_deleted;
    long count_fakmarked;
    long count_subdirsprocessed;
    long count_subdirsprocessed_last;
    long count_ALLfiles;
    long count_ALLfiles_last;
    long count_AVIfilesprocessed;
    long count_DKTfilesprocessed;
    long count_GIFfilesprocessed;
    long count_HTMfilesprocessed;
    long count_JPGfilesprocessed;
    long count_METfilesprocessed;
    long count_MPGfilesprocessed;
    long count_PRYfilesprocessed;
    long count_STDfilesprocessed;
    long count_WD3filesprocessed;
    long count_WMFfilesprocessed;
    long count_WMVfilesprocessed;
    long count_filesmoved;
    long processcount;
    long filesize;
    long guilinecount;
    char * sourcename;
    char * sourcename_last;
    char * samplesourcename;
    char * samplefilename;
    char * sampledirname;
    int samplerand;
    FILE * fp1;
    FILE * logfileFP;
    wxFile * fpSource;
    wxFile * fpTarget;
    int celdeep;
    int partfilecounter;
    int threadcount;
    time_t timet0;
    char * hostname;
    char * localip;
    char * storageid;
    int sprache;
    int basisfunktion;
    Chtmlpage * htmp;
    Chtmlpage * htmp0;
    int lmd;
    int lms;
    int lmu;
    char * urllast;
    char * urlnext;
    char * domainlast;
    char * domainnext;
    char * exblock[1025];
    char * toblock[1025];
    int exblockAnz;
    int toblockAnz;
    char * hardlock[1025];
    char * hardlockDomain[1025];
    int hardlockAnz;
    int htmpagenumber;
    int htmlinenumber;
    int jpgpagenumber;
    int jpglinenumber;
    FILE * fp_subdirs;
    long shp_fullsize;
    char fTypes[1025];
    int pds_count;
    int pds_filecount;
    long size_PDSfiles;
    unsigned long size_ALLfiles;
    int file_width;
    int file_height;
    int dirbufferDeep;
    char * dirbuffer[99];
    char * filename;
    char * filenameTMP;
    char * filepath;
    char * filepathTMP;
    DIR * subdir[99];
    char * name0;
    char * name0type;
    int name0deep;
    int name0count;
    int name0fcount;
};

#endif

