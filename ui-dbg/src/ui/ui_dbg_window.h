#pragma

#include "xna_window.h"

class UiDbgWindow : public XnaWindow
{
public:
    UiDbgWindow(std::string name, int width, int height, bool allow_resize)
            : XnaWindow(std::move(name), width, height, allow_resize) {}

    void Init() override
    {
        context = Rml::CreateContext(m_name, Rml::Vector2i(m_width, m_height), render_interface);
        if (!context) {
            throw std::runtime_error("Failed to create Rml Context");
        }
        if (Rml::ElementDocument *document = context->LoadDocument("resources/assets/ui_dbg_window.rml")) {
            document->Show();
            document->GetElementById("title")->SetInnerRML("SVG");
        }
        Rml::Debugger::Initialise(context);
    }
};


