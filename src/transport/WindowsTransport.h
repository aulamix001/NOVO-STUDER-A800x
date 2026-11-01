#pragma once
namespace a800 {
enum class ShortcutResult { queued, unsupportedHost, inactiveHost, noHostWindow, disabled, failed };
// UI thread only. Posts a Space key pair to the identified current-process DAW
// window. Never uses global keyboard injection or sends input to another process.
ShortcutResult sendDawSpaceShortcut(void* editorWindow);
}
