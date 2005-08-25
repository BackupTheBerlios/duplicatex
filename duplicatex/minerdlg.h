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

class CminerDlg:public wxPanel
{
    /****************************/
    public:
    CminerDlg(wxWindow * parent);
    ~ CminerDlg();
    void lan00German();
    void lan01English();
    void SetHostname(char * name);
    void SetLocalIP(char * ip);
    void SetStorageID(char * id);
    void ButtonCHSD(wxCommandEvent & event);
    void ButtonCHTD(wxCommandEvent & event);
    void ButtonCLEAR(wxCommandEvent & event);
    void ButtonDBHANDLING(wxCommandEvent & event);
    void ExportDatabaseTXT(wxCommandEvent & event);
    void ImportDatabaseTXT(wxCommandEvent & event);
#ifdef WITHGDBM
    void ImportDBMAIN(wxCommandEvent & event);
#endif
    void ButtonProgramminfo(wxCommandEvent & event);
    void ImportJpgFile(wxCommandEvent & event);
    void ButtonMD4HASHS(wxCommandEvent & event);
    void ButtonMETFILES(wxCommandEvent & event);
    void ButtonPARTFILES(wxCommandEvent & event);
    void ButtonRENAME(wxCommandEvent & event);
    void ButtonSTART(wxCommandEvent & event);
    void ButtonSTOP(wxCommandEvent & event);
    /* MAD=markup duples, RED=Remove .dup-Extension, DED=Delete Duples*/
    wxMenu * smPMfinddups;
    void ButtonFINDDUPS(wxCommandEvent & event);
    void RadioboxLAN(wxCommandEvent & event);
    void CheckboxMAD(wxCommandEvent & event);
    void CheckboxRED(wxCommandEvent & event);
    void CheckboxDED(wxCommandEvent & event);
    void CheckboxLOG(wxCommandEvent & event);
    /*----------------------------------------------------------------*/
    void CheckboxAHD(wxCommandEvent & event);
    void CheckboxAHF(wxCommandEvent & event);
    void CheckboxAPR(wxCommandEvent & event);
    void CheckboxCNE(wxCommandEvent & event);
    void CheckboxDEB(wxCommandEvent & event);
    void CheckboxDFI(wxCommandEvent & event);
    void CheckboxFCK(wxCommandEvent & event);
    void CheckboxFOO(wxCommandEvent & event);
    void CheckboxGST(wxCommandEvent & event);
    void CheckboxIFM(wxCommandEvent & event);
    void CheckboxIFP(wxCommandEvent & event);
    void CheckboxIGX(wxCommandEvent & event);
    void CheckboxWSF(wxCommandEvent & event);
    void CheckboxIFO(wxCommandEvent & event);
    void CheckboxCMO(wxCommandEvent & event);
    void CheckboxCPM(wxCommandEvent & event);
    void CheckboxMD4(wxCommandEvent & event);
    void CheckboxMFL(wxCommandEvent & event);
    void CheckboxMFO(wxCommandEvent & event);
    void CheckboxPFO(wxCommandEvent & event);
    void CheckboxSELPIC(wxCommandEvent & event);
    void CheckboxSELVID(wxCommandEvent & event);
    void CheckboxAPC(wxCommandEvent & event);
    void CheckboxSFT(wxCommandEvent & event);
    void CheckboxCreateDBentry(wxCommandEvent & event);
    void AddHashToSourcename(char * sourcename, bool toadd);
    void AddingDir();
    char * CheckDatabase(char * keybuffer, char * databuffer, bool tostore);
    void ClearDatabase();
    void CreateInfofile(const char * infofilename, const char * filepath, const char * filename);
    void CreateInfofileMET(char * infofilename, char * filepath, char * filename);
    void CreatePartfileMET(char * partfilename, char * filepath, char * filename);
    void CreateMetfile(const char * infofilename, const char * filepath, const char * filename);
    int GetFiletype(char * filename);
    void Process_Metfile(char * filepath, char * filename);
    void Process_Normalfile(char * filepath, char * filename);
    bool running()
    {
        return(threadcount == 1);
        };
    void Rename(char * sourcename, bool tofake);
    void WriteDirfile(char * filename, bool itemhasedpointer);
    void W(const char * text);
    void Message(int messageid, char * addon);
    void CreateThread();
    void Thread();
    void from32to16(char * source, unsigned char * target);
    int filecountTST(char * filename);
    /****************************/
    public:
    wxTextCtrl * textctrl_ARC;
    char * DATABASEp;
    /****************************/
    protected:
    CpicFrame * picfr001;
    wxButton * button_FINDDUPS;
    wxButton * button_DBHANDLING;
    wxButton * button_METFILES;
    wxButton * button_CHSD;
    wxButton * button_CHTD;
    wxButton * button_START;
    wxButton * button_STOP;
    wxButton * button_CLEAR;
    wxButton * button_PINFO;
    wxNotebook * smDlgNotebook;
    wxMenu * smPMmd4hashs;
    wxMenu * smPMmetfiles;
    wxMenu * smPMpartfiles;
    wxMenu * smPMrename;
    wxMenu * smPMdbhandling;
    wxStaticText * smSTP3V2H1;
    wxStaticText * smSTP3V2H2;
    char * buffer;
    unsigned char * buffer0;
    char * keybuffer;
    bool p_creDBentry;
    bool p_debugonoff;
    bool p_metfilesonly;
    bool p_partfilesonly;
    bool p_sepMetFtypes;
    bool p_creInfForMet;
    bool p_crePartForMet;
    bool p_creInfForPart;
    bool p_creInfForAll;
    bool p_creMetForAll;
    bool p_cNewED2Kparts;
    bool p_doMD4hashing;
    bool p_addMD4tofnam;
    bool p_doMD4fakechk;
    bool p_addPICCtodir;
    bool p_doHDNsetting;
    bool p_moveLocation;
    bool p_useAPRadding;
    bool p_selectpicfiles;
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
    wxRadioBox * radiobox_LAN;
    wxCheckBox * checkbox_APC;
    wxCheckBox * checkbox_APR;
    wxCheckBox * checkbox_DEB;
    wxCheckBox * checkbox_DFI;
    wxCheckBox * checkbox_GST;
    wxCheckBox * checkbox_FOO;
    wxCheckBox * checkbox_WSF;
    wxCheckBox * checkbox_IGX;
    wxCheckBox * checkbox_MD4;
    wxCheckBox * checkbox_MFO;
    wxCheckBox * checkbox_PFO;
    wxCheckBox * checkbox_SELPIC;
    wxCheckBox * checkbox_SELVID;
    /****************************/
    private:
    int ftype;
    Cmd4hash * hash;
    CMetfile * mfile;
    char fileextension[20];
    struct stat * statbuffer;
    long dbfilecount;
    char * databuffer;
    CDatabase * dbSM;
    DatabaseDatum dbSMmKey;
    DatabaseDatum dbSMmData;
    DatabaseDatum dbSMmResult;
    long count_duplogged;
    long count_dupmarked;
    long count_dupremoved;
    long count_deleted;
    long count_fakmarked;
    long count_subdirsprocessed;
    long count_filesprocessed;
    long processcount;
    long filesize;
    char * sourcename;
    char * samplesourcename;
    char * samplefilename;
    char * sampledirname;
    int samplerand;
    FILE * fp1;
    FILE * logfileFP;
    int celdeep;
    int partfilecounter;
    int threadcount;
    time_t timet0;
    char * hostname;
    char * localip;
    char * storageid;
    int sprache;
};

#endif

