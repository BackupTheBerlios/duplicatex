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

#ifndef PICFIDLG_H
#define PICFIDLG_H

#include <wx/wx.h>
#include <wx/mstream.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include "metfile.h"
#include "md4hash.h"
#include "database.h"

extern int debug;

class CpicfiDlg:public wxWindow
{
    public:
    CpicfiDlg(wxWindow * parent, int breite, int hoehe);
    ~ CpicfiDlg();
    void Clip(wxImage * temp_jpg);
    void OnPaint(wxPaintEvent & WXUNUSED(event));
    void OnResize(wxSizeEvent & WXUNUSED(event));
    void DrawImage(wxPaintDC * pdc);
    void LoadImage(DatabaseDatum * dataPic);
    void LoadImageGIF(DatabaseDatum * dataPic);
    void LoadImage3(DatabaseDatum * dataPic);
    int imageXfpos;
    int imageYfpos;
    int imageXsize;
    int imageYsize;
    int imageXfsize;
    int imageYfsize;
    /* ****************gui ************** */
    wxBoxSizer * sizer_root;
    int imageType;
};

#endif

