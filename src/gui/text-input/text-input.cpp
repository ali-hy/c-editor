#include "./text-input.h"
#include "../../editable-string/gap-buffer.h"
#include <X11/X.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <algorithm>
#include <iostream>

TextInput::TextInput(Display *dpy, Window win) {
  Init(dpy, win, new GapBuffer<char>());
}

TextInput::TextInput(Display *dpy, Window win, TextInput::buffer_type *buffer) {
  Init(dpy, win, buffer);
}

TextInput::~TextInput() { delete buffer; }

void TextInput::Init(Display *dpy, Window win, TextInput::buffer_type *buffer) {
  this->dpy = dpy;
  this->win = win;
  this->buffer = buffer;

  InitFont();
  int scr = XDefaultScreen(dpy);
  this->gc =
      XCreateGC(dpy, win, GCLineWidth | GCForeground | GCBackground | GCFont,
                new XGCValues{.foreground = XWhitePixel(dpy, scr),
                              .background = XBlackPixel(dpy, scr),
                              .line_width = 2,
                              .font = this->font_info->fid});
}

void TextInput::InitFont() {
  int count = 0;
  char **font_names = XListFonts(dpy, "*iso*", 10, &count);

  if (count <= 0) {
    cerr << "ERROR: no suitable font found" << endl;
    return;
  }

  this->font_info = XLoadQueryFont(dpy, font_names[0]);
  cout << "Selected font: " << font_names[0];
}

int TextInput::GetDrawnStringWidth(char *str, int length) {
  return XTextWidth(this->font_info, str, length);
}

void TextInput::Draw(int start_pos) {
  using namespace std;
  cout << buffer->ToString() << '\n' << buffer->ToDebug() << endl;

  char *str = buffer->ToArr();
  int line = 0, line_start = 0;
  for (int i = 0; i < buffer->Size(); i++) {
    if (str[i] == '\n') {
      XDrawImageString(dpy, win, gc, x, y + (line * 20), str + line_start,
                       i - line_start);
      line++;
      line_start = i + 1;
    }
  }

  if (line_start != buffer->Size()) {
    XDrawImageString(dpy, win, gc, x, y + (line * 20), str + line_start,
                     buffer->Size() - line_start);
  }
}

void TextInput::HandleEvent(XEvent event) {
  switch (event.type) {
  case KeyPress:
  case KeyRelease:
    HandleKeyEvent(event.xkey);
    break;

  case ButtonPress:
  case ButtonRelease:
    HandleButtonEvent(event.xbutton);
    break;
  }
}

void TextInput::HandleKeyEvent(XKeyEvent event) {
  using namespace std;
  if (event.type == KeyRelease)
    return;

  static int key_event_count = 0;
  key_event_count++;

  long keysym =
      XkbKeycodeToKeysym(dpy, event.keycode, 0, event.state & ShiftMask);

  char to_insert = 0;

  switch (keysym) {
  case XK_BackSpace:
    if (cursor_pos > 0) {
      --cursor_pos;
      buffer->Del(cursor_pos);
    }
    break;

  case XK_Left:
  case XK_Right:
    cursor_pos += keysym == XK_Left ? -1 : 1;
    cursor_pos = clamp(cursor_pos, 0, buffer->Size());
    break;

  case XK_Up:
  case XK_Down:
    // TODO: Calculate position
    break;

  case XK_Return:
    line_num++;
    to_insert = '\n';
    break;

  case XK_Control_R:
  case XK_Control_L:
    is_ctrl_pressed = event.type == KeyPress;
    break;

  case XK_Alt_L:
  case XK_Alt_R:
    is_alt_pressed = event.type == KeyPress;
    break;

  case XK_Shift_L:
  case XK_Shift_R:
  case XK_Shift_Lock:
    break;

  default:
    to_insert = (char)keysym;
  }

  if (is_alt_pressed || is_ctrl_pressed) {
    return;
  }

  if (to_insert != 0) {
    buffer->Insert(cursor_pos, to_insert);
    cursor_pos++;
  }

  cout << key_event_count << ": ";
  Draw(cursor_pos);
}

void TextInput::HandleButtonEvent(XButtonEvent event) {}
