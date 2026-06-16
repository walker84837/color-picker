cmake_minimum_required(VERSION 3.24)
project(colorsmithy VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(wxWidgets CONFIG QUIET)

if(wxWidgets_FOUND)
  set(wx_LIBS wx::core wx::base)
else()
  find_package(wxWidgets REQUIRED COMPONENTS core base)
  include(${wxWidgets_USE_FILE})
  set(wx_LIBS ${wxWidgets_LIBRARIES})
endif()

include(FetchContent)

FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/gracicot/stb-cmake.git
  GIT_TAG c18890c24e1e75984d67e987aa67325b6d77581c
)
set(STB_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(stb)

FetchContent_Declare(
  kmeans
  GIT_REPOSITORY https://github.com/libscran/kmeans.git
  GIT_TAG 822085feb72123ed813ec22edfb70f403c5227d0
)
set(KMEANS_FETCH_EXTERN ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(kmeans)

FetchContent_Declare(
  colorm
  GIT_REPOSITORY https://github.com/soreja/colorm.git
  GIT_TAG 4d41b6e0fbc32b3237e7e7171850e5f461368050
)
FetchContent_MakeAvailable(colorm)

add_library(colorm_lib INTERFACE)
target_include_directories(colorm_lib INTERFACE "${colorm_SOURCE_DIR}")
target_compile_features(colorm_lib INTERFACE cxx_std_17)

add_executable(colorsmithy
  src/main.cpp
  src/frame/App.hpp
  src/frame/App.cpp
  src/frame/MainFrame.hpp
  src/frame/MainFrame.cpp
  src/color/Color.hpp
  src/color/Color.cpp
  src/color/Conversion.hpp
  src/color/Conversion.cpp
  src/gui/ColorPickerCanvas.hpp
  src/gui/ColorPickerCanvas.cpp
  src/gui/ColorSlidersPanel.hpp
  src/gui/ColorSlidersPanel.cpp
  src/gui/ColorPreviewPanel.hpp
  src/gui/ColorPreviewPanel.cpp
  src/gui/ColorSwatch.hpp
  src/gui/ColorSwatch.cpp
  src/gui/ConvertPanel.hpp
  src/gui/ConvertPanel.cpp
  src/gui/HarmonyPanel.hpp
  src/gui/HarmonyPanel.cpp
  src/gui/PalettePanel.hpp
  src/gui/PalettePanel.cpp
)

target_include_directories(colorsmithy PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(colorsmithy PRIVATE
  ${wx_LIBS}
  stb::image
  ltla::kmeans
  colorm_lib
)

if(wxWidgets_USE_FILE)
  target_compile_definitions(colorsmithy PRIVATE ${wxWidgets_DEFINITIONS})
endif()
