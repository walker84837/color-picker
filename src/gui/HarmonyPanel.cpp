#include "HarmonyPanel.hpp"

#include "ColorSwatch.hpp"

#include <wx/sizer.h>
#include <wx/stattext.h>

namespace {
/** Shifts hue by degrees, wrapping to [0, 360] */
auto rotateHue(const Color& c, double degrees) -> Color {
    auto oklch = colorm::Oklch(c.rgb());
    double h = oklch.hue() + degrees;
    if (h < 0.0) {
        h += 360.0;
    }
    if (h >= 360.0) {
        h -= 360.0;
    }
    return Color(colorm::Rgb(colorm::Oklch(oklch.lightness(), oklch.chroma(), h)));
}

/** Base + adjacent hues on both sides */
auto analogous(const Color& c) -> std::vector<Color> {
    return {c, rotateHue(c, -30.0), rotateHue(c, 30.0), rotateHue(c, -60.0), rotateHue(c, 60.0)};
}

/** Opposite hue + near-complement for split effect */
auto complementary(const Color& c) -> std::vector<Color> {
    return {c, rotateHue(c, 180.0), rotateHue(c, 150.0), rotateHue(c, 210.0)};
}

/** Three hues evenly spaced 120 degrees apart */
auto triad(const Color& c) -> std::vector<Color> {
    return {c, rotateHue(c, 120.0), rotateHue(c, 240.0)};
}

/** Four hues forming a rectangle on the wheel */
auto tetrad(const Color& c) -> std::vector<Color> {
    return {c, rotateHue(c, 90.0), rotateHue(c, 180.0), rotateHue(c, 270.0)};
}

/** Base + two hues adjacent to the complement */
auto splitComplementary(const Color& c) -> std::vector<Color> {
    return {c, rotateHue(c, 150.0), rotateHue(c, 210.0)};
}

struct SchemeDef {
    const char* name;
    const char* desc;
    std::vector<Color> (*fn)(const Color&);
};
constexpr SchemeDef kSchemes[] = {
    {"Analogous",
     "Colors next to each other on the wheel. Creates calm, comfortable designs.",
     analogous},
    {"Complementary",
     "Colors opposite on the wheel. Creates high contrast, vibrant looks.",
     complementary},
    {"Triad",
     "Three colors evenly spaced. Offers strong visual contrast while retaining balance.",
     triad},
    {"Tetrad",
     "Four colors forming a rectangle. Rich, complex color schemes.",
     tetrad},
    {"Split Complementary",
     "Base + two colors adjacent to its complement. Contrast like complementary but less tension.",
     splitComplementary},
};
} // namespace

HarmonyPanel::HarmonyPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    m_placeholder = new wxStaticText(this, wxID_ANY, "Select a color in the Picker tab to see harmony schemes here.");
    auto phFont = m_placeholder->GetFont();
    phFont.SetStyle(wxFONTSTYLE_ITALIC);
    m_placeholder->SetFont(phFont);
    m_placeholder->SetForegroundColour(wxColour(128, 128, 128));
    sizer->Add(m_placeholder, 0, wxALL, 10);

    for (auto& info : kSchemes) {
        auto* label = new wxStaticText(this, wxID_ANY, info.name);
        auto font = label->GetFont();
        font.SetWeight(wxFONTWEIGHT_BOLD);
        label->SetFont(font);
        sizer->Add(label, 0, wxLEFT | wxTOP | wxBOTTOM, FromDIP(2));

        auto* descLabel = new wxStaticText(this, wxID_ANY, info.desc);
        descLabel->SetForegroundColour(wxColour(128, 128, 128));
        auto descFont = descLabel->GetFont();
        descFont.SetStyle(wxFONTSTYLE_ITALIC);
        descLabel->SetFont(descFont);
        sizer->Add(descLabel, 0, wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(2));

        auto* swatchSizer = new wxBoxSizer(wxHORIZONTAL);
        sizer->Add(swatchSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 0);

        Scheme scheme{.name = info.name,
                      .description = info.desc,
                      .label = label,
                      .descLabel = descLabel,
                      .swatchSizer = swatchSizer,
                      .swatches = {}};
        m_schemes.push_back(std::move(scheme));
    }

    SetSizer(sizer);
}

void HarmonyPanel::setColor(const Color& color) {
    m_color = color;

    if (m_placeholder != nullptr) {
        m_placeholder->Show(false);
    }

    // Walk both arrays in lockstep; the runtime vector always matches kSchemes in practice
    for (size_t i = 0; i < std::size(kSchemes) && i < m_schemes.size(); i++) {
        auto colors = kSchemes[i].fn(m_color);
        auto* sizer = m_schemes[i].swatchSizer;

        sizer->Clear(true);
        m_schemes[i].swatches.clear();

        for (auto& c : colors) {
            auto* swatch = new ColorSwatch(this, c, c == m_color);
            swatch->onClick = [this](const Color& selected) -> void {
                if (onColorChanged) {
                    onColorChanged(selected);
                }
            };
            sizer->Add(swatch, 0, wxALL, FromDIP(2));
            m_schemes[i].swatches.push_back(swatch);
        }
    }

    Layout();
}
