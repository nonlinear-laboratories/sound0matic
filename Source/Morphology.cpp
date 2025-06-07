// Morphology.cpp
#include "Morphology.h"

using namespace cimg_library;

void Morphology::extractComponents(const juce::AudioBuffer<float> &magnitude,
                                   juce::AudioBuffer<float> &sinusoid,
                                   juce::AudioBuffer<float> &transient,
                                   juce::AudioBuffer<float> &residual)
{
     const int width = magnitude.getNumSamples();
     const int height = magnitude.getNumChannels();

     cimg_library::CImg<float> spectrogram(width, height, 1, 1);
     for (int y = 0; y < height; ++y)
          for (int x = 0; x < width; ++x)
               spectrogram(x, y) = magnitude.getSample(y, x);

     cimg_library::CImg<float> sinImg, transImg, residualImg;
     extractComponents(spectrogram, sinImg, transImg, residualImg);

     convertToBuffer(sinImg, sinusoid);
     convertToBuffer(transImg, transient);
     convertToBuffer(residualImg, residual);
}

void Morphology::extractComponents(const CImg<float> &spectrogram,
                                   CImg<float> &sinusoidal, CImg<float> &transient,
                                   CImg<float> &residual)
{
     sinusoidal = spectrogram.get_erode(1, 5).get_dilate(1, 5);
     transient = spectrogram.get_erode(5, 1).get_dilate(5, 1);
     residual = spectrogram - sinusoidal - transient;
}

void Morphology::convertToBuffer(const CImg<float> &img,
                                 juce::AudioBuffer<float> &buffer)
{
     const int width = img.width();
     const int height = img.height();

     buffer.setSize(height, width);

     for (int y = 0; y < height; ++y)
          for (int x = 0; x < width; ++x)
               buffer.setSample(y, x, img(x, y));
}
