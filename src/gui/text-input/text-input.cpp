#include "./text-input.h"
#include "../../editable-string/gap-buffer.h"
#include <X11/X.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>

TextInput::TextInput(Display *dpy, Window win) {
  Init(dpy, win, new GapBuffer<char>());
}

TextInput::TextInput(Display *dpy, Window win, TextInput::buffer_type *buffer) {
  Init(dpy, win, buffer);
}

void TextInput::Init(Display *dpy, Window win, TextInput::buffer_type *buffer) {
  this->dpy = dpy;
  this->win = win;
  this->buffer = buffer;

  InitFont();
  int scr = XDefaultScreen(dpy);

  this->gcv = new XGCValues{.foreground = XWhitePixel(dpy, scr),
                            .background = XBlackPixel(dpy, scr),
                            .line_width = 2,
                            .font = this->font_info->fid};

  this->gc = XCreateGC(dpy, win,
                       GCLineWidth | GCForeground | GCBackground | GCFont, gcv);

  this->inverse_gvc = new XGCValues{.foreground = XBlackPixel(dpy, scr),
                                    .background = XWhitePixel(dpy, scr),
                                    .line_width = 2,
                                    .font = this->font_info->fid};

  this->inverse_gc =
      XCreateGC(dpy, win, GCLineWidth | GCForeground | GCBackground | GCFont,
                inverse_gvc);
}

TextInput::~TextInput() {
  XFreeGC(dpy, gc);
  XFreeGC(dpy, inverse_gc);

  delete buffer;
  delete gcv;
  delete inverse_gvc;
}

void TextInput::InitFont() {
  int count = 0;
  char **font_names = XListFonts(dpy, "*iso*", 10, &count);

  if (count <= 0) {
    cerr << "ERROR: no suitable font found" << endl;
    return;
  }

  this->font_info = XLoadQueryFont(dpy, font_names[0]);
  cout << "Selected font: " << font_names[0] << endl;
}

int TextInput::GetDrawnTextWidth(char *str, int length) {
  return XTextWidth(this->font_info, str, length);
}

void TextInput::DrawAll(int start_pos = 0) {
  using namespace std;
  if (start_pos > buffer->Size())
    throw out_of_range(to_string(start_pos) + " is out of range(" +
                       to_string(buffer->Size()) + ")");

  cout << buffer->ToString() << '\n' << buffer->ToDebug() << endl;

  char *str = buffer->ToArr();
  int i;
  LineInfo line = LineFromPos(start_pos);

  XClearArea(dpy, win, x, y + (line.num * line_height), width, height, False);

  // Draw each line
  for (; i < buffer->Size(); i++) {
    if (str[i] == '\n') {
      DrawText(0, line.num * line_height, str + line.head, i - line.head);
      line.num++;
      line.head = i + 1;
    }
  }

  // Handle last line (doesn't end with '\n')
  if (line.head != buffer->Size()) {
    DrawText(0, line.num * line_height, str + line.head,
             buffer->Size() - line.head);
  }

  delete[] str;
}

void TextInput::RedrawAt(int pos) {
  cout << "cursor at: " << cursor_pos << '\n';
  cout << buffer->ToDebug();

  LineInfo line = LineFromPos(pos);
  int col = pos - line.head;
  char *line_str = buffer->ToSubArr(line.head, line.tail - line.head);
  int prefix_width = GetDrawnTextWidth(line_str, pos - line.head);

  XClearArea(dpy, win, x + prefix_width, y + line.num * line_height,
             width - prefix_width, line_height, False);

  if (line.tail - pos > 0)
    DrawText(prefix_width, line.num * line_height, line_str + col,
             line.tail - pos);

  delete[] line_str;
}

void TextInput::DrawText(int x, int y, char *str, unsigned int length) {
  XDrawImageString(dpy, win, gc, this->x + x, this->y + font_height + y, str,
                   length);
}

void TextInput::IndexLines(int start_pos = 0) {}

LineInfo TextInput::LineFromPos(int pos) {
  if (pos > buffer->Size() || pos < 0)
    throw out_of_range(to_string(pos) + " is out of range(" +
                       to_string(buffer->Size()) + ")");

  int line = 0, line_start = 0, col = 0, prefix_width;

  for (int i = 0; i < pos; i++) {
    if (buffer->At(i) == '\n') {
      line++;
      line_start = i + 1;
    }
  }

  for (; pos < buffer->Size() && buffer->At(pos) != '\n'; pos++) {
  }

  return {.num = line, .head = line_start, .tail = pos};
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
  int redraw_pos = -1;

  switch (keysym) {
  case XK_BackSpace:
    if (cursor_pos > 0) {
      --cursor_pos;
      buffer->Del(cursor_pos);
      redraw_pos = cursor_pos;
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
    cursor_line.num++;
    cursor_line.head += 2;
    to_insert = '\n';
    redraw_pos = cursor_pos + 1;
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
    redraw_pos = cursor_pos;
  }

  if (to_insert != '\0') {
    buffer->Insert(cursor_pos, to_insert);
    cursor_pos++;
  }

  if (redraw_pos >= 0) {
    DrawAll(redraw_pos);
  }
}

void TextInput::HandleButtonEvent(XButtonEvent event) {}
