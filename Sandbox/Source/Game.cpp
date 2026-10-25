#include <Core/Application.h>

int main(int argc, char* argv[])
{
    const char* executablePath = (argc > 0 && argv != nullptr) ? argv[0] : nullptr;
    C4ndy::Application::SetExecutablePath(executablePath);
    
    C4ndy::Application app;
    app.Run();
    return 0;
}