#pragma once
#include <JuceHeader.h>

class KarplusVoice
{
public:
    KarplusVoice(double sampleRate);
    void startNote(int midiNote, float velocity, float decay, float width, int source, float cutoff);
    void stopNote();
    float renderNextSample(float sampleRate);
    bool isActive() const;

private:
    juce::AudioBuffer<float> delayBuffer;
    int delayBufferLength, delayReadPosition, delayWritePosition;
    float NoiseGain, inputPhase, frequencyValue, delayTime, currentGain;
    float decay, width;
    int source;
    bool active;
    double sampleRate;
    

    juce::dsp::IIR::Filter<float> feedbackFilter;
    juce::dsp::IIR::Coefficients<float>::Ptr feedbackCoefficients;
};
