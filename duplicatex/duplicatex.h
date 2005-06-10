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

#ifdef __WXGTK__
#define SMWX
#define GTK1
#else
#define SMWX
#endif

#define NOHELP
#define NOAUDIO

extern int debug;

#ifndef DUPLICATEX_H
#define DUPLICATEX_H

#include <wx/wx.h>
#include <wx/dir.h>
#include <wx/filefn.h>
#include <wx/socket.h>
#include <wx/notebook.h>

//#include "gdbm.h"

#define theApp	((smApp*)wxTheApp)

#include "minerdlg.h"

class smApp: public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();
	void Dbg(char *line);
	
	CminerDlg *minerdlg;
	wxNotebook *smNB;
	wxFrame *smFrame;

protected:
	void LoadParameters(char *parameterfile);
	void SaveParameters(char *parameterfile);
	void ProcessCommandline();
	void SetLocalIP(const char *ipv4);
public:
	wxNotebook *smNotebook;
	wxString targetdirectory;
	wxString sourcedirectory;
private:
	FILE *dbgfile; //Debugfile
	wxIPV4address smipv4;
	char *smFullLocalIP;
	unsigned long smLocalIP;
	wxString smHostname;
	char *smStorageNumber;
};
#endif
