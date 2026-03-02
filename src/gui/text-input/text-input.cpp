#include "./text-input.h"
#include "../../editable-string/editable-string.h"
#include "../../editable-string/gap-buffer.h"
#include <X11/X.h>
#include <X11/Xlib.h>

TextInput::TextInput(Display *dpy, Window *win, GC gc) {
  Init(dpy, win, gc, new GapBuffer<char>());
}

TextInput::TextInput(Display *dpy, Window *win, GC gc,
                     EditableString<char> *buffer) {
  Init(dpy, win, gc, buffer);
}

TextInput::~TextInput() { delete buffer; }

void TextInput::Init(Display *dpy, Window *win, GC gc,
                     EditableString<char> *buffer) {
  this->dpy = dpy;
  this->win = win;
  this->gc = gc;
  this->buffer = buffer;
}

void TextInput::Draw() {
  XDrawString(dpy, *win, gc, 20, 20, buffer->ToArr(), buffer->Size());
}
