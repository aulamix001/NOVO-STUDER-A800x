#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <array>
class A800Processor final : public juce::AudioProcessor {
public:
 A800Processor();
 const juce::String getName() const override { return "A800 Technical Prototype"; }
 void prepareToPlay(double,int) override;
 void releaseResources() override {}
 bool isBusesLayoutSupported(const BusesLayout&) const override;
 void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
 juce::AudioProcessorEditor* createEditor() override;
 bool hasEditor() const override {return true;}
 double getTailLengthSeconds() const override {return 0;}
 bool acceptsMidi() const override {return false;}
 bool producesMidi() const override {return false;}
 bool isMidiEffect() const override {return false;}
 int getNumPrograms() override {return 90;}
 int getCurrentProgram() override {return currentProgram.load();}
 void setCurrentProgram(int) override;
 const juce::String getProgramName(int) override;
 void changeProgramName(int,const juce::String&) override {}
 void getStateInformation(juce::MemoryBlock&) override;
 void setStateInformation(const void*,int) override;
 juce::AudioProcessorValueTreeState state;
 // UI observes atomics only; no editor or graphics work on the audio thread.
 std::array<std::atomic<float>,2> meterRms{{0.f,0.f}};
 std::atomic<bool> hostPlaying{false},hostRecording{false};
 std::atomic<bool> transportShortcutEnabled{true};
 std::atomic<std::uint64_t> transportRevision{0};
 std::atomic<double> hostSeconds{0.0},zeroReference{0.0};
private:
 struct Channel {
  std::atomic<float>* source{};
  std::atomic<float>* input{};
  std::atomic<float>* out1{};
  std::atomic<float>* out2{};
  std::atomic<float>* record[2]{};
  std::atomic<float>* repro[2]{};
  std::atomic<float>* sync[2]{};
  juce::SmoothedValue<float> gain;
 };
 std::array<Channel,2> channels;
 std::atomic<float>* safe{};
 std::atomic<float>* speed{};
 std::atomic<float>* sensitivity{};
 std::atomic<double> sampleRate{48000.0};
 std::atomic<float>* calibrateMeter{};
 std::atomic<int> currentProgram{18};
 float targetGain(const Channel&) const noexcept;
};
