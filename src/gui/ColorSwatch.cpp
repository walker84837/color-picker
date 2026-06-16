#include "ColorSwatch.hpp"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>

ColorSwatch::ColorSwatch(wxWindow* parent, const Color& color, bool selected, int size)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(FromDIP(size), FromDIP(size)), wxFULL_REPAINT_ON_RESIZE),
      m_color(color), m_selected(selected), m_size(FromDIP(size)) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(m_size, m_size));
    SetMaxSize(wxSize(m_size, m_size));

    Bind(wxEVT_PAINT, &ColorSwatch::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &ColorSwatch::OnLeftDown, this);
}

void ColorSwatch::setColor(const Color& color) {
    m_color = color;
    Refresh();
}

void ColorSwatch::setSelected(bool selected) {
    if (m_selected != selected) {
        m_selected = selected;
        Refresh();
    }
}

void ColorSwatch::OnPaint(wxPaintEvent& /*unused*/) {
    wxAutoBufferedPaintDC dc(this);
    dc.SetBackground(wxBrush(GetParent()->GetBackgroundColour()));
    dc.Clear();

    wxGraphicsContext* gc = dc.GetGraphicsContext();
    if (gc == nullptr) {
        return;
    }

    // Draw main color
    gc->SetBrush(wxBrush(m_color.wx()));
    gc->SetPen(*wxTRANSPARENT_PEN);
    gc->DrawRectangle(FromDIP(2), FromDIP(2), m_size - FromDIP(4), m_size - FromDIP(4));

    // Draw border
    if (m_selected) {
        gc->SetPen(wxPen(*wxWHITE, FromDIP(3)));
        gc->DrawRectangle(FromDIP(1), FromDIP(1), m_size - FromDIP(2), m_size - FromDIP(2));
        gc->SetPen(wxPen(*wxBLACK, FromDIP(1)));
        gc->DrawRectangle(0, 0, m_size, m_size);
        gc->DrawRectangle(FromDIP(3), FromDIP(3), m_size - FromDIP(6), m_size - FromDIP(6));
    } else {
        gc->SetPen(wxPen(wxColour(180, 180, 180), FromDIP(1)));
        gc->DrawRectangle(FromDIP(2), FromDIP(2), m_size - FromDIP(4), m_size - FromDIP(4));
    }
}

void ColorSwatch::OnLeftDown(wxMouseEvent& /*unused*/) {
    if (onClick) {
        onClick(m_color);
    }
}
