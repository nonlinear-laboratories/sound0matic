// Morphology.h
#pragma once

#include <JuceHeader.h> // convention to list STL/JUCE/system headers first

#define cimg_display 0    // disable all X11 use
#define cimg_undef_macros // undo delay/font/etc.
#define cimg_use_vt100    // cleaner console output if debug printing
#include "../third_party/CImg.h"

class Morphology
{
   public:
     static void extractComponents(const cimg_library::CImg<float> &spectrogram,
                                   cimg_library::CImg<float> &sinusoidal,
                                   cimg_library::CImg<float> &transient,
                                   cimg_library::CImg<float> &residual);
     static void extractComponents(const juce::AudioBuffer<float> &magnitude,
                                   juce::AudioBuffer<float> &sinusoid,
                                   juce::AudioBuffer<float> &transient,
                                   juce::AudioBuffer<float> &residual);

   private:
     static void spectralWhitening(cimg_library::CImg<float> &spectrogram);
     static void convertToBuffer(const cimg_library::CImg<float> &img,
                                 juce::AudioBuffer<float> &buffer);
};