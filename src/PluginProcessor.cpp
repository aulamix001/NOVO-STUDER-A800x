#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "dsp/TapeConstants.h"
#include "GeneratedParameters.h"

A800Processor::A800Processor()
 : AudioProcessor(BusesProperties().withInput("Stereo input",juce::AudioChannelSet::stereo(),true)
                                  .withOutput("Stereo output",juce::AudioChannelSet::stereo(),true)),
   state(*this,nullptr,"A800_STATE",makeLayout()) {
 safe=state.getRawParameterValue("mastersafe");
 speed=state.getRawParameterValue("speed_select");
 sensitivity=state.getRawParameterValue("in_sens_plus7");
 calibrateMeter=state.getRawParameterValue("calibrate");
 for(int i=0;i<2;++i) {
  const auto prefix="ch"+juce::String(i+1)+"_";
  auto get=[&](const juce::String& suffix) {return state.getRawParameterValue(prefix+suffix);};
  auto& c=channels[size_t(i)];
  c.source=get("src"); c.input=get("rec_input_level");
  c.out1=get("rep_out1_level");c.out2=get("syn_out2_level");
  for(int s=0;s<2;++s) {
   const juce::String part=s==0 ? "slow_level" : "fast_level";
   c.record[s]=get("rec_"+part);c.repro[s]=get("rep_"+part);c.sync[s]=get("syn_"+part);
  }
 }
 setLatencySamples(0); // No tape model or oversampling exists in this technical prototype.
}
bool A800Processor::isBusesLayoutSupported(const BusesLayout& b) const {
 return b.getMainInputChannelSet()==juce::AudioChannelSet::stereo()
     && b.getMainOutputChannelSet()==juce::AudioChannelSet::stereo();
}
float A800Processor::targetGain(const Channel& c) const noexcept {
 if(safe->load()>=0.5f) return 1.f;
 const int src=juce::jlimit(0,2,int(c.source->load()));
 float db=c.input->load()+(sensitivity->load()>=0.5f ? 7.f : 0.f);
 if(src!=0) {
  const int active=speed->load()>=0.5f ? 1 : 0;
  db+=c.record[active]->load();
  db+=src==1 ? c.repro[active]->load()+c.out1->load() : c.sync[active]->load()+c.out2->load();
 }
 return juce::Decibels::decibelsToGain(db);
}
void A800Processor::prepareToPlay(double sr,int) {
 sr=std::isfinite(sr)&&sr>0?sr:48000.;
 sampleRate.store(sr);
 for(auto& meter:meterRms) meter.store(0.f);
 for(auto& c:channels) {c.gain.reset(sr,0.02);c.gain.setCurrentAndTargetValue(targetGain(c));}
}
void A800Processor::processBlock(juce::AudioBuffer<float>& b,juce::MidiBuffer&) {
 juce::ScopedNoDenormals guard;
 hostPlaying.store(false);hostRecording.store(false);
 if(auto* ph=getPlayHead()) if(auto pos=ph->getPosition()) {
  hostPlaying.store(pos->getIsPlaying());hostRecording.store(pos->getIsRecording());
  if(auto t=pos->getTimeInSeconds())if(std::isfinite(*t))hostSeconds.store(*t);
 }

 for(int ch=0;ch<b.getNumChannels();++ch) {
  if(ch>=2) {b.clear(ch,0,b.getNumSamples());continue;}
  const float inputRms=b.getRMSLevel(ch,0,b.getNumSamples());
  auto& c=channels[size_t(ch)];c.gain.setTargetValue(targetGain(c));
  auto* data=b.getWritePointer(ch);
  for(int i=0;i<b.getNumSamples();++i) data[i]*=c.gain.getNextValue();
  const auto rms=calibrateMeter->load()>=0.5f
      ? b.getRMSLevel(ch,0,b.getNumSamples()) : inputRms;
  const float old=meterRms[size_t(ch)].load();
  const float alpha=1.f-std::exp(-float(b.getNumSamples())/float(sampleRate.load()*a800::kVuIntegrationSeconds));
  meterRms[size_t(ch)].store(old+alpha*(rms-old));
 }
}
void A800Processor::setCurrentProgram(int index) {
 if(index<0 || index>=90) return;
 const auto& p=factoryPresets[size_t(index)];
 for(size_t i=0;i<parameterIds.size();++i)
  if(auto* param=state.getParameter(parameterIds[i]))
   param->setValueNotifyingHost(param->convertTo0to1(p.values[i]));
 currentProgram.store(index);
}
const juce::String A800Processor::getProgramName(int index) {
 return index>=0 && index<90 ? juce::String::fromUTF8(factoryPresets[size_t(index)].name) : juce::String{};
}
void A800Processor::getStateInformation(juce::MemoryBlock& b) {
 auto tree=state.copyState(); tree.setProperty("schema_version",1,nullptr);
 tree.setProperty("program",currentProgram.load(),nullptr);
 tree.setProperty("zero_reference",zeroReference.load(),nullptr);
 if(auto xml=tree.createXml()) copyXmlToBinary(*xml,b);
}
void A800Processor::setStateInformation(const void* data,int size) {
 auto xml=getXmlFromBinary(data,size);
 if(!xml || !xml->hasTagName("A800_STATE")) return;
 auto tree=juce::ValueTree::fromXml(*xml);
 if(int(tree.getProperty("schema_version",0))!=1) return;
 // Validate fully before replacing state; reject unknown, missing, duplicate or invalid values.
 if(tree.getNumChildren()!=int(parameterIds.size())) return;
 for(auto id:parameterIds) {
  auto* p=state.getParameter(id); int matches=0;
  for(auto child:tree) if(child.getProperty("id").toString()==id) {
   ++matches;
   if(!child.hasProperty("value")) return;
   const float v=float(child.getProperty("value"));
   const auto range=p->getNormalisableRange();
   if(!std::isfinite(v) || v<range.start || v>range.end) return;
  }
  if(matches!=1) return;
 }
 const int program=int(tree.getProperty("program",0));
 if(program<0 || program>=90) return;
 state.replaceState(tree);currentProgram.store(program);
 const double zero=double(tree.getProperty("zero_reference",0.0));
 if(std::isfinite(zero))zeroReference.store(zero);
}
juce::AudioProcessorEditor* A800Processor::createEditor() {return new A800Editor(*this);}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {return new A800Processor();}
