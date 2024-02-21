#include "tv_window.hpp"

namespace tv {
TvWindow::TvWindow(int w, int h, std::string name)
    : m_width{w}, m_height{h}, m_name{name}, m_window() {
  initWindow();
}

TvWindow::~TvWindow() { glfwTerminate(); }

void TvWindow::initWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  m_window.reset(glfwCreateWindow(m_width, m_height, m_name.c_str(), nullptr, nullptr));
}
} // namespace tv
