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

#include "duplicatex.h"
#include "minerdlg.h"
#include "wxthread.h"

#define WXOEF	(wxObjectEventFunction)

#define NAMEZMAX	1000
#define BUFFERSIZE	2048

#define WX1

extern int debug;

CwxThread * minerdlgThread;

int name0deep = 0, name0count = 0, name0fcount = 0, dirdeep = 0;
char * name0;
char * name0type;
int name1z = 0;
char * name1[NAMEZMAX];
char * dirbuffer[99];
char tempname[BUFFERSIZE];
int samplefilenamez = 0;
int samplefilecount = 0;

wxString Csprachen[] =
{
    "Deutsch",
    "English"
};

CminerDlg::CminerDlg(wxWindow * parent)
:wxPanel(parent, - 1, wxDefaultPosition, wxDefaultSize, 0, "")
{
    sprache=0;
    hostname = new char[9];
    strcpy(hostname, "hostname");
    localip = new char[16];
    strcpy(localip, "127.0.0.1");
    storageid = new char[16];
    strcpy(storageid, "");
    threadcount = 0;
    DATABASEp = new char [128];
    strcpy(DATABASEp, theApp -> targetdirectory.c_str());
    for (int i = 0 ; i < 100 ; i++)
    {
        dirbuffer[i] = (char *) malloc(512);
    }
    celdeep = 0;
    name0 = new char[64];
    name0[0] = 0;
    name0type = new char[64];
    name0type[0] = 0;
    for (int i = 0 ; i < NAMEZMAX ; i++)
    {
        name1[i] = new char[1];
        name1[i][0] = 0;
    }
    statbuffer = (struct stat *) malloc(sizeof(struct stat) * 2);
    buffer = new char[BUFFERSIZE];
    buffer0 = new unsigned char[BUFFERSIZE];
    keybuffer = new char [BUFFERSIZE];
    databuffer = new char [BUFFERSIZE];
    sourcename = new char[BUFFERSIZE];
    hash = new Cmd4hash();
    dbSMmKey.ptr = (unsigned char *) malloc(16);
    dbSMmKey.size = 16;
    dbSMmData.ptr = NULL;
    dbSMmData.size = 0;
    p_debugonoff = false;
    p_creDBentry = true;
    p_metfilesonly = false;
    p_sepMetFtypes = false;
    p_creInfForMet = true;
    p_creInfForPart = true;
    p_creInfForAll = false;
    p_creMetForAll = false;
    p_cNewED2Kparts = false;
    p_partfilesonly = false;
    p_doMD4hashing = true;
    p_addMD4tofnam = false;
    p_doMD4fakechk = false;
    p_doHDNsetting = false;
    p_moveLocation = false;
    p_addPICCtodir = false;
    p_useAPRadding = false;
    p_usingdirfile = false;
    p_nosubfolders = false;
    p_createsample = false;
    p_foldersonly = false;
    p_ignorexfiles = true;
    p_markupduples = true;
    p_deleteduples = false;
    p_logduples = true;
    p_removedupext = false;
    p_selectpicfiles = false;
    p_selectvidfiles = false;
    smPMfinddups = new wxMenu("Duplikate suchen und markieren");
    smPMfinddups -> AppendCheckItem(20001, ".");
    smPMfinddups -> AppendCheckItem(20002, ".");
    smPMfinddups -> AppendSeparator();
    smPMfinddups -> AppendCheckItem(20003, ".");
    smPMfinddups -> AppendSeparator();
    smPMfinddups -> AppendCheckItem(20004, ".");
    smPMfinddups -> Check(20001, p_markupduples);
    smPMfinddups -> Check(20002, p_removedupext);
    smPMfinddups -> Check(20003, p_deleteduples);
    smPMfinddups -> Check(20004, p_logduples);
    smPMdbhandling = new wxMenu("Datenbank-Verwaltung");
    smPMdbhandling -> AppendSeparator();
    smPMdbhandling -> AppendCheckItem(20501, ".");
    smPMdbhandling -> AppendSeparator();
    smPMdbhandling -> Append(20511, ".");
    smPMdbhandling -> Append(20512, ".");
    smPMdbhandling -> Append(20513, ".");
#ifdef WITHGDBM
    smPMdbhandling -> Append(20514, "Dateiinformationen importieren aus gdbm-Datenbank (*.DBMAIN)");
#endif
    //smPMdbhandling -> Append(20521, ".");
    smPMdbhandling -> Check(20501, p_creDBentry);
    smPMmd4hashs = new wxMenu("MD4-Hashprocessing");
    smPMmd4hashs -> AppendCheckItem(20101, "Add MD4 to Filename");
    smPMmd4hashs -> AppendCheckItem(20102, "With fakecheck(slower)");
    smPMmd4hashs -> AppendSeparator();
    smPMmd4hashs -> AppendCheckItem(20103, "create .ism for *ALL");
    smPMmd4hashs -> AppendCheckItem(20104, "create .met for *ALL");
    smPMmd4hashs -> Check(20101, p_addMD4tofnam);
    smPMmd4hashs -> Check(20102, p_doMD4fakechk);
    smPMmd4hashs -> Check(20103, p_creInfForAll);
    smPMmd4hashs -> Check(20104, p_creMetForAll);
    smPMmetfiles = new wxMenu("Metfile-Processing");
    smPMmetfiles -> AppendCheckItem(20201, "create .ism for .met");
    smPMmetfiles -> AppendCheckItem(20202, "separate filetypes");
    smPMmetfiles -> Check(20201, p_creInfForMet);
    smPMmetfiles -> Check(20202, p_sepMetFtypes);
    smPMpartfiles = new wxMenu("Partfile-Processing");
    smPMpartfiles -> AppendCheckItem(20211, "create .ism for .part");
    smPMpartfiles -> AppendCheckItem(20212, "create new-ed2k-parts");
    smPMmetfiles -> Check(20211, p_creInfForPart);
    smPMmetfiles -> Check(20212, p_cNewED2Kparts);
    smPMrename = new wxMenu("Rename and/or moving files");
    smPMrename -> AppendSeparator();
    smPMrename -> AppendCheckItem(20301, "Set HDir(Homedirectory='./xyz(...)/...' to filename");
    smPMrename -> AppendCheckItem(20302, "Move file to HDir");
    smPMrename -> Check(20301, p_doHDNsetting);
    smPMrename -> Check(20302, p_moveLocation);
    button_START = new wxButton(this, 11351, ".", wxDefaultPosition, wxSize(128, 24));
    button_STOP = new wxButton(this, 11352, ".", wxDefaultPosition, wxSize(128, 24));
    button_CLEAR = new wxButton(this, 11359, ".", wxDefaultPosition, wxSize(160, 24));
    button_PINFO = new wxButton(this, 11371, ".", wxDefaultPosition, wxSize(128, 24));
    radiobox_LAN = new wxRadioBox((wxWindow *) this, 11399, "Sprache/Language", wxDefaultPosition,
    wxDefaultSize, WXSIZEOF(Csprachen), Csprachen, 0);
    textctrl_ARC = new wxTextCtrl(this, - 1, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    textctrl_ARC -> SetEditable(false);
    wxBoxSizer * sizer_root = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer * sizer_h1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * sizer_v1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer * sizer_v2 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer * sizer_v3 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer * sizer_v4 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer * sizer_v4h1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * sizer_v4h2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * sizer_h9 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * sizer_DUP = new wxBoxSizer(wxHORIZONTAL);
    smSTP3V2H1 = new wxStaticText(this, - 1, theApp -> sourcedirectory, wxDefaultPosition, wxSize(512, 24));
    smSTP3V2H2 = new wxStaticText(this, - 1, theApp -> targetdirectory, wxDefaultPosition, wxSize(512, 24));
    sizer_h1 -> Add(sizer_v1, 0, 0, 0);
    sizer_h1 -> Add(sizer_v2, 0, 0, 0);
    sizer_h1 -> Add(sizer_v3, 0, 0, 0);
    sizer_h1 -> Add(sizer_v4, 0, 0, 0);
    sizer_h9 -> Add(button_START, 0, wxALIGN_CENTER_VERTICAL | wxALL, 4);
    sizer_h9 -> Add(button_STOP, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
    sizer_h9 -> Add(button_CLEAR, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
    sizer_h9 -> Add(button_PINFO, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL, 2);
    sizer_root -> Add(sizer_h1, 0, 0, 0);
    sizer_root -> Add(sizer_h9, 0, 0, 0);
    sizer_root -> Add(textctrl_ARC, 1, wxEXPAND | wxALL, 5);
    sizer_root -> Add(radiobox_LAN, 0, wxGROW | wxALIGN_CENTER_HORIZONTAL, 0);
    button_CHSD = new wxButton(this, 11353, ".", wxDefaultPosition, wxSize(200, - 1));
    button_CHTD = new wxButton(this, 11354, ".", wxDefaultPosition, wxSize(200, - 1));
#ifdef WX1
    sizer_v4h1 -> Add(button_CHSD, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
    sizer_v4h1 -> Add(smSTP3V2H1, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
    sizer_v1 -> Add(sizer_DUP, 0, wxALIGN_CENTER_VERTICAL | wxALL, 4);
    sizer_v1 -> Add(sizer_v4h1, 0, wxALIGN_CENTER_VERTICAL | wxALL, 4);
#else
    wxBoxSizer * sizer_MD4 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * sizer_MFO = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * sizer_PFO = new wxBoxSizer(wxHORIZONTAL);
    checkbox_DEB = new wxCheckBox(this, 11301, _T("Debug on/off"), wxDefaultPosition,
    wxDefaultSize, 0, wxDefaultValidator, "deb");
    checkbox_DEB -> SetValue(p_debugonoff);
    button_FINDDUPS = new wxButton(this, 12302, "&Duplikate suchen und markieren",
    sizer_DUP -> Add(button_FINDDUPS, 0, wxALIGN_CENTER_VERTICAL, 0) ;
    checkbox_MD4 = new wxCheckBox(this, 11312, _T(""), wxDefaultPosition,
    wxDefaultSize, 0, wxDefaultValidator, "md4") ;
    checkbox_MD4 -> SetValue(p_doMD4hashing) ;
    wxButton * button_MD4HASHS = new wxButton(this, 12311, "&MD4-Hashing") ;
    sizer_MD4 -> Add(checkbox_MD4, 0, wxALIGN_CENTER_VERTICAL, 0) ;
    sizer_MD4 -> Add(button_MD4HASHS, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2) ;
    wxButton * button_RENAME = new wxButton(this, 12331, "&Renaming and/or moving files") ;
    checkbox_APR = new wxCheckBox(this, 11316, _T("Add PicRes to filename"), wxDefaultPosition,
    wxDefaultSize, 0, wxDefaultValidator, "ahd") ;
    checkbox_APR -> SetValue(p_useAPRadding) ;
    checkbox_APC = new wxCheckBox(this, 11317, _T("Add PicCount to Dir"), wxDefaultPosition,
    wxDefaultSize, 0, wxDefaultValidator, "apc") ;
    checkbox_APC -> SetValue(p_addPICCtodir) ;
    checkbox_MFO = new wxCheckBox(this, 11321, _T(""), wxDefaultPosition,
    wxDefaultSize, 0, wxDefaultValidator, "mfo") ;
    checkbox_MFO -> SetValue(p_metfilesonly) ;
    wxButton * button_METFILES = new wxButton(this, 12321, "&Metfiles only") ;
    sizer_MFO -> Add(checkbox_MFO, 0, wxALIGN_CENTER_VERTICAL, 0) ;
    sizer_MFO -> Add(button_METFILES, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2) ;
    checkbox_PFO = new wxCheckBox(this, 11322, _T(""), wxDefaultPosition,
    wxDefaultSize, 0, wxDefaultValidator, "pfo") ;
    checkbox_PFO -> SetValue(p_partfilesonly) ;
    wxButton * button_PARTFILES = new wxButton(this, 12322, "&Partfiles only") ;
    sizer_PFO -> Add(checkbox_PFO, 0, wxALIGN_CENTER_VERTICAL, 0) ;
    sizer_PFO -> Add(button_PARTFILES, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2) ;
    checkbox_SELPIC = new wxCheckBox(this, 11324, _T("Select picturefiles"), wxDefaultPosition,
    wxDefaultSize, 0, wxDefaultValidator, "pic") ;
    checkbox_SELPIC -> SetValue(p_selectpicfiles) ;
    checkbox_SELVID = new wxCheckBox(this, 11325, _T("Select videofiles"), wxDefaultPosition,
    wxDefaultSize, 0, wxDefaultValidator, "vid") ;
    checkbox_SELVID -> SetValue(p_selectvidfiles) ;
    checkbox_DFI = new wxCheckBox(this, 11332, _T("Generate Dir(/-File)tree"), wxDefaultPosition,
    wxDefaultSize, 0, wxDefaultValidator, "dfi") ;
    checkbox_DFI -> SetValue(p_usingdirfile) ;
    checkbox_GST = new wxCheckBox(this, 11333, _T("Generate sampletree"), wxDefaultPosition,
    wxDefaultSize, 0, wxDefaultValidator, "gsd") ;
    checkbox_GST -> SetValue(p_createsample) ;
    checkbox_FOO = new wxCheckBox(this, 11334, _T("...(with folders only)"), wxDefaultPosition,
    wxDefaultSize, 0, wxDefaultValidator, "foo") ;
    checkbox_FOO -> SetValue(p_foldersonly) ;
    checkbox_WSF = new wxCheckBox(this, 11335, _T("...(without subfolders)"), wxDefaultPosition,
    wxDefaultSize, 0, wxDefaultValidator, "wsf") ;
    checkbox_WSF -> SetValue(p_nosubfolders) ;
    checkbox_IGX = new wxCheckBox(this, 11336, _T("Don`t prefer *.x.*-files"), wxDefaultPosition,
    wxDefaultSize, 0, wxDefaultValidator, "igx") ;
    checkbox_IGX -> SetValue(p_createsample) ;
#endif
    button_DBHANDLING = new wxButton(this, 11355, ".", wxDefaultPosition, wxSize(200, - 1)) ;
#ifndef WX1
    sizer_v1 -> Add(checkbox_DEB, 0, wxALIGN_CENTER_VERTICAL | wxALL, 4) ;
    sizer_v1 -> Add(button_DBHANDLING, 0, wxALIGN_CENTER_VERTICAL | wxALL, 4) ;
    sizer_v1 -> Add(sizer_DUP, 0, wxALIGN_CENTER_VERTICAL | wxALL, 4) ;
    sizer_v2 -> Add(sizer_MD4, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2) ;
    sizer_v2 -> Add(button_RENAME, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2) ;
    sizer_v2 -> Add(checkbox_APR, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2) ;
    sizer_v2 -> Add(checkbox_APC, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2) ;
    sizer_v3 -> Add(sizer_MFO, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2) ;
    sizer_v3 -> Add(sizer_PFO, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2) ;
    sizer_v3 -> Add(checkbox_SELPIC, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2) ;
    sizer_v3 -> Add(checkbox_SELVID, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2) ;
    sizer_v4h1 -> Add(button_CHSD, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2) ;
    sizer_v4h1 -> Add(smSTP3V2H1, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2) ;
    sizer_v4h2 -> Add(button_CHTD, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2) ;
    sizer_v4h2 -> Add(smSTP3V2H2, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2) ;
    sizer_v4 -> Add(sizer_v4h1, 0, 0, 0) ;
    sizer_v4 -> Add(sizer_v4h2, 0, 0, 0) ;
    sizer_v4 -> Add(checkbox_DFI, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2) ;
    sizer_v4 -> Add(checkbox_GST, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2) ;
    sizer_v4 -> Add(checkbox_FOO, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2) ;
    sizer_v4 -> Add(checkbox_WSF, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2) ;
    sizer_v4 -> Add(checkbox_IGX, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2) ;
#else
    sizer_v1 -> Add(button_DBHANDLING, 0, wxALIGN_CENTER_VERTICAL | wxALL, 4) ;
    sizer_v4h2 -> Add(button_CHTD, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0) ;
    sizer_v4h2 -> Add(smSTP3V2H2, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2) ;
    sizer_v1 -> Add(sizer_v4h2, 0, wxALIGN_CENTER_VERTICAL | wxALL, 4) ;
    button_FINDDUPS = new wxButton(this, 12302, "&Duplikate suchen und markieren",
    wxDefaultPosition, wxSize(200, - 1)) ;
    sizer_DUP -> Add(button_FINDDUPS, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 0) ;
#endif
    lan00German() ;
    this -> SetSizer(sizer_root) ;
    Connect(11353, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonCHSD) ;
    Connect(11354, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonCHTD) ;
    Connect(11351, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonSTART) ;
    Connect(11352, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonSTOP) ;
    Connect(11359, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonCLEAR) ;
    Connect(11355, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonDBHANDLING) ;
    Connect(12302, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonFINDDUPS) ;
    Connect(20001, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxMAD) ;
    Connect(20002, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxRED) ;
    Connect(20003, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxDED) ;
    Connect(20004, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxLOG) ;
#ifndef WX1
    Connect(11301, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxDEB) ;
    Connect(11312, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxMD4) ;
    Connect(12311, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonMD4HASHS) ;
    Connect(11316, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxAPR) ;
    Connect(11317, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxAPC) ;
    Connect(11321, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxMFO) ;
    Connect(11322, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxPFO) ;
    Connect(12321, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonMETFILES) ;
    Connect(12322, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonPARTFILES) ;
    Connect(11324, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxSELPIC) ;
    Connect(11325, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxSELVID) ;
    Connect(12331, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonRENAME) ;
    Connect(11332, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxDFI) ;
    Connect(11333, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxGST) ;
    Connect(11334, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxFOO) ;
    Connect(11335, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxWSF) ;
    Connect(11336, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxIGX) ;
    Connect(20101, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxAHF) ;
    Connect(20102, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxFCK) ;
    Connect(20103, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxIFO) ;
    Connect(20104, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxCMO) ;
    Connect(20201, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxIFM) ;
    Connect(20202, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxSFT) ;
    Connect(20211, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxIFP) ;
    Connect(20212, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxCNE) ;
    Connect(20301, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxAHD) ;
    Connect(20302, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxMFL) ;
#endif
    Connect(11371, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonProgramminfo) ;
    Connect(11399, wxEVT_COMMAND_RADIOBOX_SELECTED, WXOEF & CminerDlg::RadioboxLAN) ;
    Connect(20501, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxCreateDBentry) ;
    Connect(20511, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::ClearDatabase) ;
    Connect(20512, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::ImportDatabaseTXT) ;
    Connect(20513, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::ExportDatabaseTXT) ;
#ifdef WITHGDBM
    Connect(20514, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::ImportDBMAIN) ;
#endif
    //Connect(20521, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::ImportJpgFile);
}

CminerDlg::~ CminerDlg()
{
    delete[] name0;
    delete[] name0type;
    for (int i = 0 ; i < NAMEZMAX ; i++)
    {
        delete[] name1[i];
    }
    free(statbuffer);
    delete[] keybuffer;
    delete[] databuffer;
    delete[] buffer;
    delete[] buffer0;
    delete[] sourcename;
    delete[] hostname;
    delete[] localip;
    delete[] storageid;
    delete hash;
    if (dbSMmData.ptr)
    {
        free(dbSMmData.ptr);
    }
    free(dbSMmKey.ptr);
    for (int i = 0 ; i < 100 ; i++)
    {
        free(dirbuffer[i]);
    }
    delete[] DATABASEp;
}

void CminerDlg::lan00German()
{
    button_FINDDUPS -> SetLabel("&Duplikate suchen und markieren");
    button_CHSD -> SetLabel("Zu analysierendes Verzeichnis");
    button_CHTD -> SetLabel("Datenbank-Verzeichnis");
    button_DBHANDLING -> SetLabel("Datenbank-Verwaltung");
    button_START -> SetLabel("Analyse starten");
    button_STOP -> SetLabel("Analyse abbrechen");
    button_CLEAR -> SetLabel("Ausgabefenster loeschen");
    button_PINFO -> SetLabel("Programminfo");
    smPMfinddups -> SetTitle(_T("Duplikate suchen und markieren"));
    smPMfinddups -> SetLabel(20001, "Duplikate markieren (.dup beim Duplikat-Dateinamen anhaengen)");
    smPMfinddups -> SetLabel(20002, "<.dup>-Markierungen wieder entfernen");
    smPMfinddups -> SetLabel(20003, "Duplikate loeschen (VORSICHT!!!, besser ^markieren^ verwenden!)");
    smPMfinddups -> SetLabel(20004, "Gefundene Duplikate in Logfile eintragen <logfile.txt>");
    smPMdbhandling -> SetTitle("Datenbank-Verwaltung");
    smPMdbhandling -> SetLabel(20501, "Neue Dateiinformationen in der Datenbank ablegen (fuer nachfolgende Analysen)");
    smPMdbhandling -> SetLabel(20511, "Datenbankinhalt loeschen (zurücksetzen)");
    smPMdbhandling -> SetLabel(20512, "Dateiinformationen importieren aus 'database.txt'");
    smPMdbhandling -> SetLabel(20513, "Dateiinformationen exportieren nach 'database.txt'");
    //smPMdbhandling ->SetLabel(20521, "JPG-File in Datenbank einbetten, Filename wird zum Key");
}

void CminerDlg::lan01English()
{
    button_FINDDUPS -> SetLabel("&Find and markup duplicates");
    button_CHSD -> SetLabel("Directory for analyzing");
    button_CHTD -> SetLabel("Database-Directory");
    button_DBHANDLING -> SetLabel("Database-Management");
    button_START -> SetLabel("START");
    button_STOP -> SetLabel("Cancel");
    button_CLEAR -> SetLabel("Clear Output-Window");
    button_PINFO -> SetLabel("Info");
    smPMfinddups -> SetTitle(_T("Find and markup duplicates"));
    smPMfinddups -> SetLabel(20001, "Markup duplicates (appending <.dup>-extension to filename (renaming)");
    smPMfinddups -> SetLabel(20002, "Remove <.dup>-extension from filenames (renaming)");
    smPMfinddups -> SetLabel(20003, "Delete duplicates (CAUTION!!) (better to use <markup duplicates>)");
    smPMfinddups -> SetLabel(20004, "Logging of duplicates to <logfile.txt>");
    smPMdbhandling -> SetTitle("Database-Management");
    smPMdbhandling -> SetLabel(20501, "Store fileinfos (for further analyzing)");
    smPMdbhandling -> SetLabel(20511, "Clear database");
    smPMdbhandling -> SetLabel(20512, "Import fileinfos to database from 'database.txt'");
    smPMdbhandling -> SetLabel(20513, "Export fileinfos from database to 'database.txt'");
    //smPMdbhandling ->SetLabel(20521, "Store jpg-file, key=filename");
}

void CminerDlg::RadioboxLAN(wxCommandEvent & event)
{
    sprache = event.GetSelection();
    switch (sprache)
    {
    case 0:
        lan00German();
        break;
    case 1:
        lan01English();
        break;
    }
}

void CminerDlg::SetHostname(char * Hostname)
{
    if (hostname)
    {
        delete [] hostname;
    }
    hostname = new char[strlen(Hostname) + 1];
    strcpy(hostname, Hostname);
}

void CminerDlg::SetLocalIP(char * ip)
{
    if (localip)
    {
        delete [] localip;
    }
    localip = new char[strlen(ip) + 1];
    strcpy(localip, ip);
}

void CminerDlg::SetStorageID(char * id)
{
    if (storageid)
    {
        delete [] storageid;
    }
    storageid = new char[strlen(id) + 1];
    strcpy(storageid, id);
}

void CminerDlg::ButtonCHSD(wxCommandEvent & event)
{
    if (!threadcount)
    {
        wxDirDialog * fdSD = new wxDirDialog(this, "Bitte ein Quellverzeichnis auswaehlen fuer die rekursiven Analysen: ", theApp -> sourcedirectory);
        if (fdSD -> ShowModal() == wxID_OK)
        {
            theApp -> sourcedirectory = fdSD -> GetPath();
            smSTP3V2H1 -> SetLabel(theApp -> sourcedirectory);
        }
        delete fdSD;
    }
    else
    {
        Message(101, "");
    }
}

void CminerDlg::ButtonCHTD(wxCommandEvent & event)
{
    if (!threadcount)
    {
        wxDirDialog * fdTD = new wxDirDialog(this, "Bitte ein Zielverzeichnis zur Aufnahme der Datenbank auswaehlen: ", theApp -> targetdirectory);
        if (fdTD -> ShowModal() == wxID_OK)
        {
            theApp -> targetdirectory = fdTD -> GetPath();
            strcpy(DATABASEp, theApp -> targetdirectory.c_str());
            smSTP3V2H2 -> SetLabel(theApp -> targetdirectory);
        }
        delete fdTD;
    }
    else
    {
        Message(101, "");
    }
}

void CminerDlg::Message(int mid, char * buffer)
{
    switch (sprache)
    {
    case 0:
        switch (mid)
        {
        case 1:
            W(_T("Datenbankinhalt geloescht (*IDXi/*IDXd): "));
            break;
        case 91:
            W(_T("... Verarbeitung laeuft, bitte warten ... "));
            break;
        case 92:
            W("beendet.\n");
            break;
        case 93:
            textctrl_ARC -> AppendText("Analyse vorzeitig abgebrochen!\n");
            break;
        case 94:
            textctrl_ARC -> AppendText("... Analyse nicht aktiv, deshalb gibts auch nix zu stoppen!\n");
            break;
        case 95:
            textctrl_ARC -> AppendText("Analyse gestartet.\n");
            break;
        case 99:
            W(_T("Fehler beim Öffnen der Datenbank\n"));
            break;
        case 101:
            W("... Funktion gesperrt, solange Analyse laueft ...\n");
            break;
        }
        break;
    case 1:
        switch (mid)
        {
        case 1:
            W(_T("Database cleared (*IDXi/*IDXd): "));
            break;
        case 91:
            W(_T("... working, please wait ..."));
            break;
        case 92:
            W("finished:\n");
            break;
        case 93:
            W("... analyzing canceled.\n");
            break;
        case 94:
            W("... analyzing not active.\n");
            break;
        case 95:
            textctrl_ARC -> AppendText("Thread started.\n");
            break;
        case 99:
            W(_T("error on database-open\n"));
            break;
        case 101:
            W("... Function blocked while analyzing ...\n");
            break;
        }
        break;
    }
    if (strlen(buffer))
    {
        W(_T(buffer));
        W("\n");
    }
}

void CminerDlg::ClearDatabase()
{
    if (!threadcount)
    {
        FILE * db;
        sprintf(buffer, "%s/dupli.DBIDXi", DATABASEp);
        db = fopen(buffer, "w");
        if (db)
        {
            fclose(db);
        }
        sprintf(buffer, "%s/dupli.DBIDXd", DATABASEp);
        db = fopen(buffer, "w");
        if (db)
        {
            fclose(db);
        }
        buffer[strlen(buffer) - 1] = 0;
        Message(1, buffer);
    }
    else
    {
        Message(101, "");
    }
}

void CminerDlg::ExportDatabaseTXT(wxCommandEvent & event)
{
    if (!threadcount)
    {
        DatabaseDatum tempData;
        char smdbfile[256];
        char smdbpath[256];
        tempData.ptr = NULL;
        sprintf(smdbpath, "%s", DATABASEp);
        sprintf(smdbfile, "%s", "dupli.DBIDXi");
        wxFileDialog * fdSMDB = new wxFileDialog(this, "Bitte eine DBIDXi-Datei auswaehlen zum Exportieren:",
        smdbpath, smdbfile, "*.DBIDXi");
        if (fdSMDB -> ShowModal() == wxID_OK)
        {
            sprintf(smdbfile, "%s", fdSMDB -> GetFilename() .c_str());
            sprintf(smdbpath, "%s", fdSMDB -> GetDirectory() .c_str());
            int counter = 0, l = strlen(smdbfile);
            if (l)
            {
                if (smdbfile[l - 1] == 'i')
                {
                    smdbfile[l - 1] = 0;
                }
                dbSM = new CDatabase(1, 16, smdbpath, smdbfile);
                if (dbSM)
                {
                    memset(dbSMmKey.ptr, 0, 16);
                    sprintf(buffer, "%s/database.txt", DATABASEp);
                    wxFile * expf = new wxFile(buffer, wxFile::write);
                    do
                    {
                        if (tempData.ptr)
                        {
                            free(tempData.ptr);
                            tempData.size = 0;
                        }
                        tempData = dbSM -> getNextKey(1, dbSMmKey);
                        if (tempData.ptr)
                        {
                            counter++;
                            int c, i = 0;
                            buffer[0] = 34;
                            for (int i = 0 ; i < 16 ; i++)
                            {
                                sprintf(buffer + 1 + i * 2, "%02x", dbSMmKey.ptr[i]);
                            }
                            i = 33;
                            buffer[i++] = 34;
                            buffer[i++] = ';';
                            buffer[i++] = 34;
                            for (int j = 0 ; j < tempData.size ; j++)
                            {
                                c = tempData.ptr[j];
                                switch (j)
                                {
                                case 12:
                                    buffer[i++] = 34;
                                    buffer[i++] = ';';
                                    buffer[i++] = 34;
                                    break;
                                default:
                                    switch (c)
                                    {
                                    case 0:
                                    case 10:
                                    case 13:
                                        break;
                                    case 34:
                                        buffer[i++] = '_';
                                        break;
                                    default:
                                        buffer[i++] = c;
                                        break;
                                    }
                                    break;
                                }
                            }
                            buffer[i++] = 34;
                            buffer[i++] = ';';
                            buffer[i++] = 13;
                            buffer[i++] = 10;
                            expf -> Write(buffer, i);
                        }
                    }
                    while (tempData.ptr);
                    delete expf;
                    delete dbSM;
                    switch (sprache)
                    {
                    case 0:
                        sprintf(buffer, "%d Datensaetze exportiert nach 'database.txt'\n", counter);
                        break;
                    case 1:
                        sprintf(buffer, "%d entries exported to 'database.txt'\n", counter);
                        break;
                    }
                    Message(0, buffer);
                }
            }
        }
        delete fdSMDB;
    }
    else
    {
        Message(101, "");
    }
}

void CminerDlg::ImportDatabaseTXT(wxCommandEvent & event)
{
    if (!threadcount)
    {
        CDatabase * dbSMtmp;
        char datafile[256];
        char datapath[256];
        sprintf(datapath, "%s", DATABASEp);
        sprintf(datafile, "%s", "database.txt");
        wxFileDialog * fdDATA = new wxFileDialog(this, "Bitte eine <database.txt>-kompatible Datei auswaehlen:",
        datapath, datafile, "*.txt");
        if (fdDATA -> ShowModal() == wxID_OK)
        {
            sprintf(datafile, "%s", fdDATA -> GetFilename() .c_str());
            sprintf(datapath, "%s", fdDATA -> GetDirectory() .c_str());
            sprintf(buffer, "%s", fdDATA -> GetPath() .c_str());
            FILE * databaseFP;
            databaseFP = fopen(buffer, "r");
            if (!databaseFP)
            {
                Message(99, "");
            }
            else
            {
                dbSM = new CDatabase(1, 16, DATABASEp, "dupli.DBIDX");
                dbSMtmp = new CDatabase(1, 16, DATABASEp, "tmp.DBIDX");
                if (dbSMtmp && dbSM)
                {
                    Message(91, "");
                    int c, p, counter = 0;
                    unsigned char byte[1];
                    char tempkey[33];
                    DatabaseDatum tempKey;
                    DatabaseDatum tempData;
                    tempKey.ptr = (unsigned char *) malloc(16);
                    tempKey.size = 16;
                    tempData.ptr = NULL;
                    tempData.size = 0;
                    int mode = 0, startpos = 0, endpos = 0, field = 0;
                    while (!feof(databaseFP))
                    {
                        p = 0;
                        do
                        {
                            c = fread(byte, 1, 1, databaseFP);
                            switch (byte[0])
                            {
                            case 10:
                            case 13:
                                if (p)
                                {
                                    c = - 1;
                                }
                                break;
                            default:
                                if (p < BUFFERSIZE)
                                {
                                    buffer[p++] = byte[0];
                                }
                                break;
                            }
                        }
                        while ((c >= 0) && (!feof(databaseFP)));
                        buffer[p] = 0;
                        if (p)
                        {
                            mode = 0;
                            field = 0;
                            for (int i = 0 ; i < p ; i++)
                            {
                                switch (buffer[i])
                                {
                                case 34:
                                    mode = 1 - mode;
                                    if (!mode)
                                    {
                                        endpos = i;
                                        if (endpos > startpos)
                                        {
                                            switch (field)
                                            {
                                            case 0:
                                                if (endpos > (startpos + 32))
                                                {
                                                    endpos = startpos + 32;
                                                }
                                                memcpy(tempkey, buffer + startpos, endpos - startpos);
                                                tempkey[32] = 0;
                                                from32to16(tempkey, tempKey.ptr);
                                                memcpy(dbSMmKey.ptr, tempKey.ptr + 8, 8);
                                                memcpy(dbSMmKey.ptr + 8, tempKey.ptr, 8);
                                                break;
                                            case 1:
                                                memset(databuffer, 0, BUFFERSIZE);
                                                if (endpos > (startpos + 12))
                                                {
                                                    endpos = startpos + 12;
                                                }
                                                memcpy(databuffer, buffer + startpos, endpos - startpos);
                                                while (strlen(databuffer) < 12)
                                                {
                                                    databuffer[strlen(databuffer) ] = 32;
                                                }
                                                databuffer[strlen(databuffer) ] = ':';
                                                break;
                                            case 2:
                                                while (strlen(databuffer) < 13)
                                                {
                                                    databuffer[strlen(databuffer) ] = ':';
                                                }
                                                memcpy(databuffer + 13, buffer + startpos, endpos - startpos);
                                                break;
                                            }
                                        }
                                    }
                                    else
                                    {
                                        startpos = i + 1;
                                    }
                                    break;
                                case ';':
                                    if (!mode)
                                    {
                                        field++;
                                    }
                                    break;
                                default:
                                    break;
                                }
                            }
                            dbSMmData = dbSMtmp -> fetchKey(1, dbSMmKey);
                            if (dbSMmData.ptr)
                            {
                    //(new CpicFrame((wxFrame *)((smApp *) wxTheApp) -> smuledlg,
                    //(char *) dbSMmData.ptr, dbSMmData.size, 200, 540, (char *) tempKey.ptr)) -> Show();
                                free(dbSMmData.ptr);
                                dbSMmData.ptr = NULL;
                                dbSMmData.size = 0;
                            }
                            else
                            {
                                dbSMmData.ptr = (unsigned char *) databuffer;
                                dbSMmData.size = strlen(databuffer) + 1;
                                if (!dbSMtmp -> storeData(1, dbSMmKey, dbSMmData))
                                {
                                    counter++;
                                    if ((counter% 1000) == 1)
                                    {
                                        textctrl_ARC -> AppendText(_T("."));
                                    }
                                }
                            }
                            dbSMmData.ptr = NULL;
                            dbSMmData.size = 0;
                        }
                    }
                    fclose(databaseFP);
                    memset(tempKey.ptr, 0, 16);
                    do
                    {
                        if (tempData.ptr)
                        {
                            free(tempData.ptr);
                            tempData.size = 0;
                        }
                        tempData = dbSMtmp -> getNextKey(1, tempKey);
                        if (tempData.ptr)
                        {
                            counter++;
                            if ((counter% 1000) == 1)
                            {
                                textctrl_ARC -> AppendText(_T("."));
                            }
                            memcpy(dbSMmKey.ptr, tempKey.ptr + 8, 8);
                            memcpy(dbSMmKey.ptr + 8, tempKey.ptr, 8);
                            (void) dbSM -> storeData(1, dbSMmKey, tempData);
                        }
                    }
                    while (tempData.ptr);
                    counter >>= 1;
                    sprintf(buffer, "%d Saetze importiert aus %s/%s\n", counter, datapath, datafile);
                    Message(92, buffer);
                    free(tempKey.ptr);
                    delete dbSMtmp;
                    dbSMtmp = NULL;
                    delete dbSM;
                    dbSM = NULL;
                    sprintf(buffer, "%s/%s", DATABASEp, "tmp.DBIDXi");
                    (void) unlink(buffer);
                    sprintf(buffer, "%s/%s", DATABASEp, "tmp.DBIDXd");
                    (void) unlink(buffer);
                }
            }
        }
        delete fdDATA;
    }
    else
    {
        Message(101, "");
    }
}

void CminerDlg::ImportJpgFile(wxCommandEvent & event)
{
    if (!threadcount)
    {
        CDatabase * dbSMtmp;
        char datafile[256];
        char datapath[256];
        sprintf(datapath, "%s", DATABASEp);
        sprintf(datafile, "%s", "picfile.jpg");
        wxFileDialog * fdDATA = new wxFileDialog(this, "Bitte eine Datei auswaehlen zum Import:",
        datapath, datafile, "*.*");
        if (fdDATA -> ShowModal() == wxID_OK)
        {
            dbSMtmp = new CDatabase(1, 16, DATABASEp, "/files.DBIDX");
            if (dbSMtmp)
            {
                strcpy(datapath, fdDATA -> GetDirectory() .GetData());
                strcpy(datafile, fdDATA -> GetFilename() .GetData());
                DatabaseDatum tempKey;
                sprintf(buffer, "Import laueft: %s ", datafile);
                textctrl_ARC -> AppendText(buffer);
                tempKey.ptr = (unsigned char *) malloc(17);
                tempKey.size = strlen(datafile) + 1;
                if (tempKey.size > 16)
                {
                    tempKey.size = 16;
                    memcpy(tempKey.ptr, datafile + strlen(datafile) - tempKey.size, tempKey.size + 1);
                }
                else
                {
                    memcpy(tempKey.ptr, datafile, tempKey.size + 1);
                }
                if (!dbSMtmp -> storeFile(1, tempKey, fdDATA -> GetPath() .GetData(), false))
                {
                    Message(92, "");
                }
                else
                {
                    textctrl_ARC -> AppendText(_T("  Datei existiert schon in der Datenbank.\n"));
                }
                dbSMmData = dbSMtmp -> fetchKey(1, tempKey);
                if (dbSMmData.ptr)
                {
                    free(dbSMmData.ptr);
                    dbSMmData.ptr = NULL;
                    dbSMmData.size = 0;
                }
                free(tempKey.ptr);
            }
            delete dbSMtmp;
        }
        delete fdDATA;
    }
    else
    {
        Message(101, "");
    }
}

#ifdef WITHGDBM
void CminerDlg::ImportDBMAIN(wxCommandEvent & event)
{
    if (!threadcount)
    {
        char gdbmfile[256];
        char gdbmpath[256];
        sprintf(gdbmpath, "%s", DATABASEp);
        sprintf(gdbmfile, "%s", "dupli.DBMAIN");
        wxFileDialog * fdGDBM = new wxFileDialog(this, "Bitte eine gdbm&*DBMAIN-kompatible Datei auswaehlen:", gdbmpath, gdbmfile, "*.DBMAIN");
        if (fdGDBM -> ShowModal() == wxID_OK)
        {
            sprintf(buffer, "%s", fdGDBM -> GetPath() .GetData());
            GDBM_FILE dbMainFP;
            dbMainFP = gdbm_open(buffer, 0, GDBM_READER | GDBM_NOLOCK, 0, NULL);
            if (!dbMainFP)
            {
                textctrl_ARC -> AppendText(_T("error on gdbm_open\n"));
            }
            else
            {
                dbSM = new CDatabase(1, 16, DATABASEp, "dupli.DBIDX");
                if (dbSM)
                {
                    int counter = 0;
                    datum gdbmKey, gdbmData, gdbmNextkey;
                    gdbmKey = gdbm_firstkey(dbMainFP);
                    Message(91, "");
                    while (gdbmKey.dptr)
                    {
                        counter++;
                        gdbmData = gdbm_fetch(dbMainFP, gdbmKey);
                        if (!gdbmData.dptr)
                        {
                            textctrl_ARC -> AppendText(_T("error after gdbm_fetch\n"));
                        }
                        else
                        {
                            from32to16(gdbmKey.dptr, dbSMmKey.ptr);
                            dbSMmData = dbSM -> fetchKey(1, dbSMmKey);
                            if ((counter% 1000) == 1)
                            {
                                textctrl_ARC -> AppendText(_T("."));
                            }
                            if (dbSMmData.ptr)
                            {
                                free(dbSMmData.ptr);
                            }
                            else
                            {
                                dbSMmData.ptr = (unsigned char *) gdbmData.dptr;
                                dbSMmData.size = gdbmData.dsize;
                                if (!dbSM -> storeData(1, dbSMmKey, dbSMmData))
                                {
                                    //ok
                                }
                            }
                            free(gdbmData.dptr);
                            dbSMmData.ptr = NULL;
                            dbSMmData.size = 0;
                        }
                        gdbmNextkey = gdbm_nextkey(dbMainFP, gdbmKey);
                        free(gdbmKey.dptr);
                        gdbmKey = gdbmNextkey;
                    }
                    sprintf(buffer, "%d Saetze importiert aus %s%s\n", counter, gdbmpath, gdbmfile);
                    Message(92, buffer);
                    delete dbSM;
                    dbSM = NULL;
                }
                gdbm_close(dbMainFP);
            }
        }
        delete fdGDBM;
    }
    else
    {
        Message(101, "");
    }
}

#endif
void CminerDlg::ButtonProgramminfo(wxCommandEvent & event)
{
    textctrl_ARC -> Clear();
    switch (sprache)
    {
    case 0:
        W("Duplicatex v1.0 (Duplikatesucher v1.0)");
		W("................(c) 2005 Frank Schaefer\n");
        W(".........................sf@mulinux.org\n");
        W("\n");
        W("Der Zweck des Programmes besteht darin, Duplikate von Dateien und Programmen aufzuspueren und zu");
        W(" markieren (an die Dateinamen der Duplikate wird die Endung <.dup> angehaengt),");
        W(" welche sowohl unterschiedlich in Dateinamen und/oder Ort im Dateisystem sein koennen.");
        W(" Ebenfalls spielen falsche Zeitangaben bei identischen Dateien keine Rolle.\n");
        W("\n");
        W("Die Funktionsweise des Programms:\n");
        W("Das Programm liest und analysiert rekursiv alle Verzeichnisse und Dateien des unter 'zu");
        W(" analysierendes Verzeichnis' ausgewaehlten Verzeichnisses und speichert eindeutige Kriterien");
        W(" zu jeder neu gefundenen Datei in der programmeigenen Datenbank im 'Datenbank-Verzeichnis'.");
        W(" Wo immer eine Datei zum ERSTEN Mal gefunden wird (unabhaengig von Verzeichnis, Dateiname und");
        W(" Zeitangaben), wird sie als Original in der Datenbank vermerkt. Jede weitere Kopie dieser Datei");
        W(" (ebenfalls unabhaengig von Verzeichnis, Dateiname und Zeitangaben), wird als Duplikat");
        W(" behandelt und protokolliert und/oder mit der Endung '.dup' versehen.");
        W(" Geloescht werden koennen die Duplikate dann ueber den Windowsexplorer oder aehnliche");
        W(" Funktionen/Tools, indem man nach Dateien mit der Extension '.dup' sucht.\n");
        W("\n");
        W("Ein kleines Anwendungsbeispiel:\n");
        W("===========================\n");
        W("Im Laufe der Zeit fertigte man unregelmaessig diverse Sicherungen von Verzeichnissen und Dateien");
        W(" an, deren Lage im Dateisystem sich eventuell teilweise mehrfach aenderte.");
        W(" Nun sucht man aber nur die Dateien, bei denen sich irgendwann einmal der Inhalt aenderte, man");
        W(" hat aber zum allergroessten Teil Dateien in den Sicherungen, die sich schon sehr lange nicht");
        W(" mehr oder noch niemals seit Anlage geaendert haben.");
        W(" Um dieses Problem zu loesen, kopiert man nun alle relevanten Sicherungen zurueck (bzw. die");
        W(" Teile, die die gesuchten Dateien auf alle Faelle beinhalten.\n");
        W("...\n");
        W("Empfehlenswert ist dabei folgende Verzeichnisstruktur:\n");
        W("...\n");
        W("d:\\KopieDesAktuellenVerzeichnisses (<<nicht unbedingt notwendig, aber sicher ist sicher)\n");
        W("d:\\AlleSicherungen\\Sicherung1\n");
        W("d:\\AlleSicherungen\\Sicherung2\n");
        W("...\n");
        W("d:\\AlleSicherungen\\SicherungX\n");
        W("\n");
        W("Nun Button 'Datenbank-Verwaltung' anklicken und 'Datenbankinhalt loeschen' waehlen. Danach");
        W(" als 'zu analysierendes Verzeichnis' das aktuelle/Original- Verzeichnis waehlen");
        W(" (d:\\KopieDesAktuellenVerzeichnisses) und 'Analyse starten'.\n");
        W("Dieser Lauf bewirkt, dass alle Dateien aus diesem Originalverzeichnis analysiert werden und");
        W(" das Programm sich eindeutige Identifikationskriterien in der Datenbank abspeichert. Sollte");
        W(" es innerhalb dieses Originalverzeichnisses identische Dateien geben, so wird die erste");
        W(" gefundene als Original behandelt, die Merkmale gespeichert, und alle nachfolgenden identischen");
        W(" Dateien als Kopie behandelt (wenn im Menu ausgewaehlt, erhalten die Kopien die Endung '.dup').");
        W("\n\n");
        W("Anschliessend ist ueber 'zu analysierendes Verzeichnis' als Verzeichnis 'd:\\AlleSicherungen`");
        W(" einzustellen und die Analyse erneut zu starten, ohne weitere Verwendung von 'Datenbankinhalt");
        W(" loeschen', da dies alle schon gespeicherten Merkmale vernichten wuerde.");
        W("\n\n");
        W("Abschliessend koennen nun die Dateien mit der Endung '.dup' geloescht bzw. die Textdatei");
        W(" 'logfile.txt' analysiert werden.\n");
        break;
    case 1:
        break;
        textctrl_ARC -> SetEditable(false);
    }
}

void CminerDlg::ButtonCLEAR(wxCommandEvent & event)
{
    textctrl_ARC -> Clear();
}

void CminerDlg::ButtonSTART(wxCommandEvent & event)
{
    if (!threadcount)
    {
        CreateThread();
        if (minerdlgThread)
        {
            Message(95, "");
            threadcount++;
        }
    }
    else
    {
        Message(101, "");
    }
}

void CminerDlg::ButtonSTOP(wxCommandEvent & event)
{
    if (threadcount)
    {
        minerdlgThread -> Delete();
        threadcount = 0;
        Message(93, "");
    }
    else
    {
        Message(94, "");
    }
}

void CminerDlg::ButtonDBHANDLING(wxCommandEvent & event)
{
    if (!threadcount)
    {
        PopupMenu(smPMdbhandling, 4, 72);
    }
    else
    {
        Message(101, "");
    }
}

void CminerDlg::ButtonFINDDUPS(wxCommandEvent & event)
{
    if (!threadcount)
    {
        PopupMenu(smPMfinddups, 4, 4);
    }
    else
    {
        Message(101, "");
    }
}

void CminerDlg::ButtonMD4HASHS(wxCommandEvent & event)
{
    if (!threadcount)
    {
        PopupMenu(smPMmd4hashs, 160, 0);
    }
    else
    {
        Message(101, "");
    }
}

void CminerDlg::ButtonMETFILES(wxCommandEvent & event)
{
    if (!threadcount)
    {
        PopupMenu(smPMmetfiles, 340, 0);
    }
    else
    {
        Message(101, "");
    }
}

void CminerDlg::ButtonPARTFILES(wxCommandEvent & event)
{
    if (!threadcount)
    {
        PopupMenu(smPMpartfiles, 340, 28);
    }
    else
    {
        Message(101, "");
    }
}

void CminerDlg::ButtonRENAME(wxCommandEvent & event)
{
    if (!threadcount)
    {
        PopupMenu(smPMrename, 168, 32);
    }
    else
    {
        Message(101, "");
    }
}

void CminerDlg::CheckboxDEB(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_debugonoff = checkbox_DEB -> IsChecked();
        if (p_debugonoff)
        {
            debug = 1;
        }
        else
        {
            debug = 0;
        }
    }
    else
    {
        checkbox_DEB -> SetValue(p_debugonoff);
        Message(101, "");
    }
}

void CminerDlg::CheckboxMD4(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_doMD4hashing = checkbox_MD4 -> IsChecked();
    }
    else
    {
        checkbox_MD4 -> SetValue(p_doMD4hashing);
        Message(101, "");
    }
}

void CminerDlg::CheckboxMFO(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_metfilesonly = checkbox_MFO -> IsChecked();
        if (p_metfilesonly)
        {
            p_selectpicfiles = false;
            checkbox_SELPIC -> SetValue(p_selectpicfiles);
            p_selectvidfiles = false;
            checkbox_SELVID -> SetValue(p_selectvidfiles);
            p_partfilesonly = false;
            checkbox_PFO -> SetValue(p_partfilesonly);
            p_doMD4hashing = false;
            checkbox_MD4 -> SetValue(p_doMD4hashing);
        }
    }
    else
    {
        checkbox_MFO -> SetValue(p_metfilesonly);
        Message(101, "");
    }
}

void CminerDlg::CheckboxPFO(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_partfilesonly = checkbox_PFO -> IsChecked();
        if (p_partfilesonly)
        {
            p_selectpicfiles = false;
            checkbox_SELPIC -> SetValue(p_selectpicfiles);
            p_selectvidfiles = false;
            checkbox_SELVID -> SetValue(p_selectvidfiles);
            p_metfilesonly = false;
            checkbox_MFO -> SetValue(p_metfilesonly);
            p_doMD4hashing = true;
            checkbox_MD4 -> SetValue(p_doMD4hashing);
            p_creInfForPart = true;
            smPMpartfiles -> Check(20211, p_creInfForPart);
            p_creDBentry = false;
            smPMdbhandling -> Check(20501, p_creDBentry);
            p_addMD4tofnam = false;
            smPMmd4hashs -> Check(20101, p_addMD4tofnam);
            p_doMD4fakechk = true;
            smPMmd4hashs -> Check(20102, p_doMD4fakechk);
            p_addMD4tofnam = false;
            smPMmd4hashs -> Check(20101, p_addMD4tofnam);
        }
    }
    else
    {
        checkbox_PFO -> SetValue(p_partfilesonly);
        Message(101, "");
    }
}

void CminerDlg::CheckboxIFM(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_creInfForMet = smPMmetfiles -> IsChecked(20201);
        if (p_creInfForMet)
        {
            p_sepMetFtypes = false;
            smPMmetfiles -> Check(20202, p_sepMetFtypes);
        }
    }
    else
    {
        smPMmetfiles -> Check(20201, p_creInfForMet);
        Message(101, "");
    }
}

void CminerDlg::CheckboxIFP(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_creInfForPart = smPMpartfiles -> IsChecked(20211);
    }
    else
    {
        smPMpartfiles -> Check(20211, p_creInfForPart);
        Message(101, "");
    }
}

