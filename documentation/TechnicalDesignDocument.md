# **The Sound-0-Matic mk1 Technical Design Document**

## **Table of Contents** ...tbd

---
## Introduction

The **Sound-0-Matic mk1** is a monophonic, performance-oriented spectral sampler synthesizer developed by nonlinear laboratories. In the Sound-0-Matic, incoming audio is analyzed via a Short-Time Fourier Transform (STFT) and split into sinusoidal (tonal), transient (attack), and noise (residual) components. Then, phase-vocoder methods and spectral modeling provide the theoretical foundation: for instance, FFT-based vocoders enabled speech coding and musical time/pitch modification as early as the 1970s.  The Sound-0-Matic builds on these insights but targets live performance: low latency processing is critical.  The core analysis and resynthesis engine is implemented in highly optimized C++ (using FFT libraries like FFTW or KISS) within the JUCE framework.  The plugin optionally embeds Faust-generated DSP code for some filters or effects modules.  The user interface is styled with a retro analog aesthetic (knobs, switches, VU meters) while providing modern conveniences like MIDI-learnable parameters and real-time waveform/spectrum displays.

In summary, the Sound-0-Matic offers a research-grade (but performer ready) spectral sampler synthesizer that can freeze sounds, blur and smear spectra, apply creative phase-domain transformations, and otherwise reshape sound in musically useful ways.  Its intended users are DSP-savvy audio programmers, plugin developers, and experimental sound designers who appreciate detailed control over spectral content.

---
## Open Source Philosophy & Licensing Strategy

The Sound-0-Matic is committed to remaining 100% open source throughout its lifecycle. All code, documentation, and associated materials will be released under GPLv3 or compatible licenses.

- **Core Plugin**: GPLv3 (ensures derivative works remain open)
- **Dependencies**: Only GPL-compatible libraries (JUCE Personal/GPL, FFTW3, KissFFT)
- **Documentation**: Creative Commons BY-SA 4.0
- **Build System**: MIT-licensed CMake scripts where possible
- **NO PROPRIETARY DEPENDENCIES**: Reject any closed-source libraries, even if performance benefits exist

---
## Project Scope

This document details the technical design of the Sound-O-Matic plugin, broken into logical components.  The project scope includes:

* **Spectral Analysis Engine:** Implement a low-latency STFT pipeline (windowing + FFT) with adjustable frame size, hop size, and overlap-add processing.  Use COLA-compliant windows (e.g. Hamming) for perfect reconstruction.  Provide options for zero-padding to allow fine-grained spectral manipulation.
* **STN Decomposition/Morphological Spectral Analysis**: Implement spectrogram-based morphological filtering using mathematical morphology operations (opening, closing, top-hat transforms). This approach analyzes the 2D time-frequency representation to separate:
  - **Harmonic Structures**: Detected via morphological opening with vertical structural elements
  - **Transient Events**: Extracted using top-hat transforms to highlight brief spectral peaks
  - **Noise Components**: Modeled through morphological closing operations
  
  This method provides more robust separation of spectral components compared to traditional sinusoidal modeling, especially for complex polyphonic material.
* **Real-Time Spectral Effects:** Provide controls for effects such as *Freeze* (hold or loop the current spectrum), *Blur* (smooth or diffuse spectral peaks), *Smear* (time-domain convolution carried out in the spectral domain), and *Phase Swap* (manipulate phase of spectral bins for chorusing or stereo effects).  Implement pitch shifting and time-stretching via a phase-vocoder (phase unwrapping and increment scaling) for creative pitch/font adjustments.  Ensure each effect is computed on-the-fly with minimal latency.
* **Stereo and Spatial Processing:** Support stereo input.  Include special stereo phase-processing features (e.g. inter-channel phase shifting, mid/side spectral effects) to create stereo textures.  Ensure phase relationships can be independently adjusted per channel for spatial modulation.
* **User Interface & MIDI:** Design a retro-analog GUI (knobs, sliders, toggle switches, vintage fonts) grouped by function (analysis, effects, envelopes, mixing).  Include visual feedback: waveform and/or spectrum display, dB meter, peak indicator.  All major parameters (e.g. effect mix levels, window size, pitch ratio) will support MIDI CC learn and host automation.
* **Performance Goals:** The audio callback must run in real time with typical buffer sizes (64–256 samples) and incur very low latency (on the order of 5–10ms or less).  CPU utilization should allow multiple instances on a modern CPU.  This requires highly optimized FFT code, efficient memory handling (no dynamic allocation or locks in the audio thread), and parameter smoothing (to avoid zipper noise) using cheap one-pole filters or JUCE’s dsp::SmoothedValue.
* **Modular Architecture:** Structure the code so DSP modules (FFT, peak tracker, filters, etc.) are encapsulated.  Use Faust to prototype individual DSP blocks; Faust’s C++ output (via `faust2api`) provides callable DSP classes.  The JUCE plugin audio processor will integrate these blocks into a coherent signal flow.
* **Quality Assurance:** Develop a comprehensive test plan.  Unit test DSP components (FFT invertibility, filter responses, partial trackers) with known signals (sinusoids, impulses, noise).  Profile performance under different CPU loads.  Plan for listening tests to validate sonic quality.  Use version control and continuous integration to ensure reliability.
* **Deployment:** Target Linux (LV2/VST3) with buildable CMake/Ninja project.  Use modern C++ (C++17 or later) and JUCE 6+.  Keep dependencies minimal (JUCE, FFT library, optionally Faust runtime).

