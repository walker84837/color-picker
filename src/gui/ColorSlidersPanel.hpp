#pragma once

#include "color/Color.hpp"
#include "color/Conversion.hpp"

#include <cstdint>
#include <unordered_map>

#include <wx/wx.h>

class ColorSlidersPanel : public wxPanel {
public:
    /** Constructs the panel with slider groups for all color spaces */
    explicit ColorSlidersPanel(wxWindow* parent);

    /** Updates all sliders and text controls to reflect the new color */
    void setColor(const Color& color);

    std::function<void(const Color&)> onColorChanged;

private:
    enum class Space : std::uint8_t { Oklch, Lch, Hsv, Hsl, Rgb, Cmyk };

    struct Row {
        wxSlider* slider;
        wxTextCtrl* text;
        int scale;
        int precision;
    };

    struct Group {
        Space space;
        std::vector<Row> rows;
    };

    Color m_color;
    bool m_updating = false;
    std::vector<Group> m_groups;
    std::unordered_map<wxWindow*, std::pair<size_t, size_t>> m_widgetMap;

    /** Builds a named group of sliders for one color space */
    void buildSection(wxSizer* sizer, Space space, const wxString& name, const std::vector<wxString>& labels,
                      const std::vector<int>& mins, const std::vector<int>& maxs, const std::vector<int>& scales,
                      const std::vector<int>& precisions);

    /** Handles slider drag events */
    void onSlider(wxCommandEvent& event);
    /** Handles enter key in a text field */
    void onTextEnter(wxCommandEvent& event);
    /** Handles focus loss in a text field */
    void onTextFocus(wxFocusEvent& event);

    /** Rebuilds the Color from the active slider group and fires the callback */
    void fireColor(size_t groupIdx);
};
