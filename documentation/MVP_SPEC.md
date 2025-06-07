# Sound0matic MVP Specification

## Primary Concept

Sound0matic is a monophonic, performance-oriented spectral sampler synthesizer. It loads a `.wav` sample into memory, decomposes it using STN analysis, pitch-shifts via phase vocoder, and applies modular FFT and phase-domain effects.

## MVP Feature Set

  * Load and buffer `.wav` sample in memory using JUCE
  * Real-time STFT and inverse STFT for audio processing
  * STN (Sinusoidal, Transient, Noise) decomposition using spectrogram shape analysis
  * Time-domain pitch-shift via Phase Vocoder
  * Spectral FX: blur, smear, freeze (bin-wise magnitude filters)
  * Phase FX: jitter, lock, drift (bin-wise phase manipulation)
  * ADSR envelope + output gain
  * Monophonic audio path, stereo output
  * `LV2`/`VST3` plugin format
  * DAW transport and parameter automation support