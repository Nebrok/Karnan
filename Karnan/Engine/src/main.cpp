#include <iostream>

#include "EngineCore.h"




int main(int argc, char** argv)
{
	EngineCore engine;

	engine.Init();

    try
    {
        engine.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}