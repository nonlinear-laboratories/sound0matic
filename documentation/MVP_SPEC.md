# Sound0matic MVP Specification

## Primary Concept

Sound0matic is a monophonic, performance-oriented spectral sampler synthesizer. It loads a `.wav` sample into memory, decomposes it using STN analysis, pitch-shifts via phase vocoder, and applies modular FFT and phase-domain effects.

## MVP Feature Set

- Sample loading via JUCE file browser
- Pitch tracking via phase vocoder (not sample-rate pitch)
- STN decomposition (Sinusoidal/Transient/Noise masks)
- Spectral FX (blur, smear, freeze)
- Phase FX (jitter, offset, lock)
- ADSR envelope, drive, stereo output
- Basic GUI: sliders, toggles, waveform preview
- VST3 build with `.vst3` install and DAW automation