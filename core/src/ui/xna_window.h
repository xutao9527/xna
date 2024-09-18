#pragma once

#include <GLFW/glfw3.h>
#include <RmlUi/Core.h>
#include "RmlUi_Renderer_GL3.h"
#include "RmlUi_Platform_GLFW.h"
#include <atomic>
#include <thread>
#include <mutex>
#include <RmlUi/Debugger/Debugger.h>
#include "Shell.h"

class XnaApp;

class XnaWindow
{
    friend class XnaApp;
protected:
    static std::mutex mutex;

    GLFWwindow *window = nullptr;
    Rml::Context *context = nullptr;
    SystemInterface_GLFW *system_interface{};
    RenderInterface_GL3 *render_interface{};
    int glfw_active_modifiers = 0;
    bool context_dimensions_dirty = true;
    int m_width;
    int m_height;
    bool m_allow_resize;
    std::atomic<bool> m_running;
    std::string m_name;

    XnaWindow(std::string name, int width, int height, bool allow_resize);

    ~XnaWindow();

    virtual void Setting() {}

    void SettingBase();

    virtual void Init() = 0;

    void Run();

    void Loop();

    void SetCallbacks()
    {
        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window,
                           [](GLFWwindow *glfw_window, int glfw_key, int scancode, int glfw_action, int glfw_mods) {
                               auto *self = static_cast<XnaWindow *>(glfwGetWindowUserPointer(glfw_window));
                               if (!self->context)
                                   return;
                               self->glfw_active_modifiers = glfw_mods;
                               switch (glfw_action) {
                                   case GLFW_PRESS:
                                   case GLFW_REPEAT: {
                                       const Rml::Input::KeyIdentifier key = RmlGLFW::ConvertKey(glfw_key);
                                       const int key_modifier = RmlGLFW::ConvertKeyModifiers(glfw_mods);
                                       float dp_ratio = 1.f;
                                       glfwGetWindowContentScale(self->window, &dp_ratio, nullptr);

                                       // See if we have any global shortcuts that take priority over the context.
                                       if (!self->ProcessKeyDownShortcuts(key, key_modifier, dp_ratio, true))
                                           break;
                                       // Otherwise, hand the event over to the context by calling the input handler as normal.
                                       if (!RmlGLFW::ProcessKeyCallback(self->context, glfw_key, glfw_action,
                                                                        glfw_mods))
                                           break;
                                       // The key was not consumed by the context either, try keyboard shortcuts of lower priority.
                                       if (!self->ProcessKeyDownShortcuts(key, key_modifier, dp_ratio, false))
                                           break;
                                   }
                                       break;
                                   case GLFW_RELEASE:
                                       RmlGLFW::ProcessKeyCallback(self->context, glfw_key, glfw_action, glfw_mods);
                                       break;
                                   default:
                                       break;
                               }
                           });

        glfwSetCharCallback(window, [](GLFWwindow *glfw_window, unsigned int codepoint) {
            auto *self = static_cast<XnaWindow *>(glfwGetWindowUserPointer(glfw_window));
            RmlGLFW::ProcessCharCallback(self->context, codepoint);
        });

        glfwSetCursorEnterCallback(window, [](GLFWwindow *glfw_window, int entered) {
            auto *self = static_cast<XnaWindow *>(glfwGetWindowUserPointer(glfw_window));
            RmlGLFW::ProcessCursorEnterCallback(self->context, entered);
        });

        // Mouse input
        glfwSetCursorPosCallback(window, [](GLFWwindow *glfw_window, double xpos, double ypos) {
            auto *self = static_cast<XnaWindow *>(glfwGetWindowUserPointer(glfw_window));
            RmlGLFW::ProcessCursorPosCallback(self->context, glfw_window, xpos, ypos, self->glfw_active_modifiers);
        });

        glfwSetMouseButtonCallback(window, [](GLFWwindow *glfw_window, int button, int action, int mods) {
            auto *self = static_cast<XnaWindow *>(glfwGetWindowUserPointer(glfw_window));
            self->glfw_active_modifiers = mods;
            RmlGLFW::ProcessMouseButtonCallback(self->context, button, action, mods);
        });

