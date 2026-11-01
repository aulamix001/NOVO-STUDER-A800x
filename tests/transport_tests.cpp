#include "../src/transport/TransportRequest.h"
#include <iostream>
#include <stdexcept>
void check(bool b){if(!b)throw std::runtime_error("transport request failed");}
int main(){
 using Request=a800::TransportRequest;using Result=Request::Result;
 Request r;int commands=0;auto dispatch=[&]{++commands;return true;};
 check(r.request(false,false,0,0,dispatch)==Result::alreadyInState);
 check(r.request(true,false,0,10,dispatch)==Result::queued);
 check(r.request(true,false,0,20,dispatch)==Result::alreadyInState);check(commands==1);
 check(r.request(false,false,0,30,dispatch)==Result::queued);check(commands==2);
 r.observe(false,2,40);
 check(r.request(false,false,2,50,dispatch)==Result::alreadyInState);check(commands==2);
 check(r.request(true,false,2,60,dispatch)==Result::queued);
 r.observe(true,3,70);
 check(r.request(true,true,3,80,dispatch)==Result::alreadyInState);
 check(r.request(false,true,3,90,[]{return false;})==Result::unavailable);
 check(r.request(false,true,3,100,dispatch)==Result::queued);
 r.observe(true,3,2201);check(r.takeExpiredConfirmation());check(!r.takeExpiredConfirmation());
 r.cancel();check(r.request(false,false,4,3000,dispatch)==Result::alreadyInState);
 std::cout<<"PASS: PLAY/STOP requests, idempotent clicks, delayed host acknowledgement, unavailable route, confirmation timeout\n";
}
