#include "RenderSystem/RenderWindow.h"

int main(int argc, char *argv[])
{
	RenderWindow testWindow;
	testWindow.CreateRenderWindow("Renderer", 800, 600);
	while (testWindow.IsOpen())
	{
		testWindow.Display();
		testWindow.Clear();
	}
	testWindow.DestroyRenderWindow();
	return 0;
}