void CminerDlg::CheckboxCNE(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_cNewED2Kparts = smPMpartfiles -> IsChecked(20212);
        if (p_cNewED2Kparts)
        {
            p_partfilesonly = true;
            checkbox_PFO -> SetValue(p_partfilesonly);
        }
    }
    else
    {
        smPMpartfiles -> Check(20212, p_cNewED2Kparts);
        Message(101, "");
    }
}

void CminerDlg::CheckboxIFO(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_creInfForAll = smPMmd4hashs -> IsChecked(20103);
    }
    else
    {
        smPMmd4hashs -> Check(20103, p_creInfForAll);
        Message(101, "");
    }
}

void CminerDlg::CheckboxCMO(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_creMetForAll = smPMmd4hashs -> IsChecked(20104);
    }
    else
    {
        smPMmd4hashs -> Check(20104, p_creMetForAll);
        Message(101, "");
    }
}

void CminerDlg::CheckboxDFI(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_usingdirfile = checkbox_DFI -> IsChecked();
    }
    else
    {
        checkbox_DFI -> SetValue(p_usingdirfile);
        Message(101, "");
    }
}

void CminerDlg::CheckboxGST(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_createsample = checkbox_GST -> IsChecked();
    }
    else
    {
        checkbox_GST -> SetValue(p_createsample);
        Message(101, "");
    }
}

