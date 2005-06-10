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

#include <wx/image.h>
#include "picfidlg.h"
#include "database.h"

#define WXOEF	(wxObjectEventFunction)

wxBitmap * bitmap_jpg;
wxImage * image_jpg;

CpicfiDlg::CpicfiDlg(wxWindow * parent, int w, int h)
:wxWindow(parent, - 1, wxPoint( - 1, - 1), wxSize(w, h))
{
    bitmap_jpg = (wxBitmap *) NULL;
    image_jpg = (wxImage *) NULL;
    SetBackgroundColour( * wxWHITE);
    SetForegroundColour( * wxBLACK);
    Connect( - 1, wxEVT_PAINT, WXOEF(wxPaintEventFunction) & CpicfiDlg::OnPaint);
    imageXfsize = w;
    imageYfsize = h;
    imageXfpos = 0;
    imageYfpos = 0;
    imageXsize = 0;
    imageYsize = 0;
}

CpicfiDlg::~ CpicfiDlg()
{
    if (bitmap_jpg)
    {
        delete bitmap_jpg;
        bitmap_jpg = NULL;
    }
    if (image_jpg)
    {
        image_jpg -> Destroy();
        image_jpg = NULL;
    }
}

void CpicfiDlg::DrawImage(wxPaintDC * pdc)
{
    int w, h;
    int x, y;
    GetSize( & x, & y);
    if ((x != imageXfsize) || (y != imageYfsize))
    {
        imageXfsize = x;
        imageYfsize = y;
    }
    if (image_jpg)
    {
        wxMemoryDC mdc;
        if (bitmap_jpg)
        {
            delete bitmap_jpg;
        }
        bitmap_jpg = new wxBitmap(image_jpg -> Scale(imageXsize, imageYsize));
        if (bitmap_jpg)
        {
            w = imageXfsize - 16;
            if (w > imageXsize)
            {
                w = imageXsize;
            }
            h = imageYfsize - 16;
            if (h > imageYsize)
            {
                h = imageYsize;
            }
            mdc.SelectObject( * bitmap_jpg);
            pdc -> Blit(0, 0, w, h, & mdc, 0, 0);
        }
    }
}

void CpicfiDlg::OnPaint(wxPaintEvent & WXUNUSED(event))
{
    wxPaintDC pdc(this);
    PrepareDC(pdc);
    if (bitmap_jpg)
    {
        pdc.DrawBitmap( * bitmap_jpg, imageXfpos, imageYfpos);
    }
}

void CpicfiDlg::Clip(wxImage * temp_jpg)
{
    int w2, h2;
    if (imageXsize > imageYsize)
    {
        w2 = imageXfsize;
        h2 = w2 * imageYsize / imageXsize;
        imageXfpos = 0;
        imageYfpos = (imageYfsize - h2) / 2;
    }
    else
    {
        h2 = imageYfsize;
        w2 = h2 * imageXsize / imageYsize;
        imageXfpos = (imageXfsize - w2) / 2;
        imageYfpos = 0;
    }
    if (bitmap_jpg)
    {
        delete bitmap_jpg;
    }
    if (temp_jpg)
    {
        bitmap_jpg = new wxBitmap(temp_jpg -> Scale(w2, h2));
    }
}

void CpicfiDlg::LoadImageGIF(DatabaseDatum *dataPic)
{
    wxMemoryInputStream * mis = new wxMemoryInputStream(dataPic->ptr, dataPic->size);
    wxPaintDC pdc(this);
    pdc.Clear();
    if (image_jpg)
    {
        image_jpg -> Destroy();
        delete image_jpg;
        image_jpg = NULL;
    }
    image_jpg = new wxImage;
    image_jpg -> LoadFile( * mis, wxBITMAP_TYPE_GIF, - 1);
    imageXsize = image_jpg -> GetWidth();
    imageYsize = image_jpg -> GetHeight();
    if (imageXsize && imageYsize)
    {
        Clip(image_jpg);
        if (bitmap_jpg)
        {
            pdc.DrawBitmap( * bitmap_jpg, imageXfpos, imageYfpos);
        }
    }
    delete mis;
}

void CpicfiDlg::LoadImage(DatabaseDatum *dataPic)
{
    wxMemoryInputStream * mis = new wxMemoryInputStream(dataPic->ptr,dataPic->size);
    wxPaintDC pdc(this);
    pdc.Clear();
    if (image_jpg)
    {
        image_jpg -> Destroy();
        delete image_jpg;
        image_jpg = NULL;
    }
    image_jpg = new wxImage;
    image_jpg -> LoadFile( * mis, wxBITMAP_TYPE_ANY, - 1);
    imageXsize = image_jpg -> GetWidth();
    imageYsize = image_jpg -> GetHeight();
    if (imageXsize && imageYsize)
    {
        Clip(image_jpg);
        if (bitmap_jpg)
        {
            pdc.DrawBitmap( * bitmap_jpg, imageXfpos, imageYfpos);
        }
    }
    delete mis;
}

