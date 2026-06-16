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
    /** Constructs the window, notebook, menu bar, and all tabs */
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

    /** Builds the Picker tab: canvas, preview, sliders */
    auto createPickerTab() -> wxPanel*;
    /** Builds the Convert tab: color comparison and WCAG */
    auto createConvertTab() -> wxPanel*;
    /** Builds the Harmonies tab: generated color schemes */
    auto createHarmonyTab() -> wxPanel*;
    /** Builds the Palette tab: image loading and extraction */
    auto createPaletteTab() -> wxPanel*;

    /**
     * App-wide synchronization path.
     * Called whenever any UI component changes the active color.
     * Updates all panels to reflect the new state.
     */
    void onColorChanged(const Color& color);

    /** Handles File > Exit */
    void OnExit(wxCommandEvent& event);
    /** Handles Help > About */
    void OnAbout(wxCommandEvent& event);
};
