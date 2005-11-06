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
#define WXEF	(wxEventFunction)
#define WXCEF	(wxCommandEventFunction)

#define NAMEZMAX	1000
#define BUFFERSIZE	2048

#define WX1

CwxThread * minerdlgThread;

int name1z = 0;
char * name1[NAMEZMAX];
char tempname[BUFFERSIZE];
int samplefilenamez = 0;
int samplefilecount = 0;
char matchdomain[65];
char matchurl[65];
char matchstart[65];
wxString Csprachen[] =
{
    "Deutsch",
    "English"
};

CminerDlg::CminerDlg(wxWindow * parent)
:wxPanel(parent, - 1, wxDefaultPosition, wxDefaultSize, 0, "")
{
    strcpy(fTypes
    , ".mp3>1500.avi>5000.mpg>5000.mpeg>5000.wmf>30000.wmv>30000.jpg>15.jpe>15.jpeg>15.png>15.gif>25.bmp>25.rar.ace.zip.iso.bin.nrg.cue.txt");
    int i;
    fpSource = NULL;
    fpTarget = NULL;
    for (i = 0 ; i < 1024 ; i++)
    {
        hardlock[i] = NULL;
        hardlockDomain[i] = NULL;
    }
    hardlockAnz = 0;
    urllast = (char *) malloc(BUFFERSIZE);
    urlnext = (char *) malloc(BUFFERSIZE);
    domainlast = (char *) malloc(BUFFERSIZE);
    domainnext = (char *) malloc(BUFFERSIZE);
    DATABASEp = (char *) malloc(BUFFERSIZE);
    filename = (char *) malloc(BUFFERSIZE);
    filenameTMP = (char *) malloc(BUFFERSIZE);
    filepath = NULL;
    filepathTMP = (char *) malloc(BUFFERSIZE);
    sprache = 0;
    hostname = new char[9];
    strcpy(hostname, "hostname");
    localip = new char[16];
    strcpy(localip, "127.0.0.1");
    storageid = new char[16];
    strcpy(storageid, "");
    threadcount = 0;
    strcpy(DATABASEp, theApp -> targetdirectory.c_str());
    TARGETDIR = (char *) malloc(BUFFERSIZE);
    sprintf(TARGETDIR, "%s/%s", DATABASEp, "targetdir");
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
    /* ************ verzeichnisse anlegen ***************** */
    statbuffer = (struct stat *) malloc(sizeof(struct stat) * 2);
    if (stat(TARGETDIR, statbuffer))
    {
        if (errno == ENOENT)
        {
#ifdef __WIN32__
            (void) mkdir(TARGETDIR);
#else
            (void) mkdir(TARGETDIR, 777);
#endif
        }
    }
    /* ************ ende verzeichnisse anlegen ***************** */
    buffer = new char[BUFFERSIZE];
    buffer0 = new unsigned char[BUFFERSIZE];
    keybuffer = new char [BUFFERSIZE];
    databuffer = new char [BUFFERSIZE];
    sourcename = new char[BUFFERSIZE];
    sourcename_last = new char[BUFFERSIZE];
    hash = new Cmd4hash();
    dbPIkey.ptr = (unsigned char *) malloc(17);
    dbPIkey.size = 16;
    dbPIdata.ptr = NULL;
    dbPIdata.size = 0;
    dbPIresult.ptr = NULL;
    dbPIresult.size = 0;
    dbSMkey.ptr = (unsigned char *) malloc(17);
    dbSMkey.size = 16;
    dbSMdata.ptr = NULL;
    dbSMdata.size = 0;
    dbSMresult.ptr = NULL;
    dbSMresult.size = 0;
    dbSHkey.ptr = (unsigned char *) malloc(33);
    dbSHkey.size = 32;
    dbSHdata.ptr = NULL;
    dbSHdata.size = 0;
    dbSHresult.ptr = NULL;
    dbSHresult.size = 0;
    dbTMPkey.ptr = (unsigned char *) malloc(17);
    dbTMPkey.size = 16;
    dbTMPdata.ptr = NULL;
    dbTMPdata.size = 0;
    dbTMPresult.ptr = NULL;
    dbTMPresult.size = 0;
    dbTXkey.ptr = (unsigned char *) malloc(17);
    dbTXkey.size = 16;
    dbTXdata.ptr = NULL;
    dbTXdata.size = 0;
    dbTXresult.ptr = NULL;
    dbTXresult.size = 0;
    dbURkey.ptr = (unsigned char *) malloc(33);
    dbURkey.size = 32;
    dbURdata.ptr = NULL;
    dbURdata.size = 0;
    dbURresult.ptr = NULL;
    dbURresult.size = 0;
    dbVIkey.ptr = (unsigned char *) malloc(17);
    dbVIkey.size = 16;
    dbVIdata.ptr = NULL;
    dbVIdata.size = 0;
    dbVIresult.ptr = NULL;
    dbVIresult.size = 0;
    p_AVIexportMD4 = false;
    p_MPGexportMD4 = false;
    p_WMFexportMD4 = false;
    p_WMVexportMD4 = false;
    p_VIDexportMD4 = false;
    p_VIDimportMD4 = false;
    p_debugonoff = false;
    p_creDBentry = true;
    p_crePageForJPG = false;
    p_crePageForHTM = false;
    p_METseparaTYP = false;
    p_METcleanuTYP = false;
    p_ED2KhashMD4 = true;
    p_useCOKEY = false;
    p_PRTcreateISM = false;
    p_PRTcreateOVN = false;
    p_PRTsearchMET = true;
    p_METcreateISM = false;
    p_METcreatePRT = false;
    p_METrepairPRT = false;
    p_STDcreateINF = false;
    p_STDcreateMET = false;
    p_delHtmPxxCxx = false;
    p_simHtmPxxCxx = false;
    p_doMD4hashing = true;
    p_addMD4tofnam = false;
    p_HTMLaddMD4 = false;
    p_DKTrenameMD4 = false;
    p_WD3deleteALL = false;
    p_delPRYfiles = false;
    p_useHARDLOCK = false;
    p_doMD4fakechk = false;
    p_doHDNsetting = false;
    p_AVIparse = false;
    p_DKTparse = false;
    p_GIFparse = false;
    p_HTMparse = false;
    p_ISMparse = false;
    p_JPGparse = false;
    p_METparse = false;
    p_MP3parse = false;
    p_MPGparse = false;
    p_PRTparse = false;
    p_STDparse = false;
    p_WAVparse = false;
    p_WD3parse = false;
    p_WMFparse = false;
    p_WMVparse = false;
    p_moveLocation = false;
    p_addPICCtodir = false;
    p_useAPRadding = false;
    p_usingdirfile = false;
    p_nosubfolders = false;
    p_createsample = false;
    p_foldersonly = false;
    p_ignorexfiles = true;
    p_markupduples = false;
    p_deleteduples = false;
    p_logduples = false;
    p_removedupext = false;
    p_selectvidfiles = false;
    p_ISMimportMD4 = true;
    p_ISMdeleteDUP = false;
    p_JPGexportMD4 = false;
    p_JPGaddRStoFN = false;
    p_JPGaddFNtoTA = false;
    p_JPGcreatePDS = false;
    p_JPGwrbackMD4 = false;
    p_JPGimportMD4 = false;
    p_GIFexportMD4 = false;
    p_GIFwrbackMD4 = false;
    p_GIFimportMD4 = false;
    p_METimportMD4 = true;
    smPMfinddups = new wxMenu("Duplikate suchen und markieren");
    smPMfinddups -> AppendCheckItem(20001, ".");
    smPMfinddups -> AppendCheckItem(20002, ".");
    smPMfinddups -> AppendSeparator();
    smPMfinddups -> AppendCheckItem(20004, ".");
    smPMfinddups -> AppendSeparator();
    smPMfinddups -> AppendCheckItem(20005, ".");
    smPMfinddups -> Check(20001, p_markupduples);
    smPMfinddups -> Check(20002, p_removedupext);
    smPMfinddups -> Check(20004, p_deleteduples);
    smPMfinddups -> Check(20005, p_logduples);
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
    smPMdbhandling -> AppendCheckItem(20515, ".");
    smPMdbhandling -> AppendCheckItem(20516, ".");
    //smPMdbhandling -> Append(20521, ".");
    smPMdbhandling -> Check(20501, p_creDBentry);
    smPMdbhandling -> Check(20515, p_ED2KhashMD4);
    smPMdbhandling -> Check(20516, p_useCOKEY);
    smPMstdfiles = new wxMenu("Standardverarbeitung von Dateien");
    smPMstdfiles -> AppendCheckItem(21101, "create .ism for *ALL");
    smPMstdfiles -> AppendCheckItem(21102, "create .met for *ALL");
    smPMstdfiles -> Check(21101, p_STDcreateINF);
    smPMstdfiles -> Check(21102, p_STDcreateMET);
    smPMhocfiles = new wxMenu("Verarbeitung überprüfter Dateien");
    smPMhocfiles -> AppendCheckItem(20401, ".");
    smPMhocfiles -> AppendCheckItem(20405, ".");
    smPMhocfiles -> AppendCheckItem(20406, ".");
    smPMhocfiles -> AppendSeparator();
    smPMhocfiles -> AppendCheckItem(20409, "delete htm/php/cgi/pry");
    smPMhocfiles -> AppendCheckItem(20410, "delsim htm/php/cgi/pry");
    smPMhocfiles -> AppendCheckItem(20411, "debugger on/off");
    smPMhocfiles -> Check(20401, p_addMD4tofnam);
    smPMhocfiles -> Check(20405, p_VIDexportMD4);
    smPMhocfiles -> Check(20406, p_VIDimportMD4);
    smPMhocfiles -> Check(20409, p_delHtmPxxCxx);
    smPMhocfiles -> Check(20410, p_simHtmPxxCxx);
    smPMhocfiles -> Check(20411, p_debugonoff);
    smPMmd4hashs = new wxMenu("MD4-Hashprocessing");
    smPMmd4hashs -> AppendCheckItem(20102, "With fakecheck(slower)");
    smPMmd4hashs -> AppendSeparator();
    smPMmd4hashs -> Check(20102, p_doMD4fakechk);
    smPMdktfiles = new wxMenu(_T("DKT-Verarbeitung"));
    smPMdktfiles -> AppendCheckItem(20901, ".");
    smPMdktfiles -> Check(20901, p_DKTrenameMD4);
    smPMwd3files = new wxMenu(_T("WD3-Verarbeitung"));
    smPMwd3files -> AppendCheckItem(21001, ".");
    smPMwd3files -> Check(21001, p_WD3deleteALL);
    smPMhtmfiles = new wxMenu(_T("HTM-Verarbeitung"));
    smPMhtmfiles -> AppendCheckItem(20601, ".");
    smPMhtmfiles -> AppendCheckItem(20602, ".");
    smPMhtmfiles -> AppendCheckItem(20603, ".");
    smPMhtmfiles -> AppendCheckItem(20604, ".");
    smPMhtmfiles -> AppendCheckItem(20605, ".");
    smPMhtmfiles -> Check(20601, p_crePageForHTM);
    smPMhtmfiles -> Check(20602, p_crePageForJPG);
    smPMhtmfiles -> Check(20603, p_delPRYfiles);
    smPMhtmfiles -> Check(20604, p_useHARDLOCK);
    smPMhtmfiles -> Check(20605, p_HTMLaddMD4);
    smPMgiffiles = new wxMenu(_T("GIF-Verarbeitung"));
    smPMgiffiles -> AppendCheckItem(20801, "Export GIF-files");
    smPMgiffiles -> AppendCheckItem(20802, "Write MD4-hash");
    smPMgiffiles -> AppendCheckItem(20803, "Import GIF-files");
    smPMgiffiles -> Check(20801, p_GIFexportMD4);
    smPMgiffiles -> Check(20802, p_GIFwrbackMD4);
    smPMgiffiles -> Check(20803, p_GIFimportMD4);
    smPMjpgfiles = new wxMenu(_T("JPG-Verarbeitung"));
    smPMjpgfiles -> AppendCheckItem(20701, "Export JPG-files");
    smPMjpgfiles -> AppendCheckItem(20705, "Add namepart to targetfilename");
    smPMjpgfiles -> AppendCheckItem(20704, "Add resolution 'widthXheight' to targetfilename");
    smPMjpgfiles -> AppendCheckItem(20706, "Create package-directories every 700 MBytes");
    smPMjpgfiles -> AppendSeparator();
    smPMjpgfiles -> AppendCheckItem(20702, "Write MD4-hash");
    smPMjpgfiles -> AppendCheckItem(20703, "Import JPG-files");
    smPMjpgfiles -> Check(20701, p_JPGexportMD4);
    smPMjpgfiles -> Check(20702, p_JPGwrbackMD4);
    smPMjpgfiles -> Check(20703, p_JPGimportMD4);
    smPMjpgfiles -> Check(20704, p_JPGaddRStoFN);
    smPMjpgfiles -> Check(20705, p_JPGaddFNtoTA);
    smPMjpgfiles -> Check(20706, p_JPGcreatePDS);
    smPMismfiles = new wxMenu(_T("ISM(ed2k)-Verarbeitung"));
    smPMismfiles -> AppendCheckItem(23381, "Importiere MD4-Hashs aus ISM-Dateien");
    smPMismfiles -> Append(23382, "Lösche Datenbank mit MD4-Hashs aus MET- und ISM-Dateien (md4s.DBIDX)");
    smPMismfiles -> Append(23383, "Exportiere Datenbank (md4s.DBIDX) nach 'md4s.export'");
    smPMismfiles -> Append(23384, "Lösche STD-Duplikate mit Einträgen in der 'md4s.DBIDX'-Datenbank");
    smPMismfiles -> Check(23381, p_ISMimportMD4);
    smPMismfiles -> Check(23384, p_ISMdeleteDUP);
    smPMmetfiles = new wxMenu(_T("MET(ed2k)-Verarbeitung"));
    smPMmetfiles -> AppendCheckItem(20207, "Importiere MD4-Hashs aus MET-Dateien");
    smPMmetfiles -> Append(20208, "Lösche Datenbank mit MD4-Hashs aus MET- und ISM-Dateien (md4s.DBIDX)");
    smPMmetfiles -> Check(20207, p_METimportMD4);
    smPMmetfiles -> AppendCheckItem(20201, "create ism-files");
    smPMmetfiles -> AppendCheckItem(20202, "sep ftypes");
    smPMmetfiles -> AppendCheckItem(20203, "create partfiles if needed");
    sprintf(buffer, "Cleanup metfiles - all but: %s", fTypes);
    smPMmetfiles -> AppendCheckItem(20204, buffer);
    smPMmetfiles -> AppendCheckItem(20205, "Repair met-files with partfiles");
    smPMmetfiles -> AppendCheckItem(20206, "Add realname to filename");
    smPMmetfiles -> Check(20201, p_METcreateISM);
    smPMmetfiles -> Check(20202, p_METseparaTYP);
    smPMmetfiles -> Check(20203, p_METcreatePRT);
    smPMmetfiles -> Check(20204, p_METcleanuTYP);
    smPMmetfiles -> Check(20205, p_METrepairPRT);
    smPMmetfiles -> Check(20206, false);
    smPMprtfiles = new wxMenu("Partfile-Processing");
    smPMprtfiles -> AppendCheckItem(20211, "create .ism for .part");
    smPMprtfiles -> AppendCheckItem(20212, "create new-ed2k-parts");
    smPMprtfiles -> AppendCheckItem(20213, "search .met for .part");
    smPMprtfiles -> Check(20211, p_PRTcreateISM);
    smPMprtfiles -> Check(20212, p_PRTcreateOVN);
    smPMprtfiles -> Check(20213, p_PRTsearchMET);
    smPMrename = new wxMenu("Rename and/or moving files");
    smPMrename -> AppendSeparator();
    smPMrename -> AppendCheckItem(20301, "Set HDir(Homedirectory='./xyz(...)/...' to filename");
    smPMrename -> AppendCheckItem(20302, "Move file to HDir");
    smPMrename -> Check(20301, p_doHDNsetting);
    smPMrename -> Check(20302, p_moveLocation);
    button_START = new wxButton(this, 11351, ".", wxDefaultPosition, wxSize(128, 24));
    button_GENTFD = new wxButton(this, 11356, ".", wxDefaultPosition, wxSize(200, 24));
    button_STOP = new wxButton(this, 11352, ".", wxDefaultPosition, wxSize(128, 24));
    button_CLEAR = new wxButton(this, 11359, ".", wxDefaultPosition, wxSize(160, 24));
    button_PARSE = new wxButton(this, 12001, ".", wxDefaultPosition, wxSize(160, 24));
    button_PINFO = new wxButton(this, 11371, ".", wxDefaultPosition, wxSize(128, 24));
    radiobox_LAN = new wxRadioBox((wxWindow *) this, 11399, "Sprache/Language", wxDefaultPosition,
    wxDefaultSize, WXSIZEOF(Csprachen), Csprachen, 0);
    tctrl_ARC = new wxTextCtrl(this, 11398, "", wxDefaultPosition, wxDefaultSize
    , wxTE_PROCESS_ENTER | wxTE_MULTILINE);
    tctrl_ARC -> SetEditable(true);
    wxBoxSizer * sizer_root = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer * sizer_h1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * sizer_v1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer * sizer_v2 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer * sizer_v3 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer * sizer_v4 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer * sizer_v4h1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * sizer_v4h2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * sizer_v4h3 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * sizer_h2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * sizer_h2v1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer * sizer_h2v1h1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * sizer_h2v1h2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * sizer_h2v1h3 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * sizer_h2v1h4 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * sizer_h4 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * sizer_h8 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * sizer_h9 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * sizer_DUP = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * sizer_HOC = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * sizer_MATCHDOMAIN = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * sizer_MATCHSTART = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * sizer_MATCHURL = new wxBoxSizer(wxHORIZONTAL);
    stext_CHSD = new wxStaticText(this, - 1, theApp -> sourcedirectory, wxDefaultPosition, wxSize(256, 24));
    stext_CHDD = new wxStaticText(this, - 1, TARGETDIR, wxDefaultPosition, wxSize(256, 24));
    stext_CHTD = new wxStaticText(this, - 1, theApp -> targetdirectory, wxDefaultPosition, wxSize(256, 24));
    button_AVIparse = new wxButton(this, 12331, "", wxPoint( - 1, - 1), wxSize(80, 24));
    button_DKTparse = new wxButton(this, 12326, "", wxPoint( - 1, - 1), wxSize(80, 24));
    button_GIFparse = new wxButton(this, 12325, "", wxPoint( - 1, - 1), wxSize(80, 24));
    button_HTMparse = new wxButton(this, 12323, "", wxPoint( - 1, - 1), wxSize(80, 24));
    button_ISMparse = new wxButton(this, 12338, "", wxPoint( - 1, - 1), wxSize(80, 24));
    button_JPGparse = new wxButton(this, 12324, "", wxPoint( - 1, - 1), wxSize(80, 24));
    button_METparse = new wxButton(this, 12321, "", wxPoint( - 1, - 1), wxSize(80, 24));
    button_MP3parse = new wxButton(this, 12335, "", wxPoint( - 1, - 1), wxSize(80, 24));
    button_MPGparse = new wxButton(this, 12332, "", wxPoint( - 1, - 1), wxSize(80, 24));
    button_PRTparse = new wxButton(this, 12337, "", wxPoint( - 1, - 1), wxSize(80, 24));
    button_STDparse = new wxButton(this, 12329, "", wxPoint( - 1, - 1), wxSize(80, 24));
    button_WAVparse = new wxButton(this, 12336, "", wxPoint( - 1, - 1), wxSize(80, 24));
    button_WD3parse = new wxButton(this, 12330, "", wxPoint( - 1, - 1), wxSize(80, 24));
    button_WMFparse = new wxButton(this, 12333, "", wxPoint( - 1, - 1), wxSize(80, 24));
    button_WMVparse = new wxButton(this, 12334, "", wxPoint( - 1, - 1), wxSize(80, 24));
    button_DBHANDLING = new wxButton(this, 11355, ".", wxDefaultPosition, wxSize(200, - 1));
    button_FINDDUPS = new wxButton(this, 12302, "&Duplikate suchen und markieren",
    wxDefaultPosition, wxSize(200, - 1));
    button_HOCFILES = new wxButton(this, 12303, "&Verarbeitung überprüfter Dateien",
    wxDefaultPosition, wxSize(200, - 1));
    button_CHSD = new wxButton(this, 11401, ".", wxDefaultPosition, wxSize(200, - 1));
    button_CHDD = new wxButton(this, 11402, ".", wxDefaultPosition, wxSize(200, - 1));
    button_CHTD = new wxButton(this, 11403, ".", wxDefaultPosition, wxSize(200, - 1));
    checkbox_AVIparse = new wxCheckBox(this, 11331, _T(""), wxPoint( - 1, - 1)
    , wxDefaultSize, 0, wxDefaultValidator, "avi");
    checkbox_AVIparse -> SetValue(p_AVIparse);
    checkbox_DKTparse = new wxCheckBox(this, 11328, "", wxDefaultPosition
    , wxSize( - 1, - 1), 0, wxDefaultValidator, "dkt");
    checkbox_DKTparse -> SetValue(p_DKTparse);
    checkbox_GIFparse = new wxCheckBox(this, 11327, "", wxDefaultPosition
    , wxSize( - 1, - 1), 0, wxDefaultValidator, "gpa");
    checkbox_GIFparse -> SetValue(p_GIFparse);
    checkbox_HTMparse = new wxCheckBox(this, 11325, "", wxDefaultPosition
    , wxSize( - 1, - 1), 0, wxDefaultValidator, "pah");
    checkbox_HTMparse -> SetValue(p_HTMparse);
    checkbox_ISMparse = new wxCheckBox(this, 11338, _T(""), wxPoint( - 1, - 1)
    , wxDefaultSize, 0, wxDefaultValidator, "ism");
    checkbox_ISMparse -> SetValue(p_ISMparse);
    checkbox_JPGparse = new wxCheckBox(this, 11326, "", wxDefaultPosition
    , wxSize( - 1, - 1), 0, wxDefaultValidator, "jpa");
    checkbox_JPGparse -> SetValue(p_JPGparse);
    checkbox_METparse = new wxCheckBox(this, 11321, _T(""), wxPoint( - 1, - 1)
    , wxDefaultSize, 0, wxDefaultValidator, "mfo");
    checkbox_METparse -> SetValue(p_METparse);
    checkbox_MP3parse = new wxCheckBox(this, 11335, _T(""), wxPoint( - 1, - 1)
    , wxDefaultSize, 0, wxDefaultValidator, "mp3");
    checkbox_MP3parse -> SetValue(p_MP3parse);
    checkbox_MPGparse = new wxCheckBox(this, 11332, _T(""), wxPoint( - 1, - 1)
    , wxDefaultSize, 0, wxDefaultValidator, "mpg");
    checkbox_MPGparse -> SetValue(p_WD3parse);
    checkbox_PRTparse = new wxCheckBox(this, 11337, _T(""), wxPoint( - 1, - 1)
    , wxDefaultSize, 0, wxDefaultValidator, "mfo");
    checkbox_PRTparse -> SetValue(p_PRTparse);
    checkbox_STDparse = new wxCheckBox(this, 11329, _T(""), wxPoint( - 1, - 1)
    , wxDefaultSize, 0, wxDefaultValidator, "std");
    checkbox_STDparse -> SetValue(p_STDparse);
    checkbox_WAVparse = new wxCheckBox(this, 11336, _T(""), wxPoint( - 1, - 1)
    , wxDefaultSize, 0, wxDefaultValidator, "wav");
    checkbox_WAVparse -> SetValue(p_WAVparse);
    checkbox_WD3parse = new wxCheckBox(this, 11330, _T(""), wxPoint( - 1, - 1)
    , wxDefaultSize, 0, wxDefaultValidator, "wd3");
    checkbox_WD3parse -> SetValue(p_WD3parse);
    checkbox_WMFparse = new wxCheckBox(this, 11333, _T(""), wxPoint( - 1, - 1)
    , wxDefaultSize, 0, wxDefaultValidator, "wmf");
    checkbox_WMFparse -> SetValue(p_WMFparse);
    checkbox_WMVparse = new wxCheckBox(this, 11334, _T(""), wxPoint( - 1, - 1)
    , wxDefaultSize, 0, wxDefaultValidator, "wmv");
    checkbox_WMVparse -> SetValue(p_WMVparse);
    sizer_DUP -> Add(button_FINDDUPS, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 0);
    sizer_HOC -> Add(button_HOCFILES, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 0);
    wxStaticText * stext_MATCHDOMAIN = new wxStaticText(this, - 1, "Matchbase"
    , wxDefaultPosition, wxSize(80, 16));
    tctrl_MATCHDOMAIN = new wxTextCtrl(this, - 1, "", wxDefaultPosition, wxSize(80, 16), 0);
    tctrl_MATCHDOMAIN -> SetMaxLength(64);
    button_MATCHDOMAINclr = new wxButton(this, 11451, "CLR", wxDefaultPosition, wxSize(32, 16));
    sizer_MATCHDOMAIN -> Add(stext_MATCHDOMAIN, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);
    sizer_MATCHDOMAIN -> Add(tctrl_MATCHDOMAIN, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);
    sizer_MATCHDOMAIN -> Add(button_MATCHDOMAINclr, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);
    wxStaticText * stext_MATCHURL = new wxStaticText(this, - 1, "Matchall"
    , wxDefaultPosition, wxSize(80, 16));
    tctrl_MATCHURL = new wxTextCtrl(this, - 1, "", wxDefaultPosition, wxSize(80, 16), 0);
    tctrl_MATCHURL -> SetMaxLength(64);
    button_MATCHURLclr = new wxButton(this, 11452, "CLR", wxDefaultPosition, wxSize(32, 16));
    sizer_MATCHURL -> Add(stext_MATCHURL, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);
    sizer_MATCHURL -> Add(tctrl_MATCHURL, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);
    sizer_MATCHURL -> Add(button_MATCHURLclr, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);
    wxStaticText * stext_MATCHSTART = new wxStaticText(this, - 1, "Startmatch"
    , wxDefaultPosition, wxSize(80, 16));
    tctrl_MATCHSTART = new wxTextCtrl(this, - 1, "", wxDefaultPosition, wxSize(80, 16), 0);
    tctrl_MATCHSTART -> SetMaxLength(64);
    button_MATCHSTARTclr = new wxButton(this, 11453, "CLR", wxDefaultPosition, wxSize(32, 16));
    sizer_MATCHSTART -> Add(stext_MATCHSTART, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);
    sizer_MATCHSTART -> Add(tctrl_MATCHSTART, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);
    sizer_MATCHSTART -> Add(button_MATCHSTARTclr, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);
    sizer_v1 -> Add(sizer_DUP, 0, wxALIGN_CENTER_VERTICAL | wxALL, 4);
    sizer_v1 -> Add(sizer_MATCHDOMAIN, 0, wxALIGN_CENTER_VERTICAL | wxALL, 4);
    sizer_v1 -> Add(sizer_MATCHURL, 0, wxALIGN_CENTER_VERTICAL | wxALL, 4);
    sizer_v1 -> Add(sizer_MATCHSTART, 0, wxALIGN_CENTER_VERTICAL | wxALL, 4);
    sizer_v1 -> Add(sizer_HOC, 0, wxALIGN_CENTER_VERTICAL | wxALL, 4);
    sizer_v4h1 -> Add(button_CHSD, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
    sizer_v4h1 -> Add(stext_CHSD, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
    sizer_v2 -> Add(sizer_v4h1, 0, wxALIGN_CENTER_VERTICAL | wxALL, 4);
    sizer_v4h2 -> Add(button_CHDD, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
    sizer_v4h2 -> Add(stext_CHDD, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);
    sizer_v2 -> Add(sizer_v4h2, 0, wxALIGN_CENTER_VERTICAL | wxALL, 4);
    sizer_v2 -> Add(button_DBHANDLING, 0, wxALIGN_CENTER_VERTICAL | wxALL, 4);
    sizer_v4h3 -> Add(button_CHTD, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
    sizer_v4h3 -> Add(stext_CHTD, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);
    sizer_v2 -> Add(sizer_v4h3, 0, wxALIGN_CENTER_VERTICAL | wxALL, 4);
    sizer_h1 -> Add(sizer_v1, 0, 0, 0);
    sizer_h1 -> Add(sizer_v2, 0, 0, 0);
    sizer_h1 -> Add(sizer_v3, 1, wxGROW, 0);
    sizer_h1 -> Add(sizer_v4, 0, 0, 0);
    sizer_h2v1h1 -> Add(checkbox_STDparse, 0, wxALIGN_CENTER_VERTICAL, 0);
    sizer_h2v1h1 -> Add(button_STDparse, 0, 0, 0);
    sizer_h2v1h1 -> Add(checkbox_HTMparse, 0, wxALIGN_CENTER_VERTICAL, 0);
    sizer_h2v1h1 -> Add(button_HTMparse, 0, 0, 0);
    sizer_h2v1h1 -> Add(checkbox_METparse, 0, wxALIGN_CENTER_VERTICAL, 0);
    sizer_h2v1h1 -> Add(button_METparse, 0, 0, 0);
    sizer_h2v1h1 -> Add(checkbox_PRTparse, 0, wxALIGN_CENTER_VERTICAL, 0);
    sizer_h2v1h1 -> Add(button_PRTparse, 0, 0, 0);
    sizer_h2v1h1 -> Add(checkbox_ISMparse, 0, wxALIGN_CENTER_VERTICAL, 0);
    sizer_h2v1h1 -> Add(button_ISMparse, 0, 0, 0);
    sizer_h2v1h1 -> Add(checkbox_WD3parse, 0, wxALIGN_CENTER_VERTICAL, 0);
    sizer_h2v1h1 -> Add(button_WD3parse, 0, 0, 0);
    sizer_h2v1h1 -> Add(checkbox_DKTparse, 0, wxALIGN_CENTER_VERTICAL, 0);
    sizer_h2v1h1 -> Add(button_DKTparse, 0, 0, 0);
    sizer_h2v1h2 -> Add(checkbox_JPGparse, 0, wxALIGN_CENTER_VERTICAL, 0);
    sizer_h2v1h2 -> Add(button_JPGparse, 0, 0, 0);
    sizer_h2v1h2 -> Add(checkbox_GIFparse, 0, wxALIGN_CENTER_VERTICAL, 0);
    sizer_h2v1h2 -> Add(button_GIFparse, 0, 0, 0);
    sizer_h2v1h3 -> Add(checkbox_AVIparse, 0, wxALIGN_CENTER_VERTICAL, 0);
    sizer_h2v1h3 -> Add(button_AVIparse, 0, 0, 0);
    sizer_h2v1h3 -> Add(checkbox_MPGparse, 0, wxALIGN_CENTER_VERTICAL, 0);
    sizer_h2v1h3 -> Add(button_MPGparse, 0, 0, 0);
    sizer_h2v1h3 -> Add(checkbox_WMFparse, 0, wxALIGN_CENTER_VERTICAL, 0);
    sizer_h2v1h3 -> Add(button_WMFparse, 0, 0, 0);
    sizer_h2v1h3 -> Add(checkbox_WMVparse, 0, wxALIGN_CENTER_VERTICAL, 0);
    sizer_h2v1h3 -> Add(button_WMVparse, 0, 0, 0);
    sizer_h2v1h4 -> Add(checkbox_MP3parse, 0, wxALIGN_CENTER_VERTICAL, 0);
    sizer_h2v1h4 -> Add(button_MP3parse, 0, 0, 0);
    sizer_h2v1h4 -> Add(checkbox_WAVparse, 0, wxALIGN_CENTER_VERTICAL, 0);
    sizer_h2v1h4 -> Add(button_WAVparse, 0, 0, 0);
    sizer_h2v1 -> Add(sizer_h2v1h1, 0, wxALL, 2);
    sizer_h2v1 -> Add(sizer_h2v1h2, 0, wxALL, 2);
    sizer_h2v1 -> Add(sizer_h2v1h3, 0, wxALL, 2);
    sizer_h2v1 -> Add(sizer_h2v1h4, 0, wxALL, 2);
    sizer_h2 -> Add(sizer_h2v1, 0, 0, 0);
    sizer_h8 -> Add(button_START, 0, wxALL, 4);
    sizer_h8 -> Add(button_GENTFD, 0, wxALL, 4);
    sizer_h8 -> Add(button_STOP, 0, wxALL, 4);
    sizer_h8 -> Add(button_PINFO, 1, wxALIGN_RIGHT | wxALL, 4);
    sizer_h9 -> Add(button_CLEAR, 0, wxALL, 4);
    sizer_h9 -> Add(button_PARSE, 0, wxALL, 4);
    stext_C1 = new wxStaticText(this, - 1, "", wxDefaultPosition, wxSize(256, 20));
    sizer_h4 -> Add(stext_C1, 0, wxALL, 2);
    sizer_root -> Add(sizer_h1, 0, 0, 0);
    sizer_root -> Add(sizer_h2, 0, 0, 0);
    sizer_root -> Add(sizer_h4, 0, 0, 0);
    sizer_root -> Add(sizer_h8, 0, 0, 0);
    sizer_root -> Add(tctrl_ARC, 1, wxEXPAND | wxALL, 5);
    sizer_root -> Add(sizer_h9, 0, 0, 0);
    sizer_root -> Add(radiobox_LAN, 0, wxGROW | wxALIGN_CENTER_HORIZONTAL, 0);
#ifndef WX1
    wxBoxSizer * sizer_MD4 = new wxBoxSizer(wxHORIZONTAL);
    checkbox_MD4 = new wxCheckBox(this, 11312, _T(""), wxDefaultPosition,
    wxDefaultSize, 0, wxDefaultValidator, "md4");
    checkbox_MD4 -> SetValue(p_doMD4hashing);
    wxButton * button_MD4HASHS = new wxButton(this, 12311, "&MD4-Hashing");
    sizer_MD4 -> Add(checkbox_MD4, 0, wxALIGN_CENTER_VERTICAL, 0);
    sizer_MD4 -> Add(button_MD4HASHS, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);
    checkbox_APR = new wxCheckBox(this, 11316, _T("Add PicRes to filename"), wxDefaultPosition,
    wxDefaultSize, 0, wxDefaultValidator, "ahd");
    checkbox_APR -> SetValue(p_useAPRadding);
    checkbox_APC = new wxCheckBox(this, 11317, _T("Add PicCount to Dir"), wxDefaultPosition,
    wxDefaultSize, 0, wxDefaultValidator, "apc");
    checkbox_APC -> SetValue(p_addPICCtodir);
    checkbox_DFI = new wxCheckBox(this, 11901, _T("Generate Dir(/-File)tree"), wxDefaultPosition,
    wxDefaultSize, 0, wxDefaultValidator, "dfi");
    checkbox_DFI -> SetValue(p_usingdirfile);
    checkbox_GST = new wxCheckBox(this, 11902, _T("Generate sampletree"), wxDefaultPosition,
    wxDefaultSize, 0, wxDefaultValidator, "gsd");
    checkbox_GST -> SetValue(p_createsample);
    checkbox_FOO = new wxCheckBox(this, 11903, _T("...(with folders only)"), wxDefaultPosition,
    wxDefaultSize, 0, wxDefaultValidator, "foo");
    checkbox_FOO -> SetValue(p_foldersonly);
    checkbox_WSF = new wxCheckBox(this, 11904, _T("...(without subfolders)"), wxDefaultPosition,
    wxDefaultSize, 0, wxDefaultValidator, "wsf");
    checkbox_WSF -> SetValue(p_nosubfolders);
    checkbox_IGX = new wxCheckBox(this, 11905, _T("Don`t prefer *.x.*-files"), wxDefaultPosition,
    wxDefaultSize, 0, wxDefaultValidator, "igx");
    checkbox_IGX -> SetValue(p_createsample);
    sizer_v1 -> Add(sizer_DUP, 0, wxALIGN_CENTER_VERTICAL | wxALL, 4);
    sizer_v2 -> Add(sizer_MD4, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
    wxButton * button_RENAME = new wxButton(this, 12901, "&Renaming and/or moving files");
    sizer_v2 -> Add(button_RENAME, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
    sizer_v2 -> Add(checkbox_APR, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
    sizer_v2 -> Add(checkbox_APC, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
    sizer_v4 -> Add(sizer_v4h1, 0, 0, 0);
    sizer_v4 -> Add(sizer_v4h2, 0, 0, 0);
    sizer_v4 -> Add(sizer_v4h3, 0, 0, 0);
    sizer_v4 -> Add(checkbox_DFI, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
    sizer_v4 -> Add(checkbox_GST, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
    sizer_v4 -> Add(checkbox_FOO, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
    sizer_v4 -> Add(checkbox_WSF, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
    sizer_v4 -> Add(checkbox_IGX, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
#endif
    lan00German();
    this -> SetSizer(sizer_root);
    Connect(11398, wxEVT_COMMAND_TEXT_ENTER, WXOEF WXEF WXCEF & CminerDlg::ButtonPARSE);
    Connect(12001, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonPARSE);
    Connect(11321, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxMETparse);
    Connect(11337, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxPRTparse);
    Connect(11338, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxISMparse);
    Connect(11401, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonCHSD);
    Connect(11402, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonCHDD);
    Connect(11403, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonCHTD);
    Connect(11351, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonSTART);
    Connect(11352, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonSTOP);
    Connect(11359, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonCLEAR);
    Connect(11355, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonDBHANDLING);
    Connect(11356, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonGENTFD);
    Connect(11371, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonProgramminfo);
    Connect(11399, wxEVT_COMMAND_RADIOBOX_SELECTED, WXOEF & CminerDlg::RadioboxLAN);
    Connect(12302, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonFINDDUPS);
    Connect(12303, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonHOCFILES);
    Connect(12331, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonAVIparse);
    Connect(12326, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonDKTparse);
    Connect(12325, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonGIFparse);
    Connect(12323, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonHTMparse);
    Connect(12338, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonISMparse);
    Connect(12324, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonJPGparse);
    Connect(12321, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonMETparse);
    Connect(12335, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonMP3parse);
    Connect(12332, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonMPGparse);
    Connect(12337, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonPRTparse);
    Connect(12329, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonSTDparse);
    Connect(12336, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonWAVparse);
    Connect(12330, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonWD3parse);
    Connect(12333, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonWMFparse);
    Connect(12334, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonWMVparse);
    Connect(20001, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxMAD);
    Connect(20002, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxRED);
    Connect(20004, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxDED);
    Connect(20005, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxLOG);
    Connect(20201, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxMETcreateISM);
    Connect(20202, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxSFT);
    Connect(20203, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxMETcreatePRT);
    Connect(20204, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxMETcleanuTYP);
    Connect(20205, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxMETrepairPRT);
    Connect(20207, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxMETimportMD4);
    Connect(20208, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::ISMclearSHDB);
    Connect(23381, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxISMimportMD4);
    Connect(23382, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::ISMclearSHDB);
    Connect(23383, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::ISMexportMD4);
    Connect(23384, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxISMdeleteDUP);
    Connect(20211, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxPRTcreateISM);
    Connect(20212, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxPRTcreateOVN);
    Connect(20213, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxPRTsearchMET);
    Connect(20401, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxAHF);
    Connect(20405, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxVIDexportMD4);
    Connect(20406, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxVIDimportMD4);
    Connect(21101, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxSTDcreateINF);
    Connect(21102, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxSTDcreateMET);
    Connect(20409, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxDHP);
    Connect(20410, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxSHP);
    Connect(20411, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxDEBUG);
    Connect(20501, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxCreateDBentry);
    Connect(20511, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::ClearDatabaseDUPLI);
    Connect(20512, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::ImportDatabaseTXT);
    Connect(20513, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::ExportDatabaseTXT);
    Connect(20515, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::ED2KhashMD4);
    Connect(20516, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckCOKEY);
    Connect(20601, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxCPH);
    Connect(20602, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxCPJ);
    Connect(20603, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxDPY);
    Connect(20604, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxHLK);
    Connect(20605, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxAHH);
    Connect(20701, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxJPGexportMD4);
    Connect(20702, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxJPGwrbackMD4);
    Connect(20703, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxJPGimportMD4);
    Connect(20704, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxJPGaddRStoFN);
    Connect(20705, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxJPGaddFNtoTA);
    Connect(20706, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxJPGcreatePDS);
    Connect(20801, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxGIFexportMD4);
    Connect(20802, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxGIFwrbackMD4);
    Connect(20803, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxGIFimportMD4);
    Connect(20901, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxDKTrenameMD4);
    Connect(21001, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxWD3deleteALL);
    Connect(11331, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxAVIparse);
    Connect(11328, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxDKTparse);
    Connect(11327, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxGIFparse);
    Connect(11325, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxHTMparse);
    Connect(11326, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxJPGparse);
    Connect(11335, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxMP3parse);
    Connect(11332, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxMPGparse);
    Connect(11329, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxSTDparse);
    Connect(11336, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxWAVparse);
    Connect(11330, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxWD3parse);
    Connect(11333, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxWMFparse);
    Connect(11334, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxWMVparse);
    Connect(11451, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonMATCHDOMAINclr);
    Connect(11452, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonMATCHURLclr);
    Connect(11453, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonMATCHSTARTclr);
#ifndef WX1
    Connect(11312, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxMD4);
    Connect(12311, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonMD4HASHS);
    Connect(11316, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxAPR);
    Connect(11317, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxAPC);
    Connect(12901, wxEVT_COMMAND_BUTTON_CLICKED, WXOEF & CminerDlg::ButtonRENAME);
    Connect(11901, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxDFI);
    Connect(11902, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxGST);
    Connect(11903, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxFOO);
    Connect(11904, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxWSF);
    Connect(11905, wxEVT_COMMAND_CHECKBOX_CLICKED, WXOEF & CminerDlg::CheckboxIGX);
    Connect(20102, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxFCK);
    Connect(20301, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxAHD);
    Connect(20302, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::CheckboxMFL);
#endif
#ifdef WITHGDBM
    Connect(20514, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::ImportDBMAIN);
#endif
    //Connect(20521, wxEVT_COMMAND_MENU_SELECTED, WXOEF & CminerDlg::ImportJpgFile);
    for (i = 0 ; i < 1024 ; i++)
    {
        toblock[i] = NULL;
    }
    toblockAnz = 0;
    strcpy(buffer, "toblock.txt");
    if (!stat(buffer, statbuffer))
    {
        int bytecount, p, b;
        FILE * fp = fopen(buffer, "r");
        p = 0;
        bytecount = 0;
        while (!feof(fp))
        {
            b = fgetc(fp);
            if (b >= 0)
            {
                bytecount++;
                switch (b)
                {
                case 10:
                case 13:
                    if (p > 0)
                    {
                        p = - p;
                    }
                    break;
                case 9:
                case 32:
                    if (p)
                    {
                        buffer[p++] = b;
                    }
                    break;
                default:
                    buffer[p++] = b;
                    break;
                }
                if (p > 0)
                {
                    if (bytecount >= statbuffer -> st_size)
                    {
                        p = - p;
                    }
                }
                if (p < 0)
                {
                    p = - p;
                    buffer[p] = 0;
                    if (toblockAnz < 1024)
                    {
                        toblockAnz++;
                        toblock[toblockAnz] = (char *) malloc(strlen(buffer) + 1);
                        strcpy(toblock[toblockAnz], buffer);
                    }
                    p = 0;
                    buffer[0] = 0;
                }
            }
        }
        fclose(fp);
    }
    strcpy(buffer, "hardlock.txt");
    if (!stat(buffer, statbuffer))
    {
        int bytecount, p, b;
        FILE * fp = fopen(buffer, "r");
        p = 0;
        bytecount = 0;
        while (!feof(fp))
        {
            b = fgetc(fp);
            if (b >= 0)
            {
                bytecount++;
                switch (b)
                {
                case 10:
                case 13:
                    if (p > 0)
                    {
                        p = - p;
                    }
                    break;
                case 9:
                case 32:
                    if (p)
                    {
                        buffer[p++] = b;
                    }
                    break;
                default:
                    buffer[p++] = b;
                    break;
                }
                if (p > 0)
                {
                    if (bytecount >= statbuffer -> st_size)
                    {
                        p = - p;
                    }
                }
                if (p < 0)
                {
                    p = - p;
                    buffer[p] = 0;
                    if (hardlockAnz < 1024)
                    {
                        char * p2;
                        hardlockAnz++;
                        p2 = strchr(buffer, ':');
                        if (!p2)
                        {
                            p = strlen(buffer);
                            while (p > 0)
                            {
                                p--;
                                switch (buffer[p])
                                {
                                case 9:
                                case 10:
                                case 13:
                                case 32:
                                    buffer[p] = 0;
                                    break;
                                default:
                                    p = - p;
                                    break;
                                }
                            }
                            hardlockDomain[hardlockAnz] = (char *) malloc(strlen(buffer) + 1);
                            strcpy(hardlockDomain[hardlockAnz], buffer);
                            hardlock[hardlockAnz] = (char *) malloc(1);
                            hardlock[hardlockAnz][0] = 0;
                        }
                        else
                        {
                            p2[0] = 0;
                            p = strlen(buffer);
                            while (p > 0)
                            {
                                p--;
                                switch (buffer[p])
                                {
                                case 9:
                                case 10:
                                case 13:
                                case 32:
                                    buffer[p] = 0;
                                    break;
                                default:
                                    p = - p;
                                    break;
                                }
                            }
                            p2++;
                            hardlockDomain[hardlockAnz] = (char *) malloc(strlen(buffer) + 1);
                            strcpy(hardlockDomain[hardlockAnz], buffer);
                            p = strlen(p2);
                            while (p > 0)
                            {
                                p--;
                                switch (p2[p])
                                {
                                case 9:
                                case 10:
                                case 13:
                                case 32:
                                    p2[p] = 0;
                                    break;
                                default:
                                    p = - p;
                                    break;
                                }
                            }
                            hardlock[hardlockAnz] = (char *) malloc(strlen(p2) + 1);
                            strcpy(hardlock[hardlockAnz], p2);
                        }
                        printf("hardlock(Domain:#%s# Inline:#%s#)\n"
                        , hardlockDomain[hardlockAnz], hardlock[hardlockAnz]);
                    }
                    p = 0;
                    buffer[0] = 0;
                }
            }
        }
        fclose(fp);
    }
}

CminerDlg::~ CminerDlg()
{
    free(filename);
    free(filenameTMP);
    free(filepathTMP);
    int i = 0;
    while (i < toblockAnz)
    {
        i++;
        free(toblock[i]);
        toblock[i] = NULL;
    }
    i = 0;
    while (i < hardlockAnz)
    {
        i++;
        free(hardlock[i]);
        hardlock[i] = NULL;
        if (hardlockDomain[i])
        {
            free(hardlockDomain[i]);
            hardlockDomain[i] = NULL;
        }
    }
    free(urllast);
    free(urlnext);
    free(domainlast);
    free(domainnext);
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
    delete[] sourcename_last;
    delete[] hostname;
    delete[] localip;
    delete[] storageid;
    delete hash;
    if (dbPIdata.ptr)
    {
        free(dbPIdata.ptr);
    }
    free(dbPIkey.ptr);
    if (dbSHdata.ptr)
    {
        free(dbSHdata.ptr);
    }
    free(dbSHkey.ptr);
    if (dbSMdata.ptr)
    {
        free(dbSMdata.ptr);
    }
    free(dbSMkey.ptr);
    if (dbTMPdata.ptr)
    {
        free(dbTMPdata.ptr);
    }
    free(dbTMPkey.ptr);
    if (dbTXdata.ptr)
    {
        free(dbTXdata.ptr);
    }
    free(dbTXkey.ptr);
    if (dbURdata.ptr)
    {
        free(dbURdata.ptr);
    }
    free(dbURkey.ptr);
    if (dbVIdata.ptr)
    {
        free(dbVIdata.ptr);
    }
    free(dbVIkey.ptr);
    free(DATABASEp);
    free(TARGETDIR);
}

void CminerDlg::lan00German()
{
    button_FINDDUPS -> SetLabel("Duplikate suchen und markieren");
    button_HOCFILES -> SetLabel("Verarbeitung überprüfter Dateien");
    button_CHDD -> SetLabel("Zielverzeichnis");
    button_CHSD -> SetLabel("Zu analysierendes Verzeichnis");
    button_CHTD -> SetLabel("Datenbank-Verzeichnis");
    button_DBHANDLING -> SetLabel("Datenbank-Verwaltung");
    button_START -> SetLabel("Analyse starten");
    button_GENTFD -> SetLabel("Generiere Baumstruktur (picdb)");
    button_STOP -> SetLabel("Analyse abbrechen");
    button_CLEAR -> SetLabel("Fensterinhalt loeschen");
    button_PARSE -> SetLabel("Fensterinhalt parsen");
    button_PINFO -> SetLabel("Programminfo");
    button_AVIparse -> SetLabel("AVI parsen");
    button_DKTparse -> SetLabel("DKT parsen");
    button_GIFparse -> SetLabel("GIF parsen");
    button_HTMparse -> SetLabel("HTM parsen");
    button_ISMparse -> SetLabel("ISM parsen");
    button_JPGparse -> SetLabel("JPG parsen");
    button_METparse -> SetLabel("MET parsen");
    button_MP3parse -> SetLabel("MP3 parsen");
    button_MPGparse -> SetLabel("MPG parsen");
    button_PRTparse -> SetLabel("PART parsen");
    button_STDparse -> SetLabel("STD parsen");
    button_WAVparse -> SetLabel("WAV parsen");
    button_WD3parse -> SetLabel("WD3 parsen");
    button_WMFparse -> SetLabel("WMF parsen");
    button_WMVparse -> SetLabel("WMV parsen");
    smPMfinddups -> SetTitle(_T("Duplikate suchen und markieren"));
    smPMfinddups -> SetLabel(20001, "Duplikate markieren (.dup beim Duplikat-Dateinamen anhaengen)");
    smPMfinddups -> SetLabel(20002, "<.dup>-Markierungen wieder entfernen");
    smPMfinddups -> SetLabel(20004, "Duplikate loeschen (VORSICHT!!!, besser ^markieren^ verwenden!)");
    smPMfinddups -> SetLabel(20005, "Gefundene Duplikate in Logfile eintragen <logfile.txt>");
    smPMdbhandling -> SetTitle("Datenbank-Verwaltung");
    smPMdbhandling -> SetLabel(20501, "Neue Dateiinformationen in der Datenbank ablegen (fuer nachfolgende Analysen)");
    smPMdbhandling -> SetLabel(20511, "Datenbankinhalt loeschen (zurücksetzen)");
    smPMdbhandling -> SetLabel(20512, "Dateiinformationen importieren aus 'database.txt'");
    smPMdbhandling -> SetLabel(20513, "Dateiinformationen exportieren nach 'database.txt'");
    smPMdbhandling -> SetLabel(20515, "ed2k-kompatibler MD4-Hash");
    smPMdbhandling -> SetLabel(20516, "Verwende Crossoverkey in der Datenbank");
    //smPMdbhandling ->SetLabel(20521, "JPG-File in Datenbank einbetten, Filename wird zum Key");
    smPMmetfiles -> SetTitle(_T("Metfile(ed2k)-Verarbeitung"));
    smPMmetfiles -> SetLabel(20201, "'.ism'-Infodateien generieren");
    smPMmetfiles -> SetLabel(20202, "nach Dateitypen trennen");
    smPMmetfiles -> SetLabel(20203, "Partfiles generieren, falls fehlend");
    smPMhocfiles -> SetTitle(_T("Verarbeitung geprüfter Dateien"));
    smPMhocfiles -> SetLabel(20401, "MD4-Hash in allen Dateinamen einbetten");
    smPMhocfiles -> SetLabel(20405, "Videodateien exportieren");
    smPMhocfiles -> SetLabel(20406, "Videodateien importieren");
    smPMstdfiles -> SetLabel(21101, "ISM-Dateien generieren");
    smPMstdfiles -> SetLabel(21102, "MET-Dateien generieren");
    smPMjpgfiles -> SetLabel(20701, "JPG-Dateien exportieren");
    smPMjpgfiles -> SetLabel(20702, "MD4-Hash schreiben");
    smPMjpgfiles -> SetLabel(20703, "JPG-Dateien importieren");
    smPMgiffiles -> SetLabel(20801, "GIF-Dateien exportieren");
    smPMgiffiles -> SetLabel(20802, "MD4-Hash schreiben");
    smPMgiffiles -> SetLabel(20803, "GIF-Dateien importieren");
    smPMhtmfiles -> SetLabel(20601, "'htmpage?.htm'-Dateien erstellen für htm-Links");
    smPMhtmfiles -> SetLabel(20602, "'jpgpage?.htm'-Dateien erstellen für jpg-Links");
    smPMhtmfiles -> SetLabel(20603, "Primary-Files löschen");
    smPMhtmfiles -> SetLabel(20604, "'hardlock.txt'-Datei verwenden");
    smPMhtmfiles -> SetLabel(20605, "MD4-Hash in html-Dateinamen einbetten");
    smPMdktfiles -> SetLabel(20901, "DKT-Dateien umbenennen");
    smPMwd3files -> SetLabel(21001, "WD3-Dateien löschen");
}

void CminerDlg::lan01English()
{
    button_FINDDUPS -> SetLabel("Find and markup duplicates");
    button_HOCFILES -> SetLabel("Processing checked files");
    button_CHDD -> SetLabel("Targetdirectory");
    button_CHSD -> SetLabel("Directory for analyzing");
    button_CHTD -> SetLabel("Databasedirectory");
    button_DBHANDLING -> SetLabel("Database-Management");
    button_START -> SetLabel("START");
    button_GENTFD -> SetLabel("Generate tree from picdb");
    button_STOP -> SetLabel("Cancel");
    button_CLEAR -> SetLabel("Clear window");
    button_PARSE -> SetLabel("Parse window");
    button_PINFO -> SetLabel("Info");
    button_AVIparse -> SetLabel("Parse AVI");
    button_DKTparse -> SetLabel("Parse DKT");
    button_GIFparse -> SetLabel("Parse GIF");
    button_HTMparse -> SetLabel("Parse HTM");
    button_ISMparse -> SetLabel("Parse ISM");
    button_JPGparse -> SetLabel("Parse JPG");
    button_METparse -> SetLabel("Parse MET");
    button_MP3parse -> SetLabel("Parse MP3");
    button_MPGparse -> SetLabel("Parse MPG");
    button_PRTparse -> SetLabel("Parse PART");
    button_STDparse -> SetLabel("Parse STD");
    button_WAVparse -> SetLabel("Parse WAV");
    button_WD3parse -> SetLabel("Parse WD3");
    button_WMFparse -> SetLabel("Parse WMF");
    button_WMVparse -> SetLabel("Parse WMV");
    smPMfinddups -> SetTitle(_T("Find and markup duplicates"));
    smPMfinddups -> SetLabel(20001, "Markup duplicates (appending <.dup>-extension to filename (renaming)");
    smPMfinddups -> SetLabel(20002, "Remove <.dup>-extension from filenames (renaming)");
    smPMfinddups -> SetLabel(20004, "Delete duplicates (CAUTION!!) (better to use <markup duplicates>)");
    smPMfinddups -> SetLabel(20005, "Logging of duplicates to <logfile.txt>");
    smPMdbhandling -> SetTitle("Database-Management");
    smPMdbhandling -> SetLabel(20501, "Store fileinfos (for further analyzing)");
    smPMdbhandling -> SetLabel(20511, "Clear database");
    smPMdbhandling -> SetLabel(20512, "Import fileinfos to database from 'database.txt'");
    smPMdbhandling -> SetLabel(20513, "Export fileinfos from database to 'database.txt'");
    smPMdbhandling -> SetLabel(20515, "ed2k-compatible MD4-hash");
    smPMdbhandling -> SetLabel(20516, "Use crossoverkey for database");
    //smPMdbhandling ->SetLabel(20521, "Store jpg-file, key=filename");
    smPMdktfiles -> SetLabel(20901, "Rename DKT-files");
    smPMgiffiles -> SetLabel(20801, "Export GIF-files");
    smPMgiffiles -> SetLabel(20802, "Write MD4-hash");
    smPMgiffiles -> SetLabel(20803, "Import GIF-files");
    smPMhocfiles -> SetTitle(_T("Handling of checked files"));
    smPMhocfiles -> SetLabel(20401, "Add MD4 to all filenames");
    smPMhocfiles -> SetLabel(20405, "Export videofiles (avi,mpg,wmf,wmv)");
    smPMhocfiles -> SetLabel(20406, "Import videofiles (avi,mpg,wmf,wmv)");
    smPMstdfiles -> SetLabel(21101, "create ISM-files");
    smPMstdfiles -> SetLabel(21102, "create MET-files");
    smPMhtmfiles -> SetLabel(20601, "Create 'htmpage?.htm'-files for htm-links");
    smPMhtmfiles -> SetLabel(20602, "Create 'jpgpage?.htm'-files for jpg-links");
    smPMhtmfiles -> SetLabel(20603, "Delete Primary-files");
    smPMhtmfiles -> SetLabel(20604, "Use 'hardlock.txt'-file for blocking");
    smPMhtmfiles -> SetLabel(20605, "Add MD4 to html-filenames");
    smPMjpgfiles -> SetLabel(20701, "Export JPG-files");
    smPMjpgfiles -> SetLabel(20702, "Write MD4-hash");
    smPMjpgfiles -> SetLabel(20703, "Import JPG-files");
    smPMmetfiles -> SetTitle(_T("Metfile-Processing"));
    smPMmetfiles -> SetLabel(20201, "Create '.ism'-infofiles");
    smPMmetfiles -> SetLabel(20202, "Separate filetypes");
    smPMmetfiles -> SetLabel(20203, "Create partfiles, if missing");
    smPMwd3files -> SetLabel(21001, "Delete WD3-files");
}

void CminerDlg::ButtonPARSE(wxCommandEvent & event)
{
    char * txt;
    char zeile[256];
    int l, p, z, zp;
    tctrl_ARC -> AppendText("\n");
    l = strlen(tctrl_ARC -> GetValue() .c_str());
    txt = (char *) malloc(l + 1);
    memset(txt, 0, l + 1);
    strncpy(txt, tctrl_ARC -> GetValue() .c_str(), l);
    p = 0;
    z = 0;
    zp = 0;
    memset(zeile, 0, 256);
    while (p < l)
    {
        switch (txt[p])
        {
        case 10:
        case 13:
            z++;
            if (p >= (l - 1))
            {
                if (strlen(zeile) > 0)
                {
                    switch (zeile[0])
                    {
                    case ':':
                        switch (zeile[1])
                        {
                        case 'c':
                            {
                                tctrl_ARC -> Clear();
                            }
                            break;
                        case 'i':
                            {
                                dbPI = new CDatabase(1, 16, DATABASEp, "pics.DBIDX");
                                int l;
                                char hash1[33], hash2[33];
                                memset(hash1, 0, 33);
                                memset(hash2, 0, 33);
                                l = strlen(zeile) - 3;
                                if (l > 32)
                                {
                                    l = 32;
                                }
                                else if(l < 0)
                                {
                                    l = 0;
                                }
                                if (l)
                                {
                                    strncpy(hash1, zeile + 3, l);
                                }
                                from32to16(hash1, dbPIkey.ptr);
                                dbPIkey.size = 16;
                                dbPIdata = dbPI -> fetchKey(1, dbPIkey);
                                zp = 0;
                                do
                                {
                                    zp++;
                                    if (!dbPIdata.ptr)
                                    {
                                        dbPIdata = dbPI -> getNextKey(1, dbPIkey);
                                    }
                                    if (dbPIdata.ptr)
                                    {
                                        for (int i = 0 ; i < 16 ; i++)
                                        {
                                            sprintf(hash2 + i * 2, "%02x", dbPIkey.ptr[i]);
                                        }
                                        if (!strncmp(hash1, hash2, l))
                                        {
                                            sprintf(buffer, ">%s: %s\n"
                                            , hash2, (char *) dbPIdata.ptr);
                                            W(buffer);
                                            if (zp > 9)
                                            {
                                                zp = 0;
                                            }
                                        }
                                        else
                                        {
                                            zp = 0;
                                        }
                                        free(dbPIdata.ptr);
                                        dbPIdata.ptr = NULL;
                                    }
                                }
                                while (zp);
                                delete dbPI;
                                dbPI = NULL;
                            }
                            break;
                        case 'u':
                            {
                                int p, l;
                                dbUR = new CDatabase(1, 32, DATABASEp, "urls.DBIDX");
                                strcpy(domainlast, zeile + 3);
                                strcpy(zeile, "");
                                l = strlen(domainlast);
                                p = 0;
                                while (p < l)
                                {
                                    switch (domainlast[p])
                                    {
                                    case ':':
                                        domainlast[p++] = 0;
                                        strcpy(zeile, domainlast + p);
                                        l = - l;
                                        sprintf(buffer, ">c:%s\n", zeile);
                                        tctrl_ARC -> AppendText(buffer);
                                        break;
                                    default:
                                        p++;
                                        break;
                                    }
                                }
                                hash -> hashBuffer((unsigned char *) domainlast, strlen(domainlast), p_ED2KhashMD4);
                                memset(dbURkey.ptr, 0, 33);
                                memcpy(dbURkey.ptr, hash -> blockhash, 16);
                                dbURdata = dbUR -> fetchKey(1, dbURkey);
                                l = 0;
                                do
                                {
                                    if (dbURdata.ptr)
                                    {
                                        memcpy( & l, dbURdata.ptr, 4);
                                        if (l & 1)
                                        {
                                            if (strlen(zeile))
                                            {
                                                l = atoi(zeile) | 1;
                                                memcpy(dbURdata.ptr, & l, 4);
                                                (void) dbUR -> updateData(1, dbURkey, dbURdata);
                                            }
                                        }
                                        sprintf(buffer, ">%08x: %s\n", l, dbURdata.ptr + 4);
                                        tctrl_ARC -> AppendText(buffer);
                                        free(dbURdata.ptr);
                                        if (strlen(zeile))
                                        {
                                            dbURdata.ptr = NULL;
                                        }
                                        else
                                        {
                                            dbURdata = dbUR -> getNextKey(1, dbURkey);
                                            if (memcmp(dbURkey.ptr, hash -> blockhash, 16))
                                            {
                                                free(dbURdata.ptr);
                                                dbURdata.ptr = NULL;
                                            }
                                        }
                                    }
                                }
                                while (dbURdata.ptr);
                                delete dbUR;
                                dbUR = NULL;
                            }
                            break;
                        default:
                            printf("unknown: _%s_\n", zeile);
                            break;
                        }
                        break;
                    default:
                        printf("input: _%s_\n", zeile);
                        break;
                    }
                }
                else
                {
                    switch (sprache)
                    {
                    case 0:
                        {
                            W(":c           - Eingabefensterinhalt loeschen\n");
                            W(":i:[md4hash] - Bildinformation zu Bild mit Hash [md4hash] anzeigen\n");
                            W(":u:[domain]  - Verweise auf diese Domain anzeigen\n");
                        }
                        break;
                    case 1:
                        {
                            W(":c           - Clear inputwindow\n");
                            W(":i:[md4hash] - Show pictureinfo from database for picturehash [md4hash]\n");
                            W(":u:[domain]  - Show referencies to this domain\n");
                        }
                        break;
                    }
                }
            }
            zp = 0;
            memset(zeile, 0, 256);
            break;
        default:
            if (zp < 255)
            {
                zeile[zp++] = txt[p];
            }
            break;
        }
        p++;
    }
    free(txt);
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

void CminerDlg::ButtonMATCHDOMAINclr(wxCommandEvent & event)
{
    tctrl_MATCHDOMAIN -> Clear();
}

void CminerDlg::ButtonMATCHURLclr(wxCommandEvent & event)
{
    tctrl_MATCHURL -> Clear();
}

void CminerDlg::ButtonMATCHSTARTclr(wxCommandEvent & event)
{
    tctrl_MATCHSTART -> Clear();
}

void CminerDlg::ButtonCHSD(wxCommandEvent & event)
{
    if (!threadcount)
    {
        wxDirDialog * fdSD = new wxDirDialog(this, "Bitte ein Quellverzeichnis auswaehlen fuer die rekursiven Analysen: ", theApp -> sourcedirectory);
        if (fdSD -> ShowModal() == wxID_OK)
        {
            theApp -> sourcedirectory = fdSD -> GetPath();
            char * sd = (char *) theApp -> sourcedirectory.c_str();
            for (unsigned int i = 0 ; i < strlen(sd) ; i++)
            {
                if (sd[i] == '\\')
                {
                    sd[i] = '/';
                }
            }
            stext_CHSD -> SetLabel(theApp -> sourcedirectory);
        }
        delete fdSD;
    }
    else
    {
        Message(101, "");
    }
}

void CminerDlg::ButtonCHDD(wxCommandEvent & event)
{
    if (!threadcount)
    {
        wxDirDialog * fdDD = new wxDirDialog(this
        , "Bitte ein Zielverzeichnis fuer exportierte Daten auswaehlen: ", _T(TARGETDIR));
        if (fdDD -> ShowModal() == wxID_OK)
        {
            strcpy(TARGETDIR, fdDD -> GetPath() .c_str());
            for (unsigned int i = 0 ; i < strlen(TARGETDIR) ; i++)
            {
                if (TARGETDIR[i] == '\\')
                {
                    TARGETDIR[i] = '/';
                }
            }
            stext_CHDD -> SetLabel(TARGETDIR);
        }
        delete fdDD;
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
            char * td = (char *) theApp -> targetdirectory.c_str();
            for (unsigned int i = 0 ; i < strlen(td) ; i++)
            {
                if (td[i] == '\\')
                {
                    td[i] = '/';
                }
            }
            strcpy(DATABASEp, theApp -> targetdirectory.c_str());
            stext_CHTD -> SetLabel(DATABASEp);
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
            W(_T("... Verarbeitung laeuft, bitte warten!\n"));
            break;
        case 92:
            W(_T("... Verarbeitung beendet.\n"));
            break;
        case 93:
            W("Analyse vorzeitig abgebrochen!\n");
            break;
        case 94:
            W("... Analyse nicht aktiv, deshalb gibts auch nix zu stoppen!\n");
            break;
        case 95:
            W("Analyse gestartet.\n");
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
            W(_T("... processing, please wait ...\n"));
            break;
        case 92:
            W(_T("... processing finished:\n"));
            break;
        case 93:
            W("... analyzing canceled.\n");
            break;
        case 94:
            W("... analyzing not active.\n");
            break;
        case 95:
            tctrl_ARC -> AppendText("Thread started.\n");
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

void CminerDlg::ClearDatabaseDUPLI()
{
    if (!threadcount)
    {
        CDatabase * db = new CDatabase(1, 16, DATABASEp, "dupli.DBIDX");
        db -> clearDatabase();
        delete db;
        Message(1, "dupli.DBIDX");
    }
    else
    {
        Message(101, "");
    }
}

void CminerDlg::ISMclearSHDB(wxCommandEvent & event)
{
    if (!threadcount)
    {
        dbSH = new CDatabase(1, 32, DATABASEp, "md4s.DBIDX");
        if (dbSH)
        {
            dbSH -> clearDatabase();
            delete dbSH;
        }
        Message(1, "md4s.DBIDX");
    }
    else
    {
        Message(101, "");
    }
}

void CminerDlg::ISMexportMD4(wxCommandEvent & event)
{
    tctrl_ARC -> Clear();
    int counter = 0, counter2 = 0;
    int c = 0, i = 0, j = 0;
    if (!threadcount)
    {
        dbSH = new CDatabase(1, 32, DATABASEp, "md4s.DBIDX");
        if (dbSH)
        {
            memset(dbSHkey.ptr, 0, 33);
            sprintf(buffer, "%s/md4s.export", TARGETDIR);
            wxFile * expf = new wxFile(buffer, wxFile::write);
            do
            {
                if (dbSHdata.ptr)
                {
                    free(dbSHdata.ptr);
                    dbSHdata.size = 0;
                }
                dbSHdata = dbSH -> getNextKey(1, dbSHkey);
                if (dbSHdata.ptr)
                {
                    counter++;
                    if (dbSHdata.ptr[0] < '5')
                    {
                        counter2++;
                        if (! (counter2% 15))
                        {
                            tctrl_ARC -> Clear();
                        }
                        sprintf(buffer, "%d: %s\n", counter2, dbSHdata.ptr + 15);
                        tctrl_ARC -> AppendText(buffer);
                    }
                    i = 0;
                    buffer[i++] = 34;
                    for (j = 0 ; j < 16 ; j++)
                    {
                        sprintf(buffer + i, "%02x", dbSHkey.ptr[j]);
                        i += 2;
                    }
                    buffer[i++] = 34;
                    buffer[i++] = ';';
                    buffer[i++] = 34;
                    for (j = 0 ; j < 16 ; j++)
                    {
                        sprintf(buffer + i, "%02x", dbSHkey.ptr[16 + j]);
                        i += 2;
                    }
                    buffer[i++] = 34;
                    buffer[i++] = ';';
                    buffer[i++] = 34;
                    for (j = 0 ; j < dbSHdata.size ; j++)
                    {
                        c = dbSHdata.ptr[j];
                        switch (j)
                        {
                        case 1:
                            buffer[i++] = 34;
                            buffer[i++] = ';';
                            buffer[i++] = 34;
                            if (dbSHdata.ptr[0] < '5')
                            {
                                buffer[i++] = '0';
                                buffer[i++] = 34;
                                buffer[i++] = ';';
                                buffer[i++] = 34;
                            }
                            break;
                        case 14:
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
            while (dbSHdata.ptr);
            delete expf;
            delete dbSH;
            switch (sprache)
            {
            case 0:
                sprintf(buffer, "%d Datensaetze exportiert nach '%s/md4s.export'\n", counter, TARGETDIR);
                break;
            case 1:
                sprintf(buffer, "%d entries exported to '%s/md4s.export'\n", counter, TARGETDIR);
                break;
            }
            Message(0, buffer);
        }
    }
    else
    {
        Message(101, "");
    }
}

void CminerDlg::ED2KhashMD4(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_ED2KhashMD4 = !p_ED2KhashMD4;
    }
    else
    {
        Message(101, "");
    }
    smPMdbhandling -> Check(20515, p_ED2KhashMD4);
}

void CminerDlg::CheckCOKEY(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_useCOKEY = !p_useCOKEY;
    }
    else
    {
        Message(101, "");
    }
    smPMdbhandling -> Check(20516, p_useCOKEY);
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
                    memset(dbSMkey.ptr, 0, 16);
                    sprintf(buffer, "%s/database.txt", TARGETDIR);
                    wxFile * expf = new wxFile(buffer, wxFile::write);
                    do
                    {
                        if (tempData.ptr)
                        {
                            free(tempData.ptr);
                            tempData.size = 0;
                        }
                        tempData = dbSM -> getNextKey(1, dbSMkey);
                        if (tempData.ptr)
                        {
                            counter++;
                            int c, i = 0;
                            buffer[0] = 34;
                            for (int i = 0 ; i < 16 ; i++)
                            {
                                sprintf(buffer + 1 + i * 2, "%02x", dbSMkey.ptr[i]);
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
                        sprintf(buffer, "%d Datensaetze exportiert nach '%s/database.txt'\n"
                        , counter, TARGETDIR);
                        break;
                    case 1:
                        sprintf(buffer, "%d entries exported to '%s/database.txt'\n"
                        , counter, TARGETDIR);
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
                                                memcpy(dbSMkey.ptr, tempKey.ptr + 8, 8);
                                                memcpy(dbSMkey.ptr + 8, tempKey.ptr, 8);
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
                            dbSMdata = dbSMtmp -> fetchKey(1, dbSMkey);
                            if (dbSMdata.ptr)
                            {
                                //(new CpicFrame((wxFrame *)((smApp *) wxTheApp) -> smuledlg,
                                //(char *) dbSMdata.ptr, dbSMdata.size, 200, 540, (char *) tempKey.ptr)) -> Show();
                                free(dbSMdata.ptr);
                                dbSMdata.ptr = NULL;
                                dbSMdata.size = 0;
                            }
                            else
                            {
                                dbSMdata.ptr = (unsigned char *) databuffer;
                                dbSMdata.size = strlen(databuffer) + 1;
                                if (!dbSMtmp -> storeData(1, dbSMkey, dbSMdata))
                                {
                                    counter++;
                                    if ((counter% 1000) == 1)
                                    {
                                        W(".");
                                    }
                                }
                            }
                            dbSMdata.ptr = NULL;
                            dbSMdata.size = 0;
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
                                W(".");
                            }
                            memcpy(dbSMkey.ptr, tempKey.ptr + 8, 8);
                            memcpy(dbSMkey.ptr + 8, tempKey.ptr, 8);
                            (void) dbSM -> storeData(1, dbSMkey, tempData);
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
                tctrl_ARC -> AppendText(buffer);
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
                    tctrl_ARC -> AppendText(_T("  Datei existiert schon in der Datenbank.\n"));
                }
                dbSMdata = dbSMtmp -> fetchKey(1, tempKey);
                if (dbSMdata.ptr)
                {
                    free(dbSMdata.ptr);
                    dbSMdata.ptr = NULL;
                    dbSMdata.size = 0;
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
                tctrl_ARC -> AppendText(_T("error on gdbm_open\n"));
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
                            tctrl_ARC -> AppendText(_T("error after gdbm_fetch\n"));
                        }
                        else
                        {
                            from32to16(gdbmKey.dptr, dbSMkey.ptr);
                            dbSMdata = dbSM -> fetchKey(1, dbSMkey);
                            if ((counter% 1000) == 1)
                            {
                                tctrl_ARC -> AppendText(_T("."));
                            }
                            if (dbSMdata.ptr)
                            {
                                free(dbSMdata.ptr);
                            }
                            else
                            {
                                dbSMdata.ptr = (unsigned char *) gdbmData.dptr;
                                dbSMdata.size = gdbmData.dsize;
                                if (!dbSM -> storeData(1, dbSMkey, dbSMdata))
                                {
                                    //ok
                                }
                            }
                            free(gdbmData.dptr);
                            dbSMdata.ptr = NULL;
                            dbSMdata.size = 0;
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
    tctrl_ARC -> Clear();
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
        tctrl_ARC -> SetEditable(false);
    }
}

void CminerDlg::ButtonCLEAR(wxCommandEvent & event)
{
    tctrl_ARC -> Clear();
}

void CminerDlg::ButtonSTART(wxCommandEvent & event)
{
    if (!threadcount)
    {
        basisfunktion = 1;
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

void CminerDlg::ButtonGENTFD(wxCommandEvent & event)
{
    if (!threadcount)
    {
        basisfunktion = 2;
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
        PopupMenu(smPMdbhandling, 200, 40);
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

void CminerDlg::ButtonHOCFILES(wxCommandEvent & event)
{
    if (!threadcount)
    {
        PopupMenu(smPMhocfiles, 4, 104);
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

void CminerDlg::ButtonSTDparse(wxCommandEvent & event)
{
    if (p_STDparse)
    {
        if (!threadcount)
        {
            PopupMenu(smPMstdfiles, 32, 140);
        }
        else
        {
            Message(101, "");
        }
    }
}

void CminerDlg::ButtonHTMparse(wxCommandEvent & event)
{
    if (p_HTMparse)
    {
        if (!threadcount)
        {
            PopupMenu(smPMhtmfiles, 136, 140);
        }
        else
        {
            Message(101, "");
        }
    }
}

void CminerDlg::ButtonMETparse(wxCommandEvent & event)
{
    if (p_METparse)
    {
        if (!threadcount)
        {
            PopupMenu(smPMmetfiles, 292, 140);
        }
        else
        {
            Message(101, "");
        }
    }
}

void CminerDlg::ButtonPRTparse(wxCommandEvent & event)
{
    if (p_PRTparse)
    {
        if (!threadcount)
        {
            PopupMenu(smPMprtfiles, 300, 140);
        }
        else
        {
            Message(101, "");
        }
    }
}

void CminerDlg::ButtonISMparse(wxCommandEvent & event)
{
    if (p_ISMparse)
    {
        if (!threadcount)
        {
            PopupMenu(smPMismfiles, 428, 140);
        }
        else
        {
            Message(101, "");
        }
    }
}

void CminerDlg::ButtonWD3parse(wxCommandEvent & event)
{
    if (p_WD3parse)
    {
        if (!threadcount)
        {
            PopupMenu(smPMwd3files, 556, 140);
        }
        else
        {
            Message(101, "");
        }
    }
}

void CminerDlg::ButtonDKTparse(wxCommandEvent & event)
{
    if (p_DKTparse)
    {
        if (!threadcount)
        {
            PopupMenu(smPMdktfiles, 684, 140);
        }
        else
        {
            Message(101, "");
        }
    }
}

void CminerDlg::ButtonJPGparse(wxCommandEvent & event)
{
    if (p_JPGparse)
    {
        if (!threadcount)
        {
            PopupMenu(smPMjpgfiles, 32, 172);
        }
        else
        {
            Message(101, "");
        }
    }
}

void CminerDlg::ButtonGIFparse(wxCommandEvent & event)
{
    if (p_GIFparse)
    {
        if (!threadcount)
        {
            PopupMenu(smPMgiffiles, 136, 172);
        }
        else
        {
            Message(101, "");
        }
    }
}

void CminerDlg::ButtonAVIparse(wxCommandEvent & event)
{
    if (p_AVIparse)
    {
        if (!threadcount)
        {
            //PopupMenu(smPMavifiles, 32, 172);
        }
        else
        {
            Message(101, "");
        }
    }
}

void CminerDlg::ButtonMP3parse(wxCommandEvent & event)
{
    if (p_MP3parse)
    {
        if (!threadcount)
        {
            //PopupMenu(smPMavifiles, 32, 172);
        }
        else
        {
            Message(101, "");
        }
    }
}

void CminerDlg::ButtonWAVparse(wxCommandEvent & event)
{
    if (p_WAVparse)
    {
        if (!threadcount)
        {
            //PopupMenu(smPMavifiles, 32, 172);
        }
        else
        {
            Message(101, "");
        }
    }
}

void CminerDlg::ButtonMPGparse(wxCommandEvent & event)
{
    if (p_MPGparse)
    {
        if (!threadcount)
        {
            //PopupMenu(smPMmpgfiles, 32, 172);
        }
        else
        {
            Message(101, "");
        }
    }
}

void CminerDlg::ButtonWMFparse(wxCommandEvent & event)
{
    if (p_WMFparse)
    {
        if (!threadcount)
        {
            //PopupMenu(smPMwmffiles, 32, 172);
        }
        else
        {
            Message(101, "");
        }
    }
}

void CminerDlg::ButtonWMVparse(wxCommandEvent & event)
{
    if (p_WMVparse)
    {
        if (!threadcount)
        {
            //PopupMenu(smPMwmvfiles, 32, 172);
        }
        else
        {
            Message(101, "");
        }
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

void CminerDlg::CheckboxMETcreatePRT(wxCommandEvent & event)
{
    p_METcreatePRT = !p_METcreatePRT;
    smPMmetfiles -> Check(20203, p_METcreatePRT);
}

void CminerDlg::CheckboxMETrepairPRT(wxCommandEvent & event)
{
    p_METrepairPRT = !p_METrepairPRT;
    smPMmetfiles -> Check(20205, p_METrepairPRT);
}

void CminerDlg::CheckboxMETcleanuTYP(wxCommandEvent & event)
{
    p_METcleanuTYP = !p_METcleanuTYP;
    smPMmetfiles -> Check(20204, p_METcleanuTYP);
}

void CminerDlg::CheckboxDEBUG(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_debugonoff = !p_debugonoff;
    }
    else
    {
        Message(101, "");
    }
    smPMhocfiles -> Check(20411, p_debugonoff);
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

void CminerDlg::CheckboxMETparse(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_METparse = !p_METparse;
    }
    else
    {
        Message(101, "");
    }
    checkbox_METparse -> SetValue(p_METparse);
}

void CminerDlg::CheckboxISMparse(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_ISMparse = !p_ISMparse;
    }
    else
    {
        Message(101, "");
    }
    checkbox_ISMparse -> SetValue(p_ISMparse);
}

void CminerDlg::CheckboxPRTparse(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_PRTparse = !p_PRTparse;
    }
    else
    {
        Message(101, "");
    }
    checkbox_PRTparse -> SetValue(p_PRTparse);
}

void CminerDlg::CheckboxMETcreateISM(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_METcreateISM = !p_METcreateISM;
        if (p_METcreateISM)
        {
            p_METseparaTYP = false;
            smPMmetfiles -> Check(20202, p_METseparaTYP);
        }
    }
    else
    {
        Message(101, "");
    }
    smPMmetfiles -> Check(20201, p_METcreateISM);
}

void CminerDlg::CheckboxISMimportMD4(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_ISMimportMD4 = !p_ISMimportMD4;
    }
    else
    {
        Message(101, "");
    }
    smPMismfiles -> Check(23381, p_ISMimportMD4);
}

void CminerDlg::CheckboxISMdeleteDUP(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_ISMdeleteDUP = !p_ISMdeleteDUP;
    }
    else
    {
        Message(101, "");
    }
    smPMismfiles -> Check(23384, p_ISMdeleteDUP);
}

void CminerDlg::CheckboxMETimportMD4(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_METimportMD4 = !p_METimportMD4;
    }
    else
    {
        Message(101, "");
    }
    smPMmetfiles -> Check(20207, p_METimportMD4);
}

void CminerDlg::CheckboxCPH(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_crePageForHTM = !p_crePageForHTM;
    }
    else
    {
        Message(101, "");
    }
    smPMhtmfiles -> Check(20601, p_crePageForHTM);
}

void CminerDlg::CheckboxCPJ(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_crePageForJPG = !p_crePageForJPG;
    }
    else
    {
        Message(101, "");
    }
    smPMhtmfiles -> Check(20602, p_crePageForJPG);
}

void CminerDlg::CheckboxPRTcreateISM(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_PRTcreateISM = !p_PRTcreateISM;
    }
    else
    {
        Message(101, "");
    }
    smPMprtfiles -> Check(20211, p_PRTcreateISM);
}

void CminerDlg::CheckboxPRTcreateOVN(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_PRTcreateOVN = !p_PRTcreateOVN;
    }
    else
    {
        Message(101, "");
    }
    smPMprtfiles -> Check(20212, p_PRTcreateOVN);
}

void CminerDlg::CheckboxPRTsearchMET(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_PRTsearchMET = !p_PRTsearchMET;
    }
    else
    {
        Message(101, "");
    }
    smPMprtfiles -> Check(20213, p_PRTsearchMET);
}

void CminerDlg::CheckboxSTDcreateINF(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_STDcreateINF = !p_STDcreateINF;
    }
    else
    {
        Message(101, "");
    }
    smPMstdfiles -> Check(21101, p_STDcreateINF);
}

void CminerDlg::CheckboxSTDcreateMET(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_STDcreateMET = !p_STDcreateMET;
    }
    else
    {
        Message(101, "");
    }
    smPMstdfiles -> Check(21102, p_STDcreateMET);
}

void CminerDlg::CheckboxDHP(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_delHtmPxxCxx = !p_delHtmPxxCxx;
    }
    else
    {
        Message(101, "");
    }
    smPMhocfiles -> Check(20409, p_delHtmPxxCxx);
    if (p_delHtmPxxCxx)
    {
        p_simHtmPxxCxx = false;
        smPMhocfiles -> Check(20410, p_simHtmPxxCxx);
    }
}

void CminerDlg::CheckboxSHP(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_simHtmPxxCxx = !p_simHtmPxxCxx;
    }
    else
    {
        Message(101, "");
    }
    smPMhocfiles -> Check(20410, p_simHtmPxxCxx);
    if (p_simHtmPxxCxx)
    {
        p_delHtmPxxCxx = false;
        smPMhocfiles -> Check(20409, p_delHtmPxxCxx);
    }
}

void CminerDlg::CheckboxDFI(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_usingdirfile = !p_usingdirfile;
    }
    else
    {
        Message(101, "");
    }
    checkbox_DFI -> SetValue(p_usingdirfile);
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
            smPMfinddups -> Check(20004, p_deleteduples);
#ifndef WX1
            p_doMD4hashing = true;
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
            smPMfinddups -> Check(20004, p_deleteduples);
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
    smPMfinddups -> Check(20004, p_deleteduples);
}

void CminerDlg::CheckboxLOG(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_logduples = !p_logduples;
        smPMfinddups -> Check(20005, p_logduples);
    }
    else
    {
        Message(101, "");
        smPMfinddups -> Check(20005, p_logduples);
    }
}

void CminerDlg::CheckboxCreateDBentry(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_creDBentry = !p_creDBentry;
        smPMdbhandling -> Check(20501, p_creDBentry);
    }
    else
    {
        Message(101, "");
        smPMdbhandling -> Check(20501, p_creDBentry);
    }
}

void CminerDlg::CheckboxSFT(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_METseparaTYP = !p_METseparaTYP;
        if (p_METseparaTYP)
        {
#ifndef WX1
            p_doMD4hashing = false;
            checkbox_MD4 -> SetValue(p_doMD4hashing);
#endif
            p_METcreateISM = false;
            smPMmetfiles -> Check(20201, p_METcreateISM);
            p_METparse = true;
            checkbox_METparse -> SetValue(p_METparse);
            p_markupduples = false;
            smPMfinddups -> Check(20001, p_markupduples);
        }
    }
    else
    {
        Message(101, "");
    }
    smPMmetfiles -> Check(20202, p_METseparaTYP);
}

void CminerDlg::CheckboxJPGwrbackMD4(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_JPGwrbackMD4 = !p_JPGwrbackMD4;
    }
    else
    {
        Message(101, "");
    }
    smPMjpgfiles -> Check(20702, p_JPGwrbackMD4);
}

void CminerDlg::CheckboxGIFwrbackMD4(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_GIFwrbackMD4 = !p_GIFwrbackMD4;
    }
    else
    {
        Message(101, "");
    }
    smPMgiffiles -> Check(20802, p_GIFwrbackMD4);
}

void CminerDlg::CheckboxAHF(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_addMD4tofnam = !p_addMD4tofnam;
    }
    else
    {
        Message(101, "");
    }
    smPMhocfiles -> Check(20401, p_addMD4tofnam);
}

void CminerDlg::CheckboxAHH(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_HTMLaddMD4 = !p_HTMLaddMD4;
    }
    else
    {
        Message(101, "");
    }
    smPMhtmfiles -> Check(20605, p_HTMLaddMD4);
}

void CminerDlg::CheckboxVIDexportMD4(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_VIDexportMD4 = !p_VIDexportMD4;
    }
    else
    {
        Message(101, "");
    }
    smPMhocfiles -> Check(20405, p_VIDexportMD4);
    p_AVIexportMD4 = p_VIDexportMD4;
    p_MPGexportMD4 = p_VIDexportMD4;
    p_WMFexportMD4 = p_VIDexportMD4;
    p_WMVexportMD4 = p_VIDexportMD4;
}

void CminerDlg::CheckboxVIDimportMD4(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_VIDimportMD4 = !p_VIDimportMD4;
    }
    else
    {
        Message(101, "");
    }
    smPMhocfiles -> Check(20406, p_VIDimportMD4);
}

void CminerDlg::CheckboxJPGexportMD4(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_JPGexportMD4 = !p_JPGexportMD4;
    }
    else
    {
        Message(101, "");
    }
    smPMjpgfiles -> Check(20701, p_JPGexportMD4);
}

