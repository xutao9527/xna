#include "xna_app.h"

std::mutex XnaApp::mutex;
std::condition_variable XnaApp::cv;

XnaApp::XnaApp() {
    glfwSetErrorCallback(LogErrorFromGLFW);
    if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW");
    Shell::Initialize();
    // 初始化Rml环境
    Rml::Initialise();
    // 加载字库
    Shell::LoadFonts();
}

XnaApp::~XnaApp(){
    Rml::Shutdown();
    glfwTerminate();
    Shell::Shutdown();
}


// 运行程序
void XnaApp::Run(){
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [this] {
        std::vector<std::string> windows_to_remove;
        for (const auto &item: m_xna_windows){
            if(!item.second.lock()){
                windows_to_remove.push_back(item.first);  // 记录销毁的窗口
            }
        }
        for (const auto& key : windows_to_remove) { // 移除销毁的窗口
            m_xna_windows.erase(key);
        }
        return m_xna_windows.empty();
    });
}

