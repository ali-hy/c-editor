#include "key-event-handler.h"
#include <X11/X.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <iostream>

using std::string, std::cout, std::endl;

KeyEventHandler::KeyEventHandler(Display *dpy) { this->dpy = dpy; }

int KeyEventHandler::HandleEvent(XKeyEvent event, void (*InsertFn)(char)) {
  long keysym =
      XkbKeycodeToKeysym(dpy, event.keycode, 0, event.state & ShiftMask);

  if (keysym == XK_Control_L || keysym == XK_Control_R) {
    is_ctrl_pressed = event.type == KeyPress;
  }

  if (keysym == XK_Alt_L || keysym == XK_Alt_R) {
    is_alt_pressed = event.type == KeyPress;
  }

  if ((keysym >= XK_a && keysym <= XK_z) &&
      (keysym >= XK_A && keysym <= XK_Z)) {
    if (InsertFn)
      InsertFn(keysym);
  }

  return 0;
}