void CminerDlg::CheckboxJPGaddRStoFN(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_JPGaddRStoFN = !p_JPGaddRStoFN;
    }
    else
    {
        Message(101, "");
    }
    smPMjpgfiles -> Check(20704, p_JPGaddRStoFN);
}

void CminerDlg::CheckboxJPGaddFNtoTA(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_JPGaddFNtoTA = !p_JPGaddFNtoTA;
    }
    else
    {
        Message(101, "");
    }
    smPMjpgfiles -> Check(20705, p_JPGaddFNtoTA);
}

void CminerDlg::CheckboxJPGcreatePDS(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_JPGcreatePDS = !p_JPGcreatePDS;
    }
    else
    {
        Message(101, "");
    }
    smPMjpgfiles -> Check(20706, p_JPGcreatePDS);
}

void CminerDlg::CheckboxJPGimportMD4(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_JPGimportMD4 = !p_JPGimportMD4;
    }
    else
    {
        Message(101, "");
    }
    smPMjpgfiles -> Check(20703, p_JPGimportMD4);
}

void CminerDlg::CheckboxDKTrenameMD4(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_DKTrenameMD4 = !p_DKTrenameMD4;
    }
    else
    {
        Message(101, "");
    }
    smPMdktfiles -> Check(20901, p_DKTrenameMD4);
}

