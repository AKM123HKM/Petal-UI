# <Project Name>

A small retained-mode C++ GUI library that is **independent of any graphics API**.

The library never draws anything itself. The host program feeds it mouse input and a text-measuring function, and every frame the library hands back an ordered list of simple drawing elements (rectangles and text). A thin piece of host code turns those into real draw calls. The included host uses SFML 3.

> **Status: early development.** Only buttons exist so far. See [Status](#status).

## Why

An earlier version of this toolkit called SFML directly, so it was tied to SFML's window, types, and drawing functions. This rewrite moves that dependency out of the library. Changing graphics APIs later (or letting someone use the library with raylib, OpenGL, or their own engine) means rewriting a small adapter, not the toolkit.

This is the same idea used by Dear ImGui and Clay: data goes in, a draw list comes out, and the host does the rendering.

## How it works

```
assets/data.json            UI described as data
        |
        |  parseButtonData()       the only code that reads JSON
        v
ButtonParam                 plain struct, no JSON types
        |
        v
ButtonWidget                owns its own state (hover, active color)
        |
        |  each frame: UI::update() with the mouse state
        v
frameRenderBuffer           ordered list of RectElement / TextElement
        |
        v
host (main.cpp)             loops over the list and draws with SFML
```

The library and the host talk to each other in three ways:

1. **Input in.** The host updates `ui.mouse` each frame (position, and the state of the mouse buttons). The library doesn't know where the input comes from.
2. **Draw elements out.** `UI::update()` fills `ui.frameRenderBuffer`, a `std::vector` of a variant over `RectElement` and `TextElement`. The host draws them in order, top to bottom, with no extra logic.
3. **Text measurement as a question to the host.** Layout needs the size of a string before anything is drawn, and only the host's font system can answer that. The host passes a measuring function to the `UI` constructor. It takes `(text, font_id, size)` and returns a `BoundingBox`. Fonts are referred to by an integer `font_id`, so the library never sees an SFML font.

### Host usage

```cpp
// measurer: (text, font_id, size) -> BoundingBox, implemented with the host's fonts
UI ui(json_data, measurer);

while (window.isOpen()) {
    ui.mouse.update({mouse_x, mouse_y});
    ui.mouse.updateLeftMouseButton(left_button_is_down);

    ui.update();                          // widgets react to input and emit draw elements

    for (auto& element : ui.frameRenderBuffer)
        draw(element);                    // std::visit: RectElement -> rectangle, TextElement -> text
}
```

## Describing UI in JSON

UI is described in a JSON file, so values can be changed and re-run without recompiling. The format is not final. Each entry in the top-level `"ui"` array has a `"type"`. Currently only `"Button"` is supported, and every field below is required:

```json
{
  "ui": [
    {
      "type": "Button",
      "text": "OK",
      "pos": [100, 50],
      "font_id": 0,
      "text_size": 24,
      "padding": 16,
      "bg_color": [200, 200, 200],
      "hover_color": [160, 160, 220],
      "text_color": [0, 0, 0]
    }
  ]
}
```

The parser turns each entry into a plain parameter struct (`ButtonParam`) before any widget is built. Supporting another file format later means writing another parser that fills the same structs.

## Project layout

```
.
├── assets/
│   ├── data.json                 UI description
│   └── PoetsenOne-Regular.ttf    font used by the demo
├── include/
│   ├── UI.hpp                    library header
│   └── json.hpp                  nlohmann/json
├── src/
│   ├── UI.cpp                    library implementation (widgets, mouse state, parsing)
│   └── main.cpp                  demo host: SFML window, input, text measuring, drawing
├── bin/                          built executable and SFML DLLs
├── build.ps1                     build script
└── README.md
```

## Building and running

- **Platform:** Windows (the build script is PowerShell).
- **Requirements:** a C++17 compiler (the code uses `std::variant`), SFML 3, and [nlohmann/json](https://github.com/nlohmann/json) (single header, included in `include/`).
- **Build:** run `build.ps1` from the project root. Check the script for the compiler and SFML paths it expects.
- **Run:** start `bin/app.exe` from the project root, because the demo loads `assets/` by relative path.

The demo opens an 800x800 window, shows the buttons from `data.json`, and prints the frame time in the top-left corner.

## Status

**Working**
- Buttons created from JSON through a parameter struct
- Buttons react to the mouse: hovering shows `hover_color`, and a left click changes the button's appearance
- Text measurement through a host-supplied function
- SFML 3 demo host

**Not yet implemented**
- Callbacks from widgets to host code (buttons change appearance but don't trigger any host function)
- Groups, nesting, and relative positioning
- Other widgets (scrollbar, checkbox, slider, ...)
- Clipping
- Images / textures as a draw element
- Reloading the JSON while the program is running
- Distinguishing a click from a hold

## Known limitations

- `TextElement` doesn't carry a `font_id` yet, so the demo host always draws with its first font even though measuring uses the requested one.
- Only the left mouse button is passed to the library by the demo host.
- Widgets are not identified by ID, so there is no way yet to look one up or keep its state across a reload.
- All JSON fields are required, and a missing one throws an exception when parsing.

## Roadmap

1. Widget callbacks, so buttons can trigger host code
2. Groups with relative positioning, and a clip element for scrolling regions
3. More widgets (scrollbar, checkbox, slider)
4. Image elements
5. Hot reload of the JSON file

## Design notes

- The library depends only on the C++ standard library and its own types (`Vec2`, `Color`, `BoundingBox`, ...). SFML-specific code belongs in the host.
- Widgets own their state. The draw elements are the output of a widget, not what it is made of.
- Time (such as frame delta) should be passed in by the host rather than read from a clock inside the library, which keeps behavior predictable and testable.