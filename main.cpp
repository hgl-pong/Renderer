#include "RenderSystem/RenderSystemInterface.h"

int main(int argc, char *argv[])
{
	IRenderWindow* window = CreateRenderWindow();
	if (window == nullptr)
		return 0;
	window->CreateRenderWindow("Renderer", 800, 600);
	while (window->IsOpen())
	{
		window->Display();
		window->Clear();
	}
	window->DestroyRenderWindow();
	delete window;
	return 1;
}