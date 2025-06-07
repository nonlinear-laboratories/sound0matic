# Sound0matic Developer Overview

- OS: Linux x64 6.13.8-4-liquorix-amd64 

## Project Layout

```
~/dev/
├── JUCE/                    # JUCE SDK (external, untouched)
├── playground/              # Testing location for project
├── sound0matic/             # Main project
│   ├── .vscode/             # Editor settings
│   ├── build/               # >CMake/ninja builds
│   ├── documentation/       # Blueprints, notes, specs, etc.
│   ├── Resource/            # Samples, splash images, icons
│   ├── Source/              # JUCE modules (PluginProcessor, SampleLoader, etc.)
│   ├── CMakeLists.txt       # Main CMake build file
│   ├── .clang-format        # Code style guide
│   └── .editorconfig        # Basic text editor rules
```

## Build Instructions (Linux)

```
sudo apt install build-essential cmake ninja-build libgtk-3-dev libasound2-dev
cd ~/dev/sound0matic
cmake -Bbuild -S. -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Resulting `.vst3` file is installed to `~/dev/playground/` for testing in Carla.

## Plugin Targets

- Format: VST3 (MVP)
- Future: Standalone (JACK/ALSA), LV2

## Editor / IDE

- VSCodium 1.100.03093
- Extensions: CMake Tools, C/C++, Clang-Format, YAML


## Notes

* Internal plugin ID: `nll.sound0matic`
* Plugin codes: `V01D` (manufacturer), `S0M1` (product)
* All `.dsp` files (if used) compiled via `faust -lang cpp`
* Pitch detection via YIN 
* Naming style: lower\_snake\_case for files, camelCase in code

