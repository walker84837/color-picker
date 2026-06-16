#pragma once

#include <wx/wx.h>

class ColorPickerApp : public wxApp {
public:
    /** Creates and shows the main frame */
    auto OnInit() -> bool override;
};
