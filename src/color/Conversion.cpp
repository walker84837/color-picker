#include "Conversion.hpp"

#include <algorithm>
#include <cmath>

namespace conv {

auto rgbToHsv(const colorm::Rgb& rgb) -> std::tuple<double, double, double> {
    double r = rgb.red() / 255.0;
    double g = rgb.green() / 255.0;
    double b = rgb.blue() / 255.0;

    double cmax = std::max({r, g, b});
    double cmin = std::min({r, g, b});
    double delta = cmax - cmin;

    double h = 0.0;
    if (delta > 1e-10) {
        if (cmax == r) {
            h = 60.0 * std::fmod((g - b) / delta, 6.0);
        } else if (cmax == g) {
            h = 60.0 * (((b - r) / delta) + 2.0);
        } else {
            h = 60.0 * (((r - g) / delta) + 4.0);
        }
    }
    if (h < 0.0) {
        h += 360.0;
    }

    double s = (cmax > 1e-10) ? delta / cmax : 0.0;
    double v = cmax;

    return {h, s, v};
}

auto hsvToRgb(double h, double s, double v) -> colorm::Rgb {
    h = std::fmod(h, 360.0);
    if (h < 0.0) {
        h += 360.0;
    }

    double c = v * s;
    double x = c * (1.0 - std::abs(std::fmod(h / 60.0, 2.0) - 1.0));
    double m = v - c;

    double r;
    double g;
    double b;
    if (h < 60.0) {
        r = c;
        g = x;
        b = 0;
    } else if (h < 120.0) {
        r = x;
        g = c;
        b = 0;
    } else if (h < 180.0) {
        r = 0;
        g = c;
        b = x;
    } else if (h < 240.0) {
        r = 0;
        g = x;
        b = c;
    } else if (h < 300.0) {
        r = x;
        g = 0;
        b = c;
    } else {
        r = c;
        g = 0;
        b = x;
    }

    return {(r + m) * 255.0, (g + m) * 255.0, (b + m) * 255.0};
}

auto rgbToCmyk(const colorm::Rgb& rgb) -> std::tuple<double, double, double, double> {
    double r = rgb.red() / 255.0;
    double g = rgb.green() / 255.0;
    double b = rgb.blue() / 255.0;

    double k = 1.0 - std::max({r, g, b});

    double c;
    double m;
    double y;
    if (k > 1.0 - 1e-10) {
        c = m = y = 0.0;
    } else {
        c = (1.0 - r - k) / (1.0 - k);
        m = (1.0 - g - k) / (1.0 - k);
        y = (1.0 - b - k) / (1.0 - k);
    }

    return {c, m, y, k};
}

auto cmykToRgb(double c, double m, double y, double k) -> colorm::Rgb {
    return {(1.0 - c) * (1.0 - k) * 255.0, (1.0 - m) * (1.0 - k) * 255.0, (1.0 - y) * (1.0 - k) * 255.0};
}

} // namespace conv
