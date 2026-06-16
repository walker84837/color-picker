#pragma once

#include "ColorSwatch.hpp"
#include "color/Color.hpp"

#include <functional>
#include <vector>
#include <wx/wx.h>

class HarmonyPanel : public wxPanel {
public:
    /** Constructs the harmony schemes panel with placeholder */
    explicit HarmonyPanel(wxWindow* parent);

    /** Generates and displays harmony swatches for the given color */
    void setColor(const Color& color);

    std::function<void(const Color&)> onColorChanged;

private:
    struct Scheme {
        wxString name;
        wxString description;
        wxStaticText* label;
        wxStaticText* descLabel;
        wxBoxSizer* swatchSizer;
        std::vector<ColorSwatch*> swatches;
    };

    Color m_color;
    wxStaticText* m_placeholder = nullptr;
    std::vector<Scheme> m_schemes;

};

