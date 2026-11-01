#include "PluginEditor.h"
#include "BinaryData.h"
#include "transport/TransportRequest.h"
#include "transport/WindowsTransport.h"
#include <vector>
#include <cmath>

namespace {
constexpr float pi=juce::MathConstants<float>::pi;
constexpr int designW=1536,designH=1024;
using APVTS=juce::AudioProcessorValueTreeState;
using SliderAttachment=APVTS::SliderAttachment;
using ComboAttachment=APVTS::ComboBoxAttachment;
using ButtonAttachment=APVTS::ButtonAttachment;

struct MachineLook final:juce::LookAndFeel_V4 {
 MachineLook(){
  setColour(juce::PopupMenu::backgroundColourId,juce::Colour(0xff202326));
  setColour(juce::PopupMenu::textColourId,juce::Colours::whitesmoke);
  setColour(juce::PopupMenu::highlightedBackgroundColourId,juce::Colour(0xff80623b));
  setColour(juce::Label::textColourId,juce::Colour(0xffe5e4de));
  setColour(juce::TextEditor::backgroundColourId,juce::Colour(0xff121619));
  setColour(juce::TextEditor::textColourId,juce::Colours::whitesmoke);
  setColour(juce::ComboBox::textColourId,juce::Colour(0xffeae9e3));
 }
 juce::Font getComboBoxFont(juce::ComboBox&)override{return juce::Font(19.f);}
 void drawComboBox(juce::Graphics& g,int w,int h,bool,int,int,int,int,juce::ComboBox&)override {
  auto r=juce::Rectangle<float>(0,0,float(w),float(h)).reduced(1);
  g.setGradientFill({juce::Colour(0xff090c0e),0,0,juce::Colour(0xff252729),0,float(h),false});g.fillRoundedRectangle(r,2);
  g.setColour(juce::Colour(0xff999b99));g.drawRoundedRectangle(r,2,1);
  g.drawLine(float(w-27),3,float(w-27),float(h-3));
  juce::Path p;p.addTriangle(float(w-19),float(h/2-3),float(w-8),float(h/2-3),float(w-13),float(h/2+4));g.fillPath(p);
 }
 void positionComboBoxText(juce::ComboBox& b,juce::Label& l)override{
  l.setBounds(5,1,b.getWidth()-34,b.getHeight()-2);l.setFont(getComboBoxFont(b));l.setJustificationType(juce::Justification::centred);
 }
 void drawButtonBackground(juce::Graphics& g,juce::Button& b,const juce::Colour&,bool over,bool down)override{
  const bool on=b.getToggleState();auto r=b.getLocalBounds().toFloat().reduced(1);
  auto c=on?juce::Colour(0xffc9c9c4):juce::Colour(0xff16191c);
  g.setGradientFill({c.brighter(over?.15f:.05f),0,0,c.darker(down?.6f:.3f),0,r.getHeight(),false});g.fillRect(r);
  g.setColour(juce::Colour(0xff565b5b));g.drawRect(r,1);
  if(on){g.setColour(juce::Colour(0xffe2ba60));g.fillRect(1.f,float(b.getHeight()-3),float(b.getWidth()-2),2.f);}
 }
 void drawButtonText(juce::Graphics& g,juce::TextButton& b,bool,bool)override{
  g.setFont(14.f);g.setColour(b.getToggleState()?juce::Colours::black:juce::Colours::whitesmoke);g.drawText(b.getButtonText(),b.getLocalBounds(),juce::Justification::centred);
 }
 void drawRotarySlider(juce::Graphics& g,int x,int y,int w,int h,float pos,float,float,juce::Slider& s)override{
  auto r=juce::Rectangle<float>(float(x),float(y),float(w),float(h)).withSizeKeepingCentre(float(juce::jmin(w,h)),float(juce::jmin(w,h))).reduced(3);
  const bool selector=bool(s.getProperties()["selector"]);
  g.setColour(juce::Colour(0xff101416));g.fillEllipse(r);
  auto metal=r.reduced(selector?7.f:6.f);
  g.setColour(juce::Colours::black.withAlpha(.8f));g.fillEllipse(metal.translated(2,4));
  if(selector){
   g.setGradientFill({juce::Colour(0xff3b3d3d),metal.getX(),metal.getY(),juce::Colour(0xff060707),metal.getRight(),metal.getBottom(),false});g.fillEllipse(metal);
   g.setColour(juce::Colour(0xff7a7a74));g.drawEllipse(metal,1);
   const float angle=-pi/3.f-pos*pi/3.f;
   auto c=metal.getCentre();juce::Path lever;
   lever.addRoundedRectangle(-metal.getWidth()*.42f,-7,metal.getWidth()*.84f,14,4);
   auto transform=juce::AffineTransform::rotation(angle-pi/2).translated(c.x,c.y);
   g.setGradientFill({juce::Colour(0xffddddcf),c.x-10,c.y-10,juce::Colour(0xff777b78),c.x+10,c.y+10,false});g.fillPath(lever,transform);
   g.setColour(juce::Colours::black);g.strokePath(lever,juce::PathStrokeType(1.2f),transform);
  }else{
   g.setGradientFill({juce::Colour(0xfff5f4ee),metal.getX(),metal.getY(),juce::Colour(0xff7d8283),metal.getRight(),metal.getBottom(),false});g.fillEllipse(metal);
   {juce::Graphics::ScopedSaveState save(g);juce::Path clip;clip.addEllipse(metal);g.reduceClipRegion(clip);
    for(int i=0;i<180;++i){float a=float(i)*pi/90.f;auto c=metal.getCentre();float rr=metal.getWidth()*.5f;
     g.setColour((i%3?juce::Colours::white:juce::Colours::black).withAlpha(.035f));
     g.drawLine(c.x,c.y,c.x+std::sin(a)*rr,c.y-std::cos(a)*rr,.7f);}}
   g.setColour(juce::Colour(0xffb5b8b5));g.drawEllipse(metal,1.2f);
   g.setColour(juce::Colours::white.withAlpha(.6f));g.drawEllipse(metal.reduced(2),.6f);
   float a=-pi*.77f+pos*pi*1.54f;auto c=metal.getCentre();float radius=metal.getWidth()*.5f;
   g.setColour(juce::Colours::black.withAlpha(.85f));g.drawLine(c.x+std::sin(a)*radius*.6f,c.y-std::cos(a)*radius*.6f,c.x+std::sin(a)*radius*.96f,c.y-std::cos(a)*radius*.96f,3.f);
  }
 }
};
struct Hotspot final:juce::Button {
 explicit Hotspot(const juce::String& name):Button(name){}
 void paintButton(juce::Graphics& g,bool over,bool down)override{
  if(over||down){g.setColour(juce::Colour(0xffffcc77).withAlpha(down?.3f:.1f));g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(1),2);
   g.setColour(juce::Colour(0xffefd191).withAlpha(.75f));g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1),2,1);}
 }
};
struct ParameterRow final:juce::Component {
 juce::Label label;juce::Slider slider;juce::ComboBox choice;juce::ToggleButton toggle;
 std::unique_ptr<SliderAttachment> sa;std::unique_ptr<ComboAttachment> ca;std::unique_ptr<ButtonAttachment> ba;
 ParameterRow(APVTS& state,const juce::String& id){
  auto* p=state.getParameter(id);jassert(p);
  setComponentID("row_"+id);
  auto name=p->getName(128).replace(" [PENDING]","").replace(" [VISUAL]","");
  if(id.startsWith("ch1_"))name="L / "+name.substring(2);
  if(id.startsWith("ch2_"))name="R / "+name.substring(2);
  label.setText(name,juce::dontSendNotification);label.setFont(18.f);addAndMakeVisible(label);
  const bool pending=p->getName(128).contains("[PENDING]");
  const auto tip=pending?"Configuração armazenada; processamento sonoro ainda não implementado.":id;
  if(auto* c=dynamic_cast<juce::AudioParameterChoice*>(p)){
   for(int i=0;i<c->choices.size();++i)choice.addItem(c->choices[i],i+1);
   choice.setTooltip(tip);addAndMakeVisible(choice);ca=std::make_unique<ComboAttachment>(state,id,choice);
  }else if(dynamic_cast<juce::AudioParameterBool*>(p)){
   toggle.setTooltip(tip);toggle.setButtonText("ON / OFF");addAndMakeVisible(toggle);ba=std::make_unique<ButtonAttachment>(state,id,toggle);
  }else{
   slider.setSliderStyle(juce::Slider::LinearHorizontal);slider.setTextBoxStyle(juce::Slider::TextBoxRight,false,95,30);
   slider.setTooltip(tip);slider.setDoubleClickReturnValue(true,p->convertFrom0to1(p->getDefaultValue()));
   slider.setColour(juce::Slider::thumbColourId,juce::Colour(0xffd3b16c));
   addAndMakeVisible(slider);sa=std::make_unique<SliderAttachment>(state,id,slider);
  }
 }
 void resized()override{
  auto r=getLocalBounds().reduced(8,4);label.setBounds(r.removeFromLeft(430));
  slider.setBounds(r);choice.setBounds(r.reduced(2,4));toggle.setBounds(r);
 }
};
float value(APVTS& s,const char* id){return s.getRawParameterValue(id)->load();}
void setParameter(APVTS& s,const char* id,float v){
 if(auto* p=s.getParameter(id)){p->beginChangeGesture();p->setValueNotifyingHost(p->convertTo0to1(v));p->endChangeGesture();}
}
}

