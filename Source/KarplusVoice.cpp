#include "KarplusVoice.h"

KarplusVoice::KarplusVoice(double sampleRate)
{
    // Class Definition
    this->sampleRate = sampleRate;

    delayBufferLength = static_cast<int>(1.0 * sampleRate);
    delayBuffer.setSize(1, delayBufferLength);
    delayBuffer.clear();
    delayReadPosition = 0;
    delayWritePosition = 0;

    NoiseGain = 0.0f;
    inputPhase = 0.0f;
    active = false;

    feedbackCoefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 2000.0f);
    feedbackFilter.coefficients = feedbackCoefficients;
    feedbackFilter.reset();
}

void KarplusVoice::startNote(int midiNote, float velocity, float decay, float width, int source, float cutoff)
{
    // Start note routine
    frequencyValue = juce::MidiMessage::getMidiNoteInHertz(midiNote);
    delayTime = 1.0 / frequencyValue;
    NoiseGain = 1.0f;
    currentGain = velocity;
    delayWritePosition = 0;
    active = true;

    this->decay = decay;
    this->width = width;
    this->source = source;

    // Update feedback filter per note dynamically
    feedbackCoefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, cutoff);
    *feedbackFilter.coefficients = *feedbackCoefficients;
    feedbackFilter.reset();
}

void KarplusVoice::stopNote()
{
    // Start note routine
    active = false;

}

float KarplusVoice::renderNextSample(float sampleRate)
{
    if (!active)
        return 0.0f;

    delayReadPosition = (delayWritePosition - static_cast<int>(delayTime * sampleRate) + delayBufferLength) % delayBufferLength;

    float in = 0.0f;
    if (NoiseGain > 0.0f)
    {
        switch (source)
        {
            // Four different exciters
            case 0: in = sinf(juce::MathConstants<float>::twoPi * inputPhase); break;                            // Sine
            case 1: in = fmod(inputPhase * 2.0f, 2.0f) - 1.0f; break;                                            // Sawtooth
            case 2: in = (sinf(juce::MathConstants<float>::twoPi * inputPhase) >= 0.0f) ? 1.0f : -1.0f; break;   // Square
            case 3: in = 2.0f * (juce::Random::getSystemRandom().nextFloat() - 0.5f); break;                     // Noise
        }
        NoiseGain -= 1.0f / (width * sampleRate);
        if (NoiseGain < 0.0f) NoiseGain = 0.0f;
    }
    
    // Adjust phase
    inputPhase += frequencyValue / sampleRate;
    if (inputPhase >= 1.0f) inputPhase -= 1.0f;

    float delayedSample = delayBuffer.getSample(0, delayReadPosition);

    // Apply filtered feedback
    float filteredFeedback = feedbackFilter.processSample(delayedSample);

    delayBuffer.setSample(0, delayWritePosition, in + filteredFeedback * decay);

    delayReadPosition = (delayReadPosition + 1) % delayBufferLength;
    delayWritePosition = (delayWritePosition + 1) % delayBufferLength;

    return filteredFeedback * currentGain;
}

bool KarplusVoice::isActive() const
{
    return active;
}
