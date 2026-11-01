#include "../src/PluginProcessor.h"
#include <iostream>
#include <stdexcept>
#define check(b) do { if(!(b))throw std::runtime_error("processor check failed at line "+std::to_string(__LINE__)+": " #b); } while(false)
void set(A800Processor& p,const char* id,float v){auto* x=p.state.getParameter(id);check(x!=nullptr);x->setValueNotifyingHost(x->convertTo0to1(v));}
juce::Component* find(juce::Component& parent,const juce::String& id) {
 if(parent.getComponentID()==id)return &parent;
 for(auto* child:parent.getChildren())if(auto* match=find(*child,id))return match;
 return nullptr;
}
// Exercise the actual APVTS -> DSP wiring, including inactive-bank rules.
void testAudioControls(){
 auto render=[](const juce::String& changed,float val,int src,int fast,bool boost,bool mechanical){
  A800Processor p;p.setCurrentProgram(18);set(p,"tape_noise",-100);set(p,"stereo_link",0);
  set(p,"ch1_src",float(src));set(p,"ch2_src",float(src));set(p,"speed_select",float(fast));
  set(p,"treble_boost_tc",boost?1.f:0.f);set(p,"transport_modelling",mechanical?1.f:0.f);
  set(p,"varispeed_on",1);set(p,"varispeed_semitones",1);
  if(changed.isNotEmpty())set(p,changed.toRawUTF8(),val);
  p.prepareToPlay(48000,127);juce::AudioBuffer<float> b(2,127);juce::MidiBuffer midi;std::vector<float> out;
  for(int block=0;block<100;++block){
   for(int ch=0;ch<2;++ch)for(int n=0;n<127;++n){const double t=double(block*127+n)/48000.;b.setSample(ch,n,float(.2*std::sin(juce::MathConstants<double>::twoPi*997*t)+.1*std::sin(juce::MathConstants<double>::twoPi*8003*t)+.1*std::sin(juce::MathConstants<double>::twoPi*73*t)));}
   p.processBlock(b,midi);if(block>30)for(int ch=0;ch<2;++ch)for(int n=0;n<127;++n){const auto v=b.getSample(ch,n);check(std::isfinite(v));out.push_back(v);}
  }
  return out;
 };
 auto compare=[&](const juce::String& id,float value,int src=1,int fast=1,bool boost=false,bool mechanical=false){
  const auto base=render("",0,src,fast,boost,mechanical),changed=render(id,value,src,fast,boost,mechanical);double d=0;
  for(size_t i=0;i<base.size();++i)d+=std::abs(base[i]-changed[i]);
  if(d/base.size()<1e-7)throw std::runtime_error("Inactive audio control: "+id.toStdString());
 };
 for(int ch=1;ch<=2;++ch){const auto prefix="ch"+juce::String(ch)+"_";
  compare(prefix+"src",0);compare(prefix+"rec_input_level",3);compare(prefix+"rep_out1_level",3);compare(prefix+"syn_out2_level",3,2);
  compare(prefix+"ch_adjust",.5);
  for(int fast=0;fast<2;++fast){const auto rate=fast?juce::String("fast_"):juce::String("slow_");
   compare(prefix+"rec_"+rate+"level",3,1,fast);compare(prefix+"rec_"+rate+"treble",3,1,fast);
   for(const auto* kind:{"rep_","syn_"})for(const auto* knob:{"level","treble","bass"})compare(prefix+kind+rate+knob,3,kind[0]=='r'?1:2,fast);
  }
 }
 for(int fast=0;fast<2;++fast){compare(fast?"bias_fast":"bias_slow",1,1,fast);compare(fast?"erase_fast":"erase_slow",-3,1,fast);}
 for(const auto& item:std::initializer_list<std::pair<const char*,float>>{{"machine_version",0},{"speed_select",0},{"eq_standard",1},{"tape_type",0},{"in_sens_plus7",1},{"osc_stop",1},{"repro_input",1},{"alignment_dbfs",-12},{"tape_drive",12},{"tape_mix",30},{"tape_noise",-50},{"tape_record",0},{"mastersafe",1},{"varispeed_semitones",6},{"varispeed_on",0}})compare(item.first,item.second);
 compare("sync_range",1,2);compare("transport_modelling",1);
 compare("tape_wow",70,1,1,false,true);compare("tape_flutter",70,1,1,false,true);
 compare("treble_boost_tc",3,1,0,true);compare("treble_boost_amt",3,1,0,true);
 // FAST controls must leave the SLOW route unchanged.
 const auto slow=render("",0,1,0,false,false),inactive=render("ch1_rec_fast_treble",3,1,0,false,false);check(slow==inactive);
 std::cout<<"PASS: every audio parameter reaches its active DSP route; inactive speed bank stays inactive\n";
}
int main(int argc,char**){
 testAudioControls();
 A800Processor p;juce::MidiBuffer midi;
 check(p.getParameters().size()==70);check(p.getNumPrograms()==90);
 for(int i=0;i<90;++i){p.setCurrentProgram(i);check(p.getCurrentProgram()==i);check(p.getProgramName(i).isNotEmpty());}
 p.setCurrentProgram(0);
 set(p,"ch1_src",0);set(p,"ch2_src",0);
 for(double sr:{44100.,48000.,96000.,192000.}) {
  p.prepareToPlay(sr,512);juce::AudioBuffer<float> b(2,512);
  for(int ch=0;ch<2;++ch)for(int n=0;n<512;++n)b.setSample(ch,n,0.125f);
  p.processBlock(b,midi);check(std::abs(b.getSample(0,511)-0.125f)<1e-6f);
 }
 set(p,"ch1_rec_input_level",3);set(p,"ch2_rec_input_level",-3);
 p.prepareToPlay(48000,512);juce::AudioBuffer<float> b(2,512);
 for(int ch=0;ch<2;++ch)for(int n=0;n<512;++n)b.setSample(ch,n,0.1f);
 p.processBlock(b,midi);check(b.getSample(0,511)>b.getSample(1,511));
 juce::MemoryBlock state;p.getStateInformation(state);
 p.transportShortcutEnabled.store(false);
 set(p,"ch1_rec_input_level",0);p.setStateInformation(state.getData(),int(state.getSize()));
 check(p.state.getRawParameterValue("ch1_rec_input_level")->load()==3);
 check(p.transportShortcutEnabled.load());
 const char invalid[]="invalid";p.setStateInformation(invalid,sizeof(invalid));
 check(p.state.getRawParameterValue("ch1_rec_input_level")->load()==3);
 set(p,"mastersafe",1);p.prepareToPlay(48000,512);
 for(int ch=0;ch<2;++ch)for(int n=0;n<512;++n)b.setSample(ch,n,0.2f);
 p.processBlock(b,midi);check(b.getSample(0,511)==0.2f && b.getSample(1,511)==0.2f);
 for(int i=0;i<450;++i)p.processBlock(b,midi);
 check(std::abs(p.meterRms[0].load()-.2f)<1e-5f);
 // Old 64-parameter sessions migrate to the six new defaults.
 set(p,"tape_drive",9);juce::MemoryBlock modern;p.getStateInformation(modern);
 auto oldXml=juce::AudioProcessor::getXmlFromBinary(modern.getData(),int(modern.getSize()));
 check(oldXml!=nullptr);auto old=juce::ValueTree::fromXml(*oldXml);old.setProperty("schema_version",1,nullptr);
 for(const auto* id:{"tape_drive","tape_mix","tape_noise","tape_wow","tape_flutter","tape_record"})
  for(int i=old.getNumChildren()-1;i>=0;--i)if(old.getChild(i).getProperty("id").toString()==id)old.removeChild(i,nullptr);
 juce::MemoryBlock legacy;auto legacyXml=old.createXml();juce::AudioProcessor::copyXmlToBinary(*legacyXml,legacy);
 p.setStateInformation(legacy.getData(),int(legacy.getSize()));check(p.state.getRawParameterValue("tape_drive")->load()==0);
 if(argc>1) {
  juce::ScopedJuceInitialiser_GUI gui;
  for(int attempt=0;attempt<3;++attempt){
   std::unique_ptr<juce::AudioProcessorEditor> e(p.createEditorIfNeeded());check(e!=nullptr);
   e->setSize(1152,768);e->addToDesktop(0);e->setVisible(true);
   auto* play=find(*e,"PLAY");auto* stop=find(*e,"STOP");check(play!=nullptr&&stop!=nullptr);
   auto* button=dynamic_cast<juce::Button*>(play);check(button!=nullptr);button->onClick();
   check(!p.hostPlaying.load()); // A failed/unsupported command must not fake playback.
   auto* input=dynamic_cast<juce::Slider*>(find(*e,"ch1_rec_input_level"));check(input!=nullptr);
   p.state.getParameter("ch1_rec_input_level")->beginChangeGesture();
   input->setValue(-1.5,juce::sendNotificationSync);
   p.state.getParameter("ch1_rec_input_level")->endChangeGesture();check(p.state.getRawParameterValue("ch1_rec_input_level")->load()==-1.5f);
   check(p.state.getRawParameterValue("ch2_rec_input_level")->load()==-1.5f);
   auto* rec=dynamic_cast<juce::Button*>(find(*e,"REC"));check(rec!=nullptr);rec->onClick();check(p.state.getRawParameterValue("tape_record")->load()==0);rec->onClick();
   auto* memo=dynamic_cast<juce::Button*>(find(*e,"MEMO"));check(memo!=nullptr);p.zeroReference.store(4);memo->onClick();p.zeroReference.store(9);memo->onClick();check(p.zeroReference.load()==4);
   auto* tape=dynamic_cast<juce::ComboBox*>(find(*e,"tape_type"));check(tape!=nullptr);
   tape->setSelectedId(9,juce::sendNotificationSync);check(p.state.getRawParameterValue("tape_type")->load()==8);
   for(auto id:{"tab_service","tab_presets","tab_settings","tab_machine"}){
    auto* button=dynamic_cast<juce::TextButton*>(find(*e,id));check(button!=nullptr);button->onClick();
   }
   check(find(*e,"presetList")!=nullptr);check(find(*e,"presetSearch")!=nullptr);
   auto* presetTab=dynamic_cast<juce::TextButton*>(find(*e,"tab_presets"));presetTab->onClick();
   auto* search=dynamic_cast<juce::TextEditor*>(find(*e,"presetSearch"));check(search!=nullptr);
   search->setText("AMPEX 456",false);search->onTextChange();
   auto* list=dynamic_cast<juce::ListBox*>(find(*e,"presetList"));check(list!=nullptr);check(list->getModel()->getNumRows()==6);
   list->selectRow(0);auto* load=dynamic_cast<juce::TextButton*>(find(*e,"loadPreset"));load->onClick();
   check(p.getProgramName(p.getCurrentProgram()).contains("AMPEX 456"));
   dynamic_cast<juce::TextButton*>(find(*e,"tab_machine"))->onClick();
   if(attempt==0){
    p.setCurrentProgram(18);
    auto snapshot=e->createComponentSnapshot(e->getLocalBounds());check(snapshot.isValid());
    auto output=juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile("a800-visual-ui.png");auto stream=output.createOutputStream();check(stream!=nullptr);check(stream->setPosition(0));check(stream->truncate().wasOk());
    juce::PNGImageFormat format;check(format.writeImageToStream(snapshot,*stream));
    dynamic_cast<juce::TextButton*>(find(*e,"tab_settings"))->onClick();
    auto fx=e->createComponentSnapshot(e->getLocalBounds());
    auto fxFile=juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile("a800-tape-fx-ui.png");
    auto fxStream=fxFile.createOutputStream();check(fxStream!=nullptr);check(fxStream->setPosition(0));check(fxStream->truncate().wasOk());check(format.writeImageToStream(fx,*fxStream));
   }
   e->setSize(768,512);check(e->getWidth()==768);e->setSize(1536,1024);e->setVisible(false);
   p.editorBeingDeleted(e.get());
  }
  std::cout<<"PASS: editor open/close 3x, all tabs, main knob/choice attachments, resize, rendered screenshot\n";
 }
 std::cout<<"PASS: 70 parameters, 90 programs, four sample rates, L/R gain, state restore, malformed state, MASTERSAFE\n";
}
