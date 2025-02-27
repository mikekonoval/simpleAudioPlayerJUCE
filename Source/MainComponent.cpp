/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"


//==============================================================================
MainComponent::MainComponent() : juce::AudioAppComponent(otherDeviceManager), state(Stopped), openButton("Open"), playButton("Play"), stopButton("Stop")
{
    otherDeviceManager.initialise(2, 2, nullptr, true);
    audioSettings.reset(new AudioDeviceSelectorComponent(otherDeviceManager, 0, 2, 0, 2, true, true, true, true));
    addAndMakeVisible(audioSettings.get());
    
    setAudioChannels (2, 2);
    
    openButton.onClick = [this] {  openButtonClicked(); };
    addAndMakeVisible(&openButton);
    
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour(TextButton::buttonColourId, Colours::green);
    playButton.setEnabled(true);
    addAndMakeVisible(&playButton);
    
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour(TextButton::buttonColourId, Colours::red);
    stopButton.setEnabled(false);
    addAndMakeVisible(&stopButton);
    
    formatManager.registerBasicFormats();
    transport.addChangeListener(this);
    
    setSize (400, 600);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    transport.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::openButtonClicked()
{
    chooser = std::make_unique<juce::FileChooser>("Choose a Wav or AIFF File", juce::File{}, "*.wav; *.mp3");
    
    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this] (const juce::FileChooser& fc)
                         {
                                     auto file = fc.getResult();
        DBG("Ok");
                                        
                                     if (file != juce::File{})
                                     {
                                         auto* reader = formatManager.createReaderFor (file);
                          
                                         if (reader != nullptr)
                                         {
                                             auto newSource = std::make_unique<juce::AudioFormatReaderSource> (reader, true);   // [11]
                                             transport.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
                                             playButton.setEnabled (true);
                                             playSource.reset (newSource.release());
                                         }
                                     }
                                 });
    transportStateChanged(Stopped);
    
    //if the user chooses a file
//    if (chooser->browseForFileToOpen())
//    {
//        File myFile;
//
//        //what did the user choose?
//        myFile = chooser->getResult();
//
//        //read the file
//        AudioFormatReader* reader = formatManager.createReaderFor(myFile);
//
//        if (reader != nullptr)
//        {
//            //get the file ready to play
//            std::unique_ptr<AudioFormatReaderSource> tempSource (new AudioFormatReaderSource (reader, true));
//
//            transport.setSource(tempSource.get());
//            transportStateChanged(Stopped);
//
//            playSource.reset(tempSource.release());
//        }
//    }
}

void MainComponent::playButtonClicked()
{
    transportStateChanged(Starting);
}

void MainComponent::stopButtonClicked()
{
    transportStateChanged(Stopping);
}

void MainComponent::transportStateChanged(TransportState newState)
{
    if (newState != state)
    {
        state = newState;
        
        switch (state) {
            case Stopped:
                playButton.setEnabled(true);
                transport.setPosition(0.0);
                break;
                
            case Playing:
                playButton.setEnabled(true);
                break;
                
            case Starting:
                stopButton.setEnabled(true);
                playButton.setEnabled(false);
                transport.start();
                break;
                
            case Stopping:
                playButton.setEnabled(true);
                stopButton.setEnabled(false);
                transport.stop();
                break;
        }
    }
}

void MainComponent::changeListenerCallback (ChangeBroadcaster *source)
{
    if (source == &transport)
    {
        if (transport.isPlaying())
        {
            transportStateChanged(Playing);
        }
        else
        {
            transportStateChanged(Stopped);
        }
    }
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    
    transport.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    openButton.setBounds(10, 10, getWidth() - 20, 30);
    playButton.setBounds(10, 50, getWidth() - 20, 30);
    stopButton.setBounds(10, 90, getWidth() - 20, 30);
    audioSettings->setBounds(10, 130, getWidth() - 20, 100);
}
