#include "ColorPreviewPanel.hpp"

#include "color/Conversion.hpp"

#include <wx/clipbrd.h>

ColorPreviewPanel::ColorPreviewPanel(wxWindow* parent, bool showHeader) : wxPanel(parent, wxID_ANY) {
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    if (showHeader) {
        m_swatch = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(-1, FromDIP(70)));
        m_swatch->SetBackgroundColour(*wxBLACK);
        sizer->Add(m_swatch, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, FromDIP(4));

        m_hexText = new wxStaticText(this, wxID_ANY, "#000000", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        auto hexFont = m_hexText->GetFont();
        hexFont.SetPointSize(hexFont.GetPointSize() + 4);
        hexFont.SetWeight(wxFONTWEIGHT_BOLD);
        m_hexText->SetFont(hexFont);
        sizer->Add(m_hexText, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(4));
    }

    auto addRow = [&](const wxString& label) -> void {
        auto* rowSizer = new wxBoxSizer(wxHORIZONTAL);

        auto* lbl = new wxStaticText(this, wxID_ANY, label, wxDefaultPosition, wxSize(FromDIP(55), -1));
        rowSizer->Add(lbl, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(6));

        auto* val = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_DONTWRAP);
        rowSizer->Add(val, 1, wxEXPAND | wxRIGHT, FromDIP(6));

        auto* btn = new wxButton(this, wxID_ANY, "Copy", wxDefaultPosition, wxSize(FromDIP(50), -1));
        rowSizer->Add(btn, 0, wxALIGN_CENTER_VERTICAL);

        sizer->Add(rowSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(4));

        m_rows.push_back({.label = lbl, .value = val, .copyBtn = btn});
    };

    addRow("HEX:");
    addRow("rgb():");
    addRow("oklch():");
    addRow("lch():");
    addRow("hsl():");
    addRow("hsv():");
    addRow("cmyk():");

    for (size_t i = 0; i < m_rows.size(); i++) {
        m_rows[i].copyBtn->Bind(wxEVT_BUTTON, [this, i](wxCommandEvent&) -> void { onCopy(i); });
    }

    SetSizer(sizer);
}

void ColorPreviewPanel::setColor(const Color& color) {
    m_color = color;

    auto wxCol = color.wx();
    if (m_swatch != nullptr) {
        m_swatch->SetBackgroundColour(wxCol);
        m_swatch->Refresh();
    }

    auto hex = color.hex();
    if (m_hexText != nullptr) {
        m_hexText->SetLabelText(hex);
    }

    auto rgbCrgb = color.rgb();

    auto ok = colorm::Oklch(rgbCrgb);
    auto lch = colorm::Lch(rgbCrgb);
    auto hsl = colorm::Hsl(rgbCrgb);
    auto [h, s, v] = conv::rgbToHsv(rgbCrgb);
    auto [c, m, y, k] = conv::rgbToCmyk(rgbCrgb);

    m_rows[0].value->ChangeValue(hex); // HEX
    m_rows[1].value->ChangeValue(wxString::Format("rgb(%d, %d, %d)", static_cast<int>(rgbCrgb.red8()),
                                                  static_cast<int>(rgbCrgb.green8()),
                                                  static_cast<int>(rgbCrgb.blue8())));                // rgb()
    m_rows[2].value->ChangeValue(ok.css());                                                           // oklch()
    m_rows[3].value->ChangeValue(lch.css());                                                          // lch()
    m_rows[4].value->ChangeValue(hsl.css());                                                          // hsl()
    m_rows[5].value->ChangeValue(wxString::Format("hsv(%.1f, %.1f%%, %.1f%%)", h, s * 100, v * 100)); // hsv()
    m_rows[6].value->ChangeValue(
        wxString::Format("cmyk(%.1f%%, %.1f%%, %.1f%%, %.1f%%)", c * 100, m * 100, y * 100, k * 100)); // cmyk()
}

void ColorPreviewPanel::onCopy(size_t index) {
    if (index >= m_rows.size()) {
        return;
    }

    wxString text = m_rows[index].value->GetValue();
    if (text.empty()) {
        return;
    }

    if (wxTheClipboard->Open()) {
        wxTheClipboard->SetData(new wxTextDataObject(text));
        wxTheClipboard->Close();
    }
}
