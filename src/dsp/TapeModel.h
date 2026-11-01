#pragma once
#include <array>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace a800 {
// Engineering approximations, not measured transfer functions of named tapes.
struct TapeProfile { double headroom, hf, bump, memory, asymmetry; };
inline constexpr std::array<TapeProfile,15> tapeProfiles{{
 {1.00, .95, 1.8,.035,.012}, {.82,.86,2.2,.050,.018}, {.94,.92,1.9,.040,.014},
 {1.16,1.04,1.4,.027,.008}, {1.21,1.07,1.3,.025,.007}, {1.12,1.02,1.5,.030,.010},
 {.91,.90,2.0,.044,.016}, {.85,.87,2.1,.048,.017}, {1.10,1.00,1.6,.032,.010},
 {.80,.83,2.4,.055,.020}, {.98,.95,1.9,.038,.013}, {.93,.91,2.0,.042,.015},
 {1.04,.97,1.7,.034,.012}, {1.14,1.03,1.5,.029,.009}, {1.07,.99,1.8,.033,.011}
}};
inline double dbGain(double db) { return std::pow(10., db/20.); }
struct Biquad {
 double b0=1,b1=0,b2=0,a1=0,a2=0,z1=0,z2=0;
 void reset(){z1=z2=0;}
 double tick(double x){const auto y=b0*x+z1;z1=b1*x-a1*y+z2;z2=b2*x-a2*y;return y;}
 // RBJ low/high shelves, peak and lowpass; coefficients updated without allocation.
 void set(int type,double sr,double hz,double db=0,double q=.70710678){
  hz=std::clamp(hz,10.,sr*.44); const double w=6.283185307179586*hz/sr,c=std::cos(w),s=std::sin(w);
  const double A=std::pow(10.,db/40.),alpha=s/(2*q),beta=2*std::sqrt(A)*alpha;
  double n0,n1,n2,d0,d1,d2;
  if(type==0){n0=(1-c)*.5;n1=1-c;n2=n0;d0=1+alpha;d1=-2*c;d2=1-alpha;}
  else if(type==1){n0=1+alpha*A;n1=-2*c;n2=1-alpha*A;d0=1+alpha/A;d1=-2*c;d2=1-alpha/A;}
  else if(type==2){n0=A*((A+1)-(A-1)*c+beta);n1=2*A*((A-1)-(A+1)*c);n2=A*((A+1)-(A-1)*c-beta);d0=(A+1)+(A-1)*c+beta;d1=-2*((A-1)+(A+1)*c);d2=(A+1)+(A-1)*c-beta;}
  else {n0=A*((A+1)+(A-1)*c+beta);n1=-2*A*((A-1)+(A+1)*c);n2=A*((A+1)+(A-1)*c-beta);d0=(A+1)-(A-1)*c+beta;d1=2*((A-1)-(A+1)*c);d2=(A+1)-(A-1)*c-beta;}
  b0=n0/d0;b1=n1/d0;b2=n2/d0;a1=d1/d0;a2=d2/d0;
 }
};
struct TapeSettings {
 double ips=15, inputDb=0,outputDb=0,recordTreble=0,recordFrequency=4500,treble=0,bass=0,bias=0,erase=0;
 double alignment=-18,driveDb=0,mix=1,noiseDb=-80,wow=0,flutter=0;
 int tape=3; bool ccir=false,sync=false,narrow=false,oscStop=false,transport=false,external=false;
};
class TapeModel {
 double sr=48000,previous=0,mag=0,dcIn=0,dcOut=0,env=0,phase=0,flutterPhase=0;
 double inGain=1,outGain=1,drive=3,asym=.01,memory=.03,noise=0,mix=1,wow=0,flutter=0;
 double inputTarget=1,outputTarget=1,driveTarget=3,mixTarget=1,smoothing=.001;
 double magnetPole=.9,dcPole=.999,envPole=.999; bool transport=false,external=false;
 std::array<Biquad,2> up,down;
 Biquad recordEq,recordEmphasis,playbackDeemphasis,bump,lowEq,highEq,loss;
 std::vector<double> delay; size_t write=0; uint32_t rng=0x61753831;
 double residual=0;
public:
 void prepare(double rate,int channel){
  sr=rate; previous=mag=dcIn=dcOut=env=phase=flutterPhase=0;write=0;
  delay.assign(size_t(sr*.03)+16,0.); rng=0x61753831u+uint32_t(channel)*8971u;
  for(auto* f:{&recordEq,&recordEmphasis,&playbackDeemphasis,&bump,&lowEq,&highEq,&loss})f->reset();
  for(auto& f:up){f.reset();f.set(0,sr*4,sr*.42);}
  for(auto& f:down){f.reset();f.set(0,sr*4,sr*.42);}
  smoothing=1-std::exp(-1/(sr*.02));dcPole=std::exp(-6.28318530718*8/sr);
  envPole=std::exp(-1/(sr*.01));magnetPole=std::exp(-6.28318530718*350/(sr*4));
 }
 void configure(const TapeSettings& s,bool immediate=false){
  const auto& p=tapeProfiles[size_t(std::clamp(s.tape,0,14))];
  inputTarget=dbGain(s.inputDb);outputTarget=dbGain(s.outputDb);mixTarget=std::clamp(s.mix,0.,1.);
  driveTarget=dbGain(-s.alignment-8+s.driveDb)/p.headroom*dbGain(-s.bias*.45);
  if(s.oscStop)driveTarget*=2.5;
  if(immediate){inGain=inputTarget;outGain=outputTarget;drive=driveTarget;mix=mixTarget;}
  asym=p.asymmetry+(s.oscStop?.15:0);memory=p.memory;external=s.external;
  const auto speed=std::clamp(s.ips,2.,60.);
  // Finite shelves approximate complementary record/playback emphasis. The
  // nonlinear tape sits between them, so HF saturation follows the EQ standard.
  const double timeConstant=s.ccir?(speed>=29?17.5e-6:(speed>=14?35e-6:70e-6)):(speed>=29?25e-6:50e-6);
  const double turnover=1/(6.283185307179586*timeConstant);
  recordEmphasis.set(3,sr,turnover,6);
  playbackDeemphasis.set(3,sr,turnover,-6);
  const double hf=std::clamp(15500.*std::pow(speed/15.,.42)*p.hf*dbGain(-s.bias*1.5),3500.,sr*.43);
  recordEq.set(3,sr,s.recordFrequency,s.recordTreble+(s.ccir?1.0:0)-s.bias*1.2);
  bump.set(1,sr,45*std::sqrt(speed/7.5),p.bump*(s.sync?.65:1.),.8);
  lowEq.set(2,sr,110,s.bass+(s.ccir?-.6:.3));
  highEq.set(3,sr,5500,s.treble-(s.sync?1.5:0));
  loss.set(0,sr,hf*(s.sync?(s.narrow?.55:.8):1.));
  noise=dbGain(s.noiseDb)*(s.oscStop?1.5:1.);if(s.noiseDb<=-100)noise=0;
  residual=std::min(.01,.0002*dbGain(-s.erase)*(s.oscStop?8:1));
  wow=s.wow;flutter=s.flutter;transport=s.transport;
 }
 double tick(double x){
  if(!std::isfinite(x))x=0;
  inGain+=smoothing*(inputTarget-inGain);outGain+=smoothing*(outputTarget-outGain);
  drive+=smoothing*(driveTarget-drive);mix+=smoothing*(mixTarget-mix);
  double y=recordEq.tick(x*inGain);
  if(!external){
   y=recordEmphasis.tick(y);
   const double pre=y;
   for(int k=1;k<=4;++k){
    double u=previous+(pre-previous)*(double(k)*.25);
    for(auto& f:up)u=f.tick(u);
    mag=magnetPole*mag+(1-magnetPole)*u;
    const double h=u+memory*(u-mag);
    u=(std::tanh(drive*h+asym)-std::tanh(asym))/(drive*(1-std::tanh(asym)*std::tanh(asym)));
    for(auto& f:down)u=f.tick(u);
    y=u;
   }
   previous=pre;
   env=envPole*env+(1-envPole)*std::abs(y);
   // Level-dependent HF self-erasure, plus stationary tape hiss.
   y/=1+.025*env*drive;
   rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;
   y+=(double(rng)/4294967295.*2-1)*noise;
  }
  y=loss.tick(highEq.tick(lowEq.tick(bump.tick(playbackDeemphasis.tick(y)))));
  // Fractional delay only when mechanical modelling is enabled (no transport gating).
  delay[write]=y;
  const double base=sr*.006;
  phase+=6.28318530718*.57/sr;flutterPhase+=6.28318530718*8.7/sr;
  if(phase>6.28318530718)phase-=6.28318530718;
  if(flutterPhase>6.28318530718)flutterPhase-=6.28318530718;
  auto read=[&](double samples){double pos=double(write)-samples;while(pos<0)pos+=double(delay.size());auto i=size_t(pos);double f=pos-double(i);return delay[i]*(1-f)+delay[(i+1)%delay.size()]*f;};
  if(transport)y=read(base+sr*(.0015*wow*std::sin(phase)+.00008*flutter*std::sin(flutterPhase)));
  y+=residual*read(sr*.02);write=(write+1)%delay.size();
  const double dc=y-dcIn+dcPole*dcOut;dcIn=y;dcOut=dc;
  return x+(dc*outGain-x)*mix;
 }
};
}
