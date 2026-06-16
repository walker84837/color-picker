#pragma once

#include "color/Color.hpp"

#include <optional>
#include <wx/wx.h>

class ColorPreviewPanel;

class ConvertPanel : public wxPanel {
public:
    /** Constructs the convert tab with color A/B preview and comparison */
    explicit ConvertPanel(wxWindow* parent);

    /** Sets the active color (color A) */
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

    /** Parses hex input from the text field */
    void OnHexInput(wxCommandEvent& event);
    /** Parses hex input when the field loses focus */
    void OnHexKillFocus(wxFocusEvent& event);

    /** Builds the comparison section (Delta E and WCAG) */
    auto buildComparisonSection() -> wxSizer*;

    /** Validates hex and updates color B */
    static auto parseHex(const wxString& str) -> std::optional<colorm::Rgb>;
    /** Updates color B from hex string and refreshes comparison */
    void updateColorB(const wxString& hex);
    /** Recalculates Delta E and WCAG metrics */
    void updateComparison();
};
