#include <iostream>

#include "src/renderer.h"

#include <filesystem>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(int argc, char *argv[]) {
    Renderer renderer;
    renderer.loadTungstenJSON(argv[1]);
    std::filesystem::remove("out.png");
    renderer.render();

//#ifdef _WIN32
//    //system("albedo_img.png");
//    //system("normal_img.png");
//    system("oidn_img.png");
//    system("traced_img.png");
//#elif __APPLE__
//    //system("open albedo_img.png");
//    //system("open normal_img.png");
//    system("open oidn_img.png");
//    system("open traced_img.png");
//#endif

    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;

    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(1200, 900, argv[1], NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, glfw_key_callback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    bool show_render_result = true;

    int tx_tab = 1;
    int gl_tx_resx = renderer.frame.resx;
    int gl_tx_resy = renderer.frame.resy;
    auto color_buf = renderer.frame.tonemap(FrameBuffer::COLOR);
    auto oidn_buf = renderer.frame.tonemap(FrameBuffer::OIDN);
    GLuint gl_tx_oidn, gl_tx_color;
    glGenTextures(1, &gl_tx_color);
    glBindTexture(GL_TEXTURE_2D, gl_tx_color);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, gl_tx_resx, gl_tx_resy, 0, GL_RGB, GL_UNSIGNED_BYTE, color_buf.data());
    glGenTextures(1, &gl_tx_oidn);
    glBindTexture(GL_TEXTURE_2D, gl_tx_oidn);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, gl_tx_resx, gl_tx_resy, 0, GL_RGB, GL_UNSIGNED_BYTE, oidn_buf.data());

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        if (show_render_result) {
            ImGui::Begin("OpenGL Texture Text");
            ImGui::SameLine();
            if (ImGui::Button("color", ImVec2(150, 25)))
            {
                tx_tab = 1;
            }
            ImGui::SameLine();
            if (ImGui::Button("oidn", ImVec2(150, 25)))
            {
                tx_tab = 2;
            }
            switch (tx_tab) {
            case 1:
                ImGui::Image((void*)(intptr_t)gl_tx_color, ImVec2(800, 800));
                break;
            case 2:
                ImGui::Image((void*)(intptr_t)gl_tx_oidn, ImVec2(800, 800));
                break;
            }
            ImGui::End();
        }

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