struct A800Editor::Canvas final:juce::Component,private juce::Timer,private juce::ListBoxModel {
 A800Processor& proc;
 MachineLook skin;
 juce::Image artwork;
 juce::OwnedArray<juce::Slider> knobs;
 juce::OwnedArray<juce::ComboBox> combos;
 juce::OwnedArray<Hotspot> hits;
 std::vector<std::unique_ptr<SliderAttachment>> sliderLinks;
 std::vector<std::unique_ptr<ComboAttachment>> comboLinks;
 std::vector<std::unique_ptr<ButtonAttachment>> buttonLinks;
 std::vector<juce::Component*> machineControls;
 juce::TextButton machine{"MACHINE"},service{"SERVICE"},presets{"PRESETS"},settings{"SETTINGS"};
 juce::Viewport serviceView,settingsView;
 juce::Component serviceRows,settingsRows;
 juce::OwnedArray<ParameterRow> rows;
 juce::TextEditor search;
 juce::ListBox presetList{"Factory presets",this};
 juce::TextButton load{"CARREGAR"},previous{"ANTERIOR"},next{"PRÓXIMO"};
 std::vector<int> filtered;
 juce::Label settingsInfo,transportStatus;
 juce::ToggleButton shortcutToggle;
 a800::TransportRequest transportRequest;
 int tab=0;float reelAngle=0;std::array<float,2> vu{{-20,-20}};

