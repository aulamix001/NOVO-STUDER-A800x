#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "dsp/TapeConstants.h"
#include "GeneratedParameters.h"

A800Processor::A800Processor()
 : AudioProcessor(BusesProperties().withInput("Stereo input",juce::AudioChannelSet::stereo(),true)
                                  .withOutput("Stereo output",juce::AudioChannelSet::stereo(),true)),
   state(*this,nullptr,"A800_STATE",makeLayout()) {
 for(size_t i=0;i<parameterIds.size();++i)parameterValues[i]=state.getRawParameterValue(parameterIds[i]);
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
   const juce::String rate=s==0?"slow_":"fast_";
   c.recordTreble[s]=get("rec_"+rate+"treble");
   c.reproTreble[s]=get("rep_"+rate+"treble");c.reproBass[s]=get("rep_"+rate+"bass");
   c.syncTreble[s]=get("syn_"+rate+"treble");c.syncBass[s]=get("syn_"+rate+"bass");
   c.record[s]=get("rec_"+part);c.repro[s]=get("rep_"+part);c.sync[s]=get("syn_"+part);
  }
  c.fineBias=get("ch_adjust");
 }
 setLatencySamples(0); // Causal IIR processing, no lookahead or block buffering.
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
float A800Processor::parameter(const char* id) const noexcept {
 for(size_t i=0;i<parameterIds.size();++i)if(std::strcmp(id,parameterIds[i])==0)return parameterValues[i]->load();
 return 0;
}
a800::TapeSettings A800Processor::tapeSettings(size_t ch) const noexcept {
 const auto& c=channels[ch];const int active=speed->load()>=.5f?1:0;
 const bool sync=int(c.source->load())==2;
 a800::TapeSettings s;
 s.ips=a800::nominalSpeedIps(int(parameter("machine_version")),active!=0);
 if(parameter("varispeed_on")>.5f)s.ips*=std::pow(2.,parameter("varispeed_semitones")/12.);
 s.inputDb=c.input->load()+c.record[active]->load()+(sensitivity->load()>.5f?7:0);
 s.outputDb=sync?c.sync[active]->load()+c.out2->load():c.repro[active]->load()+c.out1->load();
 s.recordTreble=c.recordTreble[active]->load();
 if(!active&&parameter("treble_boost_tc")>0){
  constexpr double trebleHz[]{4500,3500,5000,7000};
  s.recordFrequency=trebleHz[juce::jlimit(0,3,int(parameter("treble_boost_tc")))];
  s.recordTreble+=(parameter("treble_boost_amt")+1)*1.5f;
 }
 s.treble=sync?c.syncTreble[active]->load():c.reproTreble[active]->load();
 s.bass=sync?c.syncBass[active]->load():c.reproBass[active]->load();
 s.bias=parameter(active?"bias_fast":"bias_slow")+c.fineBias->load();
 s.erase=parameter(active?"erase_fast":"erase_slow");
 s.alignment=parameter("alignment_dbfs");s.tape=int(parameter("tape_type"));s.ccir=parameter("eq_standard")>.5f;
 s.sync=sync;s.narrow=parameter("sync_range")>.5f;s.oscStop=parameter("osc_stop")>.5f;
 s.external=parameter("repro_input")>.5f;s.transport=parameter("transport_modelling")>.5f;
 s.driveDb=parameter("tape_drive");s.mix=parameter("tape_mix")*.01;s.noiseDb=parameter("tape_noise");
 s.wow=parameter("tape_wow")*.01;s.flutter=parameter("tape_flutter")*.01;
 return s;
}
void A800Processor::prepareToPlay(double sr,int) {
 sr=std::isfinite(sr)&&sr>0?sr:48000.;
 sampleRate.store(sr);
 for(auto& meter:meterRms) meter.store(0.f);
 for(size_t i=0;i<channels.size();++i) {auto& c=channels[i];c.gain.reset(sr,0.02);c.gain.setCurrentAndTargetValue(targetGain(c));c.tape.prepare(sr,int(i));c.tape.configure(tapeSettings(i),true);}
}
void A800Processor::processBlock(juce::AudioBuffer<float>& b,juce::MidiBuffer&) {
 juce::ScopedNoDenormals guard;
 bool playing=false,recording=false;
 if(auto* ph=getPlayHead()) if(auto pos=ph->getPosition()) {
  playing=pos->getIsPlaying();recording=pos->getIsRecording();
  if(auto t=pos->getTimeInSeconds())if(std::isfinite(*t))hostSeconds.store(*t);
 }

 if(hostPlaying.exchange(playing)!=playing)transportRevision.fetch_add(1);
 hostRecording.store(recording);
 for(int ch=0;ch<b.getNumChannels();++ch) {
  if(ch>=2) {b.clear(ch,0,b.getNumSamples());continue;}
  const float inputRms=b.getRMSLevel(ch,0,b.getNumSamples());
  auto& c=channels[size_t(ch)];c.gain.setTargetValue(targetGain(c));
  auto* data=b.getWritePointer(ch);
  c.tape.configure(tapeSettings(size_t(ch)));
  const bool bypass=safe->load()>=.5f||parameter("tape_record")<.5f;
  const bool input=int(c.source->load())==0;
  for(int i=0;i<b.getNumSamples();++i) {
   const float dry=data[i];const float wet=float(c.tape.tick(dry));const float gain=c.gain.getNextValue();
   data[i]=bypass?dry:(input?dry*gain:wet);
  }
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
 auto tree=state.copyState(); tree.setProperty("schema_version",2,nullptr);
 tree.setProperty("program",currentProgram.load(),nullptr);
 tree.setProperty("zero_reference",zeroReference.load(),nullptr);
 tree.setProperty("memo_reference",memoPosition.load(),nullptr);tree.setProperty("memo_active",memoActive.load(),nullptr);
 tree.setProperty("transport_shortcut_enabled",transportShortcutEnabled.load(),nullptr);
 if(auto xml=tree.createXml()) copyXmlToBinary(*xml,b);
}
void A800Processor::setStateInformation(const void* data,int size) {
 auto xml=getXmlFromBinary(data,size);
 if(!xml || !xml->hasTagName("A800_STATE")) return;
 auto tree=juce::ValueTree::fromXml(*xml);
 const int schema=int(tree.getProperty("schema_version",0));
 if(schema!=1&&schema!=2) return;
 if(schema==1&&tree.getNumChildren()==64){
  for(size_t i=64;i<parameterIds.size();++i){
   juce::ValueTree child("PARAM");child.setProperty("id",parameterIds[i],nullptr);
   auto* p=state.getParameter(parameterIds[i]);child.setProperty("value",p->convertFrom0to1(p->getDefaultValue()),nullptr);tree.addChild(child,-1,nullptr);
  }
 }
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
 const double memo=double(tree.getProperty("memo_reference",0.0));if(std::isfinite(memo))memoPosition.store(memo);
 memoActive.store(bool(tree.getProperty("memo_active",false)));
 transportShortcutEnabled.store(bool(tree.getProperty("transport_shortcut_enabled",true)));
}
juce::AudioProcessorEditor* A800Processor::createEditor() {return new A800Editor(*this);}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {return new A800Processor();}
