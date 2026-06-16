#include "MainFrame.hpp"

#include "color/Color.hpp"
#include "gui/ColorPickerCanvas.hpp"
#include "gui/ColorPreviewPanel.hpp"
#include "gui/ColorSlidersPanel.hpp"
#include "gui/ConvertPanel.hpp"
#include "gui/HarmonyPanel.hpp"
#include "gui/PalettePanel.hpp"

#include <wx/aboutdlg.h>
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/statline.h>
#include <wx/stattext.h>

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(1000, 700)) {
    auto* menu_file = new wxMenu;
    menu_file->Append(wxID_EXIT);

    auto* menu_help = new wxMenu;
    menu_help->Append(wxID_ABOUT);

    auto* menu_bar = new wxMenuBar;
    menu_bar->Append(menu_file, "&File");
    menu_bar->Append(menu_help, "&Help");
    SetMenuBar(menu_bar);

    Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);

    m_notebook = new wxNotebook(this, wxID_ANY);

    m_notebook->AddPage(createPickerTab(), "Picker");
    m_notebook->AddPage(createConvertTab(), "Convert");
    m_notebook->AddPage(createHarmonyTab(), "Harmonies");
    m_notebook->AddPage(createPaletteTab(), "Palette");

    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    auto* hint = new wxStaticText(
        this, wxID_ANY, "Tip: click any color box or swatch in Picker, Harmonies, or Palette to set the active color.");
    auto hintFont = hint->GetFont();
    hintFont.SetStyle(wxFONTSTYLE_ITALIC);
    hint->SetFont(hintFont);
    hint->SetForegroundColour(wxColour(128, 128, 128));
    main_sizer->Add(hint, 0, wxLEFT | wxRIGHT | wxTOP, FromDIP(8));

    main_sizer->Add(m_notebook, 1, wxEXPAND | wxALL, FromDIP(5));
    SetSizer(main_sizer);

    CreateStatusBar();
    SetStatusText("Ready");

    onColorChanged(m_pickerCanvas->color());
}

auto MainFrame::createPickerTab() -> wxPanel* {
    auto* panel = new wxPanel(m_notebook, wxID_ANY);
    auto* outerSizer = new wxBoxSizer(wxVERTICAL);
    auto* contentRow = new wxBoxSizer(wxHORIZONTAL);

    auto* leftCol = new wxBoxSizer(wxVERTICAL);

    auto* resultBox = new wxStaticBoxSizer(wxVERTICAL, panel, "Resulting Color");
    m_pickerResultPanel = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, FromDIP(42)));
    m_pickerResultPanel->SetBackgroundColour(*wxBLACK);
    resultBox->Add(m_pickerResultPanel, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, FromDIP(6));

    m_pickerResultHex = new wxStaticText(panel, wxID_ANY, "#000000", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
    auto resultFont = m_pickerResultHex->GetFont();
    resultFont.SetWeight(wxFONTWEIGHT_BOLD);
    m_pickerResultHex->SetFont(resultFont);
    resultBox->Add(m_pickerResultHex, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, FromDIP(6));

    leftCol->Add(resultBox, 0, wxEXPAND | wxBOTTOM, FromDIP(8));

    m_pickerCanvas = new ColorPickerCanvas(panel);
    m_pickerCanvas->SetMinSize(FromDIP(wxSize(280, 280)));
    leftCol->Add(m_pickerCanvas, 1, wxEXPAND);
    contentRow->Add(leftCol, 0, wxEXPAND | wxRIGHT, FromDIP(10));

    m_previewPanel = new ColorPreviewPanel(panel, false);
    m_previewPanel->SetMinSize(FromDIP(wxSize(340, -1)));
    contentRow->Add(m_previewPanel, 2, wxEXPAND | wxRIGHT, FromDIP(10));

    // Right Column: Sliders (scrolled)
    auto* sliderScroll = new wxScrolledWindow(panel, wxID_ANY);
    sliderScroll->SetScrollRate(0, 10);
    sliderScroll->SetMinSize(FromDIP(wxSize(300, -1)));
    auto* sliderSizer = new wxBoxSizer(wxVERTICAL);
    m_slidersPanel = new ColorSlidersPanel(sliderScroll);
    sliderSizer->Add(m_slidersPanel, 1, wxEXPAND);
    sliderScroll->SetSizer(sliderSizer);
    sliderScroll->FitInside();
    contentRow->Add(sliderScroll, 1, wxEXPAND);

    outerSizer->Add(contentRow, 1, wxEXPAND | wxALL, FromDIP(8));
    panel->SetSizer(outerSizer);

    auto onColorUpdate = [this](const Color& c) -> void { this->onColorChanged(c); };
    m_pickerCanvas->onColorChanged = onColorUpdate;
    m_slidersPanel->onColorChanged = onColorUpdate;

    return panel;
}

auto MainFrame::createConvertTab() -> wxPanel* {
    m_convertPanel = new ConvertPanel(m_notebook);
    return m_convertPanel;
}

auto MainFrame::createHarmonyTab() -> wxPanel* {
    auto* panel = new wxPanel(m_notebook, wxID_ANY);
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    m_harmonyPanel = new HarmonyPanel(panel);
    sizer->Add(m_harmonyPanel, 1, wxEXPAND | wxALL, FromDIP(5));

    panel->SetSizer(sizer);

    m_harmonyPanel->onColorChanged = [this](const Color& c) -> void { this->onColorChanged(c); };

    return panel;
}

auto MainFrame::createPaletteTab() -> wxPanel* {
    auto* panel = new wxPanel(m_notebook, wxID_ANY);
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    m_palettePanel = new PalettePanel(panel);
    sizer->Add(m_palettePanel, 1, wxEXPAND | wxALL, FromDIP(5));

    panel->SetSizer(sizer);

    m_palettePanel->onColorChanged = [this](const Color& c) -> void { this->onColorChanged(c); };

    return panel;
}

void MainFrame::onColorChanged(const Color& color) {
    m_pickerCanvas->setColor(color);
    m_slidersPanel->setColor(color);
    m_previewPanel->setColor(color);
    m_pickerResultPanel->SetBackgroundColour(color.wx());
    m_pickerResultPanel->Refresh();
    m_pickerResultHex->SetLabelText(color.hex());
    m_convertPanel->setColorA(color);
    m_harmonyPanel->setColor(color);
    m_palettePanel->setColor(color);
    SetStatusText(color.hex());
}

void MainFrame::OnExit(wxCommandEvent& /*unused*/) {
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& /*unused*/) {
    wxAboutDialogInfo info;
    info.SetName("Color Picker");
    info.SetVersion("0.1.0");
    info.SetDescription("A desktop color picker with advanced color space support, "
                        "harmony generation, and palette extraction from images.");
    info.SetCopyright("MIT");
    wxAboutBox(info);
}
