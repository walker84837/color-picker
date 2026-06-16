#include "PalettePanel.hpp"

#include "ColorSwatch.hpp"

#include <algorithm>
#include <cmath>
#include <kmeans/kmeans.hpp>
#include <numeric>
#include <random>
#include <stb/stb_image.h>
#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include <wx/gbsizer.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>

PalettePanel::PalettePanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    auto* topRow = new wxBoxSizer(wxHORIZONTAL);

    auto* loadBtn = new wxButton(this, wxID_ANY, "Load Image...");
    topRow->Add(loadBtn, 0, wxALL, FromDIP(5));

    auto* numLabel = new wxStaticText(this, wxID_ANY, "Colors:");
    topRow->Add(numLabel, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, FromDIP(10));

    auto* numCtrl = new wxSpinCtrl(this, wxID_ANY, "8", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 2, 16, 8);
    topRow->Add(numCtrl, 0, wxALL, FromDIP(5));

    sizer->Add(topRow, 0, wxEXPAND);

    m_imageCtrl = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap, wxDefaultPosition, FromDIP(wxSize(200, 200)));
    sizer->Add(m_imageCtrl, 0, wxALL, FromDIP(5));

    m_emptyLabel = new wxStaticText(this, wxID_ANY, "No image loaded. Click 'Load Image...' to extract a palette.");
    m_emptyLabel->SetForegroundColour(wxColour(128, 128, 128));
    auto font = m_emptyLabel->GetFont();
    font.SetStyle(wxFONTSTYLE_ITALIC);
    m_emptyLabel->SetFont(font);
    sizer->Add(m_emptyLabel, 0, wxALL, FromDIP(10));

    m_paletteArea = new wxPanel(this, wxID_ANY);
    m_paletteSizer = new wxBoxSizer(wxHORIZONTAL);
    m_paletteArea->SetSizer(m_paletteSizer);
    sizer->Add(m_paletteArea, 0, wxEXPAND | wxALL, FromDIP(5));

    SetSizer(sizer);

    loadBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) -> void {
        wxFileDialog dlg(this, "Open Image", "", "",
                         "Image files (*.png;*.jpg;*.jpeg;*.bmp;*.gif)|*.png;*.jpg;*.jpeg;*.bmp;*.gif",
                         wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (dlg.ShowModal() == wxID_OK) {
            loadImage(dlg.GetPath());
        }
    });

    numCtrl->Bind(wxEVT_SPINCTRL, [this](wxSpinEvent& event) -> void {
        m_numColors = event.GetPosition();
        if (m_hasImage) {
            recalculate();
        }
    });
}

void PalettePanel::loadImage(const wxString& path) {
    int w;
    int h;
    int ch;
    unsigned char* pixels = stbi_load(path.ToStdString().c_str(), &w, &h, &ch, 3);
    if (pixels == nullptr) {
        wxMessageBox("Failed to load image", "Error", wxICON_ERROR);
        return;
    }

    m_imageWidth = w;
    m_imageHeight = h;
    int totalBytes = w * h * 3;
    m_pixels.assign(pixels, pixels + totalBytes);
    m_hasImage = true;
    m_emptyLabel->Show(false);

    wxImage img(w, h, pixels, true);
    int thumbW = std::min(FromDIP(200), w);
    int thumbH = static_cast<int>(thumbW * static_cast<double>(h) / w);
    wxImage thumb = img.Scale(thumbW, thumbH, wxIMAGE_QUALITY_HIGH);
    m_imageCtrl->SetBitmap(wxBitmap(thumb));

    recalculate();

    stbi_image_free(pixels);
    Layout();
}

void PalettePanel::recalculate() {
    if (!m_hasImage) {
        return;
    }
    auto result = extractPalette(m_pixels.data(), m_imageWidth, m_imageHeight, m_numColors);
    displayPalette(result);
}