        glfwSetScrollCallback(window, [](GLFWwindow *glfw_window, double /*xoffset*/, double yoffset) {
            auto *self = static_cast<XnaWindow *>(glfwGetWindowUserPointer(glfw_window));
            RmlGLFW::ProcessScrollCallback(self->context, yoffset, self->glfw_active_modifiers);
        });

        // Window events
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow *glfw_window, int width, int height) {
            auto *self = static_cast<XnaWindow *>(glfwGetWindowUserPointer(glfw_window));
            self->render_interface->SetViewport(width, height);
            RmlGLFW::ProcessFramebufferSizeCallback(self->context, width, height);
        });

        glfwSetWindowContentScaleCallback(window, [](GLFWwindow *glfw_window, float x_scale, float y_scale) {
            auto *self = static_cast<XnaWindow *>(glfwGetWindowUserPointer(glfw_window));
            RmlGLFW::ProcessContentScaleCallback(self->context, x_scale);
        });
    }

    bool ProcessEvents(bool power_save)
    {
        // The initial window size may have been affected by system DPI settings, apply the actual pixel size and dp-ratio to the context.
        if (context_dimensions_dirty) {
            context_dimensions_dirty = false;
            Rml::Vector2i window_size;
            float dp_ratio = 1.f;
            glfwGetFramebufferSize(window, &window_size.x, &window_size.y);
            glfwGetWindowContentScale(window, &dp_ratio, nullptr);
            this->context->SetDimensions(window_size);
            this->context->SetDensityIndependentPixelRatio(dp_ratio);
        }
        if (power_save)
            glfwWaitEventsTimeout(Rml::Math::Min(context->GetNextUpdateDelay(), 10.0));
        else
            glfwPollEvents();
        m_running = !glfwWindowShouldClose(window);
        glfwSetWindowShouldClose(window, GLFW_FALSE);
        return m_running;
    }

    bool ProcessKeyDownShortcuts(Rml::Input::KeyIdentifier key, int key_modifier, float native_dp_ratio, bool priority)
    {
        if (!context)
            return true;
        // Result should return true to allow the event to propagate to the next handler.
        bool result = false;
        // This function is intended to be called twice by the backend, before and after submitting the key event to the context. This way we can
        // intercept shortcuts that should take priority over the context, and then handle any shortcuts of lower priority if the context did not
        // intercept it.
        if (priority) {
            // Priority shortcuts are handled before submitting the key to the context.
            // Toggle debugger and set dp-ratio using Ctrl +/-/0 keys.
            if (key == Rml::Input::KI_F8) {
                Rml::Debugger::SetVisible(!Rml::Debugger::IsVisible());
            } else if (key == Rml::Input::KI_0 && key_modifier & Rml::Input::KM_CTRL) {
                context->SetDensityIndependentPixelRatio(native_dp_ratio);
            } else if (key == Rml::Input::KI_1 && key_modifier & Rml::Input::KM_CTRL) {
                context->SetDensityIndependentPixelRatio(1.f);
            } else if ((key == Rml::Input::KI_OEM_MINUS || key == Rml::Input::KI_SUBTRACT) &&
                       key_modifier & Rml::Input::KM_CTRL) {
                const float new_dp_ratio = Rml::Math::Max(context->GetDensityIndependentPixelRatio() / 1.2f, 0.5f);
                context->SetDensityIndependentPixelRatio(new_dp_ratio);
            } else if ((key == Rml::Input::KI_OEM_PLUS || key == Rml::Input::KI_ADD) &&
                       key_modifier & Rml::Input::KM_CTRL) {
                const float new_dp_ratio = Rml::Math::Min(context->GetDensityIndependentPixelRatio() * 1.2f, 2.5f);
                context->SetDensityIndependentPixelRatio(new_dp_ratio);
            } else {
                // Propagate the key down event to the context.
                result = true;
            }
        } else {
            // We arrive here when no priority keys are detected and the key was not consumed by the context. Check for shortcuts of lower priority.
            if (key == Rml::Input::KI_R && key_modifier & Rml::Input::KM_CTRL) {
                for (int i = 0; i < context->GetNumDocuments(); i++) {
                    Rml::ElementDocument *document = context->GetDocument(i);
                    const Rml::String &src = document->GetSourceURL();
                    if (src.size() > 4 && src.substr(src.size() - 4) == ".rml") {
                        document->ReloadStyleSheet();
                    }
                }
            } else {
                result = true;
            }
        }
        return result;
    }

};


