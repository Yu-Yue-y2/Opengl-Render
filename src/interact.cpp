#include "interact.h"

#include "macro.h"
#include "render.h"
#include<iostream>
ImguiWindow* imguiWindow;
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}
void ImguiWindow::WindowSizeChangeCallback(int _width, int _height)
{
    render->ChangeFrameBufferSize(_width, _height, true);
}
void ImguiWindow::ProcessInput(float deltaTime)
{
    GLFWwindow* window = glfwWindow;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera->ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera->ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera->ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera->ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        mouseControl.mouse_move = !mouseControl.mouse_move;
    }
}
void ImguiWindow::ScrollCallback(double yoffset)
{
    camera->ProcessMouseScroll(static_cast<float>(yoffset));
}
void ImguiWindow::MouseCallback(double xposIn, double yposIn)
{
    if (!mouseControl.mouse_move)
        return;
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (mouseControl.firstMouse)
    {
        mouseControl.lastX = xpos;
        mouseControl.lastY = ypos;
        mouseControl.firstMouse = false;
    }

    float xoffset = xpos - mouseControl.lastX;
    float yoffset = mouseControl.lastY - ypos; // reversed since y-coordinates go from bottom to top

    mouseControl.lastX = xpos;
    mouseControl.lastY = ypos;

    camera->ProcessMouseMovement(xoffset, yoffset);
}
bool ImguiWindow::BuildSucces() const
{
    return glfwWindow != nullptr;
}
void ImguiWindow::BindRender(Render* _render)
{
    render = _render;
    camera = render->GetScene()->GetCamera();
}
ImguiWindow::ImguiWindow()
{
    glfwInit();
    glfwSetErrorCallback(glfw_error_callback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return;
    }
    glViewport(0, 0, width, height);
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.FontGlobalScale = 2.0f; // 设置全局缩放因子为 1.5
    ImGui::StyleColorsDark();

    // 设置 ImGui 的 GLFW 和 OpenGL 后端
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    glfwWindow = window;
}
ImguiWindow::~ImguiWindow()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

