#include "event-handler.h"
#include "key-event-handler.h"
#include <X11/X.h>
#include <X11/Xlib.h>

EventHandler::EventHandler(Display *dpy, Window win)
    : key_event_handler(dpy, win) {}

int EventHandler::HandleEvent(XEvent event) {
  switch (event.type) {
  case KeyPress:
  case KeyRelease:
    key_event_handler.HandleEvent(event.xkey);
    break;
  }

  return 0;
}
