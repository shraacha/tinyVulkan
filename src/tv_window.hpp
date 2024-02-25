#pragma once

#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// glfw has it's own destroy (o.o)
struct DestroyGLFWwin {
  void operator()(GLFWwindow *ptr) { glfwDestroyWindow(ptr); }
};

typedef std::unique_ptr<GLFWwindow, DestroyGLFWwin> smart_GLFWwindow_ptr;

namespace tv {

class TvWindow {
public:
  TvWindow(int w, int h, std::string name);

  // needed for custom glfwTerminate (T_T)
  ~TvWindow();

  TvWindow(const TvWindow &) = delete;
  TvWindow &operator=(const TvWindow &) = delete;

  bool shouldClose() { return glfwWindowShouldClose(m_window.get()); }

private:
  void initWindow();

  const int m_width;
  const int m_height;
  std::string m_name;

  smart_GLFWwindow_ptr m_window;
};

} // namespace tv
