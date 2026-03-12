#ifndef CEDITOR_TEXTINPUT_H_
#define CEDITOR_TEXTINPUT_H_
#include "../../editable-string/editable-string.h"
#include "../../editable-string/gap-buffer.h"
#include "../vector2.h"
#include <X11/X.h>
#include <X11/Xlib.h>

struct LineInfo {
  int num, head, tail;
};

class TextInput {
  using buffer_type = GapBuffer<char>;

private:
  void Init(Display *dpy, Window win, buffer_type *buffer);
  buffer_type *buffer;
  const int line_height = 20, font_height = 10;
  int x = 20, y = 20, width = 0, height = line_height * 30;
  bool is_ctrl_pressed, is_alt_pressed;

  // Curosr info
  Vec2 cursor_origin;
  LineInfo cursor_line = {.num = 0, .head = 0, .tail = 0};
  int cursor_pos = 0, cursor_col = 0;

  // Xlib context
  Display *dpy;
  Window win;
  XFontStruct *font_info;
  XGCValues *gcv;
  GC gc;

  XGCValues *inverse_gvc;
  GC inverse_gc;

public:
  TextInput(Display *dpy, Window win);
  TextInput(Display *dpy, Window win, buffer_type *buffer);

  ~TextInput();

  // Line Logic
  void IndexLines(int start_pos);
  LineInfo LineFromPos(int pos);

  // Handle font and drawing
  void InitFont();
  void DrawText(int x, int y, char *str, unsigned int length);
  void DrawAll(int start_pos);
  void RedrawAt(int start_pos);
  int GetDrawnTextWidth(char *s, int n);

  // Handle Key Events
  void HandleEvent(XEvent event);
  void HandleKeyEvent(XKeyEvent event);
  void HandleButtonEvent(XButtonEvent event);
};

#endif // CEDITOR_TEXTINPUT_H_
