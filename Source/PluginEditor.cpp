#include "PluginProcessor.h"
#include "PluginEditor.h"

Karplus_Bonus_AudioProcessorEditor::Karplus_Bonus_AudioProcessorEditor(Karplus_Bonus_AudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    auto& apvts = processor.apvts;

    // Set Size
    setSize(700, 450);
    
    // Keyboard
    addAndMakeVisible(processor.keyboardComponent);
    
    // Background Image
    backgroundImage = juce::ImageCache::getFromMemory(
        BinaryData::Background_synth_png,
        BinaryData::Background_synth_pngSize
    );
    
    // Knobs
    gainSlider.setSliderStyle(juce::Slider::Rotary);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(gainSlider);

    decaySlider.setSliderStyle(juce::Slider::Rotary);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(decaySlider);

    widthSlider.setSliderStyle(juce::Slider::Rotary);
    widthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(widthSlider);

    filterCutoffSlider.setSliderStyle(juce::Slider::Rotary);
    filterCutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(filterCutoffSlider);

    lowFilterCutoffSlider.setSliderStyle(juce::Slider::Rotary);
    lowFilterCutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(lowFilterCutoffSlider);

    tremoloRateSlider.setSliderStyle(juce::Slider::Rotary);
    tremoloRateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(tremoloRateSlider);

    tremoloDepthSlider.setSliderStyle(juce::Slider::Rotary);
    tremoloDepthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(tremoloDepthSlider);

    reverbSizeSlider.setSliderStyle(juce::Slider::Rotary);
    reverbSizeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(reverbSizeSlider);

    reverbMixSlider.setSliderStyle(juce::Slider::Rotary);
    reverbMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(reverbMixSlider);
    
    sourceChoice.addItemList({ "Sinusoid", "Sawtooth", "Square", "Noise" }, 1);
    addAndMakeVisible(sourceChoice);
    
    
    //Labels
    decayLabel.setText("Decay", juce::dontSendNotification);
    decayLabel.setJustificationType(juce::Justification::centred);
    decayLabel.attachToComponent(&decaySlider, false);
    addAndMakeVisible(decayLabel);

    widthLabel.setText("Width", juce::dontSendNotification);
    widthLabel.setJustificationType(juce::Justification::centred);
    widthLabel.attachToComponent(&widthSlider, false);
    addAndMakeVisible(widthLabel);

    filterCutoffLabel.setText("Harmonics", juce::dontSendNotification);
    filterCutoffLabel.setJustificationType(juce::Justification::centred);
    filterCutoffLabel.attachToComponent(&filterCutoffSlider, false);
    addAndMakeVisible(filterCutoffLabel);

    lowFilterCutoffLabel.setText("Low Cut", juce::dontSendNotification);
    lowFilterCutoffLabel.setJustificationType(juce::Justification::centred);
    lowFilterCutoffLabel.attachToComponent(&lowFilterCutoffSlider, false);
    addAndMakeVisible(lowFilterCutoffLabel);

    tremoloRateLabel.setText("Rate", juce::dontSendNotification);
    tremoloRateLabel.setJustificationType(juce::Justification::centred);
    tremoloRateLabel.attachToComponent(&tremoloRateSlider, false);
    addAndMakeVisible(tremoloRateLabel);

    tremoloDepthLabel.setText("Depth", juce::dontSendNotification);
    tremoloDepthLabel.setJustificationType(juce::Justification::centred);
    tremoloDepthLabel.attachToComponent(&tremoloDepthSlider, false);
    addAndMakeVisible(tremoloDepthLabel);

    reverbSizeLabel.setText("Size", juce::dontSendNotification);
    reverbSizeLabel.setJustificationType(juce::Justification::centred);
    reverbSizeLabel.attachToComponent(&reverbSizeSlider, false);
    addAndMakeVisible(reverbSizeLabel);

    reverbMixLabel.setText("Mix", juce::dontSendNotification);
    reverbMixLabel.setJustificationType(juce::Justification::centred);
    reverbMixLabel.attachToComponent(&reverbMixSlider, false);
    addAndMakeVisible(reverbMixLabel);
    
    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.attachToComponent(&gainSlider, false);
    addAndMakeVisible(gainLabel);
    
    
    //Apply look and feel
    gainSlider.setLookAndFeel(&customLNF);
    decaySlider.setLookAndFeel(&customLNF);
    widthSlider.setLookAndFeel(&customLNF);
    filterCutoffSlider.setLookAndFeel(&customLNF);
    lowFilterCutoffSlider.setLookAndFeel(&customLNF);
    tremoloRateSlider.setLookAndFeel(&customLNF);
    tremoloDepthSlider.setLookAndFeel(&customLNF);
    reverbSizeSlider.setLookAndFeel(&customLNF);
    reverbMixSlider.setLookAndFeel(&customLNF);
    
    
    gainAttach         = std::make_unique<SliderAttachment>(apvts, "gain", gainSlider);
    decayAttach        = std::make_unique<SliderAttachment>(apvts, "decay", decaySlider);
    widthAttach        = std::make_unique<SliderAttachment>(apvts, "width", widthSlider);
    filterCutoffAttach = std::make_unique<SliderAttachment>(apvts, "filterCutoff", filterCutoffSlider);
    lowFilterCutoffAttach = std::make_unique<SliderAttachment>(apvts, "lowFilterCutoff", lowFilterCutoffSlider);
    tremoloRateAttach  = std::make_unique<SliderAttachment>(apvts, "tremoloRate", tremoloRateSlider);
    tremoloDepthAttach = std::make_unique<SliderAttachment>(apvts, "tremoloDepth", tremoloDepthSlider);
    reverbSizeAttach   = std::make_unique<SliderAttachment>(apvts, "reverbSize", reverbSizeSlider);
    reverbMixAttach    = std::make_unique<SliderAttachment>(apvts, "reverbMix", reverbMixSlider);
    sourceAttach       = std::make_unique<ComboBoxAttachment>(apvts, "source", sourceChoice);
}

