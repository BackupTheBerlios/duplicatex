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

#ifndef HTMLPAGE_H
#define HTMLPAGE_H

#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <wx/file.h>

class Chtmlpage
{
    public:
    Chtmlpage(char * sourcename, int filesize);
    ~ Chtmlpage();
    void ClearAll();
    char *extractFilename(char *source);
    int GetFiletype(char * filename);
    int GetFilesubtype();
    /* *********************************** end of public ********************************** */
    void lowerString(char * makelower);
    void getNextCommandline(char * commandline, int commandline_maxlength);
    void CMDexecute(char * cmd);
    void getNextPar(char * target, int startpos);
    char * getHttpLink();
    char * getHttpLinkUrl();
    char * getHttpLinkPar();
    void clrTextinfo();
    char * getTextinfo();
    char * getFUNKTION();
    char * getRESULT();
    char * getSEARCHSTRING();
    char * getTITLE();
    void mix2Urls(int deep, char * urlmix, char * url1, char * url2);
    void clrCOUNTER();
    void incCOUNTER();
    void setTMP(char * buffer, int length);
    char * getTMP();
    char * getVARNAME();
    char * GetExtension();
    int GetNextParameter(const char * parstack);
    int getTokenLast();
    int getTokenMain();
    int getToken(char * cmdbuffer);
    int parseCommand(char * htmlcode);
    void parseCommandline(char * htmlcode);
    int v_border;
    int v_height;
    int v_width;
    int fsize;
    wxFile * fpHtml;
    char * filename;
    char * fbuffer;
    int fbuffer_pointer;
    char * textinfo;
    char * parzeile;
    char * httplinkurllogin;
    char * httplinkurlpassword;
    char * httplinkparlogin;
    char * httplinkparpassword;
    char * httplink;
    char * httplinkurl;
    char * httplinkpar;
    char * cmd_a_title;
    char * cmd_a_href;
    int token;
    int token_last;
    int token_main;
    int cmdp;
    int txtp;
    int letpointer;
    int GNPcount;
    int GNPloopSTART;
    int GNPloopEND;
    int GNPparsP;
    int GNPreturnCOUNTER;
    char * fileextension;
    char * GNPpars;
    char * GNPparsFUNKTION;
    char * GNPparsIF;
    char * GNPparsSEARCH;
    char * GNPparsTMP;
    char * GNPparsVARNAME;
    char * GNPreturn;
    char * GNPreturnTITLE;
    private:
    int GetExtPosition(char * filepath);
    int filetype;
    int subtype;
    char *targetfilename;
    char *buffer;
    int exblockAnz;
    char *exblock[1025];
    struct stat *statbuffer;
};

#endif

