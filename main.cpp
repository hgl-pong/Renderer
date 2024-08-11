#include "Common/pch.h"
#include "Engine/EngineInterface.h"
#include "Engine/RenderSystemInterface.h"
int main(int argc, char *argv[])
{
    IWindow *window = CreateRenderWindow();
    window->CreateRenderWindow("Renderer", 800, 600);
    window->SetVSync(true);
    window->SetFullscreen(false);
    window->SetVisible(true);
    window->SetMouseCursorVisible(true);
    window->SetPosition(100, 100);
    window->SetIcon("icon.png");
    window->SetClearColor(MathLib::HVector4(0.0f, 0.0f, 0.0f, 1.0f));

    SharedPtr<IRenderSystem> renderSystem = CreateRenderSystem(RenderSystemType::Vulkan);
    window->BindRenderSystem(renderSystem);

    renderSystem->PreInitialize();
    renderSystem->PostInitialize();

    window->Clear();
    window->Display();

    while (window->IsOpen())
    {
        window->Clear();
        window->Display();
        // Vector2f position = window->GetPosition();
        // printf("Position: (%f, %f)\n", position.x(), position.y());
    }
    window->DestroyRenderWindow();
    return 1;
}



