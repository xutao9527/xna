#pragma once

#include <condition_variable>
#include "Shell.h"
#include "xna_window.h"

class XnaApp {
    static void LogErrorFromGLFW(int error, const char *description) {
        Rml::Log::Message(Rml::Log::LT_ERROR, "GLFW error (0x%x): %s", error, description);
    }
public:
    static std::mutex mutex;
    static std::condition_variable cv;
private:
    // 多窗口实例:key=窗口名，value=窗口实例
    std::map<std::string, std::weak_ptr<XnaWindow>> m_xna_windows;

public:
    // 单例模式
    static XnaApp &Instance() {
        static XnaApp instance;
        return instance;
    }

    // 创建窗口
    template<typename WT>
    std::shared_ptr<WT> CreateXnaWindow(const std::string &name = "main", int width = 512, int height = 384, bool allow_resize = true){
        auto it = m_xna_windows.find(name);
        std::shared_ptr<WT> xnaWindow;
        if(it == m_xna_windows.end()){
            xnaWindow = std::shared_ptr<WT>(new WT(name, width, height, allow_resize), [](WT* ptr) {
                ptr->~WT();  // 调用 private 析构函数
                delete ptr;  // 实际删除对象
            });
        }
        return xnaWindow;
    }

    // 显示窗口
    void ShowXnaWindow(std::shared_ptr<XnaWindow>&& xna_windows) {
        m_xna_windows[xna_windows->m_name] = std::weak_ptr<XnaWindow>(xna_windows);
        std::thread windowThread = std::thread([=]() {
            xna_windows->Run();
        });
        windowThread.detach();
    }

    // 运行程序
    void Run();

    XnaApp(const XnaApp &) = delete;
    XnaApp(XnaApp &&) = delete;
    XnaApp &operator=(const XnaApp &) = delete;
    XnaApp &operator=(XnaApp &&) = delete;
private:
    XnaApp();
    ~XnaApp();
};


