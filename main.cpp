#include "RenderSystem/RenderWindow.h"

int main(int argc, char* argv[]) 
{
	RenderWindow testWindow;
	testWindow.CreateRenderWindow("Renderer", 800, 600);
	testWindow.Clear();
	testWindow.Display();
	while (testWindow.IsOpen())
	{
		testWindow.Clear();
		testWindow.Display();
	}
	testWindow.DestroyRenderWindow();
	return 0;
}