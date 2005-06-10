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

#ifndef PICFRAME_H
#define PICFRAME_H

#include <wx/wx.h>
#include <wx/mstream.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include "metfile.h"
#include "md4hash.h"

extern int debug;

class CpicFrame:public wxFrame
{
    public:
    CpicFrame(wxFrame * parent, const char * buffer, int buffersize, int width, int height, char * title);
    ~ CpicFrame();
    void Clip(wxImage * temp_jpg);
    void OnMouseButton(wxMouseEvent & event);
    void OnMouseMotion(wxMouseEvent & event);
    void OnPaint(wxPaintEvent & WXUNUSED(event));
    void OnResize(wxSizeEvent & WXUNUSED(event));
    void DrawImage(wxPaintDC *pdc);
    void LoadImage(const char * buffer, int buffersize);
    int imageXfpos;
    int imageYfpos;
    int imageXsize;
    int imageYsize;
    int imageXfsize;
    int imageYfsize;
    int mouseXpos;
    int mouseYpos;
    /* ****************gui ************** */
    wxBoxSizer * sizer_root;
    wxScrollBar * sb1x;
    wxScrollBar * sb1y;
	wxBitmap *bitmap_jpg;
	wxImage *image_jpg;
};

#endif

