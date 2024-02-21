#include "first_app.hpp"

namespace tv {
void FirstApp::run() {
  while (!tvWindow.shouldClose()) {
    glfwPollEvents();
  }
}
} // namespace tv
