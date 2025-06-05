use dioxus::prelude::*;

const CSS: Asset = asset!("/assets/main.css");

fn main() {
    launch(App);
}

#[component]
fn App() -> Element {
    let mut color = use_signal(|| "#3498db".to_string());
    let mut display_mode = use_signal(|| DisplayMode::Hex);
    let mut text_color = use_signal(|| "#ffffff".to_string());

    use_effect(move || {
        let hex = color().trim_start_matches('#').to_string();
        let mut text_color = text_color.clone();

        spawn(async move {
            if hex.len() == 6 {
                if let (Ok(r), Ok(g), Ok(b)) = (
                    u8::from_str_radix(&hex[0..2], 16),
                    u8::from_str_radix(&hex[2..4], 16),
                    u8::from_str_radix(&hex[4..6], 16),
                ) {
                    let luminance =
                        (0.299 * r as f32 + 0.587 * g as f32 + 0.114 * b as f32) / 255.0;
                    let new_text_color = if luminance > 0.5 {
                        "#000000"
                    } else {
                        "#ffffff"
                    };
                    text_color.set(new_text_color.to_string());
                }
            }
        });

        ()
    });

    let preview_text = match display_mode() {
        DisplayMode::Hex => color.to_string(),
        DisplayMode::Rgb => hex_to_rgb(&color()),
    };

    rsx! {
        document::Stylesheet { href: CSS }
        main {
            class: "container",
            h1 { class: "title", "Simple colour picker" }
            div {
                class: "color-picker-container",
                label {
                    r#for: "text-color",
                    class: "color-label",
                    "Color:"
                }
                input {
                    id: "text-color",
                    class: "color-input",
                    r#type: "color",
                    value: "{color}",
                    oninput: move |e| color.set(e.value().clone())
                }
                div {
                    class: "color-preview",
                    style: "background-color: {color}; color: {text_color}",
                    onclick: move |_| {
                        display_mode.set(match display_mode() {
                            DisplayMode::Hex => DisplayMode::Rgb,
                            DisplayMode::Rgb => DisplayMode::Hex,
                        });
                    },
                    "{preview_text}"
                }
            }
            p {
                class: "description",
                "Just a simple color picker which uses your system's (or browser) picker dialog. "
                "Click on the color to toggle hex formats: hex and RGB."
            }
        }
        footer {
            class: "footer",
            a {
                href: "https://walker84837.github.io/",
                class: "footer-link",
                "Main page"
            }
        }
    }
}

#[derive(PartialEq, Clone)]
enum DisplayMode {
    Hex,
    Rgb,
}

fn hex_to_rgb(hex: &str) -> String {
    let hex = hex.trim_start_matches('#');
    if hex.len() != 6 {
        return "Invalid".to_string();
    }

    match (
        u8::from_str_radix(&hex[0..2], 16),
        u8::from_str_radix(&hex[2..4], 16),
        u8::from_str_radix(&hex[4..6], 16),
    ) {
        (Ok(r), Ok(g), Ok(b)) => format!("rgb({}, {}, {})", r, g, b),
        _ => "Invalid".to_string(),
    }
}