//Destructor
Karplus_Bonus_AudioProcessorEditor::~Karplus_Bonus_AudioProcessorEditor()
{
    gainSlider.setLookAndFeel(nullptr);
    decaySlider.setLookAndFeel(nullptr);
    widthSlider.setLookAndFeel(nullptr);
    filterCutoffSlider.setLookAndFeel(nullptr);
    lowFilterCutoffSlider.setLookAndFeel(nullptr);
    tremoloRateSlider.setLookAndFeel(nullptr);
    tremoloDepthSlider.setLookAndFeel(nullptr);
    reverbSizeSlider.setLookAndFeel(nullptr);
    reverbMixSlider.setLookAndFeel(nullptr);
}

void Karplus_Bonus_AudioProcessorEditor::paint(juce::Graphics& g)
{
    if (backgroundImage.isValid())
    {
        g.drawImage(backgroundImage,
                    0, -15, getWidth(), getHeight(),                                  // destination bounds (fills entire UI)
                    0, 0, backgroundImage.getWidth(), backgroundImage.getHeight()); // source bounds
    }
    else
    {
        g.fillAll(juce::Colours::black); // fallback if image fails to load
    }
    
    g.setColour(juce::Colours::white);           // text colour
    g.setFont(juce::Font(18.0f, juce::Font::bold)); // size and style

    g.drawText("Source", 136 -50, 30, 100, 20, juce::Justification::centred);
    g.drawText("Tremolo", 386 -50, 30, 100, 20, juce::Justification::centred);
    g.drawText("Reverb", 386 -50, 200, 100, 20, juce::Justification::centred);
    g.drawText("Output", 595 -50, 30, 100, 20, juce::Justification::centred);
}

void Karplus_Bonus_AudioProcessorEditor::resized()
{
    // Keyboard
    processor.keyboardComponent.setBounds(0, 370, getWidth(), 80);
    
    // Source
    sourceChoice.setBounds(83, 60, 110, 25);
    decaySlider.setBounds(48, 120, 80, 80);
    widthSlider.setBounds(148, 120, 80, 80);
    filterCutoffSlider.setBounds(93, 225, 90, 90);

    // FX
    tremoloRateSlider.setBounds(291, 68, 80, 80);
    tremoloDepthSlider.setBounds(406, 68, 80, 80);
    reverbSizeSlider.setBounds(291, 233, 80, 80);
    reverbMixSlider.setBounds(406, 233, 80, 80);

    // Output
    gainSlider.setBounds(545, 90, 100, 100);
    lowFilterCutoffSlider.setBounds(555, 225, 80, 80);

}
