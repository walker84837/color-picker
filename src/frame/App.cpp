#include "App.hpp"
#include "frame/MainFrame.hpp"

auto ColorSmithyApp::OnInit() -> bool {
    auto* frame = new MainFrame("colorsmithy");
    frame->Show(true);
    return true;
}
