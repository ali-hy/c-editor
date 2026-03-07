#include "./gui/editor-app.h"
#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cstdio>
#include <ft2build.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  // Initialize initialize window
  EditorApp *editor = EditorApp::GetInstance();

  return editor->StartLoop();
}