void CminerDlg::CheckboxGIFexportMD4(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_GIFexportMD4 = !p_GIFexportMD4;
    }
    else
    {
        Message(101, "");
    }
    smPMgiffiles -> Check(20801, p_GIFexportMD4);
}

void CminerDlg::CheckboxGIFimportMD4(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_GIFimportMD4 = !p_GIFimportMD4;
    }
    else
    {
        Message(101, "");
    }
    smPMgiffiles -> Check(20803, p_GIFimportMD4);
}

void CminerDlg::CheckboxHTMparse(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_HTMparse = !p_HTMparse;
    }
    else
    {
        Message(101, "");
    }
    checkbox_HTMparse -> SetValue(p_HTMparse);
}

void CminerDlg::CheckboxJPGparse(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_JPGparse = !p_JPGparse;
    }
    else
    {
        Message(101, "");
    }
    checkbox_JPGparse -> SetValue(p_JPGparse);
}

void CminerDlg::CheckboxGIFparse(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_GIFparse = !p_GIFparse;
    }
    else
    {
        Message(101, "");
    }
    checkbox_GIFparse -> SetValue(p_GIFparse);
}

void CminerDlg::CheckboxAVIparse(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_AVIparse = !p_AVIparse;
    }
    else
    {
        Message(101, "");
    }
    checkbox_AVIparse -> SetValue(p_AVIparse);
}

