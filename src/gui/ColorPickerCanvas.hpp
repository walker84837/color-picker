#pragma once

#include "color/Color.hpp"

#include <cstdint>
#include <wx/dcbuffer.h>
#include <wx/wx.h>

class ColorPickerCanvas : public wxWindow {
public:
    /** Constructs the picker with hue strip and LxC color area */
    explicit ColorPickerCanvas(wxWindow* parent);

    /** Returns the current selected color */
    [[nodiscard]] auto color() const -> const Color& { return m_color; }
    /** Sets the color and invalidates the cache */
    void setColor(const Color& color);

    std::function<void(const Color&)> onColorChanged;

private:
    Color m_color;
    double m_maxChroma = 0.35;

    wxBitmap m_colorAreaCache;
    wxBitmap m_hueStripCache;

    int m_hueStripWidth = 30;
    int m_padding = 4;

    /** Left edge of the LxC color area */
    [[nodiscard]] auto colorAreaX() const -> int { return m_padding; }
    /** Top edge of the LxC color area */
    [[nodiscard]] auto colorAreaY() const -> int { return m_padding; }
    /** Width of the LxC color area */
    [[nodiscard]] auto colorAreaWidth() const -> int;
    /** Height of the LxC color area */
    [[nodiscard]] auto colorAreaHeight() const -> int;
    /** Left edge of the hue strip */
    [[nodiscard]] auto stripX() const -> int;
    /** Top edge of the hue strip */
    [[nodiscard]] auto stripY() const -> int { return m_padding; }
    /** Height of the hue strip */
    [[nodiscard]] auto stripHeight() const -> int;

    /** Maps pixel coordinates to lightness and chroma in the color area */
    auto hitTestColorArea(int x, int y, double& l, double& c) const -> bool;
    /** Maps pixel y-coordinate to hue in the strip */
    auto hitTestHueStrip(int x, int y, double& h) const -> bool;

    /** Rebuilds the two bitmap caches for the current size */
    void rebuildCache();
    /** Renders the LxC rectangle for the current hue */
    void renderColorArea(wxImage& image) const;
    /** Renders the vertical hue strip */
    static void renderHueStrip(wxImage& image);

    /** Paints the cached color area and hue strip */
    void OnPaint(wxPaintEvent& event);
    /** Invalidates caches on resize */
    void OnSize(wxSizeEvent& event);
    /** Begins drag on color area or hue strip */
    void OnMouseDown(wxMouseEvent& event);
    /** Updates color during drag */
    void OnMouseMove(wxMouseEvent& event);
    /** Ends drag and releases mouse capture */
    void OnMouseUp(wxMouseEvent& event);
    /** Resets drag state when capture is lost externally */
    void OnCaptureLost(wxMouseCaptureLostEvent& event);

    enum class DragTarget : std::uint8_t { None, ColorArea, HueStrip };
    DragTarget m_dragTarget = DragTarget::None;
};
