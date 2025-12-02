#define STB_IMAGE_IMPLEMENTATION
#include <iostream>


#include "EngineCore.h"
#include "Editor/KarnanEditor.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

int main(int argc, char** argv)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	EngineCore* engine = EngineCore::StartupEngine();
    KarnanEditor* editor = KarnanEditor::StartupEditor();;

	engine->Init();
    editor->Init();

    engine->SetEditorMode(editor);

    try
    {
        engine->Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    editor->DestroyEditor();
    engine->DestroyEngine();
    return EXIT_SUCCESS;
}