void ImguiWindow::RenderImgui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 创建一个简单的 ImGui 窗口
    ImGui::Begin("Hello, ImGui!");

    ImGui::SetWindowSize(ImVec2(900, 900));
    ImGui::Text("Objects");
    
    if (ImGui::CollapsingHeader("Instance"))
    {
        MainPipeline* mainPipeline = dynamic_cast<MainPipeline*>(render->GetPipeline(PIPELINE_NAME::MAIN));
        std::vector<Instance*> instances;
        mainPipeline->GetInstances(instances);
        std::vector<const char*> names;
        for (Instance* instance : instances)
        {
            names.push_back(instance->name);
        }
        static int item_current = 0;
        ImGui::ListBox("ObjectName", &item_current, names.data(), instances.size() , 3);
        Instance* c_instance = instances[item_current];
        {
            static bool LockPosition = true;
            ImGui::Checkbox("Lock##pos", &LockPosition);
            ImGui::SameLine();
            glm::vec3 position = c_instance->GetPosition();
            if (ImGui::SliderFloat3("Position", glm::value_ptr(position), -50.0, 50.0)) 
            {
                if (!LockPosition)
                {
                    c_instance->SetPosition(position);
                }
            }
        }
      
        if (ImGui::CollapsingHeader("Material"))
        {
            Material mat = c_instance->GetMaterial();
            static bool diffuseLock = true;
            static bool specularLock = true;
            static bool ambientLock = true;
            if(mat.GetType() == MaterialType::BASEMATERIAL)
            {
                bool change = false;
                ImGui::Checkbox("Lock##m_dif", &diffuseLock);
                ImGui::SameLine();
                glm::vec3 diffuse = mat.GetDiffuse();
                if (ImGui::SliderFloat3("Diffuse", glm::value_ptr(diffuse), 0.0, 1.0))
                {
                    if (!diffuseLock)
                    {
                        change = true;
                        mat.SetDiffuse(diffuse);
                    }
                }
           
                ImGui::Checkbox("Lock##m_spec", &specularLock);
                ImGui::SameLine();
                glm::vec3 specular = mat.GetSpecular();
                if (ImGui::SliderFloat3("Specular", glm::value_ptr(specular), 0.0, 1.0)) 
                {
                    if (!specularLock)
                    {
                        change = true;
                        mat.SetSpecular(specular);
                    }
                }
                
                ImGui::Checkbox("Lock##m_env", &ambientLock);
                ImGui::SameLine();
                glm::vec3 ambient = mat.GetAmbient();
                if (ImGui::SliderFloat3("Ambient", glm::value_ptr(ambient), 0.0, 1.0)) 
                {
                    if (!ambientLock)
                    {
                        change = true;
                        mat.SetAmbient(ambient);
                    }
                }
                if (change)
                {
                    c_instance->SetMaterial(mat);
                }
            }
        }
    }
    if (ImGui::CollapsingHeader("Lights")) {
        Light* light = render->GetScene()->GetMainLight();
        if (light->lighttype == POINTLIGHT) {
            ImGui::Text("PointLight");
            ImGui::SliderFloat3("intensity", glm::value_ptr(light->intensity), 0.0, 10.0);
            ImGui::SliderFloat3("pos", glm::value_ptr(((PointLight*)light)->pos), -40.0, 40.0);
        }
        else if (light->lighttype == DIRECTIONLIGHT) {
            ImGui::Text("DirectionLight");
            ImGui::SliderFloat3("intensity", glm::value_ptr(light->intensity), 0.0, 10.0);
            ImGui::SliderFloat3("dir", glm::value_ptr(((DirectionLight*)light)->dir), -1.0, 1.0);
            ImGui::SliderFloat("lenth", &(((DirectionLight*)light)->lenth), 0.5, 1000.0);
        }
    }
    RenderText* renderText = render->GetRenderText();
    if (ImGui::CollapsingHeader("Shadow"))
    {
        //shadow
        SHADOWTYPE shadowType = renderText->shadowInf.shadow_type;
        ImGui::Text("SHADOW");
        const char* items[] = { "None", "Normal", "PCF", "PCSS" };
        int item_current = int(shadowType);
        ImGui::ListBox("ShadowType", &item_current, items, IM_ARRAYSIZE(items), 3);
        shadowType = (SHADOWTYPE)item_current;
        renderText->shadowInf.shadow_type = shadowType;
        if (shadowType != SHADOWTYPE::NONE)
        {
            ImGui::SliderFloat("biascontrol", &(renderText->shadowInf.biascontrol), 0.2, 2.0);
            ImGui::SliderFloat("biasoffset", &(renderText->shadowInf.biasoffset), 0.001, 0.005);
            if (shadowType == SHADOWTYPE::PCF || shadowType == SHADOWTYPE::PCSS)
            {
                ImGui::SliderInt("core_half", &(renderText->shadowInf.core_half), 0, 5);
            }
        }
    }
    //hdr
    if (ImGui::CollapsingHeader("HDR"))
    {
        ImGui::Text("HDR");
        HdrType hdrType = renderText->hdrInf.hdr_type;
        const char* items[] = { "None", "Reinhard", "Exp", "Flimic", "Aces" };
        int item_current = int(hdrType);
        ImGui::ListBox("HdrType", &item_current, items, IM_ARRAYSIZE(items), 3);
        hdrType = (HdrType)item_current;
        renderText->hdrInf.hdr_type = hdrType;
        if (hdrType == HdrType::HDR_EXP)
            ImGui::SliderFloat("exposure", &(renderText->hdrInf.exposure), 0.2, 2.0);
    }
    
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
bool ImguiWindow::RunningCheck() const
{
    return glfwWindowShouldClose(glfwWindow);
}
void ImguiWindow::SwapBuffer()
{
    glfwSwapBuffers(glfwWindow);
}
void framebuffer_size_callback(GLFWwindow* window, int _width, int _height)
{
    imguiWindow->WindowSizeChangeCallback(_width, _height);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    imguiWindow->ScrollCallback(yoffset);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    imguiWindow->MouseCallback(xposIn, yposIn);
}