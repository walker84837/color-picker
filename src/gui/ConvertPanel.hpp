#pragma once

#include "color/Color.hpp"

#include <wx/wx.h>

class ColorPreviewPanel;

class ConvertPanel : public wxPanel {
public:
    explicit ConvertPanel(wxWindow* parent);

    void setColorA(const Color& color);

private:
    Color m_colorA;
    Color m_colorB;
    bool m_colorBSet = false;
    bool m_updating = false;

    ColorPreviewPanel* m_previewA;

    wxPanel* m_swatchB;
    wxTextCtrl* m_hexInputB;
    wxStaticText* m_validationHint;

    wxStaticText* m_deltaEText;
    wxStaticText* m_deltaE00Text;
    wxStaticText* m_contrastText;

    wxStaticText* m_aaNormalText;
    wxStaticText* m_aaLargeText;
    wxStaticText* m_aaaNormalText;
    wxStaticText* m_aaaLargeText;

    void OnHexInput(wxCommandEvent& event);
    void OnHexKillFocus(wxFocusEvent& event);

    static auto parseHex(const wxString& str, colorm::Rgb& rgb) -> bool;
    void updateColorB(const wxString& hex);
    void updateComparison();
};