Each of these goals will be expanded in the following sections: architecture overview (JUCE+Faust interactions), detailed DSP theory and implementation, requirements/priorities, implementation specifics, UI design, testing strategy, and future enhancements.

---
## System Architecture Overview

### **JUCE, Faust, and C++ Integration**

The sound0matic is built as a standard JUCE audio plugin (e.g. VST3) in C++.  The JUCE **AudioProcessor** class implements the real-time audio callback.  Faust (if used) provides DSP blocks: Faust “.dsp” source files are compiled into C++ using `faust2api`. For example, running:

```
faust myEffects.dsp -o myEffects.cpp
```

produces a C++ class implementing an audio callback.  More conveniently, `faust2api -juce myEffects.dsp` generates a JUCE-compatible DSP engine (e.g. `DspFaust.cpp`/`.h`) that can be dropped into the JUCE project.  In either case, the Faust code defines DSP computations (filters, envelopes, simple effects), while the JUCE processor instantiates these objects and calls their process functions on each audio block.  The AudioProcessorEditor handles the GUI; parameter changes (from knobs or automation) are sent to the DSP via JUCE’s parameter system or direct setter calls (Faust exposes parameter paths).

Internally, the JUCE processor will hold member instances for each DSP component: FFT buffers, window function, a “sine bank” object for partial synthesis, and any Faust DSP classes.  The audio callback (`processBlock`) reads from an input circular buffer, processes through the STFT pipeline, and writes to the output buffer.  Custom C++ classes handle STN separation, transient detection, and effect algorithms.  Faust-generated classes are used for embarrassingly parallel or sample-based tasks (e.g. a one-pole filter, LFO, simple waveshaper) which Faust can express concisely.

### **Threading Model, Memory, and Real-Time Safety**

JUCE distinguishes the real-time audio thread (processing audio) from the GUI thread.  The AudioProcessor’s `processBlock()` runs in the audio thread and must be non-blocking.  All heavy DSP (windowing, FFT, ISTFT, STN analysis, etc.) occurs here.  No heap allocations, locks, or I/O are allowed on this thread.  Memory (arrays for window coefficients, FFT workspace, circular ring buffers) is pre-allocated in initialization, avoiding any `new` or `malloc` during processing.  For example, use `std::vector<float>` or raw arrays sized to fixed maximum block size (e.g. 4096 samples) for audio buffers.  The FFT plan (if using FFTW, etc.) can be created ahead of time (FFT “wisdom” loaded) to avoid on-the-fly planning overhead.

Parameter updates (knob moves, automation) are usually delivered via JUCE’s `AudioProcessorValueTreeState` or similar.  These updates may arrive asynchronously in the GUI thread or via the host’s automation callback.  To propagate them safely, we either use atomic variables/lock-free FIFOs or simply read the current parameter values at the start of each audio block (JUCE handles synchronization).  Parameter smoothing (e.g. to avoid clicks when changing parameters) is done with simple one-pole filters or JUCE’s `SmoothedValue`, which are real-time safe and do not block.

Special care is taken in the overlap-add mechanism: an output double-buffer of length *N + R* samples holds the IFFT results.  Each processed frame is written into this buffer at the frame’s time offset.  After writing, the first *R* samples are added into the audio output.  Index arithmetic is carefully handled to avoid costly modulus operations (using circular buffer pointers).  No semaphores or waiting is ever used in the audio path.

Faust DSP objects are thread-safe by design (they only do float operations).  We call a Faust `instance->compute(numSamples, inputs, outputs)` each block.  If multiple Faust instances are used, they can be updated from the audio thread as well.  The Faust parameters are updated via `setParamValue(path, value)`, which typically acquires an internal lock; however, updating parameters outside the audio thread or batching updates per block avoids audio glitches.

### Signal Flow (ASCII Diagram)

// work in progress...

