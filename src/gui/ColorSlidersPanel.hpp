#pragma once

#include "color/Color.hpp"
#include "color/Conversion.hpp"

#include <cstdint>

#include <wx/wx.h>

class ColorSlidersPanel : public wxPanel {
public:
    explicit ColorSlidersPanel(wxWindow* parent);

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

    void buildSection(wxSizer* sizer, Space space, const wxString& name, const std::vector<wxString>& labels,
                      const std::vector<int>& mins, const std::vector<int>& maxs, const std::vector<int>& scales,
                      const std::vector<int>& precisions);

    void onSlider(wxCommandEvent& event);
    void onTextEnter(wxCommandEvent& event);
    void onTextFocus(wxFocusEvent& event);

    void fireColor(size_t groupIdx);
};
