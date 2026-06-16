#pragma once

#include "color/Color.hpp"

#include <wx/wx.h>

class ColorPreviewPanel : public wxPanel {
public:
    explicit ColorPreviewPanel(wxWindow* parent, bool showHeader = true);

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

    void onCopy(size_t index);
};
