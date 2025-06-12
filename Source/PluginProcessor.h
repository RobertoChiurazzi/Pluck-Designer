/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>
#include "KarplusVoice.h"

//==============================================================================
/**
*/
class Karplus_Bonus_AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Karplus_Bonus_AudioProcessor();
    ~Karplus_Bonus_AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    // APTVS Layout
    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    // Midi Keyboard
    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboardComponent { keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard };
    

private:
    
    //Source parameters
    float NoiseGain = 0.0f;
    juce::AudioParameterChoice * sourceParam;
    juce::AudioParameterFloat* decayParam;
    juce::AudioParameterFloat* widthParam;
    juce::AudioSampleBuffer delayBuffer;
    std::vector<std::unique_ptr<KarplusVoice>> voices; //Voices
    juce::dsp::IIR::Filter<float> feedbackFilter;
    juce::dsp::IIR::Coefficients<float>::Ptr feedbackCoefficients;
    
    //Filters Parameters
    juce::dsp::IIR::Filter<float> globalFilter;
    juce::dsp::IIR::Coefficients<float>::Ptr globalFilterCoefficients;
    
    //Tremolo variables and parameters
    float tremoloPhase = 0.0f;
    float tremoloRate = 5.0f;
    float tremoloDepth = 0.5f;
    juce::AudioParameterFloat* tremoloRateParam = nullptr;
    juce::AudioParameterFloat* tremoloDepthParam = nullptr;

    //Reverb Parameters
    juce::Reverb reverb;
    juce::Reverb::Parameters reverbParams;
    juce::AudioBuffer<float> reverbBuffer;
    juce::AudioParameterFloat* reverbMixParam = nullptr;
    juce::AudioParameterFloat* reverbSizeParam = nullptr;
    
    //Output Parameters
    juce::AudioParameterFloat* gainParam; //Gain

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Karplus_Bonus_AudioProcessor)
};


