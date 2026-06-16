#include "ColorPickerCanvas.hpp"

#include <algorithm>
#include <vector>
#include <wx/graphics.h>

namespace {
constexpr int kGamutSearchIterations = 12;
constexpr double kHueStripLightness = 0.55;
constexpr double kHueStripChroma = 0.2;

auto maxDisplayableChroma(double lightness, double hue, double chromaUpperBound) -> double {
    if (chromaUpperBound <= 0.0) {
        return 0.0;
    }

    colorm::Oklch upper(lightness, chromaUpperBound, hue);
    if (upper.isDisplayable()) {
        return chromaUpperBound;
    }

    double low = 0.0;
    double high = chromaUpperBound;
    for (int i = 0; i < kGamutSearchIterations; i++) {
        const double mid = (low + high) * 0.5;
        if (colorm::Oklch(lightness, mid, hue).isDisplayable()) {
            low = mid;
        } else {
            high = mid;
        }
    }

    return low;
}
} // namespace

ColorPickerCanvas::ColorPickerCanvas(wxWindow* parent)
    : wxWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetInitialSize(FromDIP(wxSize(300, 300)));

    m_hueStripWidth = FromDIP(30);
    m_padding = FromDIP(4);

    Bind(wxEVT_PAINT, &ColorPickerCanvas::OnPaint, this);
    Bind(wxEVT_SIZE, &ColorPickerCanvas::OnSize, this);
    Bind(wxEVT_LEFT_DOWN, &ColorPickerCanvas::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &ColorPickerCanvas::OnMouseUp, this);
    Bind(wxEVT_MOTION, &ColorPickerCanvas::OnMouseMove, this);
    Bind(wxEVT_MOUSE_CAPTURE_LOST, &ColorPickerCanvas::OnCaptureLost, this);
}

void ColorPickerCanvas::setColor(const Color& color) {
    m_color = color;
    m_colorAreaCache = wxBitmap();
    Refresh();
}

auto ColorPickerCanvas::colorAreaWidth() const -> int {
    int w;
    int h;
    GetClientSize(&w, &h);
    return std::max(1, w - m_hueStripWidth - (m_padding * 3));
}

auto ColorPickerCanvas::colorAreaHeight() const -> int {
    int w;
    int h;
    GetClientSize(&w, &h);
    return std::max(1, h - (m_padding * 2));
}

auto ColorPickerCanvas::stripX() const -> int {
    return colorAreaWidth() + (m_padding * 2);
}

auto ColorPickerCanvas::stripHeight() const -> int {
    return colorAreaHeight();
}

auto ColorPickerCanvas::hitTestColorArea(int x, int y, double& l, double& c) const -> bool {
    auto ax = colorAreaX();
    auto ay = colorAreaY();
    auto aw = colorAreaWidth();
    auto ah = colorAreaHeight();

    // For initial hit test (mouse down), we want strict bounds.
    // For ongoing drag, we handle clamping in OnMouseMove.
    if (m_dragTarget == DragTarget::None) {
        if (x < ax || x >= ax + aw || y < ay || y >= ay + ah) {
            return false;
        }
    }

    // Clamp coordinates to the box for the actual color calculation
    const int cx = std::clamp(x, ax, ax + aw - 1);
    const int cy = std::clamp(y, ay, ay + ah - 1);

    l = 1.0 - (static_cast<double>(cy - ay) / (ah > 1 ? ah - 1 : 1));
    l = std::clamp(l, 0.0, 1.0);

    const double hue = colorm::Oklch(m_color.rgb()).hue();
    const double rowMax = maxDisplayableChroma(l, hue, m_maxChroma);

    if (rowMax > 0) {
        c = static_cast<double>(cx - ax) / (aw > 1 ? aw - 1 : 1) * rowMax;
    } else {
        c = 0;
    }
    c = std::clamp(c, 0.0, rowMax);

    return true;
}

auto ColorPickerCanvas::hitTestHueStrip(int x, int y, double& h) const -> bool {
    auto sx = stripX();
    auto sy = stripY();
    auto sw = m_hueStripWidth;
    auto sh = stripHeight();

    if (m_dragTarget == DragTarget::None) {
        if (x < sx || x >= sx + sw || y < sy || y >= sy + sh) {
            return false;
        }
    }

    const int cy = std::clamp(y, sy, sy + sh - 1);

    h = 360.0 * static_cast<double>(cy - sy) / (sh > 1 ? sh - 1 : 1);
    h = std::clamp(h, 0.0, 360.0);

    return true;
}

