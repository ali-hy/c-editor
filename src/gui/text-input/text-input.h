#ifndef CEDITOR_TEXTINPUT_H_
#define CEDITOR_TEXTINPUT_H_
#include "../../editable-string/editable-string.h"
#include <X11/X.h>
#include <X11/Xlib.h>

class TextInput {
private:
  void Init(Display *dpy, Window *win, GC gc, EditableString<char> *buffer);
  EditableString<char> *buffer;
  Window *win;
  Display *dpy;
  GC gc;

public:
  TextInput(Display *dpy, Window *win, GC gc);
  TextInput(Display *dpy, Window *win, GC gc, EditableString<char> *buffer);

  void Draw();

  ~TextInput();
};

#endif // CEDITOR_TEXTINPUT_H_
