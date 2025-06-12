/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
Karplus_Bonus_AudioProcessor::Karplus_Bonus_AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    // No more addParameter(...) here
}

Karplus_Bonus_AudioProcessor::~Karplus_Bonus_AudioProcessor()
{
}

//==============================================================================
const juce::String Karplus_Bonus_AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Karplus_Bonus_AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Karplus_Bonus_AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Karplus_Bonus_AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Karplus_Bonus_AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Karplus_Bonus_AudioProcessor::getNumPrograms()
{
    return 1;
}

int Karplus_Bonus_AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Karplus_Bonus_AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Karplus_Bonus_AudioProcessor::getProgramName (int index)
{
    return {};
}

void Karplus_Bonus_AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Karplus_Bonus_AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    const int maxVoices = 16; // Adjust polyphony here
    voices.clear();

    for (int i = 0; i < maxVoices; ++i)
    voices.push_back(std::make_unique<KarplusVoice>(sampleRate));

    // Initialize filter (you may control cutoff frequency dynamically)
    float lowFilterCutoff = apvts.getRawParameterValue("lowFilterCutoff")->load();
    globalFilterCoefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, lowFilterCutoff);
    globalFilter.coefficients = globalFilterCoefficients;
    globalFilter.reset();
    
    reverb.setSampleRate(sampleRate);
    
    reverbBuffer.setSize(getTotalNumOutputChannels(), samplesPerBlock);
    reverbBuffer.clear();
}


void Karplus_Bonus_AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Karplus_Bonus_AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Karplus_Bonus_AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);
    
    buffer.clear();

    const float sampleRate = (float)getSampleRate();

    // === Retrieve parameters via apvts ===
    float gain = apvts.getRawParameterValue("gain")->load();
    int source = static_cast<int>(apvts.getRawParameterValue("source")->load());
    float decay = apvts.getRawParameterValue("decay")->load();
    float width = apvts.getRawParameterValue("width")->load();
    float filterCutoff = apvts.getRawParameterValue("filterCutoff")->load();
    float lowFilterCutoff = apvts.getRawParameterValue("lowFilterCutoff")->load();
    float tremoloRate = apvts.getRawParameterValue("tremoloRate")->load();
    float tremoloDepth = apvts.getRawParameterValue("tremoloDepth")->load();
    float reverbSize = apvts.getRawParameterValue("reverbSize")->load();
    float reverbMix = apvts.getRawParameterValue("reverbMix")->load();

    // === Update filter ===
    *globalFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, lowFilterCutoff);

    // === Handle MIDI ===
    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();

        if (msg.isNoteOn())
        {
            for (auto& voice : voices)
            {
                if (!voice->isActive())
                {
                    voice->startNote(msg.getNoteNumber(),
                                     msg.getVelocity() / 127.0f,
                                     decay,
                                     width,
                                     source,
                                     filterCutoff);
                    break;
                }
            }
        }

        if (msg.isNoteOff())
        {
            for (auto& voice : voices)
            {
                voice->stopNote();
            }
        }
    }

    auto* channelDataL = buffer.getWritePointer(0);
    auto* channelDataR = buffer.getWritePointer(1);

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float mixedSample = 0.0f;

        for (auto& voice : voices)
            if (voice->isActive())
                mixedSample += voice->renderNextSample(sampleRate);

        // Apply filter
        mixedSample = globalFilter.processSample(mixedSample);

        // Apply tremolo
        float lfo = 1.0f - tremoloDepth * 0.5f * (1.0f + std::sin(2.0f * juce::MathConstants<float>::pi * tremoloPhase));
        tremoloPhase += tremoloRate / sampleRate;
        if (tremoloPhase >= 1.0f)
            tremoloPhase -= 1.0f;

        mixedSample *= lfo;

        channelDataL[sample] = mixedSample;
        channelDataR[sample] = mixedSample;
    }

    // === Reverb setup ===
    reverbParams.roomSize = reverbSize;
    reverb.setParameters(reverbParams);

    reverbBuffer.makeCopyOf(buffer);
    reverb.processStereo(reverbBuffer.getWritePointer(0), reverbBuffer.getWritePointer(1), buffer.getNumSamples());

    float dryMix = 1.0f - reverbMix;

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float mixedL = dryMix * channelDataL[sample] + reverbMix * reverbBuffer.getSample(0, sample);
        float mixedR = dryMix * channelDataR[sample] + reverbMix * reverbBuffer.getSample(1, sample);

        // Final gain applied here
        channelDataL[sample] = mixedL * gain;
        channelDataR[sample] = mixedR * gain;
    }
}

//==============================================================================
bool Karplus_Bonus_AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Karplus_Bonus_AudioProcessor::createEditor()
{
    return new Karplus_Bonus_AudioProcessorEditor(*this);
}

//==============================================================================
void Karplus_Bonus_AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Karplus_Bonus_AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Karplus_Bonus_AudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout Karplus_Bonus_AudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"gain", 1}, "Gain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{"source", 1}, "Source",
        juce::StringArray{ "Sinusoid", "Sawtooth", "Square", "Noise" }, 0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"decay", 1}, "Decay",
        juce::NormalisableRange<float>(0.80f, 1.f, 0.01f), 0.97f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"width", 1}, "Width",
        juce::NormalisableRange<float>(0.001f, 0.020f, 0.001f), 0.005f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"filterCutoff", 1}, "Acoustic Attenuator",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), 2000.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"lowFilterCutoff", 1}, "Filter Cutoff",
        juce::NormalisableRange<float>(20.0f, 500.0f, 1.0f, 0.3f), 20.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"tremoloRate", 1}, "Tremolo Rate",
        juce::NormalisableRange<float>(0.1f, 20.0f, 0.01f), 2.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"tremoloDepth", 1}, "Tremolo Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"reverbSize", 1}, "Reverb Size",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"reverbMix", 1}, "Reverb Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    return { params.begin(), params.end() };
}
