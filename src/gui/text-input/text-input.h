#ifndef CEDITOR_TEXTINPUT_H_
#define CEDITOR_TEXTINPUT_H_
#include "../../editable-string/editable-string.h"
#include "../../editable-string/gap-buffer.h"
#include <X11/X.h>
#include <X11/Xlib.h>

class TextInput {
  using buffer_type = GapBuffer<char>;

private:
  void Init(Display *dpy, Window win, buffer_type *buffer);
  buffer_type *buffer;
  const int line_height = 12;
  int cursor_pos, x = 20, y = 20, line_num = 0, width = 400,
                  height = line_height * 30;
  bool is_ctrl_pressed, is_alt_pressed;

  Display *dpy;
  Window win;
  GC gc;
  XFontStruct *font_info;

public:
  TextInput(Display *dpy, Window win);
  TextInput(Display *dpy, Window win, buffer_type *buffer);

  ~TextInput();

  // Handle font and drawing
  void InitFont();
  void Draw(int start_pos);
  int GetDrawnStringWidth(char *s, int n);

  // Handle Key Events
  void HandleEvent(XEvent event);
  void HandleKeyEvent(XKeyEvent event);
  void HandleButtonEvent(XButtonEvent event);
};

#endif // CEDITOR_TEXTINPUT_H_
