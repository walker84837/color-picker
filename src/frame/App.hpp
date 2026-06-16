#pragma once

#include <wx/wx.h>

class ColorSmithyApp : public wxApp {
public:
    /** Creates and shows the main frame */
    auto OnInit() -> bool override;
};