void CminerDlg::CheckboxFOO(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_foldersonly = !p_foldersonly;
        if (p_foldersonly)
        {
            p_nosubfolders = false;
            checkbox_WSF -> SetValue(p_nosubfolders);
        }
    }
    else
    {
        Message(101, "");
    }
    checkbox_FOO -> SetValue(p_foldersonly);
}

void CminerDlg::CheckboxWSF(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_nosubfolders = !p_nosubfolders;
        if (p_nosubfolders)
        {
            p_foldersonly = false;
            checkbox_FOO -> SetValue(p_foldersonly);
        }
    }
    else
    {
        Message(101, "");
    }
    checkbox_WSF -> SetValue(p_nosubfolders);
}

void CminerDlg::CheckboxIGX(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_ignorexfiles = !p_ignorexfiles;
    }
    else
    {
        Message(101, "");
    }
    checkbox_IGX -> SetValue(p_ignorexfiles);
}

void CminerDlg::CheckboxMAD(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_markupduples = !p_markupduples;
        if (p_markupduples)
        {
            p_removedupext = false;
            smPMfinddups -> Check(20002, p_removedupext);
            p_deleteduples = false;
            smPMfinddups -> Check(20003, p_deleteduples);
            p_doMD4hashing = true;
#ifndef WX1
            checkbox_MD4 -> SetValue(p_doMD4hashing);
#endif
        }
    }
    else
    {
        Message(101, "");
    }
    smPMfinddups -> Check(20001, p_markupduples);
}

