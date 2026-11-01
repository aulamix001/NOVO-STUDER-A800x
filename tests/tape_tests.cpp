#include "../src/dsp/TapeModel.h"
#include <iostream>
#include <stdexcept>
#include <vector>
void require(bool b,const char* message){if(!b)throw std::runtime_error(message);}
std::vector<double> render(a800::TapeSettings s,double sr=48000,double amplitude=.25,double hz=1000){
 a800::TapeModel tape;tape.prepare(sr,0);tape.configure(s,true);
 std::vector<double> result(12000);
 for(size_t i=0;i<result.size();++i){auto y=tape.tick(amplitude*std::sin(6.283185307179586*hz*double(i)/sr));require(std::isfinite(y),"non-finite output");result[i]=y;}
 return result;
}
double rms(const std::vector<double>& x){double s=0;for(size_t i=4000;i<x.size();++i)s+=x[i]*x[i];return std::sqrt(s/double(x.size()-4000));}
double diff(const std::vector<double>& a,const std::vector<double>& b){double s=0;for(size_t i=4000;i<a.size();++i)s+=std::abs(a[i]-b[i]);return s/double(a.size()-4000);}
int main(){
 a800::TapeSettings s;s.noiseDb=-100;
 for(double sr:{44100.,48000.,96000.,192000.}){
  auto low=render(s,sr,.01),hot=render(s,sr,1.);
  require(rms(hot)<rms(low)*80,"tape must compress hotter signals");
  auto extreme=s;extreme.driveDb=12;extreme.bias=-1.5;extreme.inputDb=13;extreme.oscStop=true;
  require(rms(render(extreme,sr,4))<4,"extreme settings must remain bounded");
 }
 const auto base=render(s);
 for(int type=0;type<15;++type){auto a=s;a.tape=type;require(diff(render(a),base)>(type==3?-1:1e-5),"tape profile is inactive");}
 auto check=[&](a800::TapeSettings a,const char* name,double hz=1000){require(diff(render(a,48000,.25,hz),render(s,48000,.25,hz))>1e-6,name);};
 auto a=s;a.ips=7.5;check(a,"speed inactive",9000);
 a=s;a.ccir=true;check(a,"EQ standard inactive",9000);
 a=s;a.bias=1;check(a,"bias inactive",9000);
 a=s;a.recordTreble=3;check(a,"REC treble inactive",9000);
 a=s;a.treble=3;check(a,"repro treble inactive",9000);
 a=s;a.bass=3;check(a,"repro bass inactive",70);
 a=s;a.sync=true;a.narrow=true;check(a,"SYNC range inactive",9000);
 a=s;a.erase=-3;check(a,"erase residual inactive");
 a=s;a.oscStop=true;check(a,"bias oscillator inactive");
 a=s;a.external=true;check(a,"external repro route inactive");
 a=s;a.transport=true;a.wow=.6;a.flutter=.4;check(a,"mechanical model inactive");
 a=s;a.noiseDb=-60;require(rms(render(a,48000,0))>1e-5,"hiss inactive");
 a=s;a.mix=0;auto dry=render(a);for(size_t i=0;i<dry.size();++i)require(std::abs(dry[i]-.25*std::sin(6.283185307179586*1000*double(i)/48000))<1e-12,"dry mix changed original");
 a800::TapeModel continuous,chunked;continuous.prepare(48000,0);chunked.prepare(48000,0);continuous.configure(s,true);chunked.configure(s,true);
 for(int i=0;i<12000;++i){if(i%127==0)chunked.configure(s);const auto x=.3*std::sin(i*.17);require(std::abs(continuous.tick(x)-chunked.tick(x))<1e-12,"block-dependent DSP");}
 std::cout<<"PASS: nonlinear compression, 15 profiles, four rates, all tape stages, dry mix, silence/hiss, block independence\n";
}