void CminerDlg::CheckboxMP3parse(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_MP3parse = !p_MP3parse;
    }
    else
    {
        Message(101, "");
    }
    checkbox_MP3parse -> SetValue(p_MP3parse);
}

void CminerDlg::CheckboxWAVparse(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_WAVparse = !p_WAVparse;
    }
    else
    {
        Message(101, "");
    }
    checkbox_WAVparse -> SetValue(p_WAVparse);
}

void CminerDlg::CheckboxMPGparse(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_MPGparse = !p_MPGparse;
    }
    else
    {
        Message(101, "");
    }
    checkbox_MPGparse -> SetValue(p_MPGparse);
}

void CminerDlg::CheckboxWMFparse(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_WMFparse = !p_WMFparse;
    }
    else
    {
        Message(101, "");
    }
    checkbox_WMFparse -> SetValue(p_WMFparse);
}

void CminerDlg::CheckboxWMVparse(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_WMVparse = !p_WMVparse;
    }
    else
    {
        Message(101, "");
    }
    checkbox_WMVparse -> SetValue(p_WMVparse);
}

void CminerDlg::CheckboxDKTparse(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_DKTparse = !p_DKTparse;
    }
    else
    {
        Message(101, "");
    }
    checkbox_DKTparse -> SetValue(p_DKTparse);
}

void CminerDlg::CheckboxSTDparse(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_STDparse = !p_STDparse;
    }
    else
    {
        Message(101, "");
    }
    checkbox_STDparse -> SetValue(p_STDparse);
}

void CminerDlg::CheckboxWD3parse(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_WD3parse = !p_WD3parse;
    }
    else
    {
        Message(101, "");
    }
    checkbox_WD3parse -> SetValue(p_WD3parse);
}

void CminerDlg::CheckboxWD3deleteALL(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_WD3deleteALL = !p_WD3deleteALL;
    }
    else
    {
        Message(101, "");
    }
    smPMhocfiles -> Check(21001, p_WD3deleteALL);
}

