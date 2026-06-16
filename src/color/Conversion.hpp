#pragma once

#include <colorm.h>
#include <tuple>

namespace conv {

/**
 * HSV: h [0, 360], s [0, 100], v [0, 100]
 */
auto rgbToHsv(const colorm::Rgb& rgb) -> std::tuple<double, double, double>;
auto hsvToRgb(double h, double s, double v) -> colorm::Rgb;

/**
 * CMYK: all [0, 100]
 */
auto rgbToCmyk(const colorm::Rgb& rgb) -> std::tuple<double, double, double, double>;
auto cmykToRgb(double c, double m, double y, double k) -> colorm::Rgb;

} // namespace conv
