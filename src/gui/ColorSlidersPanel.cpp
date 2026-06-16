#include "ColorSlidersPanel.hpp"

#include <algorithm>
#include <cmath>

ColorSlidersPanel::ColorSlidersPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    buildSection(sizer, Space::Oklch, "OkLCh", {"L", "C", "h"}, {0, 0, 0}, {1000, 350, 3600}, {1000, 1000, 10},
                 {3, 3, 1});

    buildSection(sizer, Space::Lch, "LCh", {"L", "C", "h"}, {0, 0, 0}, {1000, 1500, 3600}, {10, 10, 10}, {1, 1, 1});

    buildSection(sizer, Space::Hsv, "HSV", {"H", "S", "V"}, {0, 0, 0}, {3600, 1000, 1000}, {10, 1000, 1000}, {1, 3, 3});

    buildSection(sizer, Space::Hsl, "HSL", {"H", "S", "L"}, {0, 0, 0}, {3600, 1000, 1000}, {10, 1000, 1000}, {1, 3, 3});

    buildSection(sizer, Space::Rgb, "RGB", {"R", "G", "B"}, {0, 0, 0}, {255, 255, 255}, {1, 1, 1}, {0, 0, 0});

    buildSection(sizer, Space::Cmyk, "CMYK", {"C", "M", "Y", "K"}, {0, 0, 0, 0}, {1000, 1000, 1000, 1000},
                 {1000, 1000, 1000, 1000}, {3, 3, 3, 3});

    SetSizer(sizer);
}

void ColorSlidersPanel::buildSection(wxSizer* sizer, Space space, const wxString& name,
                                     const std::vector<wxString>& labels, const std::vector<int>& mins,
                                     const std::vector<int>& maxs, const std::vector<int>& scales,
                                     const std::vector<int>& precisions) {
    auto* header = new wxStaticText(this, wxID_ANY, name);
    auto headerFont = header->GetFont();
    headerFont.SetWeight(wxFONTWEIGHT_BOLD);
    header->SetFont(headerFont);
    sizer->Add(header, 0, wxLEFT | wxTOP | wxBOTTOM, FromDIP(2));

    Group group;
    group.space = space;

    for (size_t i = 0; i < labels.size(); i++) {
        auto* rowSizer = new wxBoxSizer(wxHORIZONTAL);

        auto* label = new wxStaticText(this, wxID_ANY, labels[i], wxDefaultPosition, wxSize(FromDIP(18), -1));
        rowSizer->Add(label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(4));

        auto* slider = new wxSlider(this, wxID_ANY, 0, mins[i], maxs[i]);
        rowSizer->Add(slider, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(4));

        auto* text = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxSize(FromDIP(55), -1), wxTE_PROCESS_ENTER);
        rowSizer->Add(text, 0, wxALIGN_CENTER_VERTICAL);

        sizer->Add(rowSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(1));

        slider->Bind(wxEVT_SLIDER, &ColorSlidersPanel::onSlider, this);
        text->Bind(wxEVT_TEXT_ENTER, &ColorSlidersPanel::onTextEnter, this);
        text->Bind(wxEVT_KILL_FOCUS, &ColorSlidersPanel::onTextFocus, this);

        group.rows.push_back({.slider = slider, .text = text, .scale = scales[i], .precision = precisions[i]});
    }

    m_groups.push_back(std::move(group));
}

void ColorSlidersPanel::setColor(const Color& color) {
    m_updating = true;
    m_color = color;

    auto updateGroup = [&](size_t gi, const std::vector<double>& vals) -> void {
        auto& g = m_groups[gi];
        for (size_t i = 0; i < vals.size() && i < g.rows.size(); i++) {
            int sv = static_cast<int>(std::round(vals[i] * g.rows[i].scale));
            sv = std::clamp(sv, g.rows[i].slider->GetMin(), g.rows[i].slider->GetMax());
            g.rows[i].slider->SetValue(sv);
            g.rows[i].text->ChangeValue(wxString::Format("%.*f", g.rows[i].precision, vals[i]));
        }
    };

    auto ok = colorm::Oklch(color.rgb());
    updateGroup(0, {ok.lightness(), ok.chroma(), ok.hue()});

    auto lch = colorm::Lch(color.rgb());
    updateGroup(1, {lch.lightness(), lch.chroma(), lch.hue()});

    auto [h, s, v] = conv::rgbToHsv(color.rgb());
    updateGroup(2, {h, s, v});

    auto hsl = colorm::Hsl(color.rgb());
    updateGroup(3, {hsl.hue(), hsl.saturation(), hsl.lightness()});

    auto rgbCrgb = color.rgb();
    updateGroup(4, {rgbCrgb.red(), rgbCrgb.green(), rgbCrgb.blue()});

    auto [c, m, y, k] = conv::rgbToCmyk(color.rgb());
    updateGroup(5, {c, m, y, k});

    m_updating = false;
}

