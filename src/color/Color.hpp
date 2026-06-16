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
    /** Default constructor, initializes to blue (#3498db) */
    Color();

    /** Constructs from 8-bit RGB (0-255) */
    explicit Color(const colorm::Rgb& rgb);
    /** Constructs from Oklch, converts to RGB */
    explicit Color(const colorm::Oklch& oklch);

    /** Returns the canonical RGB value */
    [[nodiscard]] auto rgb() const -> colorm::Rgb { return m_rgb; }
    /** Returns Oklch derived from RGB */
    [[nodiscard]] auto oklch() const -> colorm::Oklch { return colorm::Oklch(m_rgb); }

    /** Sets RGB directly */
    void setRgb(const colorm::Rgb& rgb);
    /** Sets from Oklch, converts to RGB */
    void setOklch(const colorm::Oklch& oklch);

    /** Perceptual lightness [0, 1] */
    [[nodiscard]] auto lightness() const -> double;
    /** Perceptual chroma [0, ~0.4] */
    [[nodiscard]] auto chroma() const -> double;
    /** Hue in degrees [0, 360) */
    [[nodiscard]] auto hue() const -> double;

    /** Sets perceptual lightness [0, 1] */
    void setLightness(double l);
    /** Sets perceptual chroma [0, ~0.4] */
    void setChroma(double c);
    /** Sets hue in degrees [0, 360) */
    void setHue(double h);

    /** Converts to wxWidgets native color type */
    auto wx() const -> wxColor;

    /** Returns #rrggbb string */
    [[nodiscard]] auto hex() const -> std::string;
    /** Returns rgb(r, g, b) string */
    [[nodiscard]] auto cssRgb() const -> std::string;
    /** Returns oklch(l c h) string */
    [[nodiscard]] auto cssOklch() const -> std::string;

    /** Compares by 8-bit RGB components */
    auto operator==(const Color& other) const -> bool;
    /** Negation of operator== */
    auto operator!=(const Color& other) const -> bool { return !(*this == other); }

private:
    colorm::Rgb m_rgb;
};
