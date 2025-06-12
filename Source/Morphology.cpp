// Morphology.cpp

#include "Morphology.h"
using namespace cimg_library;

void Morphology::convertToBuffer(const CImg<float> &img,
                                 juce::AudioBuffer<float> &buffer)
{
     const int width = img.width();
     const int height = img.height();

     buffer.setSize(width, height); // ->  Samples = width, Channels = height

     for (int y = 0; y < height; ++y)
          for (int x = 0; x < width; ++x)
               buffer.setSample(y, x, img(x, y));
}

void Morphology::spectralWhitening(CImg<float> &spectrogram)
{
     const int width = spectrogram.width();
     const int height = spectrogram.height();

     CImg<float> envelope(width, height, 1, 1, 0);

     // Estimate spectral envelope (simple PLACEHOLDER box filter for now)
     for (int y = 0; y < height; ++y)
     {
          for (int x = 1; x < width - 1; ++x)
          {
               envelope(x, y) =
                   (spectrogram(x - 1, y) + spectrogram(x, y) + spectrogram(x + 1, y)) /
                   3.0f;
          }
     }

     // Flatten spectrum
     for (int y = 0; y < height; ++y)
     {
          for (int x = 0; x < width; ++x)
          {
               spectrogram(x, y) /= std::max(envelope(x, y), 1e-6f);
          }
     }
}

void Morphology::extractComponents(const CImg<float> &spectrogram,
                                   CImg<float> &sinusoidal, CImg<float> &transient,
                                   CImg<float> &residual)
{
     CImg<float> whitened = spectrogram;
     spectralWhitening(whitened);

     sinusoidal = whitened.get_erode(1, 5).get_dilate(1, 5);
     transient = whitened.get_erode(5, 1).get_dilate(5, 1);
     residual = whitened - sinusoidal - transient;
}

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

void convertToBuffer(const cimg_library::CImg<float> &img,
                     juce::AudioBuffer<float> &buffer, float gain)
{
}