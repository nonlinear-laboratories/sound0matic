# Sound0matic Development Overview

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
│   ├── Source/              # C++/Faust/JUCE modules (PluginProcessor, SampleLoader, etc.)
│   ├── third_party/         # Dependencies (CImg, etc.)
│   ├── CMakeLists.txt       # Main CMake build file
│   ├── .clang-format        # Code style guide
│   └── .editorconfig        # Basic text editor rules
```

## What We've Got so far
* **Core pipeline**: SampleLoader → FFTProcessor → PhaseVocoder → SpectralFX → PhaseFX → ISTFT → PostFX
* **STN analysis (sinusoid/transient/residual)**: Morphology & STNModule are wired and producing masks
* **Stubs in place**: PhaseVocoder, SpectralFX, PhaseFX, PostFX, PluginEditor

## What Comes Next

* **PhaseVocoder Implementation**
   * Flesh out pitch-/time-shifting: implement basic phase vocoder internals, pitch-shift ratio control, and midi-based mapping.
   * Write tests or debug output to verify pitch correctness.
* **STN Mask Application**
   * Magnitude-only masks: multiply magnitude by masks, then reconstruct with original phase
   * Integrate masking into the `processBlock()` flow.
* **PluginEditor + Parameter Binding & GUI**
   * Define control parameters: pitch ratio, time-stretch, bypass switches, mask routing toggles.
   * Wire parameters (`AudioProcessorValueTreeState`) to GUI components.
   * Build a minimal UI for testing signal flow interactively.
* **SpectralFX Implementation**
   * Replace stub blur/freeze smear with real FFT-domain effects.
   * Test with known spectra to validate effect behavior.
* **PhaseFX Implementation**
   * Add simple stereo widening, phase offset, etc.
   * Create controls and verify audible effect.
* **End-to-End Testing in a Host (Carla, Reaper, etc.)**
   * Investigate why Carla is crashing—possibly checksum, settings, or plugin configuration.
   * Use logging or debug builds via `lldb`/`gdb` to capture the crash reason.
   * Confirm plugin loads correctly in other hosts (e.g. `Reaper`, `Helm`, `Ardour`, `Audacity`).

### Recommended Order

1. **Get audio loading and pitch/time-shift working** (e.g. map a MIDI note to pitch shift).
2. **Apply STN masks and observe changes in output (magnitude-based)**.
3. **Add a simple UI so you can tweak things in realtime**.
4. **Then move on to refining SpectralFX and PhaseFX components**.
5. **Finally stabilize the host loading and plugin metadata**.

---

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
- Extensions: C/C++, Clang-Format, CMake Tools, Clang-Format, Markdown All-in-One, Mermaid Preview Support, PlantUML,

## Dependencies

```cmake
# Add CImg library (header-only)
find_path(CIMG_INCLUDE_DIR CImg.h PATHS /usr/include /usr/local/include)
target_include_directories(sound0matic PRIVATE ${CIMG_INCLUDE_DIR})

# CImg optional dependencies for advanced features
find_package(JPEG QUIET)
find_package(PNG QUIET)
if(JPEG_FOUND AND PNG_FOUND)
    target_link_libraries(sound0matic PRIVATE ${JPEG_LIBRARIES} ${PNG_LIBRARIES})
    target_compile_definitions(sound0matic PRIVATE cimg_use_jpeg cimg_use_png)
endif()
```

### Notes

 The plugin is called `sound0matic` internally, it is only referred to as **"The Sound-0-Matic mk1"** in documentation and UI.

Internal plugin ID: `nll.sound0matic`  
Manufacturer ID: `V01D`  
Product ID: `S0M1`  
Version Number: (e.g. 0.0.1)  
Category/Type: (vst3, lv2, standalone)  

All `.dsp` files (if used) compiled via `faust -lang cpp`

### Clang-Format Preferences

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
   Regex: '^"(juce|faust)/'  
       Priority: 1  
   Regex: '.*'  
       Priority: 2*  