void ColorPickerCanvas::rebuildCache() {
    auto caw = colorAreaWidth();
    auto cah = colorAreaHeight();
    auto sh = stripHeight();

    if (caw > 0 && cah > 0) {
        wxImage areaImg(caw, cah);
        renderColorArea(areaImg);
        m_colorAreaCache = wxBitmap(areaImg);
    }

    if (m_hueStripWidth > 0 && sh > 0) {
        wxImage stripImg(m_hueStripWidth, sh);
        renderHueStrip(stripImg);
        m_hueStripCache = wxBitmap(stripImg);
    }
}

auto clamp8 = [](double v) -> unsigned char { return static_cast<unsigned char>(std::clamp(v, 0.0, 255.0)); };

/**
 * Renders the L x C rectangle for a fixed hue.
 * Horizontal axis: Chroma [0, m_maxChroma]
 * Vertical axis: Lightness [1, 0] (top to bottom)
 */
void ColorPickerCanvas::renderColorArea(wxImage& image) const {
    auto hue = colorm::Oklch(m_color.rgb()).hue();
    auto w = image.GetWidth();
    auto h = image.GetHeight();

    for (int y = 0; y < h; y++) {
        const double l = 1.0 - (static_cast<double>(y) / (h > 1 ? h - 1 : 1));
        const double rowMax = maxDisplayableChroma(l, hue, m_maxChroma);
        for (int x = 0; x < w; x++) {
            double c = 0;
            if (rowMax > 0) {
                c = static_cast<double>(x) / (w > 1 ? w - 1 : 1) * rowMax;
            }

            colorm::Oklch ok(l, c, hue);
            colorm::Rgb rgb(ok);

            image.SetRGB(x, y, rgb.red8(), rgb.green8(), rgb.blue8());
        }
    }
}

void ColorPickerCanvas::renderHueStrip(wxImage& image) {
    auto w = image.GetWidth();
    auto h = image.GetHeight();

    for (int y = 0; y < h; y++) {
        auto hue = 360.0 * static_cast<double>(y) / (h > 1 ? h - 1 : 1);
        const double cMax = maxDisplayableChroma(kHueStripLightness, hue, kHueStripChroma);
        colorm::Oklch ok(kHueStripLightness, cMax, hue);
        colorm::Rgb rgb(ok);

        for (int x = 0; x < w; x++) {
            image.SetRGB(x, y, rgb.red8(), rgb.green8(), rgb.blue8());
        }
    }
}

void ColorPickerCanvas::OnPaint(wxPaintEvent& /*unused*/) {
    wxAutoBufferedPaintDC dc(this);

    auto [cw, ch] = GetClientSize();

    dc.SetBackground(wxBrush(GetBackgroundColour()));
    dc.Clear();

    if (cw <= 0 || ch <= 0) {
        return;
    }

    if (!m_colorAreaCache.IsOk() || !m_hueStripCache.IsOk()) {
        rebuildCache();
    }

    wxGraphicsContext* gc = dc.GetGraphicsContext();
    if (gc == nullptr) {
        return;
    }

    auto hue = colorm::Oklch(m_color.rgb()).hue();

    // color area
    if (m_colorAreaCache.IsOk()) {
        gc->DrawBitmap(m_colorAreaCache, colorAreaX(), colorAreaY(), colorAreaWidth(), colorAreaHeight());
    }

    // border around color area
    gc->SetPen(wxPen(wxColour(120, 120, 120)));
    gc->SetBrush(*wxTRANSPARENT_BRUSH);
    gc->DrawRectangle(colorAreaX(), colorAreaY(), colorAreaWidth(), colorAreaHeight());

    // crosshair on color area
    const double l = m_color.lightness();
    const double rowMax = maxDisplayableChroma(l, hue, m_maxChroma);

    const int aw = colorAreaWidth();
    const int ah = colorAreaHeight();

    int cx;
    if (rowMax > 1e-6) {
        cx = static_cast<int>(m_color.chroma() / rowMax * (aw > 1 ? aw - 1 : 1));
    } else {
        cx = 0;
    }
    auto cy = static_cast<int>((1.0 - l) * (ah > 1 ? ah - 1 : 1));

    cx += colorAreaX();
    cy += colorAreaY();

    gc->SetPen(wxPen(*wxWHITE, 2));
    gc->StrokeLine(cx - 6, cy, cx - 2, cy);
    gc->StrokeLine(cx + 2, cy, cx + 6, cy);
    gc->StrokeLine(cx, cy - 6, cx, cy - 2);
    gc->StrokeLine(cx, cy + 2, cx, cy + 6);

    gc->SetPen(wxPen(*wxBLACK, 1));
    gc->SetBrush(*wxTRANSPARENT_BRUSH);
    gc->DrawEllipse(cx - 4, cy - 4, 8, 8);

    // hue strip
    if (m_hueStripCache.IsOk()) {
        gc->DrawBitmap(m_hueStripCache, stripX(), stripY(), m_hueStripWidth, stripHeight());
    }

    // border around hue strip
    gc->SetPen(wxPen(wxColour(120, 120, 120)));
    gc->DrawRectangle(stripX(), stripY(), m_hueStripWidth, stripHeight());

    // hue indicator triangle
    auto hy = stripY() + static_cast<int>(hue / 360.0 * (stripHeight() > 1 ? stripHeight() - 1 : 1));

    wxPoint2DDouble tri[3] = {
        {static_cast<double>(stripX() + m_hueStripWidth), static_cast<double>(hy)},
        {static_cast<double>(stripX() + m_hueStripWidth + 8), static_cast<double>(hy - 5)},
        {static_cast<double>(stripX() + m_hueStripWidth + 8), static_cast<double>(hy + 5)},
    };

    gc->SetPen(wxPen(*wxWHITE, 2));
    gc->SetBrush(*wxWHITE_BRUSH);
    gc->DrawLines(3, tri);
}

