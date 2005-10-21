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

#include <wx/wx.h>
#include <wx/image.h>

#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#ifdef __WIN32__
#include <winsock.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include "duplicatex.h"

IMPLEMENT_APP(smApp)

int debug = 0;

bool smApp::OnInit()
{
    sourcedirectory = "./";
#ifdef __WIN32__
    targetdirectory = "c:/temp";
#else
    targetdirectory = "/tmp";
#endif
    wxInitAllImageHandlers();
    LoadParameters("duplicatex.par");
    ProcessCommandline();
    dbgfile = fopen("duplicatex.out", "w+r");
    if (dbgfile)
    {
        smFrame = new wxFrame(NULL, 10001, "Duplicatex v1.0.3", wxDefaultPosition, wxSize(768, 556));
        minerdlg = new CminerDlg(smFrame);
        if (smipv4.AnyAddress())
        {
            smHostname = smipv4.Hostname();
            fprintf(dbgfile, "OnInit_start\n");
            SetLocalIP(smFullLocalIP);
            if (!wxDirExists(sourcedirectory))
            {
                if (!wxMkdir(sourcedirectory, 0777))
                {
                    sourcedirectory="";
                }
            }
            if (!wxDirExists(targetdirectory))
            {
                if (!wxMkdir(targetdirectory, 0777))
                {
                    targetdirectory="";
                }
            }
            SaveParameters("duplicatex.par");
            minerdlg -> SetHostname((char *) smHostname.c_str());
            minerdlg -> SetLocalIP(smFullLocalIP);
        }
        smFrame -> SetSizeHints(768, 556, 1024, 768);
        smFrame -> Show(true);
    }
    return true;
}

int smApp::OnExit()
{
    SaveParameters("duplicatex.par");
    if (dbgfile)
    {
        fprintf(dbgfile, "OnExit\n");
        fclose(dbgfile);
    }
    return 0;
}

void smApp::Dbg(char * line)
{
    if (dbgfile)
    {
        fprintf(dbgfile, "%s\n", line);
    }
}

void smApp::LoadParameters(char * parameterfile)
{
    char * buffer = (char *) malloc(1025), byte[2];
    int bytecount = 0, charcount = 0;
    FILE * parfile = fopen(parameterfile, "r");
    smFullLocalIP = NULL;
    smStorageNumber = NULL;
    if (parfile)
    {
        while (!feof(parfile))
        {
            bytecount = fread( & byte, 1, 1, parfile);
            if (bytecount)
            {
                switch (byte[0])
                {
                case 0:
                case 10:
                case 13:
                    break;
                default:
                    buffer[charcount++] = byte[0];
                    break;
                }
                if ((charcount > 999) || (byte[0] == 10))
                {
                    buffer[charcount] = 0;
                    if (!strncmp(buffer, "-i:", 3))
                    {
                        smFullLocalIP = new char[charcount - 2];
                        sprintf(smFullLocalIP, "%s", buffer + 3);
                    }
                    else if(!strncmp(buffer, "-s:", 3))
                    {
			char tmp[charcount-1];
			memset(tmp,0,charcount);
                        strncpy(tmp, buffer + 3,charcount-2);
                        sourcedirectory = wxString(tmp);
                    }
                    else if(!strncmp(buffer, "-t:", 3))
                    {
			char tmp[charcount-1];
			memset(tmp,0,charcount);
                        strncpy(tmp, buffer + 3,charcount-2);
                        targetdirectory = wxString(tmp);
                    }
                    charcount = 0;
                }
            }
        }
        fclose(parfile);
    }
    if (!smFullLocalIP)
    {
        smFullLocalIP = new char[16];
        sprintf(smFullLocalIP, "%s", "127.0.0.1");
    }
}

void smApp::SaveParameters(char * parameterfile)
{
    FILE * parfile = fopen(parameterfile, "w+r");
    if (parfile)
    {
        char * buffer = new char[1025];
        int bytecount;
        sprintf(buffer, "-i:%s\n", smFullLocalIP);
        bytecount = fwrite(buffer, 1, strlen(buffer), parfile);
        sprintf(buffer, "-s:%s\n", sourcedirectory.c_str());
        bytecount = fwrite(buffer, 1, strlen(buffer), parfile);
        sprintf(buffer, "-t:%s\n", targetdirectory.c_str());
        bytecount = fwrite(buffer, 1, strlen(buffer), parfile);
        fclose(parfile);
    }
}

void smApp::ProcessCommandline()
{
    char * parameter, * parnext;
    int i = 0;
    while (i < argc)
    {
        i++;
        parameter = argv[i];
        if (parameter)
        {
            if (parameter[0] == '-')
            {
                parnext = argv[i + 1];
                switch (parameter[1])
                {
                    //localip:
                case 'i':
                    if (parnext && (parnext[0] != '-'))
                    {
                        if (smFullLocalIP)
                        {
                            delete[] smFullLocalIP;
                        }
                        smFullLocalIP = new char[strlen(parnext) + 1];
                        sprintf(smFullLocalIP, "%s", parnext);
                        i++;
                    }
                    break;
                    //current storage-number:
                case 'n':
                    if (parnext && (parnext[0] != '-'))
                    {
                        smStorageNumber = new char[strlen(parnext) + 1];
                        sprintf(smStorageNumber, "%s", parnext);
                        i++;
                    }
                    break;
                    //sourcedirectory:
                case 's':
                    if (parnext && (parnext[0] != '-'))
                    {
                        sourcedirectory=wxString(parnext);
                        i++;
                    }
                    break;
                    //targetdirectory:
                case 't':
                    if (parnext && (parnext[0] != '-'))
                    {
                        targetdirectory=wxString(parnext);
                        i++;
                    }
                    break;
                default:
                    {
                        break;
                    }
                }
            }
        }
    }
    if (!smStorageNumber)
    {
        smStorageNumber = new char[2];
        sprintf(smStorageNumber, "%s", "0");
    }
}

void smApp::SetLocalIP(const char * ipv4)
{
    (void) smipv4.Hostname(ipv4);
    struct sockaddr_in sockAddr;
    memset( & sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_addr.s_addr = GAddress_INET_GetHostAddress(smipv4.GetAddress());
    smLocalIP = sockAddr.sin_addr.s_addr;
    if (dbgfile)
    {
        fprintf(dbgfile, "Hostname = %s: %s(%08x)\n", smHostname.c_str(), smFullLocalIP,
        (unsigned int) smLocalIP);
    }
}