void CminerDlg::CheckboxDPY(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_delPRYfiles = !p_delPRYfiles;
    }
    else
    {
        Message(101, "");
    }
    smPMhtmfiles -> Check(20603, p_delPRYfiles);
}

void CminerDlg::CheckboxHLK(wxCommandEvent & event)
{
    if (!threadcount)
    {
        p_useHARDLOCK = !p_useHARDLOCK;
    }
    else
    {
        Message(101, "");
    }
    smPMhtmfiles -> Check(20604, p_useHARDLOCK);
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

void CminerDlg::CreatingFiletree()
{
    int i, l, countermatch, counterall;
    char hash0[33], path[1025];
    dbPI = new CDatabase(1, 16, DATABASEp, "pics.DBIDX");
    dbVI = new CDatabase(1, 16, DATABASEp, "vids.DBIDX");
    dbUR = new CDatabase(1, 32, DATABASEp, "urls.DBIDX");
    if (dbPI && dbVI && dbUR)
    {
        countermatch = 0;
        counterall = 0;
        memset(dbPIkey.ptr, 0, 16);
        do
        {
            dbPIdata = dbPI -> getNextKey(1, dbPIkey);
            if (dbPIdata.ptr)
            {
                counterall++;
                if (dbPIdata.size > 10)
                {
                    l = 10;
                    if (dbPIdata.ptr[l + 1] == ':')
                    {
                        l += 2;
                    }
                    memset(path, 0, 1025);
                    strcpy(path, theApp -> sourcedirectory .c_str());
                    memcpy(path + strlen(path), dbPIdata.ptr + l, dbPIdata.size - l);
                    memset(buffer, 0, BUFFERSIZE);
                    l = strlen(path);
                    if (lmd)
                    {
                        if (!strstr(path, matchdomain))
                        {
                            l = - l;
                        }
                    }
                    if (lmu)
                    {
                        if (!strstr(path, matchurl))
                        {
                            l = - l;
                        }
                    }
                    if (l > 0)
                    {
                        i = strlen(theApp -> sourcedirectory .c_str());
                        while (i < l)
                        {
                            if ((path[i] == '/') || (path[i] == '\\'))
                            {
                                strncpy(buffer, path, i);
                                if (l > 0)
                                {
                                    if (stat(buffer, statbuffer))
                                    {
                                        if (errno == ENOENT)
                                        {
#ifdef __WIN32__
                                            mkdir(buffer);
#else
                                            mkdir(buffer, 777);
#endif
                                        }
                                    }
                                }
                            }
                            i++;
                        }
                        for (i = 0 ; i < 16 ; i++)
                        {
                            sprintf(hash0 + i * 2, "%02x", dbPIkey.ptr [i]);
                        }
                        wxFile * pf = new wxFile(path, wxFile::write);
                        pf -> Write(hash0, 32);
                        delete pf;
                        path[96] = 0;
                        countermatch++;
                        sprintf(buffer, "(%d/%d) %s: %s\n", countermatch, counterall, hash0, path);
                        wxMutexGuiEnter();
                        if ((countermatch% 20) == 1)
                        {
                            tctrl_ARC -> Clear();
                        }
                        tctrl_ARC -> AppendText(buffer);
                        wxMutexGuiLeave();
                    }
                }
                free(dbPIdata.ptr);
                if (minerdlgThread -> TD())
                {
                    dbPIdata.ptr = NULL;
                }
            }
        }
        while (dbPIdata.ptr);
    }
    delete dbPI;
    dbPI = NULL;
    delete dbVI;
    dbVI = NULL;
    delete dbUR;
    dbUR = NULL;
}

int CminerDlg::AddingDirStart()
{
    int retcode = 0;
    fp_subdirs = NULL;
    logfileFP = NULL;
    htmp0 = NULL;
    dbPI = NULL;
    dbVI = NULL;
    dbUR = NULL;
    dbTMP = NULL;
    dbSM = NULL;
    dbSM = new CDatabase(1, 16, DATABASEp, "dupli.DBIDX");
    if (dbSM)
    {
        dbPI = new CDatabase(1, 16, DATABASEp, "pics.DBIDX");
        if (dbPI)
        {
            dbVI = new CDatabase(1, 16, DATABASEp, "vids.DBIDX");
            if (dbVI)
            {
                dbUR = new CDatabase(1, 32, DATABASEp, "urls.DBIDX");
                if (dbUR)
                {
                    dbSH = new CDatabase(1, 32, DATABASEp, "md4s.DBIDX");
                    if (dbSH)
                    {
                        dbTMP = new CDatabase(1, 16, DATABASEp, "temp.DBIDX");
                        if (dbTMP)
                        {
                            dbTMP -> clearDatabase();
                            fp_subdirs = fopen("subdirs.txt", "a");
                            sprintf((char *) buffer0, "%s/logfile.txt", DATABASEp);
                            logfileFP = fopen((char *) buffer0, "a");
                            htmp0 = new Chtmlpage("", 0);
                            for (int i = 0 ; i < 99 ; i++)
                            {
                                subdir[i] = NULL;
                                dirbuffer[i] = (char *) malloc(1);
                                dirbuffer[i][0] = 0;
                            }
                            dirbufferDeep = 0;
                            pds_count = 1;
                            pds_filecount = 1;
                            size_PDSfiles = 0;
                            size_ALLfiles = 0;
                            shp_fullsize = 0;
                            htmpagenumber = 1;
                            htmlinenumber = 0;
                            jpgpagenumber = 1;
                            jpglinenumber = 0;
                            lmil = 0;
                            mil = 0;
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
                            processcount = 0;
                            count_dupremoved = 0;
                            count_dupmarked = 0;
                            count_duplogged = 0;
                            count_fakmarked = 0;
                            count_deleted = 0;
                            count_filesmoved = 0;
                            count_subdirsprocessed = 0;
                            count_subdirsprocessed_last = 0;
                            count_ALLfiles = 0;
                            count_ALLfiles_last = 0;
                            count_DKTfilesprocessed = 0;
                            count_GIFfilesprocessed = 0;
                            count_HTMfilesprocessed = 0;
                            count_JPGfilesprocessed = 0;
                            count_METfilesprocessed = 0;
                            count_PRYfilesprocessed = 0;
                            count_STDfilesprocessed = 0;
                            count_WD3filesprocessed = 0;
                            guilinecount = 20;
                            Wgui("");
                            retcode = 1;
                        }
                    }
                }
            }
        }
    }
    return retcode;
}

void CminerDlg::AddingDirEnd()
{
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
    if (dbSH)
    {
        delete dbSH;
        dbSH = NULL;
    }
    if (dbSM)
    {
        delete dbSM;
        dbSMdata.ptr = NULL;
        dbSMdata.size = 0;
    }
    if (dbTMP)
    {
        delete dbTMP;
        dbTMP = NULL;
    }
    if (dbUR)
    {
        delete dbUR;
        dbUR = NULL;
    }
    if (dbVI)
    {
        delete dbVI;
        dbVI = NULL;
    }
    if (dbPI)
    {
        delete dbPI;
        dbPI = NULL;
    }
    if (htmp0)
    {
        delete htmp0;
        htmp0 = NULL;
    }
    if (fp_subdirs)
    {
        fclose(fp_subdirs);
        fp_subdirs = NULL;
    }
    if (logfileFP)
    {
        fclose(logfileFP);
        logfileFP = NULL;
    }
    for (int i = 0 ; i < 99 ; i++)
    {
        if (dirbuffer[i])
        {
            free(dirbuffer[i]);
            dirbuffer[i] = NULL;
        }
        if (subdir[i])
        {
            closedir(subdir[i]);
            subdir[i] = NULL;
        }
    }
}

void CminerDlg::AddingDir()
{
    if (stat(TARGETDIR, statbuffer))
    {
        if (errno == ENOENT)
        {
#ifdef __WIN32__
            (void) mkdir(TARGETDIR);
#else
            (void) mkdir(TARGETDIR, 777);
#endif
        }
    }
    if (AddingDirStart())
    {
        //w+r
        wxMutexGuiEnter();
        switch (sprache)
        {
        case 0:
            tctrl_ARC -> AppendText("Quelle: ");
            tctrl_ARC -> AppendText(theApp -> sourcedirectory.c_str());
            tctrl_ARC -> AppendText("\n");
            sprintf(buffer, "%s/dupli.DBIDX", DATABASEp);
            tctrl_ARC -> AppendText(_T("Datenbank: "));
            tctrl_ARC -> AppendText(_T(buffer));
            tctrl_ARC -> AppendText("\n");
            break;
        case 1:
            tctrl_ARC -> AppendText("Source: ");
            tctrl_ARC -> AppendText(theApp -> sourcedirectory.c_str());
            tctrl_ARC -> AppendText("\n");
            sprintf(buffer, "%s/dupli.DBIDX", DATABASEp);
            tctrl_ARC -> AppendText(_T("Database: "));
            tctrl_ARC -> AppendText(_T(buffer));
            tctrl_ARC -> AppendText("\n");
            break;
        }
        if (p_logduples)
        {
            tctrl_ARC -> AppendText(_T("Logfile: "));
            tctrl_ARC -> AppendText(_T(buffer0));
            tctrl_ARC -> AppendText("\n");
        }
        Message(91, "");
        wxMutexGuiLeave();
        struct dirent * direntry;
        dbSMdata.ptr = (unsigned char *) databuffer;
        dbfilecount = 0;
 /*
 fp1 = fopen("dupli.STATUS", "r");
 if (fp1)
 {
 (void) fread( & dbfilecount, 1, sizeof(dbfilecount), fp1);
 fclose(fp1);
 }
    */
        int l;
        if (p_createsample)
        {
            sprintf(buffer, "%s/sample", DATABASEp);
            if (stat(buffer, statbuffer))
            {
                if (errno == ENOENT)
                {
#ifdef __WIN32__
                    l = mkdir(buffer);
#else
                    l = mkdir(buffer, 777);
#endif
                }
            }
        }
        strcpy(sourcename, "");
        strcpy(sourcename_last, "");
        enterSubdir(theApp -> sourcedirectory.c_str());
        do
        {
            if (subdir[dirbufferDeep])
            {
                do
                {
                    direntry = readdir(subdir[dirbufferDeep]);
                    if (direntry)
                    {
                        filetype = 0;
                        sprintf(filename, "%s", direntry -> d_name);
                        sprintf(sourcename, "%s%s", dirbuffer[dirbufferDeep], filename);
                        if (!strcmp(filename, "."))
                        {
                            filetype = - 1;
                        }
                        else if(!strcmp(filename, ".."))
                        {
                            filetype = - 2;
                        }
                        else if(!stat(sourcename, statbuffer))
                        {
                            if (filterNames(filename, sourcename))
                            {
                                if (statbuffer -> st_mode & 0040000)
                                {
                                    filetype = 0;
                                    enterSubdir(filename);
                                    sampleGetBase();
                                    count_subdirsprocessed++;
                                    if (dirbufferDeep == 2)
                                    {
                                        GetDomain(domainlast, sourcename);
                                        storeUrls(domainlast, "");
                                        protoSubdirs();
                                    }
                                }
                                else
                                {
                                    executeFiletype();
                                }
                            }
                        }
                    }
                    if (minerdlgThread -> TD())
                    {
                        direntry = NULL;
                        dirbufferDeep = - 1;
                    }
                }
                while (direntry);
                leaveSubdir();
                if (dirbufferDeep >= 0)
                {
                    if (celdeep == dirbufferDeep)
                    {
                        celdeep = 0;
                    }
                    if (name0deep == dirbufferDeep)
                    {
                        if (samplefilename)
                        {
                            if (samplesourcename)
                            {
                                if (p_nosubfolders)
                                {
                                    sprintf(buffer, "%s/sample", DATABASEp);
                                }
                                else
                                {
                                    sprintf(buffer, "%s/sample/%s", DATABASEp, name0);
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
                                sprintf(sourcename, "%s", dirbuffer[dirbufferDeep + 1]);
                                if (strlen(sourcename) > 1)
                                {
                                    sourcename[strlen(sourcename) - 1] = 0;
                                }
                                if (name0fcount != 1)
                                {
                                    sprintf(tempname, "%s%s(%s,%dpics)", dirbuffer[dirbufferDeep]
                                    , name0, name0type, name0fcount);
                                }
                                else
                                {
                                    sprintf(tempname, "%s%s(%s,%dpic)", dirbuffer[dirbufferDeep]
                                    , name0, name0type, name0fcount);
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
                dirbufferDeep--;
            }
        }
        while (dirbufferDeep > 0);
        protoSubdirs();
        checkANDstoreJPG("zzz");
        checkANDstoreHTM("zzz");
 /*
 fp1 = fopen("dupli.STATUS", "w+r");
 if (fp1)
 {
 fwrite( & dbfilecount, 1, sizeof(dbfilecount), fp1);
 fclose(fp1);
 }
    */
        wxMutexGuiEnter();
        if (dirbufferDeep < 0)
        {
            dirbufferDeep = 0;
        }
        else
        {
            Message(92, "");
        }
        switch (sprache)
        {
        case 0:
            sprintf(buffer,
            " Unterverzeichnisse=%lu Dateien=%lu Markiert(.dup)=%lu Demarkiert=%lu Geloescht=%lu Verschoben=%lu Bytes=%lu\n",
            count_subdirsprocessed, count_ALLfiles, count_dupmarked, count_dupremoved
            , count_deleted, count_filesmoved, size_ALLfiles);
            tctrl_ARC -> AppendText(buffer);
            break;
        case 1:
            sprintf(buffer,
            " Subdirs=%lu Files=%lu DUPmarked=%lu DUPremoved=%lu Deleted=%lu Moved=%lu Bytes=%lu\n",
            count_subdirsprocessed, count_ALLfiles, count_dupmarked, count_dupremoved
            , count_deleted, count_filesmoved, size_ALLfiles);
            tctrl_ARC -> AppendText(buffer);
            break;
        }
        for (int i = 0 ; i <= 2 ; i++)
        {
            switch (i)
            {
            case 0:
                sprintf(buffer, "STD: %8ld    MET: %8ld\n"
                , count_STDfilesprocessed, count_METfilesprocessed);
                break;
            case 1:
                sprintf(buffer, "GIF: %8ld    JPG: %8ld    DKT: %8ld\n"
                , count_GIFfilesprocessed, count_JPGfilesprocessed, count_DKTfilesprocessed);
                break;
            case 2:
                sprintf(buffer, "HTM: %8ld    PRY: %8ld\n"
                , count_HTMfilesprocessed, count_PRYfilesprocessed);
                break;
            }
            tctrl_ARC -> AppendText(buffer);
        }
        wxMutexGuiLeave();
    }
    AddingDirEnd();
}

void CminerDlg::protoSubdirs()
{
    if (strlen(sourcename_last) > 0)
    {
        if (count_ALLfiles_last != count_ALLfiles)
        {
            sprintf(buffer, "+(%ld,%ld)=>dir(%ld,%ld):%s\n"
            , count_subdirsprocessed - count_subdirsprocessed_last, count_ALLfiles - count_ALLfiles_last
            , count_subdirsprocessed, count_ALLfiles, sourcename);
            Wgui(buffer);
            fprintf(fp_subdirs, "%s;%ld;%ld\n", sourcename_last
            , count_subdirsprocessed
            - count_subdirsprocessed_last
            , count_ALLfiles - count_ALLfiles_last);
        }
    }
    count_subdirsprocessed_last = count_subdirsprocessed;
    count_ALLfiles_last = count_ALLfiles;
    strcpy(sourcename_last, sourcename);
}

void CminerDlg::GetDomain(char * domain, char * source)
{
    int i, l;
    strcpy(domain, source + strlen(theApp -> sourcedirectory.c_str()) + 1);
    l = strlen(domain);
    i = 0;
    while (i < l)
    {
        switch (domain[i])
        {
        case '/':
        case '\\':
            domain[i] = 0;
            i = l;
            break;
        default:
            i++;
            break;
        }
    }
}

void CminerDlg::storeUrls(char * url1, char * url2)
{
    int status;
    hash -> hashBuffer((unsigned char *) url1, strlen(url1), p_ED2KhashMD4);
    memset(dbURkey.ptr, 0, 33);
    memcpy(dbURkey.ptr, hash -> blockhash, 16);
    dbURdata = dbUR -> fetchKey(1, dbURkey);
    if (!dbURdata.ptr)
    {
        status = 1;
        dbURdata.size = strlen(url1) + 5;
        dbURdata.ptr = (unsigned char *) malloc(dbURdata.size + 1);
        memset(dbURdata.ptr, 0, dbURdata.size);
        memcpy(dbURdata.ptr, & status, 4);
        sprintf((char *)(dbURdata.ptr + 4), "%s", url1);
        (void) dbUR -> storeData(1, dbURkey, dbURdata);
    }
    free(dbURdata.ptr);
    dbURdata.ptr = NULL;
    if (strlen(url2))
    {
        status = 2;
        hash -> hashBuffer((unsigned char *) url2, strlen(url2), p_ED2KhashMD4);
        memcpy(dbURkey.ptr + 16, hash -> blockhash, 16);
        dbURdata = dbUR -> fetchKey(1, dbURkey);
        if (!dbURdata.ptr)
        {
            dbURdata.size = strlen(url1) + strlen(url2) + 6;
            dbURdata.ptr = (unsigned char *) malloc(dbURdata.size + 1);
            memset(dbURdata.ptr, 0, dbURdata.size);
            memcpy(dbURdata.ptr, & status, 4);
            sprintf((char *)(dbURdata.ptr + 4), "%s\t%s", url1, url2);
            (void) dbUR -> storeData(1, dbURkey, dbURdata);
        }
        free(dbURdata.ptr);
        dbURdata.ptr = NULL;
    }
}

void CminerDlg::Process_DUPfile()
{
    if (p_removedupext)
    {
        int i;
        strcpy(filenameTMP, sourcename);
        i = strlen(filenameTMP);
        while (i > 0)
        {
            if (filenameTMP[--i] == '.')
            {
                filenameTMP[i] = 0;
                i = - i;
            }
        }
        if (strcmp(filenameTMP, sourcename))
        {
            (void) rename(sourcename, filenameTMP);
            count_dupremoved++;
        }
    }
}

void CminerDlg::Process_WD3file(char * filename)
{
    count_WD3filesprocessed++;
    if (p_WD3deleteALL)
    {
        (void) unlink(sourcename);
    }
}

void CminerDlg::Process_PRYfile(char * filename)
{
    count_PRYfilesprocessed++;
    if (p_delPRYfiles)
    {
        (void) unlink(sourcename);
    }
}

void CminerDlg::Process_AVIfile(char * filename)
{
    Cavifile * avifile = new Cavifile(filepath, filename);
    if (avifile -> getFilesize())
    {
        avifile -> ReadInfos();
    }
    delete avifile;
}

void CminerDlg::Process_VIDEOfile(char * filepath, char * filename)
{
    wxFile * pf = NULL;
    int l1, l2, psize;
    int i = 0, l = 0;
    char hash1[33], hash2[33];
    l1 = strlen(filepath);
    l2 = strlen(filename);
    if ((l1 + l2) < 1025)
    {
        sprintf(buffer, "%s%s", filepath, filename);
        if (!stat(buffer, statbuffer))
        {
            psize = statbuffer -> st_size;
            if (psize > 32)
            {
                memset(keybuffer, 0, 33);
                l = strlen(theApp -> sourcedirectory.c_str());
                strcpy(tempname, DATABASEp);
                strcpy(tempname + strlen(tempname), sourcename + l);
                l = strlen(DATABASEp);
                hash -> hashFile(sourcename, false, p_ED2KhashMD4);
                memcpy(dbVIkey.ptr, hash -> blockhash, 16);
                dbVIdata = dbVI -> fetchKey(1, dbVIkey);
                if (!dbVIdata.ptr)
                {
                    dbVIdata.size = strlen(tempname) + 1 + 10;
                    dbVIdata.ptr = (unsigned char *) malloc(dbVIdata.size + 1);
                    sprintf((char *) dbVIdata.ptr, "%10d%s"
                    , psize, tempname);
                    (void) dbVI -> storeData(1, dbVIkey, dbVIdata);
                }
                free(dbVIdata.ptr);
                dbVIdata.ptr = NULL;
                for (i = 0 ; i < 16 ; i++)
                {
                    sprintf(hash1 + i * 2, "%02x", ((unsigned char *) hash -> blockhash) [i]);
                }
                printf("export videofile: %s\n", sourcename);
                if (p_VIDexportMD4)
                {
                    strcpy(buffer, tempname);
                    i = l + 1;
                    l = strlen(buffer);
                    while (i < l)
                    {
                        if ((buffer[i] == '/') || (buffer[i] == '\\'))
                        {
                            buffer[i] = 0;
                            i = l;
                        }
                        i++;
                    }
                    int l;
                    i = 0;
                    while (i < 2)
                    {
                        if (stat(buffer, statbuffer))
                        {
                            if (errno == ENOENT)
                            {
#ifdef __WIN32__
                                l = mkdir(buffer);
#else
                                l = mkdir(buffer, 777);
#endif
                            }
                        }
                        i++;
                        switch (i)
                        {
                        case 1:
                            sprintf(buffer + strlen(buffer), "/videos");
                            break;
                        case 2:
                            sprintf(buffer + strlen(buffer), "/%s%s", hash1, fileextension);
                            break;
                        }
                    }
                    printf("fn: %s\n", buffer);
                    unsigned char blockbuffer[1025];
                    fpSource = new wxFile(sourcename, wxFile::read);
                    if (fpSource)
                    {
                        fpTarget = new wxFile(buffer, wxFile::write);
                        if (fpTarget)
                        {
                            int bytes = 0, allbytes = 0, block = 0;
                            do
                            {
                                block = psize - allbytes;
                                if (block > 1024)
                                {
                                    block = 1024;
                                }
                                bytes = fpSource -> Read(blockbuffer, block);
                                if (bytes)
                                {
                                    allbytes += bytes;
                                    fpTarget -> Write(blockbuffer, bytes);
                                }
                            }
                            while (!fpSource -> Eof());
                            delete fpTarget;
                            fpTarget = NULL;
                        }
                        delete fpSource;
                        fpSource = NULL;
                    }
                    hash -> hashFile(buffer, false, p_ED2KhashMD4);
                    for (i = 0 ; i < 16 ; i++)
                    {
                        sprintf(hash2 + i * 2, "%02x", ((unsigned char *) hash -> blockhash) [i]);
                    }
                    if (!strcmp(hash1, hash2))
                    {
                        count_filesmoved++;
                        if (p_JPGwrbackMD4)
                        {
                            pf = new wxFile(sourcename, wxFile::write);
                            pf -> Write(hash1, 32);
                            delete pf;
                        }
                    }
                }
            }
            else if((psize == 32) && p_VIDimportMD4)
            {
                //noch zu programmieren
            }
        }
    }
}

void CminerDlg::Process_HTMfile(char * filename)
{
    count_HTMfilesprocessed++;
    if (p_HTMLaddMD4)
    {
        int i;
        char hash1[33];
        memset(hash1, 0, 33);
        memset(tempname, 0, BUFFERSIZE);
        hash -> hashFile(sourcename, false, p_ED2KhashMD4);
        for (i = 0 ; i < 16 ; i++)
        {
            sprintf(hash1 + i * 2, "%02x", ((unsigned char *) hash -> blockhash) [i]);
        }
        i = strlen(sourcename);
        while (i > 0)
        {
            if (sourcename[i] == '.')
            {
                i = - i;
            }
            else
            {
                i--;
            }
        }
        if (i < 0)
        {
            i = - i;
            if (i > 32)
            {
                if (!strncmp(sourcename + i - 32, hash1, 32))
                {
                    i = 0;
                }
            }
            if (i)
            {
                strncpy(tempname, sourcename, i);
                sprintf(tempname + strlen(tempname), ".%s%s", hash1, sourcename + i);
                if (!stat(tempname, statbuffer))
                {
                    (void) unlink(tempname);
                }
                (void) rename(sourcename, tempname);
                strcpy(tempname, sourcename);
                GetFilename(filename, sourcename);
            }
        }
    }
    int l, t, b, p, i;
    int filesize;
    t = 0;
    if (!stat(sourcename, statbuffer))
    {
        if (statbuffer -> st_size == 42)
        {
            t = 99;
        }
        else
        {
            if (hardlockAnz)
            {
                char * tmp = (char *) malloc(statbuffer -> st_size + 1);
                FILE * fp = fopen(sourcename, "r");
                p = 0;
                while (!feof(fp))
                {
                    b = fgetc(fp);
                    if (b >= 0)
                    {
                        if (p < statbuffer -> st_size)
                        {
                            tmp[p++] = b;
                        }
                    }
                }
                fclose(fp);
                tmp[p] = 0;
                i = 0;
                while (i < hardlockAnz)
                {
                    i++;
                    if (strlen(hardlockDomain[i]))
                    {
                        if (strstr(filepath, hardlockDomain[i]))
                        {
                            t |= 1;
                            if (strlen(hardlock[i]))
                            {
                                if (strstr(tmp, hardlock[i]))
                                {
                                    i = hardlockAnz;
                                }
                                else
                                {
                                    t = 0;
                                }
                            }
                        }
                    }
                    else if(strlen(hardlock[i]))
                    {
                        if (strstr(tmp, hardlock[i]))
                        {
                            t |= 2;
                            i = hardlockAnz;
                        }
                    }
                }
                free(tmp);
            }
        }
        if (t)
        {
            if (t < 99)
            {
                char hash0[33];
                hash -> hashFile(sourcename, false, p_ED2KhashMD4);
                memset(hash0, 0, 33);
                for (int i = 0 ; i < 16 ; i++)
                {
                    sprintf(hash0 + i * 2, "%02x", ((unsigned char *) hash -> blockhash) [i]);
                }
                HashAndRename(sourcename, "hardlocked", false);
                FILE * fp = fopen(sourcename, "w");
                fprintf(fp, "hardlocked%s", hash0);
                fclose(fp);
                fp = fopen("hardlock.proto", "a");
                fprintf(fp, "%10lu:%s\n", (unsigned long) statbuffer -> st_size, sourcename);
                fclose(fp);
            }
        }
        else
        {
            char * link1, zeile[1025];
            filesize = statbuffer -> st_size;
            htmp = new Chtmlpage(sourcename, statbuffer -> st_size);
            do
            {
                htmp -> getNextCommandline(zeile, 1024);
                if (strlen(zeile))
                {
                    htmp -> lowerString(zeile);
                    htmp -> parseCommandline(zeile);
                    t = htmp -> getTokenMain();
                    if ((t == 1201) || (t == 1401))
                    {
                        link1 = strstr(htmp -> getHttpLinkPar(), "/");
                        if (link1)
                        {
                            htmp -> mix2Urls(dirbufferDeep, urlnext, dirbuffer[dirbufferDeep], link1);
                        }
                        else
                        {
                            htmp -> mix2Urls(dirbufferDeep, urlnext, dirbuffer[dirbufferDeep]
                            , htmp -> getHttpLinkUrl());
                        }
                        int p1 = 0;
                        l = strlen(urlnext);
                        while (p1 < toblockAnz)
                        {
                            p1++;
                            if (strstr(urlnext, toblock[p1]))
                            {
                                l = 0;
                            }
                        }
                        if (l)
                        {
                            switch (htmp -> GetFiletype(urlnext))
                            {
                            case 4:
                                if (htmp -> GetFilesubtype() == 1)
                                {
                                    checkANDstoreHTM(urlnext);
                                }
                                break;
                            case 7:
                                if (htmp -> GetFilesubtype() == 1)
                                {
                                    checkANDstoreJPG(urlnext);
                                }
                                break;
                            default:
                                GetDomain(domainnext, urlnext);
                                if (strcmp(domainlast, domainnext))
                                {
                                    storeUrls(domainnext, domainlast);
                                }
                                break;
                            }
                        }
                    }
                }
            }
            while (zeile[0]);
            delete htmp;
        }
    }
}

void CminerDlg::checkANDstoreJPG(char * filepath)
{
    GetDomain(domainnext, filepath);
    if (strcmp(domainlast, domainnext))
    {
        storeUrls(domainnext, domainlast);
    }
    if (stat(filepath, statbuffer))
    {
        int status = 0;
        hash -> hashBuffer((unsigned char *) domainnext, strlen(domainnext), p_ED2KhashMD4);
        memset(dbURkey.ptr, 0, 33);
        memcpy(dbURkey.ptr, hash -> blockhash, 16);
        dbURdata = dbUR -> fetchKey(1, dbURkey);
        if (dbURdata.ptr)
        {
            memcpy( & status, dbURdata.ptr, 4);
            free(dbURdata.ptr);
            dbURdata.ptr = NULL;
        }
        if (status & 256)
        {
            //printf("cAs(%08x)\n", status);
        }
        else if(p_crePageForJPG)
        {
            hash -> hashBuffer((unsigned char *) filepath, strlen(filepath), p_ED2KhashMD4);
            memset(dbTMPkey.ptr, 0, 16);
            memcpy(dbTMPkey.ptr, hash -> blockhash, 16);
            dbTMPdata = dbTMP -> fetchKey(1, dbTMPkey);
            if (!dbTMPdata.ptr)
            {
                dbTMPdata.size = strlen(filepath) + 1;
                dbTMPdata.ptr = (unsigned char *) malloc(dbTMPdata.size + 1);
                strcpy((char *) dbTMPdata.ptr, filepath);
                (void) dbTMP -> storeData(1, dbTMPkey, dbTMPdata);
                char fn[512];
                sprintf(fn, "jpgpage%d.htm", jpgpagenumber);
                FILE * fp = fopen(fn, "a");
                if (!strcmp(filepath, "zzz"))
                {
                    fprintf(fp, "</html>\n");
                }
                else
                {
                    jpglinenumber++;
                    if (jpglinenumber > 256)
                    {
                        jpgpagenumber++;
                        jpglinenumber = 1;
                        sprintf(fn, "jpgpage%d.htm", jpgpagenumber);
                        fprintf(fp, "<a href=\"%s\">%s</a>\n", fn, fn);
                        fprintf(fp, "</html>\n");
                        fclose(fp);
                        fp = fopen(fn, "w");
                    }
                    if (jpglinenumber == 1)
                    {
                        fprintf(fp, "<html>\n");
                    }
                    sprintf(buffer, "<img src=\"http://%s\">pic%02d</img>\n"
                    , filepath + 1 + strlen(theApp -> sourcedirectory.c_str()), jpglinenumber);
                    fprintf(fp, buffer);
                }
                fclose(fp);
            }
            free(dbTMPdata.ptr);
            dbTMPdata.ptr = NULL;
        }
    }
}

void CminerDlg::checkANDstoreHTM(char * filepath)
{
    GetDomain(domainnext, filepath);
    if (strcmp(domainlast, domainnext))
    {
        storeUrls(domainnext, domainlast);
    }
    int status = 0;
    hash -> hashBuffer((unsigned char *) domainnext, strlen(domainnext), p_ED2KhashMD4);
    memset(dbURkey.ptr, 0, 33);
    memcpy(dbURkey.ptr, hash -> blockhash, 16);
    dbURdata = dbUR -> fetchKey(1, dbURkey);
    if (dbURdata.ptr)
    {
        memcpy( & status, dbURdata.ptr, 4);
        free(dbURdata.ptr);
        dbURdata.ptr = NULL;
    }
    if (status & 256)
    {
        //printf("cAs(%08x)\n", status);
    }
    else if(p_crePageForHTM)
    {
        char fn[512];
        sprintf(fn, "htmpage%d.htm", htmpagenumber);
        FILE * fp = fopen(fn, "a");
        if (!strcmp(filepath, "zzz"))
        {
            fprintf(fp, "</html>\n");
        }
        else
        {
            htmlinenumber++;
            if (htmlinenumber > 256)
            {
                htmpagenumber++;
                htmlinenumber = 1;
                sprintf(fn, "htmpage%d.htm", htmpagenumber);
                fprintf(fp, "<a href=\"%s\">%s</a>\n", fn, fn);
                fprintf(fp, "</html>\n");
                fclose(fp);
                fp = fopen(fn, "w");
            }
            if (htmlinenumber == 1)
            {
                fprintf(fp, "<html>\n");
            }
            sprintf(buffer, "<a href=\"http://%s\">picpage%02d</a>\n"
            , filepath + 1 + strlen(theApp -> sourcedirectory.c_str()), htmlinenumber);
            fprintf(fp, buffer);
        }
        fclose(fp);
    }
}

void CminerDlg::Process_DKTfile(char * filepath, char * filename)
{
    count_DKTfilesprocessed++;
    if (p_DKTrenameMD4)
    {
        int l;
        l = strlen(sourcename);
        if (l > 37)
        {
            if (sourcename[l - 4] == '.')
            {
                if (sourcename[l - 37] == '.')
                {
                    char tmp[1025];
                    strcpy(tmp, sourcename);
                    tmp[l - 37] = 0;
                    if (!stat(tmp, statbuffer))
                    {
                        (void) unlink(tmp);
                    }
                    (void) rename(sourcename, tmp);
                }
            }
        }
    }
}

void CminerDlg::Process_PRTfile()
{
    if (p_PRTcreateISM || p_PRTsearchMET)
    {
        hash -> hashFile(sourcename, true, true);
        if (p_PRTsearchMET)
        {
            char temp[strlen(sourcename) + 5];
            int htw = 0;
            CMetfile * metfile = NULL;
            int htp, pointer, s, z, completeCount = 0;
            unsigned char * ht;
            unsigned long partfileSize, start = 0, end = 0;
            sprintf(temp, "%s.met", sourcename);
            if (stat(temp, statbuffer))
            {
                partfileSize = filesize;
                htp = hash -> hashtableindex;
                ht = (unsigned char *)(hash -> hashtable);
                for (z = 0 ; z < htp ; z++)
                {
                    start = z <<4;
                    pointer = 0;
                    for (s = 0 ; s < 16 ; s++)
                    {
                        if (s < 3)
                        {
                            if (ht[start + s] == 0xaa)
                            {
                                pointer++;
                            }
                        }
                        else if(s > 12)
                        {
                            if (ht[start + s] == 0xcc)
                            {
                                pointer++;
                            }
                        }
                    }
                    if (!pointer)
                    {
                        pointer = checkSubhash(ht + start, z);
                        if (pointer)
                        {
                            partfileSize = atol(buffer + 2);
                            completeCount++;
                            if (!metfile)
                            {
                                metfile = new CMetfile();
                                metfile -> Create(temp);
                                metfile -> SetFilename(buffer + 15);
                                metfile -> SetPartfilename(filename);
                                metfile -> SetFilesize(partfileSize);
                                metfile -> SetFilehash(dbSHkey.ptr);
                                strcpy((char *) dbSHkey.ptr + 16, "hashtable");
                                dbSHdata = dbSH -> fetchKey(1, dbSHkey);
                                if (dbSHdata.ptr)
                                {
                                    htw++;
                                    metfile -> SetHashtable(atoi((char *) dbSHdata.ptr), dbSHdata.ptr + 8);
                                    free(dbSHdata.ptr);
                                    dbSHdata.ptr = NULL;
                                }
                                metfile -> ClearGaplist();
                                metfile -> AddGap(0, partfileSize);
                            }
                            start = z * 9728000;
                            end = start + 9728000;
                            if (end > partfileSize)
                            {
                                end = partfileSize;
                            }
                            metfile -> FillGap(start, end);
                        }
                    }
                }
                if (metfile)
                {
                    metfile -> WriteTo();
                    delete metfile;
                }
            }
        }
        if (p_PRTcreateISM)
        {
            sprintf(tempname, "%s%s.ism", filepath, filename);
            CreateInfofile(tempname, filepath, filename);
        }
    }
    if (p_PRTcreateOVN)
    {
        sub_PRTcreateOVN();
    }
}

void CminerDlg::Process_STDfile(char * filename)
{
    if (p_STDparse)
    {
        int l = 0, p1 = 0;
        count_STDfilesprocessed++;
        if (p_ISMparse)
        {
            if (p_ISMdeleteDUP)
            {
                l = CheckDatabaseISM((unsigned char *)hash -> blockhash);
            }
        }
        if (!l)
        {
            strcpy(filepathTMP, dirbuffer[dirbufferDeep]);
            bool itemhashedpointer = false;
            name0fcount++;
            strcpy(buffer, filename);
    /*++++++++++++++++++++++++++++++changing source-filename&/position ++++++++++++++++*/
            if (filetype == 7)
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
                                strcpy(filepathTMP, dirbuffer[name0deep]);
                            }
                            if (p_doHDNsetting)
                            {
                                do
                                {
                                    name0count++;
                                    switch (filetype)
                                    {
                                    case 7:
                                        //picfile
                                        sprintf(buffer, "%s.%08d.jpg", name0, name0count);
                                        sprintf(tempname, "%s%s", filepathTMP, buffer);
                                        break;
                                    default:
                                        sprintf(buffer, "%s.%08d.???", name0, name0count);
                                        sprintf(tempname, "%s%s", filepathTMP, buffer);
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
                                sprintf(tempname, "%s%s", filepathTMP, filename);
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
            if (p_createsample)
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
                hash -> hashFile(sourcename, p_doMD4fakechk, p_ED2KhashMD4);
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
 RenameNF(sourcename, true);
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
                if (p_useCOKEY)
                {
                    for (i = 0 ; i < 16 ; i++)
                    {
                        if (i < 8)
                        {
                            sprintf(keybuffer + i * 2, "%02x", ((unsigned char *) hash -> blockhash) [i + 8]);
                        }
                        else
                        {
                            sprintf(keybuffer + i * 2, "%02x", ((unsigned char *) hash -> blockhash) [i - 8]);
                        }
                    }
                }
                else
                {
                    for (i = 0 ; i < 16 ; i++)
                    {
                        sprintf(keybuffer + i * 2, "%02x", ((unsigned char *) hash -> blockhash) [i]);
                    }
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
                    RenameNF(sourcename, false);
                    free(dbSMresult.ptr);
                    dbSMresult.ptr = NULL;
                }
 /*
 }
    */
                if (p_STDcreateINF)
                {
                    sprintf(tempname, "%s%s.ism", filepathTMP, filename);
                    CreateInfofile(tempname, filepathTMP, filename);
                }
                if (p_STDcreateMET)
                {
                    sprintf(tempname, "%s%s.met", filepathTMP, filename);
                    Create_METfile(tempname, filepathTMP, filename);
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
        }
    }
}

void CminerDlg::sub_PRTcreateOVN()
{
    int l = 0, i = 0;
    sprintf(tempname, "%s%s.ovn", filepath, filename);
    /* filecopy:*/
    l = strlen(filename);
    char partid[l + 1];
    strcpy(partid, filename);
    i = 0;
    while (i < l)
    {
        if (partid[i] == '.')
        {
            if (i == (l - 5))
            {
                partid[i] = 0;
            }
            else
            {
                i = 0;
            }
            l = - l;
        }
        else
        {
            i++;
        }
    }
    if (i)
    {
        if (stat(tempname, statbuffer))
        {
            if (errno == ENOENT)
            {
#ifdef __WIN32__
                (void) mkdir(tempname);
#else
                (void) mkdir(tempname, 777);
#endif
            }
        }
        FILE * fps, * fpt;
        fps = fopen(sourcename, "r");
        if (fps)
        {
            int bytes = 0, bytet = 0;
            i = 0;
            while (!feof(fps))
            {
                ++i;
                sprintf(tempname, "%s%s.ovn/%s.%d.part", filepath, filename, partid, i);
                fpt = fopen(tempname, "w+");
                if (fpt)
                {
                    bytet = 9728000;
                    while ((!feof(fps)) && bytet)
                    {
                        if (bytet > BUFFERSIZE)
                        {
                            bytes = fread(buffer0, 1, BUFFERSIZE, fps);
                        }
                        else
                        {
                            bytes = fread(buffer0, 1, bytet, fps);
                        }
                        bytet -= bytes;
                        if (bytes)
                        {
                            bytes = fwrite(buffer0, 1, bytes, fpt);
                        }
                    }
                    fclose(fpt);
                }
            }
            fclose(fps);
        }
    }
    /* filecopy_end*/
}

void CminerDlg::sub_METimportMD4()
{
    unsigned long start;
    unsigned int htp, i;
    unsigned char * ht;
    htp = mfile -> parts;
    ht = mfile -> parts_hashtable;
    sprintf(buffer, "1_%12lu_%s", mfile -> mf_filesize, mfile -> mf_filename);
    if (htp)
    {
        Cmd4hash * ismHash = new Cmd4hash();
        (void) ismHash -> hashBuffer(ht, htp << 4, true);
        storeSubhash(0, NULL, (unsigned char *) ismHash -> blockhash, buffer);
        strcpy((char *) dbSHkey.ptr + 16, "hashtable");
        dbSHdata = dbSH -> fetchKey(1, dbSHkey);
        if (!dbSHdata.ptr)
        {
            dbSHdata.size = 8 + (htp << 4);
            dbSHdata.ptr = (unsigned char *) malloc(dbSHdata.size + 1);
            memset(dbSHdata.ptr, 0, dbSHdata.size + 1);
            sprintf((char *) dbSHdata.ptr, "%7d", htp);
            memcpy(dbSHdata.ptr + 8, ht, htp << 4);
            (void) dbSH -> storeData(1, dbSHkey, dbSHdata);
        }
        free(dbSHdata.ptr);
        dbSHdata.ptr = NULL;
        for (i = 0 ; i < htp ; i++)
        {
            start = i <<4;
            sprintf(buffer, "5_%d", i);
            storeSubhash(1, ht + start, (unsigned char *) ismHash -> blockhash, buffer);
        }
        delete ismHash;
    }
    else
    {
        unsigned char blockhash[17];
        from32to16(mfile -> mf_filehash, blockhash);
        storeSubhash(0, NULL, blockhash, buffer);
    }
}

void CminerDlg::sub_ISMimportMD4()
{
    int p = 0, bytecount = 0, b;
    int mode = 0;
    unsigned long start;
    char * bufferp = NULL;
    char * ismFilename = NULL;
    char * ismFilepath = NULL;
    char ismFilehash[33];
    memset(ismFilehash, 0, 33);
    unsigned long ismFilesize = 0;
    unsigned char * ismHashtable = NULL;
    unsigned long htp = 0;
    FILE * fp = fopen(sourcename, "r");
    if (fp)
    {
        while (!feof(fp))
        {
            b = fgetc(fp);
            if (b >= 0)
            {
                bytecount++;
                switch (b)
                {
                case 10:
                case 13:
                    if (p > 0)
                    {
                        p = - p;
                    }
                    break;
                case 9:
                case 32:
                    if (p)
                    {
                        buffer[p++] = b;
                    }
                    break;
                default:
                    buffer[p++] = b;
                    break;
                }
                if (p > 0)
                {
                    if (bytecount >= statbuffer -> st_size)
                    {
                        p = - p;
                    }
                }
                if (p < 0)
                {
                    p = - p;
                    buffer[p] = 0;
                    if (!strncmp(buffer, "Filename  : ", 12))
                    {
                        ismFilename = (char *) malloc(strlen(buffer) - 11);
                        strcpy(ismFilename, buffer + 12);
                        //printf("Filename:%s\n", ismFilename);
                    }
                    else if(!strncmp(buffer, "Filepath  : ", 12))
                    {
                        ismFilepath = (char *) malloc(strlen(buffer) - 11);
                        strcpy(ismFilepath, buffer + 12);
                        //printf("Filepath:%s\n", ismFilepath);
                    }
                    else if(!strncmp(buffer, "MD4Hash   : ", 12))
                    {
                        memset(ismFilehash, 0, 33);
                        strncpy(ismFilehash, buffer + 12, 32);
                        //printf("Filehash:%s\n", ismFilehash);
                    }
                    else if(!strncmp(buffer, "Filesize  : ", 12))
                    {
                        ismFilesize = atol(buffer + 12);
                        htp = ismFilesize + 9727999;
                        htp/= 9728000;
                        ismHashtable = (unsigned char *) malloc(htp * 16 + 32);
                        memset(ismHashtable, 0, htp * 16 + 32);
                        //printf("Filesize:%lu Index:%lu\n", ismFilesize, htp);
                    }
                    else if(!strncmp(buffer, "Hashtable :", 11))
                    {
                        mode = 1;
                        //printf("hashtable follows(%d)\n", mode);
                    }
                    else if(mode == 1)
                    {
                        if ((buffer[0] >= '0') && (buffer[0] <= '9'))
                        {
                            if (htp)
                            {
                                bufferp = strstr((char *) buffer, ":");
                                if (bufferp)
                                {
                                    start = atol(buffer) / 9728000;
                                    if (start < htp)
                                    {
                                        start <<= 4;
                                        bufferp += 2;
                                        from32to16(bufferp, ismHashtable + start);
                                    }
                                }
                            }
                        }
                        else
                        {
                            mode = 0;
                        }
                    }
                    p = 0;
                    buffer[p] = 0;
                }
            }
        }
        fclose(fp);
        unsigned char * ht;
        ht = ismHashtable;
        bufferp = strstr(ismFilepath, "id");
        if (bufferp)
        {
            bufferp = strstr(ismFilepath, "0");
        }
        if (bufferp)
        {
            sprintf(buffer, "2_%12lu_%s", ismFilesize, ismFilename);
        }
        else
        {
            sprintf(buffer, "4_%12lu_%s", ismFilesize, ismFilename);
        }
        if (htp)
        {
            Cmd4hash * ismHash = new Cmd4hash();
            (void) ismHash -> hashBuffer(ht, htp << 4, true);
            storeSubhash(0, NULL, (unsigned char *) ismHash -> blockhash, buffer);
            strcpy((char *) dbSHkey.ptr + 16, "hashtable");
            dbSHdata = dbSH -> fetchKey(1, dbSHkey);
            if (!dbSHdata.ptr)
            {
                dbSHdata.size = 8 + (htp << 4);
                dbSHdata.ptr = (unsigned char *) malloc(dbSHdata.size + 1);
                memset(dbSHdata.ptr, 0, dbSHdata.size + 1);
                sprintf((char *) dbSHdata.ptr, "%7ld", htp);
                memcpy(dbSHdata.ptr + 8, ht, htp << 4);
                (void) dbSH -> storeData(1, dbSHkey, dbSHdata);
            }
            free(dbSHdata.ptr);
            dbSHdata.ptr = NULL;
            for (unsigned int i = 0 ; i < htp ; i++)
            {
                start = i <<4;
                if (bufferp)
                {
                    sprintf(buffer, "6_%d", i);
                    storeSubhash(1, ht + start, (unsigned char *) ismHash -> blockhash, buffer);
                }
                else
                {
                    sprintf(buffer, "8_%d", i);
                    storeSubhash(1, ht + start, (unsigned char *) ismHash -> blockhash, buffer);
                }
            }
            delete ismHash;
        }
        else
        {
            unsigned char blockhash[17];
            from32to16(ismFilehash, blockhash);
            storeSubhash(0, NULL, blockhash, buffer);
        }
    }
    if (ismFilename)
    {
        free(ismFilename);
    }
    if (ismFilepath)
    {
        free(ismFilepath);
    }
    if (ismHashtable)
    {
        free(ismHashtable);
    }
}

void CminerDlg::Process_ISMfile()
{
    if (p_ISMimportMD4)
    {
        sub_ISMimportMD4();
    }
}

void CminerDlg::Process_METfile(char * filename)
{
    int ft = 0;
    char * fext = NULL;
    strcpy(filepath, dirbuffer[dirbufferDeep]);
    count_METfilesprocessed++;
    mfile = new CMetfile(filepath, filename);
    if (mfile -> mf_filename)
    {
        (void) htmp0 -> GetFiletype(mfile -> mf_filename);
        char * ftp = NULL;
        fext = htmp0 -> GetExtension();
        if (p_METimportMD4)
        {
            sub_METimportMD4();
        }
        if (p_METcleanuTYP)
        {
            ftp = strstr(fTypes, fext);
            if (ftp)
            {
                ft = atoi(ftp + 1 + strlen(fext));
                if (mfile -> mf_filesize < ft * 1024)
                {
                    ft = 1;
                }
                else
                {
                    ft = 0;
                }
            }
            else
            {
                ft = 1;
            }
            if (ft)
            {
                delete mfile;
                sprintf(buffer, "%s%s.bak", filepath, filename);
                ft = strlen(buffer);
                if (!stat(buffer, statbuffer))
                {
                    HashAndRename(buffer, "cleanup", false);
                }
                buffer[ft - 4] = 0;
                if (!stat(buffer, statbuffer))
                {
                    HashAndRename(buffer, "cleanup", false);
                }
                buffer[ft - 8] = 0;
                if (!stat(buffer, statbuffer))
                {
                    HashAndRename(buffer, "cleanup", false);
                }
                ft = 1;
            }
        }
        if (!ft)
        {
            strcpy(keybuffer, mfile -> mf_filehash);
            sprintf(databuffer, "%12lu:%s", mfile -> mf_filesize, filename);
            char tempname[strlen(sourcename) + 32 + strlen(mfile -> mf_filename) ];
            if (CheckDatabase(keybuffer, databuffer, true))
            {
                //duplikat
                //strcmp(databuffer, (char *) dbSMresult.ptr)):
                if (p_debugonoff)
                {
 /*
 sprintf(buffer, "\nDuplikate %lu: ", count_STDfilesprocessed);
 Wgui(buffer);
 tctrl_ARC -> AppendText(filepath);
 tctrl_ARC -> AppendText(filename);
 tctrl_ARC -> AppendText(mfile->mf_partfile);
 tctrl_ARC -> AppendText(" >> ");
 tctrl_ARC -> AppendText(mfile -> mf_filename);
 sprintf(buffer, " %lu Bytes %s\n", mfile -> mf_filesize, keybuffer);
 tctrl_ARC -> AppendText(buffer);
    */
                }
                if (p_logduples || p_markupduples)
                {
                    RenameNF(sourcename, false);
                }
                strcpy(tempname, sourcename);
                sprintf(tempname + strlen(tempname) - 4, "(%lu)%s.dup.ism", mfile -> mf_filesize, mfile -> mf_filename);
                free(dbSMresult.ptr);
                dbSMresult.ptr = NULL;
            }
            else
            {
                strcpy(tempname, sourcename);
                sprintf(tempname + strlen(tempname) - 4, "(%lu)%s.ism", mfile -> mf_filesize, mfile -> mf_filename);
            }
            if (p_METcreateISM)
            {
                Create_ISMfile(tempname, filepath, filename);
            }
            if (p_METrepairPRT)
            {
                Repair_METfile();
            }
            strcpy(tempname, sourcename);
            tempname[strlen(tempname) - 4] = 0;
            if (p_METcreatePRT)
            {
                CreatePartfileMET(tempname, filepath, filename);
            }
            if (p_METseparaTYP)
            {
                char * ext = new char[strlen(mfile -> mf_filename) + 1];
                if (strlen(fext))
                {
                    sprintf(ext, "_%s", fext);
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
    }
    else
    {
        printf("metfile-error on: %s\n", sourcename);
    }
    if (!ft)
    {
        delete mfile;
    }
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
    char hash0[33];
    char tempname[34 + strlen(sourcename) ];
    hash -> hashFile(sourcename, false, !p_ED2KhashMD4);
    memset(hash0, 0, 33);
    for (i = 0 ; i < 16 ; i++)
    {
        sprintf(hash0 + i * 2, "%02x", ((unsigned char *) hash -> blockhash) [i]);
    }
    strcpy(tempname, sourcename);
    i = strlen(tempname);
    while (i > 0)
    {
        --i;
        if (tempname[i] == '.')
        {
            i = - i;
        }
    }
    if (i < 0)
    {
        i = - i;
        if (i > 32)
        {
            if (toadd)
            {
                if (!strncmp(tempname + i - 32, hash0, 32))
                {
                    //do not add hash
                }
                else if(!strncmp(tempname + i - 32, keybuffer, 32))
                {
                    //do not add hash
                }
                else
                {
                    strcpy(tempname + i + 1, keybuffer);
                    strcpy(tempname + i + 33, sourcename + i);
                }
            }
            else
            {
                if (!strncmp(tempname + i - 32, hash0, 32))
                {
                    strcpy(tempname + i - 33, sourcename + i);
                }
                else if(!strncmp(tempname + i - 32, keybuffer, 32))
                {
                    strcpy(tempname + i - 33, sourcename + i);
                }
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
            if (!rename(sourcename, tempname))
            {
                strcpy(sourcename, tempname);
            }
        }
    }
}

void CminerDlg::HashAndRename(char * sn, char * ext, bool tohash)
{
    char tmp[1025];
    strcpy(tmp, sn);
    if (tohash)
    {
        char hash0[33];
        hash -> hashFile(sn, false, p_ED2KhashMD4);
        memset(hash0, 0, 33);
        for (int i = 0 ; i < 16 ; i++)
        {
            sprintf(hash0 + i * 2, "%02x", ((unsigned char *) hash -> blockhash) [i]);
        }
        sprintf(tmp + strlen(tmp), ".%s.%s", hash0, ext);
    }
    else
    {
        sprintf(tmp + strlen(tmp), ".%s", ext);
    }
    if (!stat(tmp, statbuffer))
    {
        (void) unlink(tmp);
    }
    (void) rename(sn, tmp);
    //printf("%s: %s\n", ext,tmp);
}

void CminerDlg::RenameNF(char * sourcename, bool fakeorduple)
{
    bool itemfoundpointer = false;
    int dbl, mrl;
    dbl = strlen(databuffer);
    mrl = strlen((char *) dbSMresult.ptr);
    if (dbl == mrl)
    {
        if (!strcmp(databuffer, (char *) dbSMresult.ptr))
        {
            itemfoundpointer = true;
        }
    }
    else if(dbl == (mrl + 33))
    {
        while (mrl > 0)
        {
            if (dbSMresult.ptr[mrl] == '.')
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
            if (!strncmp(databuffer, (char *) dbSMresult.ptr, mrl))
            {
                if (p_useCOKEY)
                {
                    if (!strncmp(keybuffer, databuffer + mrl + 17, 16))
                    {
                        if (!strncmp(keybuffer + 16, databuffer + mrl + 1, 16))
                        {
                            itemfoundpointer = true;
                        }
                    }
                }
                else
                {
                    if (!strncmp(keybuffer, databuffer + mrl + 1, 32))
                    {
                        itemfoundpointer = true;
                    }
                }
            }
        }
    }
    else if((dbl + 33) == mrl)
    {
        while (dbl > 0)
        {
            if (dbSMresult.ptr[dbl] == '.')
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
            if (!strncmp(databuffer, (char *) dbSMresult.ptr, dbl))
            {
                if (p_useCOKEY)
                {
                    if (!strncmp(keybuffer, (char *)(dbSMresult.ptr + dbl + 17), 16))
                    {
                        if (!strncmp(keybuffer + 16, (char *)(dbSMresult.ptr + dbl + 1), 16))
                        {
                            itemfoundpointer = true;
                        }
                    }
                }
                else
                {
                    if (!strncmp(keybuffer, (char *)(dbSMresult.ptr + dbl + 1), 32))
                    {
                        itemfoundpointer = true;
                    }
                }
            }
        }
    }
    if (fakeorduple)
    {
        if (!itemfoundpointer)
        {
            count_fakmarked++;
            HashAndRename(sourcename, "fak", false);
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
                fprintf(logfileFP, "Original : %s\r\n", dbSMresult.ptr);
                fprintf(logfileFP, "Duplikat : %s\r\n", databuffer);
            }
            if (p_markupduples)
            {
                count_dupmarked++;
                HashAndRename(sourcename, "dup", false);
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

int CminerDlg::CheckDatabaseISM(unsigned char * md4hash)
{
    int retcode = 0;
    memset(dbSHkey.ptr, 0, 33);
    memcpy(dbSHkey.ptr, md4hash, 16);
    dbSHdata = dbSH -> fetchKey(1, dbSHkey);
    if (dbSHdata.ptr)
    {
        if (dbSHdata.ptr[0] == '2')
        {
            retcode++;
printf("dup-delete:%s\n",(char*)dbSHdata.ptr);
        }
        free(dbSHdata.ptr);
        dbSHdata.ptr = NULL;
    }
    return retcode;
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
    from32to16(keybuffer, dbSMkey.ptr);
    dbSMresult = dbSM -> fetchKey(1, dbSMkey);
    if (tostore)
    {
        if (!dbSMresult.ptr)
        {
            tempData.ptr = (unsigned char *) databuffer;
            tempData.size = strlen(databuffer) + 1;
            if (p_creDBentry)
            {
                (void) dbSM -> storeData(1, dbSMkey, tempData);
                dbfilecount++;
            }
        }
    }
    return(char *) dbSMresult.ptr;
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

void CminerDlg::Create_METfile(const char * metfilefullname, const char * filepath, const char * filename)
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

void CminerDlg::sub_Repair_METfile(char * filename)
{
    int l;
    l = strlen(filepath) + strlen(filename);
    char filenamePRT[l + 1];
    sprintf(filenamePRT, "%s%s", filepath, filename);
    if (l > 4)
    {
        filenamePRT[l - 4] = 0;
        if (!stat(filenamePRT, statbuffer))
        {
            Cmd4hash * hashPRT = new Cmd4hash();
            hashPRT -> hashFile(filenamePRT, false, true);
            if (statbuffer -> st_size == mfile -> mf_filesize)
            {
                if (hashPRT -> hashtableindex == mfile -> parts)
                {
                    wxFile * fpPRT = new wxFile(filenamePRT, wxFile::read);
                    CMetfile * metfile = new CMetfile();
                    sprintf(filenameTMP, "%s%s.new", filepath, filename);
                    metfile -> Create(filenameTMP);
                    metfile -> SetFilename(mfile -> mf_filename);
                    metfile -> SetPartfilename(mfile -> mf_partfile);
                    metfile -> SetFilesize(mfile -> mf_filesize);
                    metfile -> SetFilehash(mfile -> checkid);
                    metfile -> SetHashtable(mfile -> parts, mfile -> parts_hashtable);
                    metfile -> ClearGaplist();
                    metfile -> AddGap(0, statbuffer -> st_size);
                    long pos, i, p;
                    long start = 0, end = 0;
                    long bytes = 0;
                    char block[1025];
                    long lbyte, lposEnd, lposStart, lcount;
                    i = 0;
                    p = 0;
                    while (i < hashPRT -> hashtableindex)
                    {
                        p = i <<4;
                        start = i * 9728000;
                        end = start + 9728000;
                        if (end > statbuffer -> st_size)
                        {
                            end = statbuffer -> st_size;
                        }
                        if (!memcmp(hashPRT -> hashtable + p, mfile -> parts_hashtable + p, 16))
                        {
                            metfile -> FillGap(start, end);
                            bytes += end - start;
                        }
                        else
                        {
                            fpPRT -> Seek(start, wxFromStart);
                            pos = start;
                            lbyte = 0;
                            lcount = 0;
                            lposStart = start;
                            lposEnd = start;
                            while (pos < end)
                            {
                                (void) fpPRT -> Read(block, 1);
                                if (lbyte == block[0])
                                {
                                    lcount++;
                                }
                                else
                                {
                                    if (lcount > 32)
                                    {
                                        if (lposEnd > (lposStart + 8192))
                                        {
                                            metfile -> FillGap(lposStart, lposEnd);
                                            bytes += lposEnd - lposStart;
                                        }
                                        //gap überspringen
                                        lposStart = pos;
                                    }
                                    lposEnd = pos;
                                    lcount = 0;
                                }
                                lbyte = block[0];
                                pos++;
                            }
                            if (lposEnd > (lposStart + 8192))
                            {
                                metfile -> FillGap(lposStart, lposEnd);
                                bytes += lposEnd - lposStart;
                            }
                        }
                        i++;
                    }
                    metfile -> SortGaplist();
                    metfile -> WriteTo();
                    delete metfile;
                    delete fpPRT;
                }
            }
            delete hashPRT;
        }
    }
}

void CminerDlg::Repair_METfile()
{
    sub_Repair_METfile(filename);
}

void CminerDlg::CreatePartfileMET(char * partfilename, char * filepath, char * filename)
{
    FILE * tf = fopen(partfilename, "a");
    if (tf)
    {
        fclose(tf);
    }
}

void CminerDlg::Create_ISMfile(char * infofilename, char * filepath, char * filename)
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
            mfile -> WriteHashtable(tf);
            fprintf(tf, "---------------------------------------------------------------------------------\n");
        }
        mfile -> WriteGaplist(tf);
        fprintf(tf, "=================================================================================\n");
        fclose(tf);
    }
}

void CminerDlg::W(const char * text)
{
    tctrl_ARC -> AppendText(text);
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
    memset(matchdomain, 0, 65);
    strcpy(matchdomain, tctrl_MATCHDOMAIN -> GetValue() .c_str());
    lmd = strlen(matchdomain);
    memset(matchurl, 0, 65);
    strcpy(matchurl, tctrl_MATCHURL -> GetValue() .c_str());
    lmu = strlen(matchurl);
    memset(matchstart, 0, 65);
    strcpy(matchstart, tctrl_MATCHSTART -> GetValue() .c_str());
    lms = strlen(matchstart);
    switch (basisfunktion)
    {
    case 1:
        AddingDir();
        break;
    case 2:
        CreatingFiletree();
        break;
    }
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

void CminerDlg::Process_JPGfile(char * filename)
{
    int psize;
    int asciiPointer = 0;
    file_width = 0;
    file_height = 0;
    count_JPGfilesprocessed++;
    Cjpgfile * jpgfile = new Cjpgfile(filepath, filename);
    psize = jpgfile -> getFilesize();
    if (psize > 32)
    {
        jpgfile -> ReadInfos();
        if (jpgfile -> isAscii())
        {
            asciiPointer++;
        }
        else
        {
            file_width = jpgfile -> getWidth();
            file_height = jpgfile -> getHeight();
        }
    }
    if (psize)
    {
        wxFile * pf = NULL;
        int i = 0, l = 0;
        char hash1[33], hash2[33];
        sprintf(buffer, "%s%s", filepath, filename);
        if (psize > 32)
        {
            if (!file_width)
            {
                if (asciiPointer)
                {
                    FILE * fpasc = fopen("proto_a.txt", "a+");
                    (void) fprintf(fpasc, "jpg-asciifile(%d): %s\n"
                    , psize, sourcename);
                    fclose(fpasc);
                    HashAndRename(sourcename, "txt", true);
                }
                else
                {
                    FILE * fpasc = fopen("proto_a.txt", "a+");
                    (void) fprintf(fpasc, "jpg-asciis(%d<%d): %s\n"
                    , asciiPointer, psize, sourcename);
                    fclose(fpasc);
                }
            }
            else if(!jpgfile -> hasEndmark())
            {
                FILE * fpasc = fopen("proto_defekte.txt", "a+");
                (void) fprintf(fpasc, "jpg-file(%d): %s\n"
                , psize, sourcename);
                fclose(fpasc);
                HashAndRename(sourcename, "dkt", true);
            }
            else
            {
                if (file_width > 9999)
                {
                    file_width = 9999;
                }
                if (file_height > 9999)
                {
                    file_height = 9999;
                }
                memset(keybuffer, 0, 33);
                l = strlen(theApp -> sourcedirectory.c_str());
                strcpy(tempname, TARGETDIR);
                strcpy(tempname + strlen(tempname), sourcename + l);
                l = strlen(TARGETDIR);
                hash -> hashFile(sourcename, false, p_ED2KhashMD4);
                memcpy(dbPIkey.ptr, hash -> blockhash, 16);
                dbPIdata = dbPI -> fetchKey(1, dbPIkey);
                if (!dbPIdata.ptr)
                {
                    dbPIdata.size = strlen(tempname) + 1 + 10;
                    dbPIdata.ptr = (unsigned char *) malloc(dbPIdata.size + 1);
                    sprintf((char *) dbPIdata.ptr, "R%04dx%04d%s"
                    , file_width, file_height, tempname);
                    (void) dbPI -> storeData(1, dbPIkey, dbPIdata);
                }
                else if(dbPIdata.ptr[0] != 'R')
                {
                    if (dbPIdata.size > 10)
                    {
                        char t1[11];
                        sprintf(t1, "R%04dx%04d", file_width, file_height);
                        memcpy(dbPIdata.ptr, t1, 10);
                        (void) dbPI -> updateData(1, dbPIkey, dbPIdata);
                    }
                }
                for (i = 0 ; i < 16 ; i++)
                {
                    sprintf(hash1 + i * 2, "%02x", ((unsigned char *) hash -> blockhash) [i]);
                }
                if (p_JPGexportMD4)
                {
                    strcpy(buffer, tempname);
                    i = l + 1;
                    l = strlen(buffer);
                    while (i < l)
                    {
                        if ((buffer[i] == '/') || (buffer[i] == '\\'))
                        {
                            buffer[i] = 0;
                            i = l;
                        }
                        i++;
                    }
                    int l;
                    if (p_JPGcreatePDS)
                    {
                        pds_filecount++;
                        i = (psize >> 10);
                        i <<= 10;
                        size_PDSfiles += i;
                        i = 0;
                        if (size_PDSfiles > 0x2bb00000)
                        {
                            size_PDSfiles = i;
                            pds_count++;
                            if (pds_count > 999999)
                            {
                                pds_count = 1;
                            }
                            pds_filecount = 1;
                        }
                        size_PDSfiles += 2560;
                        if ((rand() % 10) == 7)
                        {
                            sprintf(tempname
                            , "Subdirs: %ld  Files: %ld  PDScount: %d  PDSfiles: %d  PDSbytes: %ld/%lu"
                            , count_subdirsprocessed, count_ALLfiles, pds_count, pds_filecount
                            , size_PDSfiles, size_ALLfiles);
                            wxMutexGuiEnter();
                            stext_C1 -> SetLabel(tempname);
                            wxMutexGuiLeave();
                        }
                    }
                    else
                    {
                        i = 2;
                    }
                    while (i < 4)
                    {
                        if (i)
                        {
                            if (stat(buffer, statbuffer))
                            {
                                if (errno == ENOENT)
                                {
#ifdef __WIN32__
                                    l = mkdir(buffer);
#else
                                    l = mkdir(buffer, 777);
#endif
                                }
                            }
                        }
                        i++;
                        switch (i)
                        {
                        case 1:
                            strcpy(tempname, buffer + strlen(TARGETDIR));
                            sprintf(buffer + strlen(TARGETDIR), "/oe%06d", pds_count);
                            break;
                        case 2:
                            strcpy(buffer + strlen(buffer), tempname);
                            break;
                        case 3:
                            sprintf(buffer + strlen(buffer), "/%dx%d", file_width, file_height);
                            break;
                        case 4:
                            sprintf(buffer + strlen(buffer), "/");
                            if (p_JPGaddFNtoTA)
                            {
                                char * fn;
                                fn = htmp0 -> extractFilename((char *)(dbPIdata.ptr + 10));
                                if (fn[0])
                                {
                                    sprintf(buffer + strlen(buffer), "%s.", fn);
                                }
                            }
                            if (p_JPGaddRStoFN)
                            {
                                sprintf(buffer + strlen(buffer), "%dx%d."
                                , file_width, file_height);
                            }
                            sprintf(buffer + strlen(buffer), "%s.jpg", hash1);
                            break;
                        }
                    }
                    pf = new wxFile(buffer, wxFile::write);
                    pf -> Write(jpgfile -> getPicbuf(), jpgfile -> getFilesize());
                    delete pf;
                    hash -> hashFile(buffer, false, p_ED2KhashMD4);
                    for (i = 0 ; i < 16 ; i++)
                    {
                        sprintf(hash2 + i * 2, "%02x", ((unsigned char *) hash -> blockhash) [i]);
                    }
                    mil = dbPI -> GetMaxIndexLevel(1);
                    if (mil != lmil)
                    {
                        //printf("IL%02d: (%d)%s\n", mil, psize, buffer);
                        lmil = mil;
                    }
                    if (!strcmp(hash1, hash2))
                    {
                        count_filesmoved++;
                        if (p_JPGwrbackMD4)
                        {
                            pf = new wxFile(sourcename, wxFile::write);
                            pf -> Write(hash1, 32);
                            delete pf;
                        }
                    }
                }
                free(dbPIdata.ptr);
                dbPIdata.ptr = NULL;
            }
        }
        else if((psize == 32) && p_JPGimportMD4)
        {
            wxFile * fpi = new wxFile(buffer, wxFile::read);
            if (fpi)
            {
                memset(hash1, 0, 33);
                if (fpi -> IsOpened())
                {
                    fpi -> Read(hash1, 32);
                }
                delete fpi;
                from32to16(hash1, dbPIkey.ptr);
                dbPIdata = dbPI -> fetchKey(1, dbPIkey);
                if (dbPIdata.ptr)
                {
                    char t1[11];
                    memset(t1, 0, 11);
                    strncpy(t1, (char *) dbPIdata.ptr, 10);
                    file_width = atoi(t1 + 1);
                    file_height = atoi(t1 + 6);
                    printf("importing(%s,%dx%d): %s\n", hash1, file_width, file_height, sourcename);
                    l = strlen(theApp -> sourcedirectory.c_str());
                    strcpy(buffer, TARGETDIR);
                    strcpy(buffer + strlen(buffer), sourcename + l);
                    l = strlen(TARGETDIR);
                    i = l + 1;
                    l = strlen(buffer);
                    while (i < l)
                    {
                        if ((buffer[i] == '/') || (buffer[i] == '\\'))
                        {
                            buffer[i] = 0;
                            i = l;
                        }
                        i++;
                    }
                    i = 0;
                    while (i < 2)
                    {
                        i++;
                        switch (i)
                        {
                        case 1:
                            sprintf(buffer + strlen(buffer), "/%dx%d", file_width, file_height);
                            break;
                        case 2:
                            sprintf(buffer + strlen(buffer), "/%s.jpg", hash1);
                            break;
                        }
                    }
                    if (!stat(buffer, statbuffer))
                    {
                        psize = statbuffer -> st_size;
                        unsigned char * picbuf = (unsigned char *) malloc(psize + 1);
                        pf = new wxFile(buffer, wxFile::read);
                        pf -> Read(picbuf, psize);
                        delete pf;
                        sprintf(tempname, "%s%s", filepath, filename);
                        pf = new wxFile(tempname, wxFile::write);
                        pf -> Write(picbuf, psize);
                        delete pf;
                        free(picbuf);
                        picbuf = NULL;
                    }
                    delete dbPIdata.ptr;
                    dbPIdata.ptr = NULL;
                }
            }
        }
    }
    delete jpgfile;
    jpgfile = NULL;
}

void CminerDlg::Process_GIFfile(char * filename)
{
    count_GIFfilesprocessed++;
    wxFile * pf = NULL;
    int l1, l2, psize;
    unsigned char * picbuf;
    int asciiPointer;
    int i = 0, l = 0;
    char hash1[33], hash2[33];
    l1 = strlen(filepath);
    l2 = strlen(filename);
    if ((l1 + l2) < 1025)
    {
        sprintf(buffer, "%s%s", filepath, filename);
        if (!stat(buffer, statbuffer))
        {
            psize = statbuffer -> st_size;
            if (psize > 32)
            {
                if (psize < 40960000)
                {
                    file_width = 0;
                    file_height = 0;
                    picbuf = (unsigned char *) malloc(psize + 1);
                    wxFile * fpi = new wxFile(buffer, wxFile::read);
                    if (fpi)
                    {
                        int bytes = 0, block = 0;
                        if (fpi -> IsOpened())
                        {
                            do
                            {
                                block = psize - bytes;
                                if (block > 1024)
                                {
                                    block = 1024;
                                }
                                bytes += fpi -> Read(picbuf + bytes, block);
                            }
                            while (!fpi -> Eof());
                        }
                        delete fpi;
                        //printf("GIFload(%d): %s\n", bytes, filename);
                        i = 0;
                        while (i < bytes)
                        {
                            switch (i)
                            {
                            case 0:
                                //header
                                if (!strncasecmp((const char *) picbuf, "gif", 3))
                                {
                                    i += 6;
                                }
                                else
                                {
                                    i = bytes;
                                }
                                break;
                            case 6:
                                //screener
                                file_width = picbuf[i] + picbuf[i + 1] * 256;
                                i += 2;
                                break;
                            case 8:
                                file_height = picbuf[i] + picbuf[i + 1] * 256;
                                bytes = - bytes;
                                break;
                            default:
                                i++;
                                break;
                            }
                        }
                        asciiPointer = 0;
                        if (!file_width)
                        {
                            if (bytes < 0)
                            {
                                bytes = - bytes;
                            }
                            asciiPointer = filetestASCII(picbuf, bytes);
                            if (asciiPointer == bytes)
                            {
                                FILE * fpasc = fopen("proto_a.txt", "a+");
                                (void) fprintf(fpasc, "gif-asciifile(%d): %s\n"
                                , bytes, sourcename);
                                fclose(fpasc);
                                HashAndRename(sourcename, "txt", true);
                            }
                            else
                            {
                                FILE * fpasc = fopen("proto_a.txt", "a+");
                                (void) fprintf(fpasc, "gif-asciis(%d<%d): %s\n"
                                , asciiPointer, bytes, sourcename);
                                fclose(fpasc);
                            }
                        }
                        else if(bytes > 0)
                        {
                            FILE * fpasc = fopen("proto_defekte.txt", "a+");
                            (void) fprintf(fpasc, "gif-file(%d): %s\n"
                            , bytes, sourcename);
                            fclose(fpasc);
                            HashAndRename(sourcename, "dkt", true);
                        }
                        if (bytes < - 32)
                        {
                            if (file_width > 9999)
                            {
                                file_width = 9999;
                            }
                            if (file_height > 9999)
                            {
                                file_height = 9999;
                            }
                            memset(keybuffer, 0, 33);
                            bytes = - bytes;
                            l = strlen(theApp -> sourcedirectory.c_str());
                            strcpy(tempname, TARGETDIR);
                            strcpy(tempname + strlen(tempname), sourcename + l);
                            l = strlen(TARGETDIR);
                            hash -> hashFile(sourcename, false, p_ED2KhashMD4);
                            memcpy(dbPIkey.ptr, hash -> blockhash, 16);
                            dbPIdata = dbPI -> fetchKey(1, dbPIkey);
                            if (!dbPIdata.ptr)
                            {
                                dbPIdata.size = strlen(tempname) + 1 + 10;
                                dbPIdata.ptr = (unsigned char *) malloc(dbPIdata.size + 1);
                                sprintf((char *) dbPIdata.ptr, "R%04dx%04d%s"
                                , file_width, file_height, tempname);
                                (void) dbPI -> storeData(1, dbPIkey, dbPIdata);
                            }
                            else if(dbPIdata.ptr[0] != 'R')
                            {
                                if (dbPIdata.size > 10)
                                {
                                    char t1[11];
                                    sprintf(t1, "R%04dx%04d", file_width, file_height);
                                    memcpy(dbPIdata.ptr, t1, 10);
                                    (void) dbPI -> updateData(1, dbPIkey, dbPIdata);
                                }
                            }
                            free(dbPIdata.ptr);
                            dbPIdata.ptr = NULL;
                            for (i = 0 ; i < 16 ; i++)
                            {
                                sprintf(hash1 + i * 2, "%02x", ((unsigned char *) hash -> blockhash) [i]);
                            }
                            if (p_GIFexportMD4)
                            {
                                strcpy(buffer, tempname);
                                i = l + 1;
                                l = strlen(buffer);
                                while (i < l)
                                {
                                    if ((buffer[i] == '/') || (buffer[i] == '\\'))
                                    {
                                        buffer[i] = 0;
                                        i = l;
                                    }
                                    i++;
                                }
                                int l;
                                i = 0;
                                while (i < 2)
                                {
                                    if (stat(buffer, statbuffer))
                                    {
                                        if (errno == ENOENT)
                                        {
#ifdef __WIN32__
                                            l = mkdir(buffer);
#else
                                            l = mkdir(buffer, 777);
#endif
                                        }
                                    }
                                    i++;
                                    switch (i)
                                    {
                                    case 1:
                                        sprintf(buffer + strlen(buffer), "/%dx%d", file_width, file_height);
                                        break;
                                    case 2:
                                        sprintf(buffer + strlen(buffer), "/%s.gif", hash1);
                                        break;
                                    }
                                }
                                pf = new wxFile(buffer, wxFile::write);
                                pf -> Write(picbuf, psize);
                                delete pf;
                                hash -> hashFile(buffer, false, p_ED2KhashMD4);
                                for (i = 0 ; i < 16 ; i++)
                                {
                                    sprintf(hash2 + i * 2, "%02x", ((unsigned char *) hash -> blockhash) [i]);
                                }
                                mil = dbPI -> GetMaxIndexLevel(1);
                                if (mil != lmil)
                                {
                                    //printf("IL%02d: (%d)%s\n", mil, psize, buffer);
                                    lmil = mil;
                                }
                                if (!strcmp(hash1, hash2))
                                {
                                    count_filesmoved++;
                                    if (p_GIFwrbackMD4)
                                    {
                                        pf = new wxFile(sourcename, wxFile::write);
                                        pf -> Write(hash1, 32);
                                        delete pf;
                                    }
                                }
                            }
                        }
                    }
                    free(picbuf);
                }
            }
            else if((psize == 32) && p_GIFimportMD4)
            {
                wxFile * fpi = new wxFile(buffer, wxFile::read);
                if (fpi)
                {
                    memset(hash1, 0, 33);
                    if (fpi -> IsOpened())
                    {
                        fpi -> Read(hash1, 32);
                    }
                    delete fpi;
                    from32to16(hash1, dbPIkey.ptr);
                    dbPIdata = dbPI -> fetchKey(1, dbPIkey);
                    if (dbPIdata.ptr)
                    {
                        char t1[11];
                        memset(t1, 0, 11);
                        strncpy(t1, (char *) dbPIdata.ptr, 10);
                        file_width = atoi(t1 + 1);
                        file_height = atoi(t1 + 6);
                        printf("importing(%s,%dx%d): %s\n", hash1, file_width, file_height, sourcename);
                        l = strlen(theApp -> sourcedirectory.c_str());
                        strcpy(buffer, TARGETDIR);
                        strcpy(buffer + strlen(buffer), sourcename + l);
                        l = strlen(TARGETDIR);
                        i = l + 1;
                        l = strlen(buffer);
                        while (i < l)
                        {
                            if ((buffer[i] == '/') || (buffer[i] == '\\'))
                            {
                                buffer[i] = 0;
                                i = l;
                            }
                            i++;
                        }
                        i = 0;
                        while (i < 2)
                        {
                            i++;
                            switch (i)
                            {
                            case 1:
                                sprintf(buffer + strlen(buffer), "/%dx%d", file_width, file_height);
                                break;
                            case 2:
                                sprintf(buffer + strlen(buffer), "/%s.gif", hash1);
                                break;
                            }
                        }
                        if (!stat(buffer, statbuffer))
                        {
                            psize = statbuffer -> st_size;
                            picbuf = (unsigned char *) malloc(psize + 1);
                            pf = new wxFile(buffer, wxFile::read);
                            pf -> Read(picbuf, psize);
                            delete pf;
                            sprintf(tempname, "%s%s", filepath, filename);
                            pf = new wxFile(tempname, wxFile::write);
                            pf -> Write(picbuf, psize);
                            delete pf;
                            free(picbuf);
                            picbuf = NULL;
                        }
                        delete dbPIdata.ptr;
                        dbPIdata.ptr = NULL;
                    }
                }
            }
        }
    }
}

void CminerDlg::GetFilename(char * filename, char * sourcepath)
{
    int i;
    i = strlen(sourcepath);
    while (i > 0)
    {
        i--;
        switch (sourcepath[i])
        {
        case '/':
        case '\\':
            strcpy(filename, sourcepath + i);
            i = - i;
            break;
        }
    }
}

int CminerDlg::filetestASCII(unsigned char * buffer, int bufferlength)
{
    int pointer = 0;
    int b, i = 0;
    while (i < bufferlength)
    {
        b = buffer[i++];
        if ((b >= 'a') && (b <= 'z'))
        {
            pointer++;
        }
        else if((b >= 'A') && (b <= 'Z'))
        {
            pointer++;
        }
        else if((b >= '0') && (b <= '9'))
        {
            pointer++;
        }
        else if(strchr("@ \\| ~§$&%`'!#*+_-/.,;:=><(){}?ßöäüÖÄÜ", b))
        {
            pointer++;
        }
        else
        {
            switch (b)
            {
            case 9:
            case 10:
            case 13:
            case 34:
            case 0x92:
                pointer++;
                break;
            default:
                //printf("pic=%02x\n", b);
                break;
            }
        }
    }
    return pointer;
}

void CminerDlg::Wgui(const char * zeile)
{
    guilinecount++;
    wxMutexGuiEnter();
    if (guilinecount > 18)
    {
        guilinecount = 1;
        tctrl_ARC -> Clear();
    }
    tctrl_ARC -> AppendText(zeile);
    wxMutexGuiLeave();
}

unsigned int CminerDlg::GetUint32(Cavifile * source, unsigned char * buffer)
{
    int count = 0;
    unsigned int uint32 = 0;
    memset(buffer, 0, 5);
    while ((count < 4) && (!source -> Eof()))
    {
        (void) source -> Read(buffer + count, 1);
        uint32 = (uint32 << 8) | buffer[count++];
    }
    return uint32;
}

unsigned int CminerDlg::GetUint32i(Cavifile * source, unsigned char * buffer)
{
    int count = 0;
    unsigned int uint32 = 0;
    memset(buffer, 0, 5);
    while ((count < 4) && (!source -> Eof()))
    {
        (void) source -> Read(buffer + count, 1);
        uint32 = (uint32 >> 8) | (buffer[count++] << 24);
    }
    return uint32;
}

int CminerDlg::filterNames(char *& filename, char *& sourcename)
{
    int l = 1;
    if (dirbufferDeep == 1)
    {
        if (lms)
        {
            if (strstr(sourcename, matchstart))
            {
                lms = 0;
            }
        }
        if (!lms)
        {
            if (lmd)
            {
                if (!strstr(filename, matchdomain))
                {
                    l = 0;
                }
                else
                {
                    printf("matchbase: %s\n", sourcename);
                }
            }
        }
        else
        {
            l = 0;
        }
    }
    else if(!lms)
    {
        if (lmu)
        {
            if (!strstr(sourcename, matchurl))
            {
                l = 0;
            }
            else
            {
                printf("matchall: %s\n", sourcename);
            }
        }
    }
    else
    {
        l = 0;
    }
    if (l)
    {
        if (p_debugonoff)
        {
            sprintf(buffer, "%s(%ld)\n", sourcename
            , (long) statbuffer -> st_size);
            Wgui(buffer);
        }
    }
    return l;
}

void CminerDlg::executeFiletype()
{
    hash -> hashtableindex = 0;
    filetype = htmp0 -> GetFiletype(filename);
    fileextension = htmp0 -> GetExtension();
    count_ALLfiles++;
    filesize = statbuffer -> st_size;
    size_ALLfiles += filesize;
    if (filesize > 500000)
    {
        sprintf(buffer, "%ld: %s\n", filesize, sourcename);
        FILE * fp = fopen("bigfile.log", "a");
        fprintf(fp, buffer);
        fclose(fp);
    }
    filepath = dirbuffer[dirbufferDeep];
    timet0 = time(NULL);
    switch (filetype)
    {
    case 1:
        if (!p_removedupext)
        {
            Process_STDfile(filename);
        }
        break;
    case 4:
        if (p_delHtmPxxCxx || p_simHtmPxxCxx)
        {
            if (htmp0 -> GetFilesubtype() < 99)
            {
                if (!p_simHtmPxxCxx)
                {
                    (void) unlink(sourcename);
                }
                shp_fullsize += statbuffer -> st_size;
                sprintf(buffer, "del(%ld):%s\n"
                , shp_fullsize, sourcename);
                Wgui(buffer);
            }
        }
        else if(!p_removedupext)
        {
            switch (htmp0 -> GetFilesubtype())
            {
            case 0:
                Process_STDfile(filename);
                break;
            case 101:
                Process_PRYfile(filename);
                break;
            default:
                if (htmp0 -> GetFilesubtype() < 99)
                {
                    if (p_HTMparse)
                    {
                        Process_HTMfile(filename);
                    }
                    else
                    {
                        Process_STDfile(filename);
                    }
                }
                else
                {
                    Process_STDfile(filename);
                }
                break;
            }
        }
        break;
    case 5:
        if (p_delHtmPxxCxx || p_simHtmPxxCxx)
        {
            if (htmp0 -> GetFilesubtype() < 99)
            {
                if (!p_simHtmPxxCxx)
                {
                    (void) unlink(sourcename);
                }
                shp_fullsize += statbuffer -> st_size;
                sprintf(buffer, "del(%ld):%s\n"
                , shp_fullsize, sourcename);
                Wgui(buffer);
            }
        }
        else if(!p_removedupext)
        {
            if (p_WD3parse)
            {
                Process_WD3file(filename);
            }
        }
        break;
    case 6:
        //vidfiles
        if (!p_removedupext)
        {
            switch (htmp0 -> GetFilesubtype())
            {
            case 1:
                if (p_AVIparse)
                {
                    Process_AVIfile(filename);
                }
                break;
            }
            if (p_VIDexportMD4 | p_VIDimportMD4)
            {
                Process_VIDEOfile(dirbuffer[dirbufferDeep], filename);
            }
            else
            {
                Process_STDfile(filename);
            }
        }
        break;
    case 7:
        if (!p_removedupext)
        {
            switch (htmp0 -> GetFilesubtype())
            {
            case 1:
                if (p_JPGparse)
                {
                    Process_JPGfile(filename);
                }
                else
                {
                    Process_STDfile(filename);
                }
                break;
            case 3:
                if (p_GIFparse)
                {
                    Process_GIFfile(filename);
                }
                else
                {
                    Process_STDfile(filename);
                }
                break;
            default:
                Process_STDfile(filename);
                break;
            }
        }
        break;
    case 8:
        //.part-files
        if (p_PRTparse)
        {
            Process_PRTfile();
        }
        else
        {
            Process_STDfile(filename);
        }
        break;
    case 9:
        //.met-files
        if (p_METparse)
        {
            Process_METfile(filename);
        }
        else
        {
            Process_STDfile(filename);
        }
        break;
    case 11:
        if (p_DKTparse)
        {
            Process_DKTfile(dirbuffer[dirbufferDeep], filename);
        }
        else
        {
            Process_STDfile(filename);
        }
        //dup-files:
    case 91:
        Process_DUPfile();
        break;
    case 92:
        //.bak-files
        break;
    case 93:
        //.ism-files(infofiles)
        if (p_ISMparse)
        {
            Process_ISMfile();
        }
        else
        {
            Process_STDfile(filename);
        }
        break;
    default:
        break;
    }
}

void CminerDlg::enterSubdir(const char * subdirname)
{
    if (dirbufferDeep == 1)
    {
        FILE * fp = fopen("dirproto.txt", "a");
        fprintf(fp, "%s\n", sourcename);
        fclose(fp);
    }
    dirbufferDeep++;
    if (dirbuffer[dirbufferDeep])
    {
        free(dirbuffer[dirbufferDeep]);
    }
    dirbuffer[dirbufferDeep] =
    (char *) malloc(strlen(dirbuffer[dirbufferDeep - 1]) + strlen(subdirname) + 2);
    sprintf(dirbuffer[dirbufferDeep], "%s%s/"
    , dirbuffer[dirbufferDeep - 1], subdirname);
    if (subdir[dirbufferDeep])
    {
        closedir(subdir[dirbufferDeep]);
    }
    subdir[dirbufferDeep] = opendir(dirbuffer[dirbufferDeep]);
    if (!subdir[dirbufferDeep])
    {
        free(dirbuffer[dirbufferDeep]);
        dirbuffer[dirbufferDeep] = (char *) malloc(1);
        dirbuffer[dirbufferDeep][0] = 0;
        dirbufferDeep--;
    }
}

void CminerDlg::leaveSubdir()
{
    if (subdir[dirbufferDeep])
    {
        closedir(subdir[dirbufferDeep]);
        subdir[dirbufferDeep] = NULL;
        sprintf(buffer
        , "Subdirs: %ld  Files: %ld  PDScount: %d  PDSfiles: %d  PDSbytes: %ld/%lu"
        , count_subdirsprocessed, count_ALLfiles, pds_count
        , pds_filecount, size_PDSfiles, size_ALLfiles);
        wxMutexGuiEnter();
        stext_C1 -> SetLabel(buffer);
        wxMutexGuiLeave();
    }
    dirbufferDeep--;
}

void CminerDlg::sampleGetBase()
{
    int l = 0, i = 0, status = 0;
    unsigned int j;
    if (!name0deep)
    {
        l = strlen(filename);
        if (l)
        {
            --l;
            if (filename[l] == ')')
            {
                //specialdirfile
                i = l;
                do
                {
                    i--;
                    if (filename[i] == '(')
                    {
                        if (!status)
                        {
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
                        }
                    }
                }
                while (i > 0);
                if (strlen(name0type))
                {
                    if (!strlen(name0))
                    {
                        celdeep = dirbufferDeep;
                    }
                    else if(!name0deep)
                    {
                        name0count = 0;
                        name0fcount = 0;
                        name0deep = dirbufferDeep;
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

void CminerDlg::storeSubhash(int mode, unsigned char * subHash, unsigned char * mainHash, char * datas)
{
    memset(dbSHkey.ptr, 0, 33);
    switch (mode)
    {
    case 1:
        memcpy(dbSHkey.ptr, subHash, 16);
        memcpy(dbSHkey.ptr + 16, mainHash, 16);
        break;
    default:
        memcpy(dbSHkey.ptr, mainHash, 16);
        break;
    }
    dbSHdata = dbSH -> fetchKey(1, dbSHkey);
    if (!dbSHdata.ptr)
    {
        dbSHdata.size = strlen(datas) + 1;
        dbSHdata.ptr = (unsigned char *) malloc(dbSHdata.size + 1);
        memset(dbSHdata.ptr, 0, dbSHdata.size);
        memcpy(dbSHdata.ptr, datas, dbSHdata.size);
        (void) dbSH -> storeData(1, dbSHkey, dbSHdata);
    }
    free(dbSHdata.ptr);
    dbSHdata.ptr = NULL;
}

int CminerDlg::checkSubhash(unsigned char * subHash, int partid)
{
    int retcode;
    memset(buffer, 0, BUFFERSIZE);
    memset(dbSHkey.ptr, 0, 33);
    memcpy(dbSHkey.ptr, subHash, 16);
    do
    {
        retcode = 0;
        dbSHdata = dbSH -> getNextKey(1, dbSHkey);
        if (dbSHdata.ptr)
        {
            if (!memcmp(dbSHkey.ptr, subHash, 16))
            {
                if (dbSHdata.ptr[0] > '4')
                {
                    retcode = 1 + atoi((char *) dbSHdata.ptr + 2);
                }
                else
                {
                    retcode = - 1;
                }
                free(dbSHdata.ptr);
                dbSHdata.ptr = NULL;
                if (retcode == (1 + partid))
                {
                    memcpy(dbSHkey.ptr, dbSHkey.ptr + 16, 16);
                    memset(dbSHkey.ptr + 16, 0, 17);
                    dbSHdata = dbSH -> fetchKey(1, dbSHkey);
                    if (dbSHdata.ptr)
                    {
                        if (filesize > atol((char *) dbSHdata.ptr + 2))
                        {
                            retcode = - 1;
                        }
                        else if(dbSHdata.size > BUFFERSIZE)
                        {
                            memcpy(buffer, (char *) dbSHdata.ptr, BUFFERSIZE);
                        }
                        else
                        {
                            memcpy(buffer, (char *) dbSHdata.ptr, dbSHdata.size);
                        }
                        free(dbSHdata.ptr);
                        dbSHdata.ptr = NULL;
                    }
                }
                else
                {
                    retcode = - 1;
                }
            }
        }
    }
    while (retcode < 0);
    return retcode;
}

