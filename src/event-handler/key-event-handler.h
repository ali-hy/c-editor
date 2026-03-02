#ifndef CEDITOR_KEYEVENTHANDLER_H_
#define CEDITOR_KEYEVENTHANDLER_H_

#include <X11/X.h>
#include <X11/Xlib.h>

class KeyEventHandler {
  bool is_shift_pressed, is_ctrl_pressed, is_alt_pressed;
  int min_keycode, max_keycode, keysym_per_keycode;

  Display *dpy;

public:
  KeyEventHandler(Display *dpy);
  int HandleEvent(XKeyEvent event, void (*InsertFn)(char));
};

#endif // !CEDITOR_KEYEVENTHANDLER_H_
