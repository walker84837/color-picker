# color-picker

<img width="1920" height="1044" alt="image" src="https://github.com/user-attachments/assets/341a736b-ff73-4f62-baba-65de71a4e0a9" />

A C++23 desktop color picker built with wxWidgets, featuring advanced color space support, harmony generation, and palette extraction from images.

## Features

- **Picker**: Custom-drawn Oklch hue strip and L x C area for perceptually accurate color selection.
- **Convert**: Color comparison using CIE76 and CIEDE2000 Delta E, with WCAG contrast evaluation.
- **Harmonies**: Automatic generation of Analogous, Complementary, Triad, Tetrad, and Split Complementary schemes.
- **Palette**: K-means clustering in Oklab space to extract the dominant palette from any image.

## Build Requirements

- C++23 compatible compiler (GCC 12+, Clang 15+, or MSVC 19.34+)
- CMake 3.24+
- wxWidgets 3.2+

## Build & Run

```bash
# Debug build
cmake --preset debug && cmake --build --preset debug
./build/color_picker

# Release build
cmake --preset release && cmake --build --preset release
./build/color_picker
```

## Dependencies

- [**wxWidgets**](https://github.com/wxWidgets/wxWidgets): GUI framework
- [**colorm**](https://github.com/soreja/colorm): Modern C++ color library
- **stb_image**: Image loading ([cmake wrapper](https://github.com/gracicot/stb-cmake) | [actual c lib](https://github.com/nothings/stb))
- [**ltla::kmeans**](https://github.com/libscran/kmeans): K-means clustering implementation

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
