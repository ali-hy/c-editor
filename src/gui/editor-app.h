#ifndef CEDITOR_EDITORAPP_H_
#define CEDITOR_EDITORAPP_H_

#include "../event-handler/event-handler.h"
#include "./text-input/text-input.h"
#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

class EditorApp {
private:
  Display *dpy;
  int scr;
  Window root;
  GLint *att;
  XVisualInfo *vi;
  Colormap cmap;
  XSetWindowAttributes swa;
  Window win;
  GC gc;
  XGCValues *gcv;
  XWindowAttributes gwa;
  EventHandler *event_handler;

  static EditorApp *_instance;
  EditorApp();

public:
  XEvent xev; // XEvent

  // Base
  static EditorApp *GetInstance();
  Display *GetDpy();
  Window GetWin();
  XWindowAttributes *GetWindowAttributes();
  ~EditorApp();

  // Elements
  TextInput *text_input;

  // Behavior
  int StartLoop();

  // Not clonable, not assignable
  EditorApp(EditorApp &other) = delete;
  void operator=(const EditorApp &) = delete;
};
#endif // CEDITOR_EDITORAPP_H_
