#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider& slider) override
    {
        auto radius = juce::jmin(width / 2.0f, height / 2.0f);
        auto centreX = x + width  * 0.5f;
        auto centreY = y + height * 0.5f;
        auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        // Background circle
        g.setColour(juce::Colours::darkgrey);
        g.fillEllipse(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f);

        // Pointer
        g.setColour(juce::Colours::white);
        juce::Path p;
        p.addRectangle(-2.0f, -radius, 4.0f, radius * 0.7f);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        g.fillPath(p);
    }
};


class Karplus_Bonus_AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    Karplus_Bonus_AudioProcessorEditor(Karplus_Bonus_AudioProcessor&);
    ~Karplus_Bonus_AudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    
    

private:
    
    
    Karplus_Bonus_AudioProcessor& processor;
    
    // Background Image
    juce::Image backgroundImage;

    // Sliders
    juce::Slider gainSlider, decaySlider, widthSlider;
    juce::Slider filterCutoffSlider, lowFilterCutoffSlider;
    juce::Slider tremoloRateSlider, tremoloDepthSlider;
    juce::Slider reverbSizeSlider, reverbMixSlider;
    
    //Labels
    juce::Label gainLabel, decayLabel, widthLabel;
    juce::Label filterCutoffLabel, lowFilterCutoffLabel;
    juce::Label tremoloRateLabel, tremoloDepthLabel;
    juce::Label reverbSizeLabel, reverbMixLabel;
    juce::Label sourceLabel;

    // Choice
    juce::ComboBox sourceChoice;

    // Attachments
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<SliderAttachment> gainAttach, decayAttach, widthAttach;
    std::unique_ptr<SliderAttachment> filterCutoffAttach, lowFilterCutoffAttach;
    std::unique_ptr<SliderAttachment> tremoloRateAttach, tremoloDepthAttach;
    std::unique_ptr<SliderAttachment> reverbSizeAttach, reverbMixAttach;
    std::unique_ptr<ComboBoxAttachment> sourceAttach;
    

    CustomLookAndFeel customLNF;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Karplus_Bonus_AudioProcessorEditor)
};
