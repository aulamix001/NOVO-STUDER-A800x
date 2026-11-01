#include "../src/PluginProcessor.h"
#include <iostream>
#include <stdexcept>
void check(bool b){if(!b)throw std::runtime_error("processor check failed");}
void set(A800Processor& p,const char* id,float v){auto* x=p.state.getParameter(id);check(x!=nullptr);x->setValueNotifyingHost(x->convertTo0to1(v));}
juce::Component* find(juce::Component& parent,const juce::String& id) {
 if(parent.getComponentID()==id)return &parent;
 for(auto* child:parent.getChildren())if(auto* match=find(*child,id))return match;
 return nullptr;
}
int main(int argc,char**){
 A800Processor p;juce::MidiBuffer midi;
 check(p.getParameters().size()==64);check(p.getNumPrograms()==90);
 for(int i=0;i<90;++i){p.setCurrentProgram(i);check(p.getCurrentProgram()==i);check(p.getProgramName(i).isNotEmpty());}
 p.setCurrentProgram(0);
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
 set(p,"ch1_rec_input_level",0);p.setStateInformation(state.getData(),int(state.getSize()));
 check(p.state.getRawParameterValue("ch1_rec_input_level")->load()==3);
 const char invalid[]="invalid";p.setStateInformation(invalid,sizeof(invalid));
 check(p.state.getRawParameterValue("ch1_rec_input_level")->load()==3);
 set(p,"mastersafe",1);p.prepareToPlay(48000,512);
 for(int ch=0;ch<2;++ch)for(int n=0;n<512;++n)b.setSample(ch,n,0.2f);
 p.processBlock(b,midi);check(b.getSample(0,511)==0.2f && b.getSample(1,511)==0.2f);
 for(int i=0;i<450;++i)p.processBlock(b,midi);
 check(std::abs(p.meterRms[0].load()-.2f)<1e-5f);
 if(argc>1) {
  juce::ScopedJuceInitialiser_GUI gui;
  for(int attempt=0;attempt<3;++attempt){
   std::unique_ptr<juce::AudioProcessorEditor> e(p.createEditorIfNeeded());check(e!=nullptr);
   e->setSize(1152,768);e->addToDesktop(0);e->setVisible(true);
   auto* input=dynamic_cast<juce::Slider*>(find(*e,"ch1_rec_input_level"));check(input!=nullptr);
   input->setValue(-1.5,juce::sendNotificationSync);check(p.state.getRawParameterValue("ch1_rec_input_level")->load()==-1.5f);
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
    juce::File output("/tmp/a800-visual-ui.png");auto stream=output.createOutputStream();check(stream!=nullptr);
    juce::PNGImageFormat format;check(format.writeImageToStream(snapshot,*stream));
   }
   e->setSize(768,512);check(e->getWidth()==768);e->setSize(1536,1024);e->setVisible(false);
   p.editorBeingDeleted(e.get());
  }
  std::cout<<"PASS: editor open/close 3x, all tabs, main knob/choice attachments, resize, rendered screenshot\n";
 }
 std::cout<<"PASS: 64 parameters, 90 programs, four sample rates, L/R gain, state restore, malformed state, MASTERSAFE\n";
}
