#pragma once
#include <functional>
#include <stdexcept>

#include "../Common.h"

namespace platform
{

enum class CameraMove
{
    Front,
    Back,
    Left,
    Right
};

class Window
{
  public:
    Window(int width, int height);
    ~Window();

    bool shouldClose() const;
    void pollEvents();

    // 函数指令永远在代码区  所以 std::move(fn) 搬运的不是机器码，而是 fn 这个小结构体里记录的指针和上下文数据
    void        setOnResize(std::function<void(int, int)> fn) { mOnResize = std::move(fn); }
    GLFWwindow *getWindow() { return mWindow; }

    void processEvents();
    void setOnKeyMove(std::function<void(CameraMove)> fn) { mOnKeyMove = std::move(fn); }

    void setOnMouseMove(std::function<void(double, double)> fn) { mOnMouseMove = std::move(fn); }
    bool isLeftMousePressed() const;

  private:
    GLFWwindow *mWindow; // Core !!!

    int mWidth;
    int mHeight;

    std::function<void(int, int)>       mOnResize;
    std::function<void(CameraMove)>     mOnKeyMove;
    std::function<void(double, double)> mOnMouseMove;

  private:
    static void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos);
    static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
};

} // namespace platform