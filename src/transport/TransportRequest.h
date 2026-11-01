#pragma once
#include <cstdint>
namespace a800 {
// UI-thread command policy. Pending requests prevent a stale playhead snapshot
// from turning two PLAY clicks into play-then-stop when using a toggle shortcut.
class TransportRequest {
public:
 enum class Result { queued, alreadyInState, unavailable };
 template<class Dispatch>
 Result request(bool play,bool observedPlaying,std::uint64_t revision,double nowMs,Dispatch&& dispatch) {
  observe(observedPlaying,revision,nowMs);
  if((pending?desired:observedPlaying)==play)return Result::alreadyInState;
  if(!dispatch())return Result::unavailable;
  pending=true;desired=play;baseRevision=revision;sentAt=nowMs;confirmationExpired=false;
  return Result::queued;
 }
 void observe(bool playing,std::uint64_t revision,double nowMs) noexcept {
  if(pending && revision!=baseRevision && playing==desired)pending=false;
  if(pending && nowMs-sentAt>2000.){pending=false;confirmationExpired=true;}
 }
 bool takeExpiredConfirmation() noexcept {bool result=confirmationExpired;confirmationExpired=false;return result;}
 void cancel() noexcept {pending=false;confirmationExpired=false;}
private:
 bool pending=false,desired=false,confirmationExpired=false;
 std::uint64_t baseRevision=0;
 double sentAt=0;
};
}
