#include "Shell.h"
#include "ShellFileInterface.h"
#include <windows.h>

#include <RmlUi/Core/Core.h>
#include <RmlUi/Core/Input.h>
#include <RmlUi/Debugger/Debugger.h>


static Rml::UniquePtr<ShellFileInterface> file_interface;

bool Shell::Initialize()
{
    auto findRootFile = [] {
        char executable_file_name[MAX_PATH];
        if (GetModuleFileName(nullptr, executable_file_name, MAX_PATH) >= MAX_PATH &&
            GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            executable_file_name[0] = 0;
        }
        Rml::String executable_path(executable_file_name);
        if (executable_path.empty()) {
            return Rml::String();
        } else {
            executable_path = executable_path.substr(0, executable_path.rfind('\\') + 1);
        }
        return Rml::String(executable_path);
    };
    std::string root = findRootFile();
    file_interface = Rml::MakeUnique<ShellFileInterface>(root);
    Rml::SetFileInterface(file_interface.get());
    return true;
}

void Shell::Shutdown()
{
    file_interface.reset();
}


void Shell::LoadFonts()
{
    const Rml::String directory = "resources/assets/";

    struct FontFace
    {
        const char *filename;
        bool fallback_face;
    };
    FontFace font_faces[] = {
        {"SourceHanSansSC-Regular.ttf", false},
        {"SourceHanSansSC-Bold.ttf",    false},
        // {"SourceHanSansSC-Light.ttf",    false},
        // {"SourceHanSansSC-ExtraLight.ttf",    false},
        // {"SourceHanSansSC-Heavy.ttf",    false},
        // {"SourceHanSansSC-Medium.ttf",    false},
        // {"SourceHanSansSC-Normal.ttf", true},

        // {"SourceHanSansSC-Normal.ttf",     false},
        // {"simhei.ttf",     false},
        // {"SourceHanSansSC-Regular-2.otf",     false},
        // {"FiraCode-Bold.ttf",        false},
        // {"FiraCode-Light.ttf",       false},
        // {"FiraCode-Medium.ttf",      false},
        // {"FiraCode-Retina.ttf",      false},
    };


    for (const FontFace &face: font_faces)
        Rml::LoadFontFace(directory + face.filename, face.fallback_face);
}


