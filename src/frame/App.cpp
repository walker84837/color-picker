#include "App.hpp"
#include "frame/MainFrame.hpp"

auto ColorPickerApp::OnInit() -> bool {
    auto* frame = new MainFrame("Color Picker");
    frame->Show(true);
    return true;
}