void CminerDlg::CheckboxRED(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_removedupext = !p_removedupext;
        if (p_removedupext)
        {
            p_markupduples = false;
            smPMfinddups -> Check(20001, p_markupduples);
            p_deleteduples = false;
            smPMfinddups -> Check(20003, p_deleteduples);
        }
    }
    else
    {
        Message(101, "");
    }
    smPMfinddups -> Check(20002, p_removedupext);
}

void CminerDlg::CheckboxDED(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_deleteduples = !p_deleteduples;
        if (p_deleteduples)
        {
            p_markupduples = false;
            smPMfinddups -> Check(20001, p_markupduples);
            p_removedupext = false;
            smPMfinddups -> Check(20002, p_removedupext);
        }
    }
    else
    {
        Message(101, "");
    }
    smPMfinddups -> Check(20003, p_deleteduples);
}

void CminerDlg::CheckboxLOG(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_logduples = !p_logduples;
    }
    else
    {
        Message(101, "");
    }
    smPMfinddups -> Check(20004, p_logduples);
}

void CminerDlg::CheckboxCreateDBentry(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_creDBentry = !p_creDBentry;
    }
    else
    {
        Message(101, "");
    }
    smPMdbhandling -> Check(20501, p_creDBentry);
}

void CminerDlg::CheckboxSFT(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_sepMetFtypes = !p_sepMetFtypes;
        if (p_sepMetFtypes)
        {
            p_doMD4hashing = false;
            checkbox_MD4 -> SetValue(p_doMD4hashing);
            p_creInfForMet = false;
            smPMmetfiles -> Check(20201, p_creInfForMet);
            p_metfilesonly = true;
            checkbox_MFO -> SetValue(p_metfilesonly);
            p_markupduples = false;
            smPMfinddups -> Check(20001, p_markupduples);
        }
    }
    else
    {
        Message(101, "");
    }
    smPMmetfiles -> Check(20202, p_doMD4hashing);
}