void ColorSlidersPanel::onSlider(wxCommandEvent& event) {
    if (m_updating) {
        return;
    }

    auto* slider = dynamic_cast<wxSlider*>(event.GetEventObject());
    if (slider == nullptr) {
        return;
    }

    for (size_t g = 0; g < m_groups.size(); g++) {
        for (size_t r = 0; r < m_groups[g].rows.size(); r++) {
            if (m_groups[g].rows[r].slider == slider) {
                double val = static_cast<double>(slider->GetValue()) / m_groups[g].rows[r].scale;
                m_groups[g].rows[r].text->ChangeValue(wxString::Format("%.*f", m_groups[g].rows[r].precision, val));
                fireColor(g);
                return;
            }
        }
    }
}

void ColorSlidersPanel::onTextEnter(wxCommandEvent& event) {
    if (m_updating) {
        return;
    }

    auto* text = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
    if (text == nullptr) {
        return;
    }

    for (size_t g = 0; g < m_groups.size(); g++) {
        for (size_t r = 0; r < m_groups[g].rows.size(); r++) {
            if (m_groups[g].rows[r].text == text) {
                double val;
                if (!text->GetValue().ToDouble(&val)) {
                    return;
                }

                int sv = static_cast<int>(std::round(val * m_groups[g].rows[r].scale));
                sv = std::clamp(sv, m_groups[g].rows[r].slider->GetMin(), m_groups[g].rows[r].slider->GetMax());
                m_groups[g].rows[r].slider->SetValue(sv);
                fireColor(g);
                return;
            }
        }
    }
}

void ColorSlidersPanel::onTextFocus(wxFocusEvent& event) {
    if (m_updating) {
        event.Skip();
        return;
    }

    auto* text = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
    if (text == nullptr) {
        event.Skip();
        return;
    }

    for (size_t g = 0; g < m_groups.size(); g++) {
        for (size_t r = 0; r < m_groups[g].rows.size(); r++) {
            if (m_groups[g].rows[r].text == text) {
                double val;
                if (!text->GetValue().ToDouble(&val)) {
                    event.Skip();
                    return;
                }

                int sv = static_cast<int>(std::round(val * m_groups[g].rows[r].scale));
                sv = std::clamp(sv, m_groups[g].rows[r].slider->GetMin(), m_groups[g].rows[r].slider->GetMax());
                m_groups[g].rows[r].slider->SetValue(sv);
                fireColor(g);
                event.Skip();
                return;
            }
        }
    }
    event.Skip();
}

void ColorSlidersPanel::fireColor(size_t groupIdx) {
    auto& group = m_groups[groupIdx];
    std::vector<double> vals;

    vals.reserve(group.rows.size());
    for (auto& row : group.rows) {
        vals.push_back(static_cast<double>(row.slider->GetValue()) / row.scale);
    }

    Color c;
    switch (group.space) {
    case Space::Oklch:
        c = Color(colorm::Oklch(vals[0], vals[1], vals[2]));
        break;
    case Space::Lch:
        c = Color(colorm::Rgb(colorm::Lch(vals[0], vals[1], vals[2])));
        break;
    case Space::Hsv:
        c = Color(conv::hsvToRgb(vals[0], vals[1], vals[2]));
        break;
    case Space::Hsl:
        c = Color(colorm::Rgb(colorm::Hsl(vals[0], vals[1], vals[2])));
        break;
    case Space::Rgb:
        c = Color(colorm::Rgb(vals[0], vals[1], vals[2]));
        break;
    case Space::Cmyk:
        c = Color(conv::cmykToRgb(vals[0], vals[1], vals[2], vals[3]));
        break;
    }

    setColor(c);

    if (onColorChanged) {
        onColorChanged(c);
    }
}
