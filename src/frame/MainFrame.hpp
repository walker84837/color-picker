#pragma once

#include "color/Color.hpp"

#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/wx.h>

class ColorPickerCanvas;
class ColorSlidersPanel;
class ColorPreviewPanel;
class ConvertPanel;
class HarmonyPanel;
class PalettePanel;

/**
 * Central application window.
 * Owns the notebook and coordinates color synchronization across all tabs.
 */
class MainFrame : public wxFrame {
public:
    explicit MainFrame(const wxString& title);

private:
    wxNotebook* m_notebook;

    ColorPickerCanvas* m_pickerCanvas = nullptr;
    ColorSlidersPanel* m_slidersPanel = nullptr;
    ColorPreviewPanel* m_previewPanel = nullptr;
    wxPanel* m_pickerResultPanel = nullptr;
    wxStaticText* m_pickerResultHex = nullptr;
    ConvertPanel* m_convertPanel = nullptr;
    HarmonyPanel* m_harmonyPanel = nullptr;
    PalettePanel* m_palettePanel = nullptr;

    auto createPickerTab() -> wxPanel*;
    auto createConvertTab() -> wxPanel*;
    auto createHarmonyTab() -> wxPanel*;
    auto createPaletteTab() -> wxPanel*;

    /**
     * App-wide synchronization path.
     * Called whenever any UI component changes the active color.
     * Updates all panels to reflect the new state.
     */
    void onColorChanged(const Color& color);

    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
};
