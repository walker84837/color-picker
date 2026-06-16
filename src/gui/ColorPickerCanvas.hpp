#pragma once

#include "color/Color.hpp"

#include <cstdint>
#include <wx/dcbuffer.h>
#include <wx/wx.h>

class ColorPickerCanvas : public wxWindow {
public:
    explicit ColorPickerCanvas(wxWindow* parent);

    [[nodiscard]] auto color() const -> const Color& { return m_color; }
    void setColor(const Color& color);

    std::function<void(const Color&)> onColorChanged;

private:
    Color m_color;
    double m_maxChroma = 0.35;

    wxBitmap m_colorAreaCache;
    wxBitmap m_hueStripCache;

    int m_hueStripWidth = 30;
    int m_padding = 4;

    [[nodiscard]] auto colorAreaX() const -> int { return m_padding; }
    [[nodiscard]] auto colorAreaY() const -> int { return m_padding; }
    [[nodiscard]] auto colorAreaWidth() const -> int;
    [[nodiscard]] auto colorAreaHeight() const -> int;
    [[nodiscard]] auto stripX() const -> int;
    [[nodiscard]] auto stripY() const -> int { return m_padding; }
    [[nodiscard]] auto stripHeight() const -> int;

    auto hitTestColorArea(int x, int y, double& l, double& c) const -> bool;
    auto hitTestHueStrip(int x, int y, double& h) const -> bool;

    void rebuildCache();
    void renderColorArea(wxImage& image) const;
    static void renderHueStrip(wxImage& image);

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnCaptureLost(wxMouseCaptureLostEvent& event);

    enum class DragTarget : std::uint8_t { None, ColorArea, HueStrip };
    DragTarget m_dragTarget = DragTarget::None;
};
