#pragma once

#include <colorm.h>
#include <wx/wx.h>

/**
 * Canonical color storage for the application.
 * Uses colorm::Rgb (0-255 range) as the source of truth.
 * All conversions to other spaces (Oklch, Oklab) are derived from this.
 */
class Color {
public:
    Color();

    /** 0-255 range for components */
    explicit Color(const colorm::Rgb& rgb);
    explicit Color(const colorm::Oklch& oklch);

    [[nodiscard]] auto rgb() const -> colorm::Rgb { return m_rgb; }
    [[nodiscard]] auto oklch() const -> colorm::Oklch { return colorm::Oklch(m_rgb); }

    void setRgb(const colorm::Rgb& rgb);
    void setOklch(const colorm::Oklch& oklch);

    /** Perceptual lightness [0, 1] */
    [[nodiscard]] auto lightness() const -> double;
    /** Perceptual chroma [0, ~0.4] */
    [[nodiscard]] auto chroma() const -> double;
    /** Hue in degrees [0, 360) */
    [[nodiscard]] auto hue() const -> double;

    void setLightness(double l);
    void setChroma(double c);
    void setHue(double h);

    /** Convert to wxWidgets native color type */
    auto wx() const -> wxColor;

    /** #rrggbb string */
    [[nodiscard]] auto hex() const -> std::string;
    /** rgb(r, g, b) string */
    [[nodiscard]] auto cssRgb() const -> std::string;
    /** oklch(l c h) string */
    [[nodiscard]] auto cssOklch() const -> std::string;

    auto operator==(const Color& other) const -> bool;
    auto operator!=(const Color& other) const -> bool { return !(*this == other); }

private:
    colorm::Rgb m_rgb;
};
