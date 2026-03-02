#ifndef CEDITOR_EVENTHANDLER_H_
#define CEDITOR_EVENTHANDLER_H_
#include "key-event-handler.h"
#include <X11/Xlib.h>

class EventHandler {
  KeyEventHandler key_event_handler;

public:
  EventHandler(Display *dpy);
  int HandleEvent(XEvent event, void (*InsertFn)(char));
};

#endif // CEDITOR_EVENTHANDLER_H_
