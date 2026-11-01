#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include "../src/transport/WindowsTransport.h"
#include "../src/transport/TransportRequest.h"
// Isolated test process deliberately named Studio One.exe. This is not Studio One
// and does NOT validate that product's shortcut mapping. It verifies Win32 delivery.
static bool playing=false;static int toggles=0;
LRESULT CALLBACK windowProc(HWND h,UINT m,WPARAM w,LPARAM l){
 if(m==WM_KEYDOWN&&w==VK_SPACE){playing=!playing;++toggles;return 0;}
 return DefWindowProcW(h,m,w,l);
}
void pump(){MSG m;for(int i=0;i<50;++i){while(PeekMessageW(&m,nullptr,0,0,PM_REMOVE)){TranslateMessage(&m);DispatchMessageW(&m);}Sleep(2);}}
int main(){
 WNDCLASSW c{};c.hInstance=GetModuleHandleW(nullptr);c.lpfnWndProc=windowProc;c.lpszClassName=L"A800TransportTestHost";
 if(!RegisterClassW(&c))return 1;
 HWND window=CreateWindowW(c.lpszClassName,L"A800 transport bridge test - Studio One",WS_OVERLAPPEDWINDOW,0,0,600,400,nullptr,nullptr,c.hInstance,nullptr);
 if(!window)return 2;
 ShowWindow(window,SW_SHOW);SetForegroundWindow(window);SetFocus(window);pump();
 a800::TransportRequest request;
 auto send=[&]{return a800::sendDawSpaceShortcut(window)==a800::ShortcutResult::queued;};
 if(request.request(true,playing,0,0,send)!=a800::TransportRequest::Result::queued)return 3;
 pump();if(!playing||toggles!=1)return 4;
 request.observe(playing,1,100);
 request.request(true,playing,1,110,send);pump();if(toggles!=1)return 5;
 if(request.request(false,playing,1,120,send)!=a800::TransportRequest::Result::queued)return 6;
 pump();if(playing||toggles!=2)return 7;
 DestroyWindow(window);
 std::puts("PASS: targeted Windows Space delivery, PLAY then STOP, repeated PLAY does not toggle twice (simulated host only)");
}
