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

#include "picframe.h"
#include "database.h"
#include <wx/image.h>

#define WXOEF	(wxObjectEventFunction)

CpicFrame::CpicFrame(wxFrame * parent, const char * buffer, int buffersize, int w, int h, char * title)
:wxFrame(parent, - 1, "")
{
    wxMemoryInputStream * mis = new wxMemoryInputStream(buffer, buffersize);
    image_jpg = new wxImage;
    image_jpg -> LoadFile( * mis, wxBITMAP_TYPE_ANY, - 1);
    imageXsize = image_jpg -> GetWidth();
    imageYsize = image_jpg -> GetHeight();
    imageXfsize = w;
    imageYfsize = h;
    imageXfpos = 0;
    imageYfpos = 0;
    mouseXpos = - 1;
    mouseYpos = - 1;
    if (imageXfsize == - 1)
    {
        imageXfsize = imageXsize;
    }
    else if(imageXfsize > imageXsize)
    {
        imageXfsize = imageXsize;
    }
    if (imageYfsize == - 1)
    {
        imageYfsize = imageYsize;
    }
    else if(imageYfsize > imageYsize)
    {
        imageYfsize = imageYsize;
    }
    if (bitmap_jpg)
    {
        delete bitmap_jpg;
    }
    bitmap_jpg = new wxBitmap(image_jpg -> Scale(imageXsize, imageYsize));
    if (bitmap_jpg)
    {
        SetClientSize(imageXfsize, imageYfsize);
        sb1x = new wxScrollBar(this, - 1, wxDefaultPosition, wxSize( - 1, 16), wxSB_HORIZONTAL);
        sb1y = new wxScrollBar(this, - 1, wxDefaultPosition, wxSize(16, - 1), wxSB_VERTICAL);
        sb1x -> SetScrollbar(0, imageXfsize - 16, imageXsize, imageXfsize - 16);
        sb1y -> SetScrollbar(0, imageYfsize - 16, imageYsize, imageYfsize - 16);
    }
    sizer_root = new wxBoxSizer(wxVERTICAL);
    SetSizeHints(imageXfsize, imageYfsize, imageXsize + 16, imageYsize + 16);
    sizer_root -> Add(sb1y, 1, wxALIGN_RIGHT, 0);
    sizer_root -> Add(sb1x, 0, wxGROW | wxALIGN_BOTTOM, 0);
    SetSizer(sizer_root);
    if (strlen(title))
    {
        SetTitle(title);
    }
    Connect( - 1, wxEVT_PAINT, WXOEF(wxPaintEventFunction) & CpicFrame::OnPaint);
    Connect( - 1, wxEVT_SCROLL_THUMBTRACK, WXOEF(wxScrollEventFunction) & CpicFrame::OnPaint);
    Connect( - 1, wxEVT_SCROLL_LINEUP, WXOEF(wxScrollEventFunction) & CpicFrame::OnPaint);
    Connect( - 1, wxEVT_SCROLL_LINEDOWN, WXOEF(wxScrollEventFunction) & CpicFrame::OnPaint);
    Connect( - 1, wxEVT_SCROLL_PAGEUP, WXOEF(wxScrollEventFunction) & CpicFrame::OnPaint);
    Connect( - 1, wxEVT_SCROLL_PAGEDOWN, WXOEF(wxScrollEventFunction) & CpicFrame::OnPaint);
    Connect( - 1, wxEVT_MOTION, WXOEF(wxMouseEventFunction) & CpicFrame::OnMouseMotion);
    delete mis;
}

CpicFrame::~ CpicFrame()
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