auto PalettePanel::extractPalette(const unsigned char* pixels, int width, int height, int numColors)
    -> PalettePanel::PaletteResult {
    // Subsample pixels if image is large to keep k-means fast.
    // We use Oklab for clustering because it is perceptually uniform,
    // meaning distances between colors match human perception better than RGB.
    int totalPixels = width * height;
    int maxSamples = 10000;
    int step = std::max(1, totalPixels / maxSamples);

    int nobs = 0;
    for (int i = 0; i < totalPixels; i += step) {
        nobs++;
    }

    std::vector<double> data;
    data.reserve(nobs * 3);

    for (int i = 0; i < totalPixels; i += step) {
        int idx = i * 3;
        colorm::Rgb rgb(pixels[idx], pixels[idx + 1], pixels[idx + 2]);
        colorm::Oklab lab(rgb);
        data.push_back(lab.lightness());
        data.push_back(lab.a());
        data.push_back(lab.b());
    }

    // kmeans expects column-major: 3 rows (L, a, b), nobs columns
    kmeans::SimpleMatrix<int, double> mat(3, nobs, data.data());

    auto res = kmeans::compute(mat, kmeans::InitializeKmeanspp<int, double, int, double>(),
                               kmeans::RefineLloyd<int, double, int, double>(), numColors);

    PaletteResult result;
    result.colors.reserve(numColors);
    result.counts.resize(numColors, 0);

    for (int c = 0; c < res.details.sizes.size(); c++) {
        result.counts[c] = res.details.sizes[c];
    }

    for (int c = 0; c < numColors; c++) {
        double L = res.centers[(c * 3) + 0];
        double a = res.centers[(c * 3) + 1];
        double b = res.centers[(c * 3) + 2];
        result.colors.emplace_back(colorm::Rgb(colorm::Oklab(L, a, b)));
    }

    // Sort by count descending
    std::vector<int> indices(numColors);
    std::ranges::iota(indices, 0);
    std::ranges::sort(indices, [&](int i, int j) -> bool { return result.counts[i] > result.counts[j]; });

    PaletteResult sorted;
    sorted.colors.reserve(numColors);
    sorted.counts.reserve(numColors);
    for (int i : indices) {
        sorted.colors.push_back(result.colors[i]);
        sorted.counts.push_back(result.counts[i]);
    }

    return sorted;
}

void PalettePanel::displayPalette(const PalettePanel::PaletteResult& result) {
    m_paletteSizer->Clear(true);
    m_swatches.clear();

    int totalSamples = std::accumulate(result.counts.begin(), result.counts.end(), 0);

    for (size_t i = 0; i < result.colors.size(); i++) {
        auto* col = new wxBoxSizer(wxVERTICAL);

        auto* swatch = new ColorSwatch(m_paletteArea, result.colors[i], result.colors[i] == m_color);
        swatch->onClick = [this](const Color& c) -> void {
            if (onColorChanged) {
                onColorChanged(c);
            }
        };
        m_swatches.push_back(swatch);
        col->Add(swatch, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, FromDIP(2));

        auto* hex = new wxStaticText(m_paletteArea, wxID_ANY, result.colors[i].hex());
        col->Add(hex, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, FromDIP(2));

        double pct = (totalSamples > 0) ? (100.0 * result.counts[i] / totalSamples) : 0.0;
        auto* count = new wxStaticText(m_paletteArea, wxID_ANY, wxString::Format("%.1f%%", pct));
        count->SetForegroundColour(wxColour(128, 128, 128));
        auto font = count->GetFont();
        font.SetPointSize(font.GetPointSize() - 1);
        count->SetFont(font);
        col->Add(count, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(2));

        m_paletteSizer->Add(col, 0, wxALL, FromDIP(2));
    }

    m_paletteArea->Layout();
    Layout();
}

void PalettePanel::setColor(const Color& color) {
    m_color = color;
    for (auto* swatch : m_swatches) {
        swatch->setSelected(swatch->color() == m_color);
    }
}
