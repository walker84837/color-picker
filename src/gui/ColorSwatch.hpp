#pragma once

#include "color/Color.hpp"

#include <functional>
#include <wx/wx.h>

class ColorSwatch : public wxPanel {
public:
    /** Constructs a square clickable color swatch */
    ColorSwatch(wxWindow* parent, const Color& color, bool selected = false, int size = 50);

    /** Updates the displayed color */
    void setColor(const Color& color);
    /** Toggles the selection border */
    void setSelected(bool selected);

    /** Returns the displayed color */
    [[nodiscard]] auto color() const -> const Color& { return m_color; }

    std::function<void(const Color&)> onClick;

private:
    Color m_color;
    bool m_selected;
    int m_size;

    /** Draws the swatch with optional selection highlight */
    void OnPaint(wxPaintEvent& event);
    /** Fires the onClick callback */
    void OnLeftDown(wxMouseEvent& event);
};