void ColorPickerCanvas::OnSize(wxSizeEvent& event) {
    m_colorAreaCache = wxBitmap();
    m_hueStripCache = wxBitmap();
    Refresh();
    event.Skip();
}

void ColorPickerCanvas::OnMouseDown(wxMouseEvent& event) {
    double value;

    if (hitTestColorArea(event.GetX(), event.GetY(), value, value)) {
        m_dragTarget = DragTarget::ColorArea;
        if (!HasCapture()) {
            CaptureMouse();
        }
        OnMouseMove(event);
    } else if (hitTestHueStrip(event.GetX(), event.GetY(), value)) {
        m_dragTarget = DragTarget::HueStrip;
        if (!HasCapture()) {
            CaptureMouse();
        }
        OnMouseMove(event);
    }
}

void ColorPickerCanvas::OnMouseMove(wxMouseEvent& event) {
    if (m_dragTarget == DragTarget::None) {
        return;
    }

    // If mouse button is released but we didn't get the Up event (unlikely with capture but safe)
    if (!event.LeftIsDown()) {
        m_dragTarget = DragTarget::None;
        if (HasCapture()) {
            ReleaseMouse();
        }
        return;
    }

    double l;
    double c;
    double h;

    if (m_dragTarget == DragTarget::ColorArea && hitTestColorArea(event.GetX(), event.GetY(), l, c)) {
        auto ok = colorm::Oklch(m_color.rgb());
        ok.setLightness(l);
        ok.setChroma(c);
        m_color = Color(colorm::Rgb(ok));

        m_colorAreaCache = wxBitmap();
        Refresh();

        if (onColorChanged) {
            onColorChanged(m_color);
        }
    } else if (m_dragTarget == DragTarget::HueStrip && hitTestHueStrip(event.GetX(), event.GetY(), h)) {
        auto ok = colorm::Oklch(m_color.rgb());
        ok.setHue(h);

        // Keep the relative chroma position when hue changes
        double oldL = ok.lightness();
        double oldH = colorm::Oklch(m_color.rgb()).hue();
        double oldRowMax = maxDisplayableChroma(oldL, oldH, m_maxChroma);
        double relativeC = (oldRowMax > 1e-6) ? (ok.chroma() / oldRowMax) : 0.0;

        double newRowMax = maxDisplayableChroma(oldL, h, m_maxChroma);
        ok.setChroma(relativeC * newRowMax);

        m_color = Color(colorm::Rgb(ok));

        m_colorAreaCache = wxBitmap();
        Refresh();

        if (onColorChanged) {
            onColorChanged(m_color);
        }
    }
}

void ColorPickerCanvas::OnMouseUp(wxMouseEvent& /*unused*/) {
    if (m_dragTarget != DragTarget::None) {
        m_dragTarget = DragTarget::None;
        if (HasCapture()) {
            ReleaseMouse();
        }
    }
}

void ColorPickerCanvas::OnCaptureLost(wxMouseCaptureLostEvent& /*unused*/) {
    m_dragTarget = DragTarget::None;
}
