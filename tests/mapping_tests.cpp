#include "../src/dsp/TapeConstants.h"
#include <iostream>
#include <stdexcept>
void check(bool result) {if(!result) throw std::runtime_error("mapping check failed");}
int main() {
 check(a800::fluxFromVu(0)==320);
 check(std::abs(a800::fluxFromVu(20*std::log10(2.0))-640)<1e-9);
 check(std::abs(a800::fluxFromVu(6)-638.48394079)<1e-6);
 for(int j: {0,4,6,8}) check(a800::calibrateVu(j)==-3-j);
 for(int i=0;i<3;++i) check(a800::nominalSpeedIps(i,true)==2*a800::nominalSpeedIps(i,false));
 check(a800::nominalSpeedIps(1,true)==15);
 check(a800::nominalSpeedIps(-1,false)==0);
 std::cout<<"PASS: flux, calibration and speed pairs\n";
}