 explicit Canvas(A800Processor& p):proc(p){
  setComponentID("machineCanvas");setLookAndFeel(&skin);
  artwork=juce::ImageCache::getFromMemory(BinaryData::A800Panel_png,BinaryData::A800Panel_pngSize);
  for(auto* b:{&machine,&service,&presets,&settings})addAndMakeVisible(b);
  machine.setBounds(124,34,125,38);service.setBounds(249,34,124,38);presets.setBounds(373,34,125,38);settings.setBounds(498,34,125,38);
  machine.onClick=[this]{showTab(0);};service.onClick=[this]{showTab(1);};presets.onClick=[this]{showTab(2);};settings.onClick=[this]{showTab(3);};
  machine.setComponentID("tab_machine");service.setComponentID("tab_service");presets.setComponentID("tab_presets");settings.setComponentID("tab_settings");
  addKnob("ch1_rec_input_level",{372,875,106,106},false);
  addKnob("ch1_rep_out1_level",{576,875,106,106},false);
  addKnob("ch2_rec_input_level",{1069,875,106,106},false);
  addKnob("ch2_rep_out1_level",{1271,875,106,106},false);
  addKnob("ch1_src",{194,886,90,90},true);addKnob("ch2_src",{893,886,90,90},true);
  addCombo("machine_version",{161,753,194,36}, {"7.5 / 3.75 ips","15 / 7.5 ips","30 / 15 ips"});
  juce::StringArray tapeNames;
  auto* tape=dynamic_cast<juce::AudioParameterChoice*>(proc.state.getParameter("tape_type"));
  for(auto t:tape->choices)tapeNames.add(t.replace("SCOTCH ",""));
  addCombo("tape_type",{909,753,174,36},tapeNames);
  addHit("slow",{443,754,82,36},[this]{setParameter(proc.state,"speed_select",0);});
  addHit("fast",{525,754,82,36},[this]{setParameter(proc.state,"speed_select",1);});
  addHit("NAB",{672,753,80,36},[this]{setParameter(proc.state,"eq_standard",0);});
  addHit("CCIR",{753,753,80,36},[this]{setParameter(proc.state,"eq_standard",1);});
  for(auto spec: {std::pair<const char*,juce::Rectangle<int>>{"calibrate",{1175,735,62,43}},{"mastersafe",{1302,735,62,43}}}){
   auto* b=addHit(spec.first,spec.second,[]{});b->setClickingTogglesState(true);
   buttonLinks.push_back(std::make_unique<ButtonAttachment>(proc.state,spec.first,*b));
  }
  addHit("ZERO LOC",{704,320,38,38},[this]{proc.zeroReference.store(proc.hostSeconds.load());});
  addHit("MEMO",{763,320,38,38},[]{})->setTooltip("Detalhe visual da referência; não reposiciona a DAW.");
  for(auto entry:{std::pair<const char*,juce::Rectangle<int>>{"STOP",{649,650,60,46}},{"PLAY",{740,650,61,46}},{"REC",{829,650,60,46}}}){
   const juce::String name=entry.first;
   auto* b=addHit(name,entry.second,[this,name]{if(name!="REC")requestTransport(name=="PLAY");});
   b->setTooltip(name=="REC"?"Indicador de gravação da DAW.":"Controla PLAY/STOP pela tecla Espaço da janela da DAW no Windows. Requer atalho padrão; configuração em SETTINGS.");
  }
  setupService();setupSettings();setupPresets();showTab(0);startTimerHz(30);
 }
 ~Canvas()override{
  stopTimer();presetList.setModel(nullptr);setLookAndFeel(nullptr);
 }
 juce::Slider* addKnob(const char* id,juce::Rectangle<int> r,bool selector){
  auto* s=knobs.add(new juce::Slider);s->setComponentID(id);s->getProperties().set("selector",selector);
  s->setSliderStyle(juce::Slider::RotaryVerticalDrag);s->setTextBoxStyle(juce::Slider::NoTextBox,false,0,0);
  s->setMouseDragSensitivity(selector?100:180);s->setScrollWheelEnabled(true);
  auto* param=proc.state.getParameter(id);s->setDoubleClickReturnValue(true,param->convertFrom0to1(param->getDefaultValue()));
  addAndMakeVisible(s);s->setBounds(r);machineControls.push_back(s);
  sliderLinks.push_back(std::make_unique<SliderAttachment>(proc.state,id,*s));return s;
 }
 void addCombo(const char* id,juce::Rectangle<int> r,juce::StringArray items){
  auto* c=combos.add(new juce::ComboBox);c->setTooltip(proc.state.getParameter(id)->getName(128)+": configuração armazenada; DSP de fita pendente.");for(int i=0;i<items.size();++i)c->addItem(items[i],i+1);
  c->setComponentID(id);addAndMakeVisible(c);c->setBounds(r);machineControls.push_back(c);
  comboLinks.push_back(std::make_unique<ComboAttachment>(proc.state,id,*c));
 }
 Hotspot* addHit(const juce::String& name,juce::Rectangle<int> r,std::function<void()> action){
  auto* b=hits.add(new Hotspot(name));b->setComponentID(name);b->onClick=std::move(action);addAndMakeVisible(b);b->setBounds(r);machineControls.push_back(b);return b;
 }
 void setupService(){
  addAndMakeVisible(serviceView);serviceView.setBounds(112,181,1310,783);serviceView.setViewedComponent(&serviceRows,false);serviceView.setScrollBarsShown(true,false);
  int y=0;
  for(auto* parameter:proc.getParameters())if(auto* p=dynamic_cast<juce::RangedAudioParameter*>(parameter)){
   const auto& id=p->paramID;
   if(id=="stereo_link"||id=="transport_modelling"||id=="alignment_dbfs"||id.startsWith("varispeed"))continue;
   auto* row=rows.add(new ParameterRow(proc.state,id));serviceRows.addAndMakeVisible(row);row->setBounds(0,y,1270,52);y+=54;
  }
  serviceRows.setSize(1270,y);
 }
 void setupSettings(){
  settingsInfo.setText("VERSÃO VISUAL 0.2.1\n\nPLAY e STOP enviam a tecla Espaço à janela da DAW no Windows (Studio One / REAPER). Mantenha Espaço configurado como reproduzir/parar. As luzes continuam seguindo o estado real do host. REC é apenas indicador.\n\nO processamento de fita, EQ, bias e varispeed ainda está pendente. Os 90 presets são alinhamentos, não 90 timbres distintos.\n\nCALIBRATE seleciona a medição de saída; não gera sinal. Os VUs são indicadores RMS aproximados. RESET define a referência do contador sem mover o host.\n\nDuplo clique nos knobs restaura o padrão; Shift permite ajuste fino. Os ajustes sem DSP permanecem salvos no projeto.",juce::dontSendNotification);
  settingsInfo.setFont(21.f);settingsInfo.setJustificationType(juce::Justification::topLeft);addAndMakeVisible(settingsInfo);settingsInfo.setBounds(125,190,1260,395);
  shortcutToggle.setComponentID("transport_shortcut_enabled");shortcutToggle.setButtonText("PLAY / STOP via tecla Espaço da DAW (Windows)");
  shortcutToggle.setToggleState(proc.transportShortcutEnabled.load(),juce::dontSendNotification);
  shortcutToggle.onClick=[this]{proc.transportShortcutEnabled.store(shortcutToggle.getToggleState());transportRequest.cancel();};
  addAndMakeVisible(shortcutToggle);shortcutToggle.setBounds(125,605,1260,36);
  transportStatus.setText("Use o atalho padrão de espaço da DAW. Comandos são enviados apenas à janela do host.",juce::dontSendNotification);
  transportStatus.setFont(17.f);addAndMakeVisible(transportStatus);transportStatus.setBounds(125,648,1260,36);
  addAndMakeVisible(settingsView);settingsView.setBounds(112,700,1310,245);settingsView.setViewedComponent(&settingsRows,false);settingsView.setScrollBarsShown(true,false);
  int y=0;for(const auto* id:{"stereo_link","alignment_dbfs","transport_modelling","varispeed_on","varispeed_semitones"}){
   auto* row=rows.add(new ParameterRow(proc.state,id));settingsRows.addAndMakeVisible(row);row->setBounds(0,y,1270,52);y+=54;
  }settingsRows.setSize(1270,y);
 }
 void setupPresets(){
  search.setComponentID("presetSearch");presetList.setComponentID("presetList");load.setComponentID("loadPreset");
  addAndMakeVisible(search);search.setBounds(125,183,1260,48);search.setTextToShowWhenEmpty("Buscar alinhamento, fita, velocidade ou EQ…",juce::Colour(0xff93938d));search.setFont(21.f);
  search.onTextChange=[this]{filterPresets();};
  addAndMakeVisible(presetList);presetList.setBounds(125,250,1260,620);presetList.setRowHeight(44);presetList.setColour(juce::ListBox::backgroundColourId,juce::Colour(0xff14191b));
  addAndMakeVisible(load);addAndMakeVisible(previous);addAndMakeVisible(next);
  previous.setBounds(125,895,190,48);load.setBounds(1025,895,160,48);next.setBounds(1200,895,185,48);
  load.onClick=[this]{loadPreset(presetList.getSelectedRow());};
  previous.onClick=[this]{selectProgram((proc.getCurrentProgram()+89)%90);};next.onClick=[this]{selectProgram((proc.getCurrentProgram()+1)%90);};
  filterPresets();
 }
 void filterPresets(){
  filtered.clear();for(int i=0;i<proc.getNumPrograms();++i)if(proc.getProgramName(i).containsIgnoreCase(search.getText()))filtered.push_back(i);
  presetList.updateContent();presetList.repaint();
 }
 void selectProgram(int i){proc.setCurrentProgram(i);for(size_t row=0;row<filtered.size();++row)if(filtered[row]==i){presetList.selectRow(int(row));presetList.scrollToEnsureRowIsOnscreen(int(row));}repaint();}
 void loadPreset(int row){if(row>=0&&row<int(filtered.size()))selectProgram(filtered[size_t(row)]);}
 int getNumRows()override{return int(filtered.size());}
 void paintListBoxItem(int row,juce::Graphics& g,int w,int h,bool selected)override{
  if(row<0||row>=int(filtered.size()))return;
  if(selected)g.fillAll(juce::Colour(0xff4e4531));
  g.setColour(juce::Colour(0xff3b4041));g.drawHorizontalLine(h-1,0,float(w));
  g.setFont(20.f);g.setColour(juce::Colour(0xffe6e3d7));g.drawText(proc.getProgramName(filtered[size_t(row)]),16,0,w-30,h,juce::Justification::centredLeft);
 }
 void listBoxItemDoubleClicked(int row,const juce::MouseEvent&)override{loadPreset(row);}
 void returnKeyPressed(int row)override{loadPreset(row);}
 void showTab(int page){
  tab=page;machine.setToggleState(tab==0,juce::dontSendNotification);service.setToggleState(tab==1,juce::dontSendNotification);presets.setToggleState(tab==2,juce::dontSendNotification);settings.setToggleState(tab==3,juce::dontSendNotification);
  for(auto* c:machineControls)c->setVisible(tab==0);
  serviceView.setVisible(tab==1);settingsView.setVisible(tab==3);settingsInfo.setVisible(tab==3);shortcutToggle.setVisible(tab==3);transportStatus.setVisible(tab==3);
  for(juce::Component* c:std::array<juce::Component*,5>{{&search,&presetList,&load,&previous,&next}})c->setVisible(tab==2);
  repaint();
 }
 void requestTransport(bool play){
  a800::ShortcutResult dispatchResult=a800::ShortcutResult::disabled;
  auto result=transportRequest.request(play,proc.hostPlaying.load(),proc.transportRevision.load(),juce::Time::getMillisecondCounterHiRes(),[&]{
   if(!proc.transportShortcutEnabled.load())return false;
   auto* peer=getPeer();dispatchResult=a800::sendDawSpaceShortcut(peer?peer->getNativeHandle():nullptr);
   return dispatchResult==a800::ShortcutResult::queued;
  });
  juce::String text;
  if(result==a800::TransportRequest::Result::queued)text=play?"PLAY solicitado à DAW pela tecla Espaço.":"STOP solicitado à DAW pela tecla Espaço.";
  else if(result==a800::TransportRequest::Result::alreadyInState)text=play?"PLAY já ativo ou solicitado.":"STOP já ativo ou solicitado.";
  else if(dispatchResult==a800::ShortcutResult::disabled)text="Ative PLAY / STOP via tecla Espaço em SETTINGS.";
  else if(dispatchResult==a800::ShortcutResult::unsupportedHost)text="Transporte por atalho disponível no Studio One / REAPER para Windows.";
  else text="Não foi possível enviar o comando à janela da DAW. Confira a janela ativa e as configurações.";
  transportStatus.setText(text,juce::dontSendNotification);
  if(auto* b=dynamic_cast<Hotspot*>(findChildWithID(play?"PLAY":"STOP")))b->setTooltip(text+" Abra SETTINGS para configurar.");
 }
 void timerCallback()override{
  transportRequest.observe(proc.hostPlaying.load(),proc.transportRevision.load(),juce::Time::getMillisecondCounterHiRes());
  if(transportRequest.takeExpiredConfirmation())transportStatus.setText("Comando enviado sem confirmação do host. Confira se Espaço é o atalho reproduzir/parar da DAW.",juce::dontSendNotification);
  if(tab==3)shortcutToggle.setToggleState(proc.transportShortcutEnabled.load(),juce::dontSendNotification);
  if(proc.hostPlaying.load())reelAngle+=.035f;
  for(int ch=0;ch<2;++ch){
   auto alignment=value(proc.state,"alignment_dbfs");
   auto target=juce::Decibels::gainToDecibels(proc.meterRms[size_t(ch)].load(),-80.f)-alignment;
   vu[size_t(ch)]+=.18f*(juce::jlimit(-23.f,5.f,target)-vu[size_t(ch)]);
  }
  for(auto* k:knobs)if(auto* p=proc.state.getParameter(k->getComponentID()))k->setTooltip(p->getName(64)+": "+p->getCurrentValueAsText());
  if(tab==0){
   repaint(330,88,1000,115);repaint(695,250,150,40);
   repaint(440,730,940,65);repaint(645,649,250,50);
   if(proc.hostPlaying.load())repaint(140,183,1248,395);
  }
 }
 void drawReel(juce::Graphics& g,float x,float y,float radius){
  if(reelAngle==0)return;juce::Graphics::ScopedSaveState save(g);
  juce::Path circle;circle.addEllipse(x-radius,y-radius,radius*2,radius*2);g.reduceClipRegion(circle);
  g.addTransform(juce::AffineTransform::rotation(reelAngle,x,y));g.drawImageAt(artwork,0,0);
 }
 void drawVu(juce::Graphics& g,int ch){
  const float x=ch==0?338.f:885.f,y=95.f,w=309.f,h=83.f;
  juce::Graphics::ScopedSaveState meterClip(g);g.reduceClipRegion(juce::Rectangle<int>(int(x),int(y),int(w),int(h)));
  g.setGradientFill({juce::Colour(0xffffd781),x,y,juce::Colour(0xfff3cd7c),x,y+h,false});g.fillRect(x,y,w,h);
  auto pivot=juce::Point<float>(x+w*.5f,y+174.f);
  const std::array<float,10> levels{{-20,-10,-7,-5,-3,-1,0,1,3,5}};
  const std::array<float,10> positions{{.09f,.19f,.29f,.38f,.47f,.56f,.63f,.71f,.81f,.90f}};
  const std::array<const char*,10> labels{{"-20","-10","-7","-5","-3","-1","0","+1","+3","+5"}};
  auto point=[&](float t){return juce::Point<float>(x+w*t,y+35.f+100.f*(t-.5f)*(t-.5f));};
  juce::Path arc;for(int i=0;i<=100;++i){auto pt=point(.055f+i*.009f);if(i==0)arc.startNewSubPath(pt);else arc.lineTo(pt);}
  g.setColour(juce::Colour(0xff30271c));g.strokePath(arc,juce::PathStrokeType(1.3f));
  for(size_t i=0;i<levels.size();++i){auto pt=point(positions[i]);g.setColour(levels[i]>0?juce::Colour(0xffb62422):juce::Colour(0xff28251b));g.drawLine(pt.x,pt.y,pt.x,pt.y-8,1.2f);g.setFont(13.f);g.drawText(labels[i],int(pt.x)-16,int(pt.y)-28,32,18,juce::Justification::centred);}
  g.setColour(juce::Colour(0xff3c2a1e));g.setFont(22.f);g.drawText("VU",int(x),int(y+48),int(w),28,juce::Justification::centred);
  float t=positions.front();const float db=juce::jlimit(-20.f,5.f,vu[size_t(ch)]);
  for(size_t i=1;i<levels.size();++i)if(db>=levels[i-1]){const float fraction=juce::jlimit(0.f,1.f,(db-levels[i-1])/(levels[i]-levels[i-1]));t=positions[i-1]+fraction*(positions[i]-positions[i-1]);}
  auto end=point(t).translated(0,-12.f);
  g.setColour(juce::Colours::black.withAlpha(.22f));g.drawLine({pivot.translated(2,1),end.translated(2,1)},3);
  g.setColour(juce::Colour(0xff1d1812));g.drawLine({pivot,end},2.f);
  g.setGradientFill({juce::Colours::white.withAlpha(.15f),x,y,juce::Colours::white.withAlpha(0.f),x,y+h*.6f,false});g.fillRect(x,y,w,h);
 }
 void segmentDigit(juce::Graphics& g,int digit,float x,float y){
  constexpr int bits[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
  const std::array<juce::Rectangle<float>,7> bars{{{x+3,y,16,3},{x+19,y+3,3,12},{x+17,y+18,3,12},{x,y+30,17,3},{x-2,y+18,3,12},{x,y+3,3,12},{x+1,y+15,17,3}}};
  for(int i=0;i<7;++i){g.setColour((bits[digit]&(1<<i))?juce::Colour(0xffff2143):juce::Colour(0xff2c1016));g.fillRoundedRectangle(bars[size_t(i)],1);}
 }
 void drawCounter(juce::Graphics& g){
  g.setColour(juce::Colour(0xff08090a));g.fillRect(697,251,147,38);
  const auto delta=proc.hostSeconds.load()-proc.zeroReference.load();
  if(delta<0){g.setColour(juce::Colour(0xffff2143));g.fillRect(699,269,7,3);}
  auto time=std::abs(delta);int ticks=int(std::fmod(time*10.,100000.));
  for(int i=4;i>=0;--i){segmentDigit(g,ticks%10,711.f+i*25.f,254.f);ticks/=10;}
  g.setColour(juce::Colour(0xffff2143));g.fillEllipse(808.f,283.f,3.f,3.f);
 }
 void lamp(juce::Graphics& g,juce::Rectangle<float> r,bool on,bool red=false){
  auto c=red?(on?juce::Colour(0xfff92a23):juce::Colour(0xff85221e)):(on?juce::Colour(0xffffd28b):juce::Colour(0xffa4a296));
  g.setColour(juce::Colour(0xff131515));g.fillRect(r.expanded(4));
  g.setGradientFill({c.brighter(.2f),r.getX(),r.getY(),c.darker(.2f),r.getRight(),r.getBottom(),false});g.fillRect(r);
  g.setColour(juce::Colour(0xff444642));g.drawRect(r,1);
 }
 void paint(juce::Graphics& g)override{
  g.drawImageAt(artwork,0,0);
  if(tab!=0){
   auto r=juce::Rectangle<float>(93,84,1352,913);
   g.setGradientFill({juce::Colour(0xff2a3033),93,84,juce::Colour(0xff151a1c),1445,997,false});g.fillRoundedRectangle(r,3);
   g.setColour(juce::Colour(0xff747775));g.drawRoundedRectangle(r.reduced(1),3,1);
   g.setColour(juce::Colour(0xffe6e1d3));g.setFont(30.f);
   g.drawText(tab==1?"SERVICE / AUDIO ALIGNMENT":tab==2?"PRESETS / FACTORY ALIGNMENTS":"SETTINGS / CONFIGURAÇÃO",125,110,1200,45,juce::Justification::centredLeft);
   g.setColour(juce::Colour(0xffb99b64));g.fillRect(125,163,1260,1);
   return;
  }
  drawReel(g,351,378,188);drawReel(g,1173,379,188);drawVu(g,0);drawVu(g,1);drawCounter(g);
  const bool fast=value(proc.state,"speed_select")>.5f,nab=value(proc.state,"eq_standard")<.5f;
  for(int i=0;i<2;++i){auto r=juce::Rectangle<float>(i==0?449.f:531.f,759,69,23);lamp(g,r,i==int(fast));g.setColour(juce::Colours::black);g.setFont(13.f);g.drawText(i==0?"SLOW":"FAST",r.toNearestInt(),juce::Justification::centred);}
  for(int i=0;i<2;++i){auto r=juce::Rectangle<float>(i==0?678.f:759.f,759,68,23);lamp(g,r,i==int(!nab));g.setColour(juce::Colours::black);g.setFont(13.f);g.drawText(i==0?"NAB":"CCIR",r.toNearestInt(),juce::Justification::centred);}
  lamp(g,{1182,742,49,28},value(proc.state,"calibrate")>.5f);lamp(g,{1309,742,48,28},value(proc.state,"mastersafe")>.5f);
  const bool playing=proc.hostPlaying.load(),recording=proc.hostRecording.load();
  lamp(g,{657,656,45,31},!playing);lamp(g,{748,656,45,31},playing);lamp(g,{836,656,45,31},recording,true);
  g.setColour(juce::Colour(0xff111416));g.fillRect(670,664,16,15);juce::Path play;play.addTriangle(762,661,762,682,782,672);g.fillPath(play);g.fillEllipse(851,665,14,14);
 }
};
A800Editor::A800Editor(A800Processor& p):AudioProcessorEditor(p),canvas(std::make_unique<Canvas>(p)){
 addAndMakeVisible(*canvas);canvas->setSize(designW,designH);
 setResizable(true,false);getConstrainer()->setFixedAspectRatio(1.5);setResizeLimits(768,512,3072,2048);setSize(1152,768);
}
A800Editor::~A800Editor()=default;
void A800Editor::resized(){
 if(canvas){canvas->setTransform(juce::AffineTransform::scale(float(getWidth())/designW,float(getHeight())/designH));canvas->setTopLeftPosition(0,0);}
}
