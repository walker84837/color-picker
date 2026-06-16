#include "ConvertPanel.hpp"

#include "color/Conversion.hpp"
#include "gui/ColorPreviewPanel.hpp"

#include <wx/clipbrd.h>
#include <wx/statline.h>

ConvertPanel::ConvertPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
    auto* outer = new wxBoxSizer(wxVERTICAL);

    // -- Top row: Color A | Color B --
    auto* topRow = new wxBoxSizer(wxHORIZONTAL);

    // -- Color A --
    auto* boxASizer = new wxStaticBoxSizer(wxVERTICAL, this, "Color A (picker)");
    m_previewA = new ColorPreviewPanel(this);
    boxASizer->Add(m_previewA, 1, wxEXPAND | wxALL, FromDIP(5));
    topRow->Add(boxASizer, 1, wxEXPAND | wxRIGHT, FromDIP(4));

    // -- Color B --
    auto* boxBSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Color B (compare)");

    m_swatchB = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(-1, FromDIP(80)));
    m_swatchB->SetBackgroundColour(*wxBLACK);
    boxBSizer->Add(m_swatchB, 0, wxEXPAND | wxALL, FromDIP(5));

    auto* hexRow = new wxBoxSizer(wxHORIZONTAL);
    hexRow->Add(new wxStaticText(this, wxID_ANY, "Hex:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(4));
    m_hexInputB = new wxTextCtrl(this, wxID_ANY, "#000000", wxDefaultPosition, wxSize(FromDIP(120), -1), wxTE_PROCESS_ENTER);
    hexRow->Add(m_hexInputB, 1, wxEXPAND);
    boxBSizer->Add(hexRow, 0, wxEXPAND | wxALL, FromDIP(5));

    m_validationHint = new wxStaticText(this, wxID_ANY, "Enter 3 or 6 digit hex");
    m_validationHint->SetForegroundColour(wxColour(128, 128, 128));
    auto hintFont = m_validationHint->GetFont();
    hintFont.SetStyle(wxFONTSTYLE_ITALIC);
    hintFont.SetPointSize(hintFont.GetPointSize() - 1);
    m_validationHint->SetFont(hintFont);
    boxBSizer->Add(m_validationHint, 0, wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(5));

    boxBSizer->AddStretchSpacer();
    topRow->Add(boxBSizer, 1, wxEXPAND | wxLEFT, FromDIP(4));

    outer->Add(topRow, 0, wxEXPAND | wxALL, FromDIP(5));

    // -- Comparison section --
    auto* compSizer = new wxBoxSizer(wxVERTICAL);

    auto addHeader = [&](const wxString& title, bool isMain) -> void {
        auto* lbl = new wxStaticText(this, wxID_ANY, title);
        auto font = lbl->GetFont();
        font.SetWeight(wxFONTWEIGHT_BOLD);
        font.SetPointSize(font.GetPointSize() + (isMain ? 2 : 1));
        lbl->SetFont(font);
        compSizer->Add(lbl, 0, wxLEFT | wxTOP | wxBOTTOM, FromDIP(isMain ? 4 : 8));
    };

    auto addNote = [&](const wxString& text) -> void {
        auto* lbl = new wxStaticText(this, wxID_ANY, text);
        lbl->SetForegroundColour(wxColour(128, 128, 128));
        auto font = lbl->GetFont();
        font.SetStyle(wxFONTSTYLE_ITALIC);
        lbl->SetFont(font);
        compSizer->Add(lbl, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(6));
    };

    addHeader("Comparison", true);

    // Subsection 1: Color Difference
    addHeader("Color Difference", false);
    addNote("Delta E measures perceived color difference (< 1.0 imperceptible, > 1.0 noticeable).");

    auto* diffGrid = new wxFlexGridSizer(2, FromDIP(8), FromDIP(12));
    diffGrid->AddGrowableCol(1);

    auto addGridRow = [&](wxFlexGridSizer* grid, const wxString& label, wxStaticText*& valueOut) -> void {
        grid->Add(new wxStaticText(this, wxID_ANY, label), 0, wxALIGN_CENTER_VERTICAL);
        valueOut = new wxStaticText(this, wxID_ANY, "-", wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
        grid->Add(valueOut, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND);
    };

    addGridRow(diffGrid, "Delta E (CIE76):", m_deltaEText);
    addGridRow(diffGrid, "Delta E00 (CIEDE2000):", m_deltaE00Text);
    addGridRow(diffGrid, "Contrast Ratio:", m_contrastText);
    compSizer->Add(diffGrid, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(10));

    compSizer->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, FromDIP(10));

    // Subsection 2: WCAG Contrast
    addHeader("WCAG Contrast Evaluation", false);
    addNote("AA = minimum accessible (4.5:1). AAA = enhanced (7.0:1). \"Large\" = 18pt+ or 14pt+ bold.");

    auto* wcagGrid = new wxFlexGridSizer(2, FromDIP(6), FromDIP(12));
    wcagGrid->AddGrowableCol(1);

    auto wcagFont = GetFont();
    wcagFont.SetWeight(wxFONTWEIGHT_BOLD);

    auto addWcagGridRow = [&](wxFlexGridSizer* grid, const wxString& label, wxStaticText*& valueOut) -> void {
        auto* lbl = new wxStaticText(this, wxID_ANY, label);
        lbl->SetFont(wcagFont);
        grid->Add(lbl, 0, wxALIGN_CENTER_VERTICAL);
        valueOut = new wxStaticText(this, wxID_ANY, "-", wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
        grid->Add(valueOut, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND);
    };

    addWcagGridRow(wcagGrid, "AA Normal:", m_aaNormalText);
    addWcagGridRow(wcagGrid, "AA Large:", m_aaLargeText);
    addWcagGridRow(wcagGrid, "AAA Normal:", m_aaaNormalText);
    addWcagGridRow(wcagGrid, "AAA Large:", m_aaaLargeText);
    compSizer->Add(wcagGrid, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(10));

    outer->Add(compSizer, 0, wxEXPAND | wxALL, FromDIP(5));

    SetSizer(outer);

    // Events
    m_hexInputB->Bind(wxEVT_TEXT_ENTER, &ConvertPanel::OnHexInput, this);
    m_hexInputB->Bind(wxEVT_KILL_FOCUS, &ConvertPanel::OnHexKillFocus, this);
}

void ConvertPanel::setColorA(const Color& color) {
    m_updating = true;
    m_colorA = color;
    m_previewA->setColor(color);
    updateComparison();
    m_updating = false;
}

bool ConvertPanel::parseHex(const wxString& str, colorm::Rgb& rgb) {
    auto s = str;
    s.Trim(true).Trim(false);
    if (s.StartsWith("#")) {
        s = s.Mid(1);
    }

    if (s.length() == 3) {
        // Shorthand #rgb -> #rrggbb
        wxString expanded;
        expanded << s[0] << s[0] << s[1] << s[1] << s[2] << s[2];
        s = expanded;
    }

    if (s.length() != 6) {
        return false;
    }

    unsigned long val;
    if (!s.ToULong(&val, 16)) {
        return false;
    }

    rgb = colorm::Rgb(static_cast<unsigned int>(val));
    return true;
}

void ConvertPanel::updateColorB(const wxString& hex) {
    colorm::Rgb rgb;
    if (!parseHex(hex, rgb)) {
        m_colorBSet = false;
        m_validationHint->SetLabelText("Invalid hex format");
        m_validationHint->SetForegroundColour(wxColour(200, 0, 0));
        m_swatchB->SetBackgroundColour(GetBackgroundColour());
        m_swatchB->Refresh();
        updateComparison();
        return;
    }

    m_colorB = Color(rgb);
    m_colorBSet = true;
    m_validationHint->SetLabelText("Valid hex color");
    m_validationHint->SetForegroundColour(wxColour(0, 140, 0));
    m_swatchB->SetBackgroundColour(m_colorB.wx());
    m_swatchB->Refresh();
    updateComparison();
}

void ConvertPanel::OnHexInput(wxCommandEvent& /*unused*/) {
    if (m_updating) {
        return;
    }
    updateColorB(m_hexInputB->GetValue());
}

void ConvertPanel::OnHexKillFocus(wxFocusEvent& event) {
    if (!m_updating) {
        updateColorB(m_hexInputB->GetValue());
    }
    event.Skip();
}

void ConvertPanel::updateComparison() {
    auto setText = [](wxStaticText* t, const wxString& s) -> void { t->SetLabelText(s); };

    if (!m_colorBSet) {
        setText(m_deltaEText, "-");
        setText(m_deltaE00Text, "-");
        setText(m_contrastText, "-");
        setText(m_aaNormalText, "-");
        setText(m_aaLargeText, "-");
        setText(m_aaaNormalText, "-");
        setText(m_aaaLargeText, "-");
        return;
    }

    auto ra = m_colorA.rgb();
    auto rb = m_colorB.rgb();

    double deltaE = ra.deltaE(rb);
    double deltaE00 = ra.deltaE_00(rb);
    double contrast = ra.contrast(rb);

    setText(m_deltaEText, wxString::Format("%.2f", deltaE));
    setText(m_deltaE00Text, wxString::Format("%.2f", deltaE00));
    setText(m_contrastText, wxString::Format("%.2f : 1", contrast));

    auto setWcag = [&](wxStaticText* t, double ratio, double threshold) -> void {
        bool pass = ratio >= threshold - 0.005;
        auto label = wxString::Format("%s  (%.2f : 1, %.1f required)", pass ? "PASS" : "FAIL", ratio, threshold);
        t->SetLabelText(label);
        t->SetForegroundColour(pass ? wxColour(0, 140, 0) : wxColour(200, 0, 0));
    };

    setWcag(m_aaNormalText, contrast, 4.5);
    setWcag(m_aaLargeText, contrast, 3.0);
    setWcag(m_aaaNormalText, contrast, 7.0);
    setWcag(m_aaaLargeText, contrast, 4.5);
}
