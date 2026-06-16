#pragma once

#include "color/Color.hpp"

#include <wx/wx.h>

class ColorPreviewPanel : public wxPanel {
public:
    /** Constructs a color preview with swatch, hex, and CSS value rows */
    explicit ColorPreviewPanel(wxWindow* parent, bool showHeader = true);

    /** Updates the swatch and all CSS value displays */
    void setColor(const Color& color);

private:
    wxPanel* m_swatch = nullptr;
    wxStaticText* m_hexText = nullptr;

    struct CssRow {
        wxStaticText* label;
        wxTextCtrl* value;
        wxButton* copyBtn;
    };

    std::vector<CssRow> m_rows;
    Color m_color;

    enum RowIndex : size_t { Hex, Rgb, Oklch, Lch, Hsl, Hsv, Cmyk };

    /** Copies the CSS value at the given row to the clipboard */
    void onCopy(size_t index);
};
