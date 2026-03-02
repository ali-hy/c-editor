#include "event-handler.h"
#include "key-event-handler.h"
#include <X11/X.h>
#include <X11/Xlib.h>

EventHandler::EventHandler(Display *dpy) : key_event_handler(dpy) {}

int EventHandler::HandleEvent(XEvent event, void (*InsertFn)(char)) {
  switch (event.type) {
  case KeyPress:
  case KeyRelease:
    key_event_handler.HandleEvent(event.xkey, InsertFn);
    break;
  }

  return 0;
}
