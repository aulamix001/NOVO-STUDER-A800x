#include "WindowsTransport.h"
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <string>
#include <cwctype>
namespace a800 {
namespace {
std::wstring lower(std::wstring s){for(auto& c:s)c=wchar_t(std::towlower(c));return s;}
bool isDawProcess(){
 wchar_t file[1024]{};const DWORD n=GetModuleFileNameW(nullptr,file,1024);
 if(n==0||n>=1024)return false;
 auto name=lower(std::wstring(file,n));auto slash=name.find_last_of(L"\\/");
 if(slash!=std::wstring::npos)name.erase(0,slash+1);
 return name==L"studio one.exe"||name==L"studioone.exe"||name==L"reaper.exe"||name==L"reaper64.exe";
}
bool ownWindow(HWND hwnd){DWORD pid=0;if(hwnd)GetWindowThreadProcessId(hwnd,&pid);return hwnd&&pid==GetCurrentProcessId();}
struct Search {HWND selected=nullptr;int best=-1;HWND editorRoot=nullptr;};
BOOL CALLBACK findWindow(HWND hwnd,LPARAM data){
 auto& search=*reinterpret_cast<Search*>(data);
 if(!ownWindow(hwnd)||!IsWindowVisible(hwnd)||!IsWindowEnabled(hwnd)||(GetWindowLongPtrW(hwnd,GWL_STYLE)&WS_CHILD))return TRUE;
 if(GetWindow(hwnd,GW_OWNER)!=nullptr)return TRUE;
 wchar_t title[1024]{};GetWindowTextW(hwnd,title,1024);auto text=lower(title);
 int score=0;
 if(text.find(L"studio one")!=std::wstring::npos||text.find(L"reaper")!=std::wstring::npos)score+=1000;
 if(hwnd==search.editorRoot)score+=10;
 // Do not pick an arbitrary dialog when the main application cannot be identified.
 if(score==0)return TRUE;
 if(score>search.best){search.selected=hwnd;search.best=score;}
 return TRUE;
}
}
ShortcutResult sendDawSpaceShortcut(void* editorWindow){
 if(!isDawProcess())return ShortcutResult::unsupportedHost;
 if(!ownWindow(GetForegroundWindow()))return ShortcutResult::inactiveHost;
 Search search;auto view=static_cast<HWND>(editorWindow);
 if(ownWindow(view))search.editorRoot=GetAncestor(view,GA_ROOTOWNER);
 EnumWindows(findWindow,reinterpret_cast<LPARAM>(&search));
 auto target=search.selected;
 if(!target||!ownWindow(target))return ShortcutResult::noHostWindow;
 SetForegroundWindow(target);
 if(GetWindowThreadProcessId(target,nullptr)==GetCurrentThreadId())SetFocus(target);
 const auto scan=MapVirtualKeyW(VK_SPACE,MAPVK_VK_TO_VSC);
 const LPARAM down=LPARAM(1u|(scan<<16));
 const LPARAM up=LPARAM(1u|(scan<<16)|(1u<<30)|(1u<<31));
 const bool pressed=PostMessageW(target,WM_KEYDOWN,VK_SPACE,down)!=0;
 const bool released=PostMessageW(target,WM_KEYUP,VK_SPACE,up)!=0;
 return pressed&&released?ShortcutResult::queued:ShortcutResult::failed;
}
}
#else
namespace a800 {ShortcutResult sendDawSpaceShortcut(void*){return ShortcutResult::unsupportedHost;}}
#endif
