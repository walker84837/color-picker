#pragma once

#include "color/Color.hpp"

#include <functional>
#include <wx/wx.h>

class ColorSwatch : public wxPanel {
public:
    ColorSwatch(wxWindow* parent, const Color& color, bool selected = false, int size = 50);

    void setColor(const Color& color);
    void setSelected(bool selected);

    [[nodiscard]] auto color() const -> const Color& { return m_color; }

    std::function<void(const Color&)> onClick;

private:
    Color m_color;
    bool m_selected;
    int m_size;

    void OnPaint(wxPaintEvent& event);
    void OnLeftDown(wxMouseEvent& event);
};