void CpicFrame::OnMouseMotion(wxMouseEvent & event)
{
    wxPoint position = event.GetPosition();
    wxClientDC dc(this);
    PrepareDC(dc);
    int x, y, state = 0;
    x = dc.DeviceToLogicalX(position.x);
    y = dc.DeviceToLogicalY(position.y);
    if (event.LeftIsDown())
    {
        state |= 1;
        mouseXpos = - 1;
        mouseYpos = - 1;
    }
    if (event.RightIsDown())
    {
        state |= 2;
        if (mouseXpos < 0)
        {
            mouseXpos = x;
            mouseYpos = y;
        }
        else
        {
            int x3, y3, x2, y2, dx, dy;
            x2 = sb1x -> GetThumbPosition();
            y2 = sb1y -> GetThumbPosition();
            dx = (x - mouseXpos);
            dy = (y - mouseYpos);
            x3 = x2 + dx;
            y3 = y2 + dy;
            if (x3 < 0)
            {
                x3 = 0;
            }
            else if(x3 > (imageXsize + 16 - imageXfsize))
            {
                x3 = imageXsize + 16 - imageXfsize;
            }
            if (y3 < 0)
            {
                y3 = 0;
            }
            else if(y3 > (imageYsize + 16 - imageYfsize))
            {
                y3 = imageYsize + 16 - imageYfsize;
            }
            if ((x3 != x2) || (y3 != y2))
            {
                sb1x -> SetScrollbar(x3, imageXfsize - 16, imageXsize, imageXfsize - 16);
                sb1y -> SetScrollbar(y3, imageYfsize - 16, imageYsize, imageYfsize - 16);
                wxPaintDC pdc(this);
                PrepareDC(pdc);
                DrawImage( & pdc);
            }
            mouseXpos = x;
            mouseYpos = y;
        }
    }
    else
    {
        mouseXpos = - 1;
        mouseYpos = - 1;
    }
}

void CpicFrame::OnMouseButton(wxMouseEvent & event)
{
    wxPoint position = event.GetPosition();
    wxClientDC dc(this);
    PrepareDC(dc);
    int x, y, state = 0;
    x = dc.DeviceToLogicalX(position.x);
    y = dc.DeviceToLogicalY(position.y);
    if (event.LeftIsDown())
    {
        state |= 1;
    }
    if (event.RightIsDown())
    {
        state |= 2;
    }
}

void CpicFrame::DrawImage(wxPaintDC * pdc)
{
    int px, py, w, h;
    int x, y;
    GetSize( & x, & y);
    if ((x != imageXfsize) || (y != imageYfsize))
    {
        imageXfsize = x;
        imageYfsize = y;
        x = sb1x -> GetThumbPosition();
        y = sb1y -> GetThumbPosition();
        if ((x + imageXfsize - 16) > imageXsize)
        {
            x = imageXsize - imageXfsize + 16;
        }
        if ((y + imageYfsize - 16) > imageYsize)
        {
            y = imageYsize - imageYfsize + 16;
        }
        if (x < 0)
        {
            x = 0;
        }
        if (y < 0)
        {
            y = 0;
        }
        sb1x -> SetScrollbar(x, imageXfsize - 16, imageXsize, imageXfsize - 16);
        sb1y -> SetScrollbar(y, imageYfsize - 16, imageYsize, imageYfsize - 16);
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
            px = sb1x -> GetThumbPosition();
            py = sb1y -> GetThumbPosition();
            mdc.SelectObject( * bitmap_jpg);
            pdc -> Blit(0, 0, w, h, & mdc, px, py);
        }
    }
}

void CpicFrame::OnPaint(wxPaintEvent & WXUNUSED(event))
{
    wxPaintDC pdc(this);
    PrepareDC(pdc);
    DrawImage( & pdc);
}

void CpicFrame::Clip(wxImage * temp_jpg)
{
    int w2, h2;
    if (imageXsize > imageYsize)
    {
        w2 = imageXsize - 8;
        h2 = w2 * (imageYsize - 8) / (imageXsize - 8);
        imageXfpos = 0;
        imageYfpos = ((imageYsize - 8) - h2) / 2;
    }
    else
    {
        h2 = imageYsize - 8;
        w2 = h2 * (imageXsize - 8) / (imageYsize - 8);
        imageXfpos = ((imageXsize - 8) - w2) / 2;
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

void CpicFrame::LoadImage(const char * buffer, int buffersize)
{
    wxMemoryInputStream * mis = new wxMemoryInputStream(buffer, buffersize);
    wxPaintDC pdc(this);
    pdc.Clear();
    if (image_jpg)
    {
        image_jpg -> Destroy();
        delete image_jpg;
    }
    image_jpg = new wxImage;
    image_jpg -> LoadFile( * mis, wxBITMAP_TYPE_ANY, - 1);
    imageXsize = image_jpg -> GetWidth();
    imageYsize = image_jpg -> GetHeight();
    if (bitmap_jpg)
    {
        delete bitmap_jpg;
    }
    bitmap_jpg = new wxBitmap(image_jpg);
    //Clip(image_jpg);
    if (bitmap_jpg)
    {
        printf("draw: %d,%d (%d,%d)\n", imageXfpos, imageYfpos, bitmap_jpg -> GetHeight(), bitmap_jpg -> GetWidth());
        pdc.DrawBitmap( * bitmap_jpg, 0, 0);
        //imageXfpos, imageYfpos);
    }
    delete mis;
}

