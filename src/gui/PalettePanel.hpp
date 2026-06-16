#pragma once

#include "ColorSwatch.hpp"
#include "color/Color.hpp"

#include <functional>
#include <vector>
#include <wx/wx.h>

class PalettePanel : public wxPanel {
public:
    /** Constructs the palette extraction tab */
    explicit PalettePanel(wxWindow* parent);

    /** Updates the selection state on existing swatches */
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

    // Stored pixel data for recalculation without reloading
    std::vector<unsigned char> m_pixels;
    int m_imageWidth = 0;
    int m_imageHeight = 0;
    bool m_hasImage = false;

    /** Loads an image from disk and extracts its palette */
    void loadImage(const wxString& path);
    /** Re-runs palette extraction on the loaded image */
    void recalculate();
    /** Runs k-means in Oklab space and returns sorted palette */
    static auto extractPalette(const unsigned char* pixels, int width, int height, int numColors) -> PaletteResult;
    /** Rebuilds the swatch display for the given palette */
    void displayPalette(const PaletteResult& result);
};
