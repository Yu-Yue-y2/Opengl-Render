#ifndef INTERACT_H
#define INTERACT_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "render.h"
#include "window.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
static void glfw_error_callback(int error, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
struct MouseControl {
	bool mouse_move = true;
	bool firstMouse = true;
	float lastX = width / 2.0f;
	float lastY = height / 2.0f;
};
class ImguiWindow {
	Render* render = nullptr;
	Camera* camera = nullptr;
	GLFWwindow* glfwWindow;
	MouseControl mouseControl;
public:

	~ImguiWindow();
	ImguiWindow();
	void WindowSizeChangeCallback(int width, int height);
	void ProcessInput(float deltaTime);
	void ScrollCallback(double yoffset);
	void MouseCallback(double xposIn, double yposIn);
	bool BuildSucces() const;
	void RenderImgui();
	bool RunningCheck() const;
	void SwapBuffer();
	void BindRender(Render* render);
};
extern ImguiWindow* imguiWindow;
#endif