void CminerDlg::CheckboxSELPIC(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_selectpicfiles = !p_selectpicfiles;
        if (p_selectpicfiles)
        {
            p_metfilesonly = false;
            checkbox_MFO -> SetValue(p_metfilesonly);
            p_partfilesonly = false;
            checkbox_PFO -> SetValue(p_metfilesonly);
        }
    }
    else
    {
        Message(101, "");
    }
    checkbox_SELPIC -> SetValue(p_selectpicfiles);
}

void CminerDlg::CheckboxSELVID(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_selectvidfiles = checkbox_SELVID -> IsChecked();
        if (p_selectvidfiles)
        {
            p_metfilesonly = false;
            checkbox_MFO -> SetValue(p_metfilesonly);
            p_partfilesonly = false;
            checkbox_PFO -> SetValue(p_metfilesonly);
        }
    }
    else
    {
        checkbox_SELVID -> SetValue(p_selectvidfiles);
        Message(101, "");
    }
}

void CminerDlg::CheckboxAHF(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_addMD4tofnam = smPMmd4hashs -> IsChecked(20101);
    }
    else
    {
        smPMmd4hashs -> Check(20101, p_addMD4tofnam);
        Message(101, "");
    }
}

void CminerDlg::CheckboxFCK(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_doMD4fakechk = smPMmd4hashs -> IsChecked(20102);
    }
    else
    {
        smPMmd4hashs -> Check(20102, p_doMD4fakechk);
        Message(101, "");
    }
}

void CminerDlg::CheckboxAHD(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_doHDNsetting = smPMrename -> IsChecked(20301);
    }
    else
    {
        smPMrename -> Check(20301, p_doHDNsetting);
        Message(101, "");
    }
}

void CminerDlg::CheckboxMFL(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_moveLocation = smPMmetfiles -> IsChecked(20302);
        if (0)
        {
            p_doHDNsetting = true;
            smPMmetfiles -> Check(20301, p_doHDNsetting);
        }
    }
    else
    {
        smPMmetfiles -> Check(20302, p_moveLocation);
        Message(101, "");
    }
}

void CminerDlg::CheckboxAPR(wxCommandEvent & event)
{
    if (!threadcount)
    {
        //add picture-resolution to filename
        p_useAPRadding = checkbox_APR -> IsChecked();
    }
    else
    {
        checkbox_APR -> SetValue(p_useAPRadding);
        Message(101, "");
    }
}

void CminerDlg::CheckboxAPC(wxCommandEvent & event)
{
    //add picture-counter to directoryname
    if (!threadcount)
    {
        p_addPICCtodir = checkbox_APC -> IsChecked();
    }
    else
    {
        checkbox_APC -> SetValue(p_addPICCtodir);
        Message(101, "");
    }
}

