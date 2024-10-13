#include "interact.h"
#include "render.h"
#include<iostream>
double deltaTime = 0.0;	// time between current frame and last frame
double lastFrame = 0.0;
int main()
{
	
	uPtr<ImguiWindow> pImguiWinodw = mkU<ImguiWindow>();
	imguiWindow = pImguiWinodw.get();
	if (!imguiWindow->BuildSucces())
	{
		return -1;
	}
	uPtr<Render> render = mkU<Render>();
	imguiWindow->BindRender(render.get());
	while (!imguiWindow->RunningCheck())
	{
		double currentFrame = static_cast<double>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		render->SingleRender();
		imguiWindow->ProcessInput(deltaTime);
		imguiWindow->RenderImgui();
		imguiWindow->SwapBuffer();
		glfwPollEvents();
	}
	return 0;
}