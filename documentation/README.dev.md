# Sound0matic Developer Overview

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
│   ├── third_party/         # Dependencies (CImg, etc.)
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

- OS: Linux x64 6.13.8-4-liquorix-amd64 
- VSCodium 1.100.03093
- Extensions: C/C++, Clang-Format, CMake Tools, Clang-Format, Markdown All-in-One, Mermaid Preview Support, PlantUML, YAML


## Notes

* Internal plugin ID: `nll.sound0matic`
* Plugin codes: `V01D` (manufacturer), `S0M1` (product)
* All `.dsp` files (if used) compiled via `faust -lang cpp`
* Pitch detection via YIN 

// clang formatting preferences
Language: Cpp
BasedOnStyle: LLVM
IndentWidth: 5
TabWidth: 5
UseTab: Never
BreakBeforeBraces: Allman
AllowShortIfStatementsOnASingleLine: false
AllowShortBlocksOnASingleLine: false
AlwaysBreakTemplateDeclarations: Yes
ReflowComments:  true
PointerAlignment: Right
IdentifierNaming:
     FunctionCase: lower_case
     VariableCase: lower_case
     ClassCase: CamelCase
     StructCase: CamelCase
     EnumCase: CamelCase
     EnumConstantCase: UPPER_CASE
     MemberCase: lower_case
     PrivateMemberPrefix: "_"
     ConstantCase: UPPER_CASE
     GlobalConstantCase: UPPER_CASE
IncludeCategories:
     - Regex: '^"(juce|faust)/'
       Priority: 1
     - Regex: '.*'
       Priority: 2

