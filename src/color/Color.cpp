#include "Color.hpp"

Color::Color() : m_rgb(0x3498db) {}

Color::Color(const colorm::Rgb& rgb) : m_rgb(rgb) {}

Color::Color(const colorm::Oklch& oklch) : m_rgb(colorm::Rgb(oklch)) {}

void Color::setRgb(const colorm::Rgb& rgb) {
    m_rgb = rgb;
}

void Color::setOklch(const colorm::Oklch& oklch) {
    m_rgb = colorm::Rgb(oklch);
}

auto Color::lightness() const -> double {
    return colorm::Oklch(m_rgb).lightness();
}

auto Color::chroma() const -> double {
    return colorm::Oklch(m_rgb).chroma();
}

auto Color::hue() const -> double {
    return colorm::Oklch(m_rgb).hue();
}

void Color::setLightness(double l) {
    auto ok = colorm::Oklch(m_rgb);
    ok.setLightness(l);
    m_rgb = colorm::Rgb(ok);
}

void Color::setChroma(double c) {
    auto ok = colorm::Oklch(m_rgb);
    ok.setChroma(c);
    m_rgb = colorm::Rgb(ok);
}

void Color::setHue(double h) {
    auto ok = colorm::Oklch(m_rgb);
    ok.setHue(h);
    m_rgb = colorm::Rgb(ok);
}

auto Color::wx() const -> wxColor {
    return wxColor(static_cast<unsigned char>(m_rgb.red8()), static_cast<unsigned char>(m_rgb.green8()),
                   static_cast<unsigned char>(m_rgb.blue8()));
}

auto Color::hex() const -> std::string {
    return m_rgb.hex();
}

auto Color::cssRgb() const -> std::string {
    return m_rgb.css();
}

auto Color::cssOklch() const -> std::string {
    auto ok = colorm::Oklch(m_rgb);
    return ok.css();
}

auto Color::operator==(const Color& other) const -> bool {
    return m_rgb.red8() == other.m_rgb.red8() && m_rgb.green8() == other.m_rgb.green8() &&
           m_rgb.blue8() == other.m_rgb.blue8();
}