void CminerDlg::AddingDir()
{
    sprintf((char *) buffer0, "%s/logfile.txt", DATABASEp);
    logfileFP = fopen((char *) buffer0, "w");
    //w+r
    samplerand = rand() % 65536;
    samplefilename = NULL;
    samplesourcename = NULL;
    sampledirname = NULL;
    samplefilenamez = 0;
    partfilecounter = 0;
    name0deep = 0;
    name0count = 0;
    name0fcount = 0;
    celdeep = 0;
    count_dupremoved = 0;
    count_dupmarked = 0;
    count_duplogged = 0;
    count_fakmarked = 0;
    count_deleted = 0;
    count_subdirsprocessed = 0;
    count_filesprocessed = 0;
    dbSM = new CDatabase(1, 16, DATABASEp, "dupli.DBIDX");
    wxMutexGuiEnter();
    switch (sprache)
    {
    case 0:
        textctrl_ARC -> AppendText("Quelle: ");
        textctrl_ARC -> AppendText(theApp -> sourcedirectory.c_str());
        textctrl_ARC -> AppendText("\n");
        sprintf(buffer, "%s/dupli.DBIDX", DATABASEp);
        textctrl_ARC -> AppendText(_T("Datenbank: "));
        textctrl_ARC -> AppendText(_T(buffer));
        textctrl_ARC -> AppendText("\n");
        break;
    case 1:
        textctrl_ARC -> AppendText("Source: ");
        textctrl_ARC -> AppendText(theApp -> sourcedirectory.c_str());
        textctrl_ARC -> AppendText("\n");
        sprintf(buffer, "%s/dupli.DBIDX", DATABASEp);
        textctrl_ARC -> AppendText(_T("Database: "));
        textctrl_ARC -> AppendText(_T(buffer));
        textctrl_ARC -> AppendText("\n");
        break;
    }
    if (p_logduples)
    {
        textctrl_ARC -> AppendText(_T("Logfile: "));
        textctrl_ARC -> AppendText(_T(buffer0));
        textctrl_ARC -> AppendText("\n");
    }
    Message(91, "");
    wxMutexGuiLeave();
    DIR * subdir[99];
    struct dirent * direntry;
    char * filename;
    filename = new char[1024];
    int filetype = 0;
    char * filename3;
    filename3 = new char[1024];
    int deep = 0;
    processcount = 0;
    dbSMmData.ptr = (unsigned char *) databuffer;
    sprintf(dirbuffer[0], "%s/", theApp -> sourcedirectory.c_str());
    deep++;
    subdir[deep] = opendir(dirbuffer[deep - 1]);
    dbfilecount = 0;
/*
    fp1 = fopen("dupli.STATUS", "r");
    if (fp1)
    {
        (void) fread( & dbfilecount, 1, sizeof(dbfilecount), fp1);
        fclose(fp1);
    }
*/
    if (p_createsample)
    {
        sprintf(buffer, "%s/sample", theApp -> targetdirectory.c_str());
        if (stat(buffer, statbuffer))
        {
            if (errno == ENOENT)
            {
                int l;
#ifdef __WIN32__
                l = mkdir(buffer);
#else
                l = mkdir(buffer, 777);
#endif
            }
        }
    }
    do
    {
        if (subdir[deep])
        {
            do
            {
                direntry = readdir(subdir[deep]);
                if (direntry)
                {
                    sprintf(filename, "%s", direntry -> d_name);
                    sprintf(sourcename, "%s%s", dirbuffer[deep - 1], filename);
                    if (!stat(sourcename, statbuffer))
                    {
                        if (statbuffer -> st_mode & 0040000)
                        {
                            //directory
                            if (!strcmp(filename, "."))
                            {
                                sprintf(dirbuffer[deep], "%s%s/", dirbuffer[deep - 1], filename);
                                filetype = - 1;
                            }
                            else if(!strcmp(filename, ".."))
                            {
                                sprintf(dirbuffer[deep], "%s%s/", dirbuffer[deep - 1], filename);
                                filetype = - 2;
                            }
                            else
                            {
                                sprintf(dirbuffer[deep], "%s%s/", dirbuffer[deep - 1], filename);
                                filetype = 0;
                                if (!name0deep)
                                {
                                    int l;
                                    l = strlen(filename);
                                    if (l)
                                    {
                                        --l;
                                        if (filename[l] == ')')
                                        {
                                            //specialdirfile
                                            int status = 0, i;
                                            unsigned int j;
                                            i = l;
                                            do
                                            {
                                                i--;
                                                if (filename[i] == '(')
                                                {
                                                    switch (status)
                                                    {
                                                    case 0:
                                                        delete[] name0type;
                                                        name0type = new char[l - i];
                                                        name0type[l - i - 1] = 0;
                                                        strncpy(name0type, filename + i + 1, l - i - 1);
                                                        if (i)
                                                        {
                                                            strncpy(name0, filename, i);
                                                        }
                                                        name0[i] = 0;
                                                        j = 0;
                                                        while (j < strlen(name0type))
                                                        {
                                                            if (name0type[j] == ',')
                                                            {
                                                                name0type[j] = 0;
                                                                j += 1000;
                                                            }
                                                            else
                                                            {
                                                                j++;
                                                            }
                                                        }
                                                        i = - i;
                                                        status++;
                                                        break;
                                                    default:
                                                        break;
                                                    }
                                                }
                                            }
                                            while (i > 0);
                                            if (strlen(name0type))
                                            {
                                                if (!strlen(name0))
                                                {
                                                    celdeep = deep;
                                                }
                                                else if(!name0deep)
                                                {
                                                    name0count = 0;
                                                    name0fcount = 0;
                                                    name0deep = deep;
                                                    if (p_createsample)
                                                    {
                                                        if (sampledirname)
                                                        {
                                                            delete [] sampledirname;
                                                        }
                                                        sampledirname = new char[strlen(name0) + 1];
                                                        strcpy(sampledirname, name0);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            hash -> hashtableindex = 0;
                            filetype = GetFiletype(filename);
                        }
                        if (filetype > 0)
                        {
                            if (p_selectvidfiles)
                            {
                                switch (filetype)
                                {
                                case 6:
                                case 7:
                                case 8:
                                case 9:
                                case 91:
                                    break;
                                default:
                                    filetype = - 6;
                                    break;
                                }
                            }
                            if (p_selectpicfiles)
                            {
                                switch (filetype)
                                {
                                case 6:
                                case 7:
                                case 8:
                                case 9:
                                case 91:
                                    break;
                                default:
                                    filetype = - 7;
                                    break;
                                }
                            }
                            if (p_metfilesonly)
                            {
                                switch (filetype)
                                {
                                case 8:
                                case 9:
                                case 91:
                                    break;
                                default:
                                    filetype = - 9;
                                    break;
                                }
                            }
                            if (p_partfilesonly)
                            {
                                switch (filetype)
                                {
                                case 8:
                                    break;
                                default:
                                    filetype = - 8;
                                    break;
                                }
                            }
                        }
                        if (filetype >= 0)
                        {
                            timet0 = time(NULL);
                            filesize = statbuffer -> st_size;
                            switch (filetype)
                            {
                            case 0:
                                count_subdirsprocessed++;
                                deep++;
                                subdir[deep] = opendir(dirbuffer[deep - 1]);
                                if (!subdir[deep])
                                {
                                    deep--;
                                }
                                break;
                            case 1:
                            case 6:
                            case 7:
                                count_filesprocessed++;
                                if (!p_removedupext)
                                {
                                    Process_Normalfile(dirbuffer[deep - 1], filename);
                                }
                                break;
                            case 8:
                                //.part-files
                                if (p_partfilesonly)
                                {
                                    if (!p_removedupext)
                                    {
                                        count_filesprocessed++;
                                        Process_Normalfile(dirbuffer[deep - 1], filename);
                                    }
                                }
                                break;
                            case 9:
                                //.met-files
                                if (p_metfilesonly)
                                {
                                    count_filesprocessed++;
                                    Process_Metfile(dirbuffer[deep - 1], filename);
                                }
                                break;
                                //dup-files:
                            case 91:
                                if (p_removedupext)
                                {
                                    count_dupremoved++;
                                    int i;
                                    strcpy(filename3, sourcename);
                                    i = strlen(filename3);
                                    while (i > 0)
                                    {
                                        if (filename3[--i] == '.')
                                        {
                                            filename3[i] = 0;
                                            i = - i;
                                        }
                                    }
                                    if (strcmp(filename3, sourcename))
                                    {
                                        (void) rename(sourcename, filename3);
                                    }
                                }
                                break;
                            case 92:
                                //.bak-files
                                break;
                            case 93:
                                //.ism-files(infofiles)
                                break;
                            default:
                                break;
                            }
                        }
                    }
                }
                if (minerdlgThread -> TD())
                {
                    direntry = NULL;
                    deep = - 1;
                }
            }
            while (direntry);
            closedir(subdir[deep--]);
            if (deep >= 0)
            {
                if (celdeep == deep)
                {
                    celdeep = 0;
                }
                if (name0deep == deep)
                {
                    if (samplefilename)
                    {
                        if (samplesourcename)
                        {
                            if (p_nosubfolders)
                            {
                                sprintf(buffer, "%s/sample", theApp -> targetdirectory.c_str());
                            }
                            else
                            {
                                sprintf(buffer, "%s/sample/%s", theApp -> targetdirectory.c_str(), name0);
                            }
                            if (p_foldersonly)
                            {
                                sprintf(buffer + strlen(buffer), "(%s)", name0type);
                                if (stat(buffer, statbuffer))
                                {
                                    if (errno == ENOENT)
                                    {
                                        int l;
#ifdef __WIN32__
                                        l = mkdir(buffer);
#else
                                        l = mkdir(buffer, 777);
#endif
                                    }
                                }
                            }
                            else
                            {
    /* filecopy:*/
                                FILE * fps, * fpt;
                                fps = fopen(samplesourcename, "r");
                                if (fps)
                                {
                                    sprintf(buffer + strlen(buffer), "/%s", samplefilename);
                                    fpt = fopen(buffer, "w+r");
                                    if (fpt)
                                    {
                                        long bytes = 0, bytet = 0;
                                        while ((!feof(fps)) && (bytet == bytes))
                                        {
                                            bytes = fread(buffer0, 1, BUFFERSIZE, fps);
                                            if (bytes)
                                            {
                                                bytet = fwrite(buffer0, 1, bytes, fpt);
                                            }
                                        }
                                        fclose(fpt);
                                    }
                                    fclose(fps);
                                }
                            }
    /* filecopy_end*/
                            delete[] samplesourcename;
                            samplesourcename = NULL;
                        }
                        delete[] samplefilename;
                        samplefilename = NULL;
                    }
                    if (sampledirname)
                    {
                        delete[] sampledirname;
                        sampledirname = NULL;
                        samplefilenamez = 0;
                    }
                    if (name0deep)
                    {
                        if (p_addPICCtodir)
                        {
                            sprintf(sourcename, "%s", dirbuffer[deep]);
                            if (strlen(sourcename) > 1)
                            {
                                sourcename[strlen(sourcename) - 1] = 0;
                            }
                            if (name0fcount != 1)
                            {
                                sprintf(tempname, "%s%s(%s,%dpics)", dirbuffer[deep - 1], name0, name0type, name0fcount);
                            }
                            else
                            {
                                sprintf(tempname, "%s%s(%s,%dpic)", dirbuffer[deep - 1], name0, name0type, name0fcount);
                            }
                            if (strlen(tempname))
                            {
                                if (rename(sourcename, tempname))
                                {
                                    printf("rename-error:\n");
                                    printf("tn: %s\n", tempname);
                                    printf("tn: %s\n", tempname);
                                }
                            }
                        }
                    }
                    name0deep = 0;
                    name0fcount = 0;
                }
            }
        }
        else
        {
            deep--;
        }
    }
    while (deep > 0);
    delete[] filename;
    delete[] filename3;
/*
    fp1 = fopen("dupli.STATUS", "w+r");
    if (fp1)
    {
        fwrite( & dbfilecount, 1, sizeof(dbfilecount), fp1);
        fclose(fp1);
    }
*/
    wxMutexGuiEnter();
    if (deep < 0)
    {
        deep = 0;
    }
    else
    {
        Message(92, "");
    }
    switch (sprache)
    {
    case 0:
        sprintf(buffer,
        " Unterverzeichnisse=%lu Dateien=%lu Markiert(.dup)=%lu Demarkiert=%lu Geloescht=%lu\n",
        count_subdirsprocessed, count_filesprocessed, count_dupmarked, count_dupremoved, count_deleted);
        break;
    case 1:
        sprintf(buffer,
        " Subdirs=%lu Files=%lu DUPmarked=%lu DUPremoved=%lu Deleted=%lu\n",
        count_subdirsprocessed, count_filesprocessed, count_dupmarked, count_dupremoved, count_deleted);
        break;
    }
    textctrl_ARC -> AppendText(buffer);
    wxMutexGuiLeave();
    if (samplefilename)
    {
        delete [] samplefilename;
        samplefilename = NULL;
    }
    if (samplesourcename)
    {
        delete [] samplesourcename;
        samplesourcename = NULL;
    }
    if (sampledirname)
    {
        delete [] sampledirname;
        sampledirname = NULL;
    }
    delete dbSM;
    dbSMmData.ptr = NULL;
    dbSMmData.size = 0;
    fclose(logfileFP);
}

void CminerDlg::Process_Normalfile(char * filepath, char * filename)
{
    int l, p1 = 0;
    char * filepath2 = new char[strlen(filepath) + 1];
    strcpy(filepath2, filepath);
    bool itemhashedpointer = false;
    name0fcount++;
    strcpy(buffer, filename);
    /*++++++++++++++++++++++++++++++changing source-filename&/position ++++++++++++++++*/
    if (ftype == 1)
    {
        if ((p_doHDNsetting || p_moveLocation))
        {
            if (name0deep)
            {
                int nochange = 0;
                if (p_doHDNsetting)
                {
                    l = strlen(name0);
                    if (!strncmp(filename, name0, l))
                    {
                        nochange = filecountTST(filename);
                    }
                    if (nochange)
                    {
                        printf("-->(%d)nochange(==%d) %s\n", name0fcount, nochange, filename);
                    }
                    else
                    {
                        //    printf("-->(%d)change   %s\n", name0fcount, filename);
                    }
                }
                if (!nochange)
                {
    /* filename zu aendern, da noch bislang ohne nummer: */
                    if (p_moveLocation)
                    {
                        strcpy(filepath2, dirbuffer[name0deep]);
                    }
                    if (p_doHDNsetting)
                    {
                        do
                        {
                            name0count++;
                            switch (ftype)
                            {
                            case 1:
                                sprintf(buffer, "%s.%08d.jpg", name0, name0count);
                                sprintf(tempname, "%s%s", filepath2, buffer);
                                break;
                            default:
                                sprintf(buffer, "%s.%08d.???", name0, name0count);
                                sprintf(tempname, "%s%s", filepath2, buffer);
                                break;
                            }
                            nochange = 0;
                            if (!stat(tempname, statbuffer))
                            {
                                printf("file exists: %s\n", tempname);
                                nochange = 1;
                            }
                        }
                        while (nochange);
                    }
                    else
                    {
                        sprintf(tempname, "%s%s", filepath2, filename);
                        if (!strcmp(tempname, sourcename))
                        {
                            nochange = 1;
                        }
                        else
                        {
                            nochange = 0;
                            if (!stat(tempname, statbuffer))
                            {
                                printf("file exists: %s\n", tempname);
                                nochange = 1;
                            }
                        }
                    }
                    if (!nochange)
                    {
                        if (!rename(sourcename, tempname))
                        {
                            sprintf(filename, "%s.jpg", buffer);
                            strcpy(sourcename, tempname);
                        }
                    }
                }
            }
        }
    }
    /*++++++++++++++++++++++++++++++end of changing source-filename&/position ++++++++++++++++*/
    if ((p_createsample) && (!p_partfilesonly))
    {
        if (samplefilenamez < 2)
        {
            if (!p_ignorexfiles)
            {
                for (unsigned int i = 0 ; i < strlen(filename) ; i++)
                {
                    if (filename[i] == '.')
                    {
                        if (filename[i + 1] == 'x')
                        {
                            if (samplefilename)
                            {
                                delete[] samplefilename;
                            }
                            if (samplesourcename)
                            {
                                delete[] samplesourcename;
                            }
                            samplesourcename = new char[strlen(sourcename) + 1];
                            samplefilename = new char[strlen(filename) + 8];
                            strcpy(samplesourcename, sourcename);
                            strcpy(samplefilename, filename);
                            if (strlen(samplefilename) > 3)
                            {
                                sprintf(samplefilename + strlen(samplefilename) - 3, "%04x.jpg", samplerand);
                            }
                            samplefilenamez = 2;
                            i += 1000;
                        }
                    }
                }
            }
            if (samplefilenamez < 2)
            {
                if ((rand() % name0fcount) <= 1)
                {
                    if (samplefilename)
                    {
                        delete[] samplefilename;
                    }
                    if (samplesourcename)
                    {
                        delete[] samplesourcename;
                    }
                    samplesourcename = new char[strlen(sourcename) + 1];
                    samplefilename = new char[strlen(filename) + 8];
                    strcpy(samplesourcename, sourcename);
                    strcpy(samplefilename, filename);
                    if (strlen(samplefilename) > 3)
                    {
                        sprintf(samplefilename + strlen(samplefilename) - 3, "%04x.jpg", samplerand);
                    }
                    samplefilenamez = 1;
                }
            }
        }
    }
    sprintf(buffer, "%12lu", filesize);
    p1 = 0;
    while (p1 < 12)
    {
        switch (buffer[p1])
        {
        case 32:
            buffer[p1] = '0';
            break;
        }
        p1++;
    }
    sprintf(buffer + 12, "%s", filename);
    if (p_doMD4hashing)
    {
        hash -> hashFile(sourcename, p_doMD4fakechk);
        itemhashedpointer = true;
        int i;
        if (p_doMD4fakechk)
        {
            if (hash -> IsFake())
            {
                printf("This file looks like a fake: %s\n", sourcename);
            }
            else
            {
                printf("This file looks OK         : %s\n", sourcename);
            }
        }
 /*
 Rename(sourcename, true);
 for (i = 0 ; i < 16 ; i++)
 {
 sprintf(keybuffer + i * 2, "%02x", ((unsigned char *) hash -> blockhash) [i]);
 }
 if (strlen(sourcename) > strlen(theApp->sourcedirectory.c_str()))
 {
 sprintf(databuffer, "%12lu:%s", filesize,
 sourcename + strlen(theApp->sourcedirectory.c_str()) + 1);
 }
 else
 {
 sprintf(databuffer, "%12lu:%s", filesize, sourcename);
 }
 }
 else
 {
    */
        for (i = 0 ; i < 16 ; i++)
        {
            sprintf(keybuffer + i * 2, "%02x", ((unsigned char *) hash -> blockhash) [i]);
        }
        AddHashToSourcename(sourcename, p_addMD4tofnam);
        if (strlen(sourcename) > strlen(theApp -> sourcedirectory.c_str()))
        {
            sprintf(databuffer, "%12lu:%s", filesize,
            sourcename + strlen(theApp -> sourcedirectory.c_str()) + 1);
        }
        else
        {
            sprintf(databuffer, "%12lu:%s", filesize, sourcename);
        }
        if (CheckDatabase(keybuffer, databuffer, true))
        {
            //hashkey existiert
            Rename(sourcename, false);
            free(dbSMmResult.ptr);
            dbSMmResult.ptr = NULL;
        }
 /*
 }
    */
        if (p_partfilesonly)
        {
            if (p_creInfForPart)
            {
                sprintf(tempname, "%s%s.ism", filepath2, filename);
                CreateInfofile(tempname, filepath2, filename);
            }
            if (p_cNewED2Kparts)
            {
                sprintf(tempname, "%s%s.ovn", filepath2, filename);
                if (stat(tempname, statbuffer))
                {
                    if (errno == ENOENT)
                    {
                        int l;
#ifdef __WIN32__
                        l = mkdir(tempname);
#else
                        l = mkdir(tempname, 777);
#endif
                    }
                }
    /* filecopy:*/
                unsigned int i = 1;
                char partid[strlen(filename) + 1];
                while (i < strlen(filename))
                {
                    if (filename[i] == '.')
                    {
                        memcpy(partid, filename, i);
                        partid[i] = 0;
                        i += 1000;
                    }
                    else
                    {
                        ++i;
                    }
                }
                FILE * fps, * fpt;
                sprintf(buffer, "%s%s", filepath2, filename);
                fps = fopen(buffer, "r");
                if (fps)
                {
                    int i = 0, bytes = 0, bytet = 0;
                    unsigned char * ht;
                    ht = hash -> hashtable;
                    while (!feof(fps))
                    {
                        if ((ht[i * 16] != 0xaa) || (ht[i * 16 + 8] != 0xcc))
                        {
                            ++i;
                            sprintf(tempname, "%s%s.ovn/%s.%d.part", filepath2, filename, partid, i);
                            fpt = fopen(tempname, "w+r");
                            if (fpt)
                            {
                                bytet = 9728000;
                                while ((!feof(fps)) && bytet)
                                {
                                    bytes = fread(buffer0, 1, BUFFERSIZE, fps);
                                    bytet -= bytes;
                                    if (bytes)
                                    {
                                        bytes = fwrite(buffer0, 1, bytes, fpt);
                                    }
                                }
                                fclose(fpt);
                            }
                        }
                        else
                        {
                            ++i;
                            bytet = 9728000;
                            while ((!feof(fps)) && bytet)
                            {
                                bytes = fread(buffer0, 1, BUFFERSIZE, fps);
                                bytet -= bytes;
                            }
                        }
                    }
                    fclose(fps);
                }
    /* filecopy_end*/
            }
        }
        else if(p_creInfForAll)
        {
            sprintf(tempname, "%s%s.ism", filepath2, filename);
            CreateInfofile(tempname, filepath2, filename);
        }
        if (p_creMetForAll)
        {
            sprintf(tempname, "%s%s.met", filepath2, filename);
            CreateMetfile(tempname, filepath2, filename);
        }
    }
    else
    {
        itemhashedpointer = false;
    }
    sprintf(tempname, "%s/", theApp -> targetdirectory.c_str());
    if (p_usingdirfile && (hash -> hashtableindex > 1))
    {
        p1 = strlen(tempname);
        for (int i = 0 ; i < 12 ; i++)
        {
            tempname[p1++] = buffer[i];
            switch (i)
            {
            case 2:
            case 5:
            case 8:
            case 11:
                tempname[p1] = 0;
                if (stat(tempname, statbuffer))
                {
                    if (errno == ENOENT)
                    {
#ifdef __WIN32__
                        l = mkdir(tempname);
#else
                        l = mkdir(tempname, 777);
#endif
                    }
                }
                tempname[p1++] = '/';
                break;
            }
        }
        sprintf(tempname + p1, "%s", filename);
    }
    if (p_usingdirfile && (hash -> hashtableindex > 1))
    {
        WriteDirfile(tempname, itemhashedpointer);
    }
    processcount++;
    if ((processcount% 20) == 19)
    {
        wxMutexGuiEnter();
        textctrl_ARC -> AppendText(".");
        if ((processcount% 4000) == 3999)
        {
            textctrl_ARC -> AppendText("\n");
        }
        wxMutexGuiLeave();
    }
    delete [] filepath2;
}

void CminerDlg::Process_Metfile(char * filepath, char * filename)
{
    mfile = new CMetfile(filepath, filename);
    if (mfile -> mf_filename)
    {
        strcpy(keybuffer, mfile -> mf_filehash);
        if (strlen(sourcename) > strlen(theApp -> sourcedirectory.c_str()))
        {
            sprintf(databuffer, "%12lu:%s", mfile -> mf_filesize,
            mfile -> mf_filename + strlen(theApp -> sourcedirectory.c_str()) + 1);
        }
        else
        {
            sprintf(databuffer, "%12lu:%s", mfile -> mf_filesize, mfile -> mf_filename);
        }
        char tempname[strlen(sourcename) + 32 + strlen(mfile -> mf_filename) ];
        if (CheckDatabase(keybuffer, databuffer, true))
        {
            wxMutexGuiEnter();
            sprintf(buffer, "%lu: ", count_filesprocessed);
            textctrl_ARC -> AppendText(buffer);
            //textctrl_ARC -> AppendText(filepath);
            textctrl_ARC -> AppendText(filename);
            //textctrl_ARC -> AppendText(mfile->mf_partfile);
            textctrl_ARC -> AppendText(" >> ");
            textctrl_ARC -> AppendText(mfile -> mf_filename);
            sprintf(buffer, " %lu Bytes ", mfile -> mf_filesize);
            textctrl_ARC -> AppendText(buffer);
            textctrl_ARC -> AppendText(keybuffer);
            textctrl_ARC -> AppendText("\n");
            textctrl_ARC -> AppendText(dbSMmResult.ptr);
            textctrl_ARC -> AppendText("\n");
            wxMutexGuiLeave();
            if (p_logduples || p_markupduples)
            {
                Rename(sourcename, false);
            }
            strcpy(tempname, sourcename);
            sprintf(tempname + strlen(tempname) - 4, "(%lu)%s.dup.ism", mfile -> mf_filesize, mfile -> mf_filename);
            free(dbSMmResult.ptr);
            dbSMmResult.ptr = NULL;
        }
        else
        {
            strcpy(tempname, sourcename);
            sprintf(tempname + strlen(tempname) - 4, "(%lu)%s.ism", mfile -> mf_filesize, mfile -> mf_filename);
        }
        if (p_creInfForMet)
        {
            CreateInfofileMET(tempname, filepath, filename);
        }
        if (p_sepMetFtypes)
        {
            int p;
            char * ext = new char[strlen(mfile -> mf_filename) + 1];
            p = strlen(mfile -> mf_filename);
            while (p > 0)
            {
                if (mfile -> mf_filename[--p] == '.')
                {
                    p = - p;
                }
            }
            if (p < 0)
            {
                p = - p;
                sprintf(ext, "_%s", mfile -> mf_filename + p + 1);
            }
            else
            {
                strcpy(ext, "_ubk");
            }
            bool isdir = false;
            int l = 0;
            char * tempdir = new char[strlen(filepath) + strlen(ext) + 5];
            sprintf(tempdir, "%s%s", filepath, ext);
            if (!stat(tempdir, statbuffer))
            {
                if (statbuffer -> st_mode & 0040000)
                {
                    isdir = true;
                }
            }
            else
            {
#ifdef __WIN32__
                l = mkdir(tempdir);
#else
                l = mkdir(tempdir, 0777);
#endif
                if (!l)
                {
                    isdir = true;
                }
            }
            if (isdir && (strlen(filename) > 4))
            {
                char * spath = new char[strlen(filepath) + strlen(filename) + 1];
                char * tpath = new char[strlen(tempdir) + strlen(filename) + 1];
                sprintf(spath, "%s%s", filepath, filename);
                sprintf(tpath, "%s/%s", tempdir, filename);
                (void) rename(spath, tpath);
                spath[strlen(spath) - 4] = 0;
                tpath[strlen(tpath) - 4] = 0;
                (void) rename(spath, tpath);
                delete[] spath;
                delete[] tpath;
            }
            delete[] tempdir;
            delete[] ext;
        }
    }
    else
    {
        printf("metfile-error on: %s\n", sourcename);
    }
    delete mfile;
}

int CminerDlg::GetFiletype(char * filename)
{
    int filetype = 1;
    int l = strlen(filename);
    while (l > 0)
    {
        l--;
        switch (filename[l])
        {
        case 32:
            filename[l] = 0;
            break;
        default:
            l = - l;
            break;
        }
    }
    l = - l;
    while (l > 0)
    {
        if (filename[l] == '.')
        {
            l = - l;
        }
        else
        {
            l--;
        }
    }
    l = - l;
    ftype = 0;
    if ((strlen(filename) - l) < 15)
    {
        strcpy(fileextension, filename + l);
    }
    else
    {
        strncpy(fileextension, filename + l, 15);
    }
    if (!strncasecmp(fileextension, ".dup", 4))
    {
        filetype = 91;
    }
    else if(!strncasecmp(fileextension, ".bak", 4))
    {
        filetype = 92;
    }
    else if(!strncasecmp(fileextension, ".ism", 4))
    {
        filetype = 93;
    }
    if (filetype == 1)
    {
        if (!strncasecmp(fileextension, ".png", 4))
        {
            filetype = 7;
            ftype = 2;
        }
        else if(!strncasecmp(fileextension, ".tif", 4))
        {
            filetype = 7;
        }
        else if(!strncasecmp(fileextension, ".bmp", 4))
        {
            filetype = 7;
        }
        else if(!strncasecmp(fileextension, ".gif", 4))
        {
            filetype = 7;
            ftype = 3;
        }
        else if(!strncasecmp(fileextension, ".jpe", 4))
        {
            ftype = 1;
            filetype = 7;
        }
        else if(!strncasecmp(fileextension, ".jpg", 4))
        {
            ftype = 1;
            filetype = 7;
        }
        else if(!strncasecmp(fileextension, ".jpeg", 5))
        {
            ftype = 1;
            filetype = 7;
        }
    }
    if (filetype == 1)
    {
        if (!strncasecmp(fileextension, ".avi", 4))
        {
            filetype = 6;
        }
        if (!strncasecmp(fileextension, ".mpg", 4))
        {
            filetype = 6;
        }
        if (!strncasecmp(fileextension, ".mpeg", 4))
        {
            filetype = 6;
        }
        if (!strncasecmp(fileextension, ".wmv", 4))
        {
            filetype = 6;
        }
        if (!strncasecmp(fileextension, ".asf", 4))
        {
            filetype = 6;
        }
    }
    if (filetype == 1)
    {
        if (!strncmp(fileextension, ".part", 5))
        {
            filetype = 8;
        }
        else if(!strncmp(fileextension, ".met", 4))
        {
            filetype = 9;
        }
    }
    return filetype;
}

void CminerDlg::WriteDirfile(char * filename, bool itemhashedpointer)
{
    int l = 0;
    FILE * fp;
    if (stat(filename, statbuffer))
    {
        if (errno == ENOENT)
        {
            fp = fopen(filename, "w+r");
            if (fp)
            {
                sprintf(buffer, "host:%8s:%15s\n", hostname, localip);
                sprintf(buffer + strlen(buffer), "fc:%12ld td:%27s\n", dbfilecount, ctime( & timet0));
                sprintf(buffer + strlen(buffer), "-n:%11s\n", storageid);
                l = fwrite(buffer, strlen(buffer), 1, fp);
                if (itemhashedpointer)
                {
                    sprintf(buffer + strlen(buffer), "-db:%s\n", databuffer);
                    sprintf(buffer + strlen(buffer), "-fh:%s\n", keybuffer);
                    l = fwrite(buffer, strlen(buffer), 1, fp);
                    if (filesize > 9728000)
                    {
                        long start, end;
                        for (int i0 = 0 ; i0 < hash -> hashtableindex ; i0++)
                        {
                            start = i0 * 9728000;
                            end = start + 9727999;
                            if (end >= filesize)
                            {
                                end = filesize - 1;
                            }
                            sprintf(keybuffer, "%012ld-%012ld:", start, end);
                            for (int i1 = 0 ; i1 < 16 ; i1++)
                            {
                                sprintf(keybuffer + 26 + i1 * 2, "%02x", hash -> hashtable[i0 * 16 + i1]);
                            }
                            sprintf(buffer + strlen(buffer), "-bh:%s\n", keybuffer);
                            itemhashedpointer = true;
                            l = fwrite(buffer, strlen(buffer), 1, fp);
                        }
                    }
                }
                l = fclose(fp);
            }
        }
    }
}

void CminerDlg::AddHashToSourcename(char * sourcename, bool toadd)
{
    int i;
    char tempname[34 + strlen(sourcename) ];
    strcpy(tempname, sourcename);
    i = strlen(tempname);
    while (i > 0)
    {
        if (tempname[--i] == '.')
        {
            i = - i;
        }
    }
    if (i < 0)
    {
        i = - i;
        if (i > 33)
        {
            if (!strncmp(tempname + i - 34, keybuffer, 32))
            {
                if (!toadd)
                {
                    strcpy(tempname + i - 34, sourcename + i + 1);
                }
            }
            if (!strncmp(tempname + i - 33, keybuffer, 32))
            {
                if (!toadd)
                {
                    strcpy(tempname + i - 33, sourcename + i + 1);
                }
            }
            else if(!strncmp(tempname + i - 32, keybuffer, 32))
            {
                if (!toadd)
                {
                    strcpy(tempname + i - 32, sourcename + i + 1);
                }
            }
            else if(toadd)
            {
                //add hash
                strcpy(tempname + i + 1, keybuffer);
                strcpy(tempname + i + 33, sourcename + i);
            }
        }
        else if(toadd)
        {
            //add hash
            strcpy(tempname + i + 1, keybuffer);
            strcpy(tempname + i + 33, sourcename + i);
        }
    }
    if (strlen(tempname))
    {
        if (strcmp(sourcename, tempname))
        {
            //printf("sn:%s\n", sourcename);
            //printf(">>:%s\n", tempname);
            if (!rename(sourcename, tempname))
            {
                strcpy(sourcename, tempname);
            }
        }
    }
}

void CminerDlg::Rename(char * sourcename, bool fakeorduple)
{
    bool itemfoundpointer = false;
    char tempname[4 + strlen(sourcename) ];
    int dbl, mrl;
    dbl = strlen(databuffer);
    mrl = strlen((char *) dbSMmResult.ptr);
    if (dbl == mrl)
    {
        if (!strcmp(databuffer, (char *) dbSMmResult.ptr))
        {
            itemfoundpointer = true;
        }
    }
    else if(dbl == (mrl + 33))
    {
        while (mrl > 0)
        {
            if (dbSMmResult.ptr[mrl] == '.')
            {
                mrl = - mrl;
            }
            else
            {
                mrl--;
            }
        }
        if (mrl < 0)
        {
            mrl = - mrl;
            if (!strncmp(databuffer, (char *) dbSMmResult.ptr, mrl))
            {
                if (!strncmp(keybuffer, databuffer + mrl + 1, 32))
                {
                    itemfoundpointer = true;
                }
            }
        }
    }
    else if((dbl + 33) == mrl)
    {
        while (dbl > 0)
        {
            if (dbSMmResult.ptr[dbl] == '.')
            {
                dbl = - dbl;
            }
            else
            {
                dbl--;
            }
        }
        if (dbl < 0)
        {
            dbl = - dbl;
            if (!strncmp(databuffer, (char *) dbSMmResult.ptr, dbl))
            {
                if (!strncmp(keybuffer, (char *)(dbSMmResult.ptr + dbl + 1), 32))
                {
                    itemfoundpointer = true;
                }
            }
        }
    }
    if (fakeorduple)
    {
        if (!itemfoundpointer)
        {
            strcpy(tempname, sourcename);
            strcpy(tempname + strlen(tempname), ".fak");
            count_fakmarked++;
            (void) rename(sourcename, tempname);
        }
    }
    else
    {
        if (!itemfoundpointer)
        {
            if (p_logduples)
            {
                count_duplogged++;
                fprintf(logfileFP, "---%ld---\n", count_duplogged);
                fprintf(logfileFP, "Original : %s\r\n", dbSMmResult.ptr);
                fprintf(logfileFP, "Duplikat : %s\r\n", databuffer);
            }
            if (p_markupduples)
            {
                strcpy(tempname, sourcename);
                strcpy(tempname + strlen(tempname), ".dup");
                count_dupmarked++;
                (void) rename(sourcename, tempname);
            }
            else if(p_deleteduples)
            {
                if (!unlink(sourcename))
                {
                    fprintf(logfileFP, "geloescht: %s\r\n", sourcename);
                    count_deleted++;
                }
                else
                {
                    fprintf(logfileFP, "Fehler beim Loeschen: %s\r\n", sourcename);
                }
            }
        }
    }
}

char * CminerDlg::CheckDatabase(char * keybuffer, char * databuffer, bool tostore)
{
    int p1;
    DatabaseDatum tempData;
    p1 = 0;
    while (p1 < 12)
    {
        switch (databuffer[p1])
        {
        case 32:
            databuffer[p1] = '0';
            break;
        }
        p1++;
    }
    from32to16(keybuffer, dbSMmKey.ptr);
    dbSMmResult = dbSM -> fetchKey(1, dbSMmKey);
    if (tostore)
    {
        if (!dbSMmResult.ptr)
        {
            tempData.ptr = (unsigned char *) databuffer;
            tempData.size = strlen(databuffer) + 1;
            if (p_creDBentry)
            {
                (void) dbSM -> storeData(1, dbSMmKey, tempData);
                dbfilecount++;
            }
        }
    }
    return(char *) dbSMmResult.ptr;
}

void CminerDlg::CreateInfofile(const char * infofilename, const char * filepath, const char * filename)
{
    int i0 = 0;
    FILE * tf = fopen(infofilename, "w+r");
    if (tf)
    {
        if (strlen(filepath) > strlen(theApp -> sourcedirectory.c_str()))
        {
            sprintf(buffer, "%s", filepath + strlen(theApp -> sourcedirectory.c_str()) + 1);
        }
        else
        {
            sprintf(buffer, "%s", filepath);
        }
        unsigned int p;
        delete[] storageid;
        storageid = new char[2];
        strcpy(storageid, "0");
        p = strlen(buffer);
        if (p > 4)
        {
            if (!strncmp(buffer, "fsid", 4))
            {
                p = 0;
                while (p < strlen(buffer))
                {
                    if (buffer[p] == '/')
                    {
                        p += 10000;
                    }
                    else
                    {
                        p++;
                    }
                }
                if (p > 9999)
                {
                    p -= 10000;
                    if (p)
                    {
                        delete[] storageid;
                        storageid = new char[p];
                        strncpy(storageid, buffer, p);
                        storageid[p] = 0;
                    }
                }
                else
                {
                    delete[] storageid;
                    storageid = new char[strlen(buffer) + 1];
                    strcpy(storageid, buffer);
                }
            }
        }
        fprintf(tf, "%s\n", "=================================================================================");
        fprintf(tf, "Filename  : %s\n", filename);
        fprintf(tf, "Filepath  : %s\n", buffer);
        fprintf(tf, "Filesize  : %lu\n", filesize);
        fprintf(tf, "MD4Hash   : %s\n", keybuffer);
        fprintf(tf, "---------------------------------------------------------------------------------\n");
        fprintf(tf, "Hostname  : %8s (ip=%15s)\n", hostname, localip);
        fprintf(tf, "Medium-ID : %11s  DBfilecount: %lu\n", storageid, dbfilecount);
        fprintf(tf, "Systime   : %27s", ctime( & timet0));
        fprintf(tf, "---------------------------------------------------------------------------------\n");
        if (filesize > 9728000)
        {
            fprintf(tf, "Hashtable :\n");
            long start, end;
            for (i0 = 0 ; i0 < hash -> hashtableindex ; i0++)
            {
                start = i0 * 9728000;
                end = start + 9727999;
                if (end >= filesize)
                {
                    end = filesize - 1;
                }
                fprintf(tf, "%012ld-%012ld: ", start + 1, end + 1);
                for (int i1 = 0 ; i1 < 16 ; i1++)
                {
                    fprintf(tf, "%02x", hash -> hashtable[i0 * 16 + i1]);
                }
                fprintf(tf, "\n");
            }
        }
        fprintf(tf, "%s\n", "=================================================================================");
        fclose(tf);
    }
}

void CminerDlg::CreateMetfile(const char * metfilefullname, const char * filepath, const char * filename)
{
    int l;
    partfilecounter++;
    bool isdir = false;
    char temp[strlen(metfilefullname) + 64];
    if (!stat(metfilefullname, statbuffer))
    {
        if (statbuffer -> st_mode & 0040000)
        {
            isdir = true;
        }
    }
    else
    {
#ifdef __WIN32__
        l = mkdir(metfilefullname);
#else
        l = mkdir(metfilefullname, 0777);
#endif
        if (!l)
        {
            isdir = true;
        }
    }
    if (isdir)
    {
        sprintf(temp, "%s/%05d%s", metfilefullname, partfilecounter, ".part.met");
        printf("temp=%s\n", temp);
        CMetfile * metfile = new CMetfile();
        metfile -> Create(temp);
        metfile -> SetFilename(filename);
        metfile -> SetFilesize(filesize);
        metfile -> SetFilehash((unsigned char *) hash -> blockhash);
        metfile -> SetHashtable(hash -> hashtableindex, hash -> hashtable);
        metfile -> WriteTo();
        delete metfile;
    }
}

void CminerDlg::CreateInfofileMET(char * infofilename, char * filepath, char * filename)
{
    FILE * tf = fopen(infofilename, "w+r");
    if (tf)
    {
        if (strlen(filepath) > strlen(theApp -> sourcedirectory.c_str()))
        {
            sprintf(buffer, "%s", filepath + strlen(theApp -> sourcedirectory.c_str()) + 1);
        }
        else
        {
            sprintf(buffer, "%s", filepath);
        }
        fprintf(tf, "=================================================================================\n");
        fprintf(tf, "Filename  : %s\n", mfile -> mf_filename);
        fprintf(tf, "Filepath  : %s\n", buffer);
        fprintf(tf, "Filesize  : %lu\n", mfile -> mf_filesize);
        fprintf(tf, "MD4Hash   : %s\n", keybuffer);
        fprintf(tf, "---------------------------------------------------------------------------------\n");
        fprintf(tf, "Metfile   : %s\n", filename);
        fprintf(tf, "Partfile  : %s\n", mfile -> mf_partfile);
        fprintf(tf, "Systime   : %27s", ctime( & timet0));
        fprintf(tf, "---------------------------------------------------------------------------------\n");
        if (mfile -> parts)
        {
            long start, end;
            fprintf(tf, "Hashtable :\n");
            for (int i0 = 0 ; i0 < mfile -> parts ; i0++)
            {
                start = i0 * 9728000;
                end = start + 9727999;
                if (end >= mfile -> mf_filesize)
                {
                    end = mfile -> mf_filesize - 1;
                }
                fprintf(tf, "%012ld-%012ld:", start + 1, end + 1);
                for (int i1 = 0 ; i1 < 16 ; i1++)
                {
                    sprintf(keybuffer + i1 * 2, "%02x", mfile -> parts_hashtable[i0 * 16 + i1]);
                }
                fprintf(tf, " %s\n", keybuffer);
            }
        }
        fprintf(tf, "=================================================================================\n");
        fclose(tf);
    }
}

void CminerDlg::W(const char * text)
{
    textctrl_ARC -> AppendText(text);
}

void CminerDlg::CreateThread()
{
    minerdlgThread = new CwxThread(this);
    if (minerdlgThread -> Create() != wxTHREAD_NO_ERROR)
    {
        minerdlgThread -> Delete();
        minerdlgThread = NULL;
    }
    else if(minerdlgThread -> Run() != wxTHREAD_NO_ERROR)
    {
        minerdlgThread -> Delete();
        minerdlgThread = NULL;
    }
}

void CminerDlg::Thread()
{
    AddingDir();
    threadcount = 0;
}

void CminerDlg::from32to16(char * source, unsigned char * dest)
{
    int c, i;
    for (i = 0 ; i < 32 ; i++)
    {
        c = source[i];
        if ((c >= '0') && (c <= '9'))
        {
            c -= '0';
        }
        else if((c >= 'a') && (c <= 'f'))
        {
            c -= 'a' - 10;
        }
        else if((c >= 'A') && (c <= 'F'))
        {
            c -= 'A' - 10;
        }
        if (i & 1)
        {
            dest[i >> 1] = (dest[i >> 1] << 4) | (c & 0xf);
        }
        else
        {
            dest[i >> 1] = c;
        }
    }
}

int CminerDlg::filecountTST(char * filename)
{
    char c;
    int retcode = 0;
    int l, p, punktz;
    l = strlen(filename);
    p = 0;
    do
    {
        if ((filename[p] == '.') && ((p + 9) < l))
        {
            punktz = 0;
            retcode = p;
            for (int i = 0 ; i < 10 ; i++)
            {
                c = filename[p + i];
                switch (c)
                {
                case '.':
                    punktz++;
                    break;
                default:
                    if (c < '0')
                    {
                        retcode = 0;
                        i += 1000;
                    }
                    else if(c > '9')
                    {
                        retcode = 0;
                        i += 1000;
                    }
                    break;
                }
            }
            if (punktz != 2)
            {
                retcode = false;
            }
        }
        else
        {
            p += 1000;
        }
    }
    while ((!retcode) && (p < l));
    return retcode;
}

