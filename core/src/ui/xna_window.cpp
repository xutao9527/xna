#include "xna_app.h"
#include "xna_window.h"

std::mutex XnaWindow::mutex;

XnaWindow::XnaWindow(std::string name, int width, int height, bool allow_resize)
        : m_name(std::move(name)),
          m_width(width),
          m_height(height),
          m_allow_resize(allow_resize),
          m_running(false)
    {
}

XnaWindow::~XnaWindow() {
    // 通知唤醒主线程
    XnaApp::cv.notify_all();
}


void XnaWindow::SettingBase() {
    mutex.lock();
    Setting();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, m_allow_resize ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    window = glfwCreateWindow(m_width, m_height, m_name.c_str(), nullptr, nullptr);
    if (!window) throw std::runtime_error("Failed to create GLFW window");
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    Rml::String renderer_message;
    if (!RmlGL3::Initialize(&renderer_message)) {
        throw std::runtime_error("Failed to initialize RmlGL3");
    }

    system_interface = new SystemInterface_GLFW();
    if (!system_interface) throw std::runtime_error("Failed to create system interface!");
    system_interface->SetWindow(window);
    system_interface->LogMessage(Rml::Log::LT_INFO, renderer_message);
    glfwGetFramebufferSize(window, &m_width, &m_height);

    render_interface = new RenderInterface_GL3();
    if (!render_interface) throw std::runtime_error("Failed to create render interface!");
    render_interface->SetViewport(m_width, m_height);

    glfwSetInputMode(window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
    mutex.unlock();
}

void XnaWindow::Run(){
    // 窗口基本设置
    SettingBase();
    // 窗口回调设置
    SetCallbacks();
    // 窗口初始化
    Init();
    // 窗口运行标志
    m_running = true;
    // 窗口事件与渲染循环
    Loop();
}

void XnaWindow::Loop(){
    while (m_running) {
        ProcessEvents(true);
        context->Update();
        render_interface->Clear();
        render_interface->BeginFrame();
        context->Render();
        render_interface->EndFrame();
        glfwSwapBuffers(window);
    }
    // 销毁窗口
    glfwDestroyWindow(window);
    // 释放渲染资源
    Rml::ReleaseTextures(render_interface);
    // 移除上下文
    Rml::RemoveContext(m_name);
}


