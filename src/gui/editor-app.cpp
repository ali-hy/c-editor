#include "./editor-app.h"
#include "../editable-string/gap-buffer.h"
#include "../event-handler/event-handler.h"
#include "text-input/text-input.h"
#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <cstdio>
#include <ft2build.h>

EditorApp *EditorApp::_instance = nullptr;

EditorApp::EditorApp() {
  att = new GLint[]{GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};

  dpy = XOpenDisplay(NULL); // Display on same computer
  scr = DefaultScreen(dpy);

  if (dpy == NULL) {
    printf("\n\tcannot connect to X server\n\n");
    exit(0);
  }

  root = DefaultRootWindow(dpy);

  vi = glXChooseVisual(dpy, 0, att);
  if (vi == NULL) {
    printf("\n\tnoappropriate visual found\n\n");
    exit(0);
  } else {
    printf(
        "\n\tvisual %p selected\n",
        (void *)vi->visualid); // %p creates hexadecimal output like in glxinfo
  }

  cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);

  swa.colormap = cmap;
  swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask;

  win = XCreateWindow(dpy, root, 0, 0, 800, 600, 0, vi->depth, InputOutput,
                      vi->visual, CWColormap | CWEventMask, &swa);

  gc = XCreateGC(dpy, win, GCLineWidth | GCForeground | GCBackground,
                 new XGCValues{
                     .foreground = XWhitePixel(dpy, scr),
                     .background = XBlackPixel(dpy, scr),
                     .line_width = 2,
                 });

  // Make window appear
  XMapWindow(dpy, win);
  XStoreName(dpy, win, "Ceditor");

  XSync(dpy, False);

  event_handler = new EventHandler(dpy, win);
}

EditorApp *EditorApp::GetInstance() {
  if (_instance == nullptr) {
    _instance = new EditorApp();
  }

  return _instance;
}

Display *EditorApp::GetDpy() { return dpy; }
Window EditorApp::GetWin() { return win; }
XWindowAttributes *EditorApp::GetWindowAttributes() { return &gwa; }

int EditorApp::StartLoop() {
  using namespace std;
  GapBuffer<char> buffer{};
  text_input = new TextInput(dpy, win);

  while (1) {
    XNextEvent(GetDpy(), &xev);

    if (xev.type == Expose) {
      XGetWindowAttributes(GetDpy(), GetWin(), GetWindowAttributes());
      XSync(dpy, False);
    } else if (xev.type == ButtonPress || xev.type == ButtonRelease ||
               xev.type == KeyPress || xev.type == KeyRelease) {
      text_input->HandleEvent(xev);
    }
  }

  return 0;
}

EditorApp::~EditorApp() {
  // glXMakeCurrent(dpy, None, NULL);
  // glXDestroyContext(dpy, gc);
  XFreeGC(dpy, gc);
  XDestroyWindow(dpy, win);
  XCloseDisplay(dpy);
  exit(0);
}
