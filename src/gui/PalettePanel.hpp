#pragma once

#include "ColorSwatch.hpp"
#include "color/Color.hpp"

#include <functional>
#include <vector>
#include <wx/wx.h>

class PalettePanel : public wxPanel {
public:
    explicit PalettePanel(wxWindow* parent);

    void setColor(const Color& color);

    std::function<void(const Color&)> onColorChanged;

private:
    struct PaletteResult {
        std::vector<Color> colors;
        std::vector<int> counts;
    };

    Color m_color;
    wxStaticBitmap* m_imageCtrl = nullptr;
    wxStaticText* m_emptyLabel = nullptr;
    wxPanel* m_paletteArea = nullptr;
    wxBoxSizer* m_paletteSizer = nullptr;
    std::vector<ColorSwatch*> m_swatches;
    int m_numColors = 8;

    // Stored image data for recalculation
    std::vector<unsigned char> m_pixels;
    int m_imageWidth = 0;
    int m_imageHeight = 0;
    bool m_hasImage = false;

    void loadImage(const wxString& path);
    void recalculate();
    static auto extractPalette(const unsigned char* pixels, int width, int height, int numColors) -> PaletteResult;
    void displayPalette(const PaletteResult& result);
};
