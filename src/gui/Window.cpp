#include "./Window.h"
#include "../editable-string/gap-buffer.cpp"
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
  event_handler = new EventHandler(dpy);

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

  win = XCreateWindow(dpy, root, 0, 0, 600, 600, 0, vi->depth, InputOutput,
                      vi->visual, CWColormap | CWEventMask, &swa);

  // Make window appear
  XMapWindow(dpy, win);
  XStoreName(dpy, win, "Ceditor");

  // gc = XCreateGC(dpy, win, );
  // glXMakeCurrent(dpy, win, gc);
}

EditorApp *EditorApp::GetInstance() {
  if (_instance == nullptr) {
    _instance = new EditorApp();
  }

  return _instance;
}

Display *EditorApp::GetDpy() { return dpy; }
Window *EditorApp::GetWin() { return &win; }
XWindowAttributes *EditorApp::GetWindowAttributes() { return &gwa; }

int EditorApp::StartLoop() {
  using namespace std;
  // TextInput input = TextInput(dpy, win, gc);
  GapBuffer<char> buffer{};

  // InsertFn = [&buffer](char c) { buffer.Insert(buffer.Size(), c); };

  while (1) {
    XNextEvent(GetDpy(), &xev);

    if (xev.type == Expose) {
      printf("EVENT: Expose\n");

      XGetWindowAttributes(GetDpy(), *GetWin(), GetWindowAttributes());
      // glViewport(0, 0, GetWindowAttributes()->width,
      // GetWindowAttributes()->height);
      // glXSwapBuffers(GetDpy(), *GetWin());
    } else if (xev.type == KeyPress || xev.type == KeyRelease) {
      event_handler->HandleEvent(xev, nullptr);
    }
  }

  return 0;
}

EditorApp::~EditorApp() {
  glXMakeCurrent(dpy, None, NULL);
  // glXDestroyContext(dpy, gc);
  XDestroyWindow(dpy, win);
  XCloseDisplay(dpy);
  exit(0);
}
