//#include "Image.h"
#include <framework/trackball.h>
#include "mesh.h"

#include "texture.h"
#include "camera/camera.h"
#include "objects/PrimitiveFactory.h"
#include "objects/GasStation.h"
#include "objects/Cashier.h"

// Always include window first (because it includes glfw, which includes GL which needs to be included AFTER glew).
// Can't wait for modules to fix this stuff...
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <glad/glad.h>
// Include glad before glfw3
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <imgui/imgui.h>
DISABLE_WARNINGS_POP()
#include <framework/shader.h>
#include <framework/window.h>
#include <functional>
#include <iostream>
#include <vector>

class Application {
public:
    std::unique_ptr<Cashier> cashier;
    Application()
        : m_window("Final Project", glm::ivec2(1024, 1024), OpenGLVersion::GL41)
        , m_camera {&m_window, glm::vec3(0.0f, 1.1f, 0.9f), -glm::vec3(1.2f, 1.1f, 0.9f) }  // Default camera position for now
        , m_trackball(&m_window, glm::radians(50.0f), 18.0f, glm::radians(35.0f), glm::radians(45.0f))
        , m_texture(RESOURCE_ROOT "resources/checkerboard.png")
        , m_sunMesh(PrimitiveFactory::createSphere())
    {
        cashier = std::make_unique<Cashier>();
        m_camera.setUserInteraction(false);
        m_window.registerKeyCallback([this](int key, int scancode, int action, int mods) {
            if (action == GLFW_PRESS)
                onKeyPressed(key, mods);
            else if (action == GLFW_RELEASE)
                onKeyReleased(key, mods);
        });
        m_window.registerMouseMoveCallback(std::bind(&Application::onMouseMove, this, std::placeholders::_1));
        m_window.registerMouseButtonCallback([this](int button, int action, int mods) {
            if (action == GLFW_PRESS)
                onMouseClicked(button, mods);
            else if (action == GLFW_RELEASE)
                onMouseReleased(button, mods);
        });

        m_meshes = GPUMesh::loadMeshGPU(RESOURCE_ROOT "resources/dragon.obj");

        // Mesh cubeMesh = PrimitiveFactory::createCube();
        // Mesh cylinderMesh = PrimitiveFactory::createCylinder();
        // Mesh sphereMesh = PrimitiveFactory::createSphere();
        // m_meshes.push_back(GPUMesh(cubeMesh));
        // m_meshes.push_back(GPUMesh(cylinderMesh));
        // m_meshes.push_back(GPUMesh(sphereMesh));

        // Store transformation matrices for each shape
        // Cube: scale 0.3 and position at x = -0.8
        m_meshTransforms.push_back(glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.3f)), glm::vec3(-2.7f, 0.0f, 0.0f)));
        // Cylinder: scale 0.3 and position at x = 0.0 (center)
        m_meshTransforms.push_back(glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.3f)), glm::vec3(0.0f, 0.0f, 2.7f)));
        // Sphere: scale 0.3 and position at x = 0.8
        m_meshTransforms.push_back(glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.3f)), glm::vec3(2.7f, 0.0f, 0.0f)));

        m_meshes.clear();
        m_meshTransforms.clear();
        m_meshColors.clear();
        GasStation::build(m_meshes, m_meshTransforms, m_meshColors);

        try {
            ShaderBuilder defaultBuilder;
            defaultBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/shader_vert.glsl");
            defaultBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "shaders/shader_frag.glsl");
            m_defaultShader = defaultBuilder.build();

            ShaderBuilder shadowBuilder;
            shadowBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/shadow_vert.glsl");
            shadowBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "Shaders/shadow_frag.glsl");
            m_shadowShader = shadowBuilder.build();

            ShaderBuilder colorBuilder;
            colorBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/shader_vert.glsl");
            colorBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "Shaders/color_frag.glsl");
            m_colorShader = colorBuilder.build();

            ShaderBuilder brickBuilder;
            brickBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/shader_vert.glsl");
            brickBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "Shaders/brick_frag.glsl");
            m_brickShader = brickBuilder.build();

            ShaderBuilder textureBuilder;
            textureBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/shader_vert.glsl");
            textureBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "Shaders/texture_frag.glsl");
            m_textureShader = textureBuilder.build();

            // ShaderBuilder woodBuilder;
            // woodBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/shader_vert.glsl");
            // woodBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "Shaders/wood_frag.glsl");
            // m_woodShader = woodBuilder.build();
            //
            //
            // Any new shaders can be added below in similar fashion.
            // ==> Don't forget to reconfigure CMake when you do!
            //     Visual Studio: PROJECT => Generate Cache for ComputerGraphics
            //     VS Code: ctrl + shift + p => CMake: Configure => enter
            // ....
        } catch (ShaderLoadingException e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void update()
    {
        int dummyInteger = 0; // Initialized to 0
        while (!m_window.shouldClose()) {
            // This is your game loop
            // Put your real-time logic and rendering in here
            m_window.updateInput();

            float timeInSeconds = static_cast<float>(glfwGetTime());

            // --- Sun Position & Day/Night Animation ---
            float orbitSpeed = timeInSeconds * 0.5f;
            float orbitRadius = 15.0f;
            m_sunPosition = glm::vec3(std::sin(orbitSpeed) * orbitRadius, std::cos(orbitSpeed) * orbitRadius, 0.0f);

            if (m_sunPosition.y > 0.0f) {
                m_sunColor = glm::vec3(1.0f, 0.95f, 0.8f); // Warm daytime sun
            } else {
                m_sunColor = glm::vec3(0.1f, 0.2f, 0.4f); // Cool night sky
            }

            // Use ImGui for easy input/output of ints, floats, strings, etc...
            if (m_showUI) {
                ImGui::SetNextWindowSize(ImVec2(350,150), ImGuiCond_FirstUseEver);
                ImGui::Begin("Window");
                ImGui::Text("Camera Mode");
                if (ImGui::RadioButton(("Trackball"), &m_activeCamera, 0)) {
                    m_camera.setUserInteraction(false);
                }
                ImGui::SameLine();
                if (ImGui::RadioButton(("First-Person"), &m_activeCamera, 1)) {
                    m_camera.setUserInteraction(true);
                }
                ImGui::InputInt("This is an integer input", &dummyInteger); // Use ImGui::DragInt or ImGui::DragFloat for larger range of numbers.
                ImGui::Text("Value is: %i", dummyInteger); // Use C printf formatting rules (%i is a signed integer)
                ImGui::Checkbox("Use material if no texture", &m_useMaterial);
                ImGui::End();
            }
            // Clear the screen
            glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // ...
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            if (m_activeCamera == 0) {
                m_viewMatrix = m_trackball.viewMatrix();
                m_projectionMatrix = m_trackball.projectionMatrix();
            } else {
                m_camera.updateInput();
                m_viewMatrix = m_camera.viewMatrix();
            }


            // const glm::mat4 mvpMatrix = m_projectionMatrix * m_viewMatrix * m_modelMatrix;
            const glm::mat4 mvpMatrix = m_projectionMatrix * m_viewMatrix * m_modelMatrix;

            //cashier->walk(timeInSeconds, 1.0f);
            cashier->waveHand(timeInSeconds);
            cashier->draw(m_defaultShader, mvpMatrix);
            // Normals should be transformed differently than positions (ignoring translations + dealing with scaling):
            // https://paroj.github.io/gltut/Illumination/Tut09%20Normal%20Transformation.html
            // const glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(m_modelMatrix));
            for (size_t i = 0; i < m_meshes.size(); ++i) {
                glm::mat4 modelMatrix = m_modelMatrix * m_meshTransforms[i];
                const glm::mat4 mvpMatrix = m_projectionMatrix * m_viewMatrix * modelMatrix;
                const glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));

                Shader* currentShader;

                if (i >= 1 && i <= 4) {
                    currentShader = &m_brickShader;
                } else {
                    currentShader = &m_colorShader;
                }

                currentShader->bind();
                glUniformMatrix4fv(currentShader->getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
                //Uncomment this line when you use the modelMatrix (or fragmentPosition)
                //glUniformMatrix4fv(m_defaultShader.getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
                glUniformMatrix3fv(currentShader->getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));

                if (currentShader == &m_colorShader) {
                    glUniform3fv(currentShader->getUniformLocation("cameraPos"), 1, glm::value_ptr(m_camera.cameraPos()));
                    glUniform3fv(currentShader->getUniformLocation("sunPos"), 1, glm::value_ptr(m_sunPosition));
                    glUniform3fv(currentShader->getUniformLocation("sunColor"), 1, glm::value_ptr(m_sunColor));

                    if (i < m_meshColors.size()) {
                        glUniform4fv(currentShader->getUniformLocation("baseColor"), 1, glm::value_ptr(m_meshColors[i]));
                    }
                }
                // GLint camLoc = currentShader->getUniformLocation("cameraPos");
                // if (camLoc != -1) glUniform3fv(camLoc, 1, glm::value_ptr(m_camera.cameraPos()));
                // GLint sunLoc = currentShader->getUniformLocation("sunPos");
                // if (sunLoc != -1) glUniform3fv(sunLoc, 1, glm::value_ptr(m_sunPosition));
                // GLint sunCol = currentShader->getUniformLocation("sunColor");
                // if (sunCol != -1) glUniform3fv(sunCol, 1, glm::value_ptr(m_sunColor));
                // if (i < m_meshColors.size() && currentShader != &m_brickShader) {
                //     glUniform4fv(currentShader->getUniformLocation("baseColor"), 1, glm::value_ptr(m_meshColors[i]));
                // }

                // if (m_meshes[i].hasTextureCoords()) {
                //     m_texture.bind(GL_TEXTURE0);
                //     glUniform1i(m_defaultShader.getUniformLocation("colorMap"), 0);
                //     glUniform1i(m_defaultShader.getUniformLocation("hasTexCoords"), GL_TRUE);
                //     glUniform1i(m_defaultShader.getUniformLocation("useMaterial"), GL_FALSE);
                // } else {
                //     glUniform1i(m_defaultShader.getUniformLocation("hasTexCoords"), GL_FALSE);
                //     glUniform1i(m_defaultShader.getUniformLocation("useMaterial"), m_useMaterial);
                // }

                bool isTransparent = {i < m_meshColors.size() && m_meshColors[i].a < 0.99f};
                if (isTransparent) {
                    glDepthMask(GL_FALSE);
                } else {
                    glDepthMask(GL_TRUE);
                }

                m_meshes[i].draw(*currentShader);
            }
            glDepthMask(GL_TRUE);
            // Sun
            glm::mat4 sunModelMatrix = glm::translate(glm::mat4(1.0f), m_sunPosition);
            sunModelMatrix = glm::scale(sunModelMatrix, glm::vec3(1.5f));

            const glm::mat4 sunMvpMatrix = m_projectionMatrix * m_viewMatrix * sunModelMatrix;
            const glm::mat3 sunNormalMatrix = glm::inverseTranspose(glm::mat3(sunModelMatrix));

            m_colorShader.bind();
            glUniformMatrix4fv(m_colorShader.getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(sunMvpMatrix));
            glUniformMatrix3fv(m_colorShader.getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(sunNormalMatrix));

            glUniform3fv(m_colorShader.getUniformLocation("cameraPos"), 1, glm::value_ptr(m_camera.cameraPos()));
            glUniform3fv(m_colorShader.getUniformLocation("sunPos"), 1, glm::value_ptr(m_sunPosition));
            glUniform3fv(m_colorShader.getUniformLocation("sunColor"), 1, glm::value_ptr(m_sunColor));

            glm::vec4 sunDrawColor = glm::vec4(m_sunColor, 1.0f);
            glUniform4fv(m_colorShader.getUniformLocation("baseColor"), 1, glm::value_ptr(sunDrawColor));

            m_sunMesh.draw(m_colorShader);

            // Processes input and swaps the window buffer
            m_window.swapBuffers();

        }
    }


    // In here you can handle key presses
    // key - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__keys.html
    // mods - Any modifier keys pressed, like shift or control
    void onKeyPressed(int key, int mods)
    {
        if (key == '\\') {
            m_showUI = !m_showUI;
        }
        std::cout << "Key pressed: " << key << std::endl;
    }

    // In here you can handle key releases
    // key - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__keys.html
    // mods - Any modifier keys pressed, like shift or control
    void onKeyReleased(int key, int mods)
    {
        std::cout << "Key released: " << key << std::endl;
    }

    // If the mouse is moved this function will be called with the x, y screen-coordinates of the mouse
    void onMouseMove(const glm::dvec2& cursorPos)
    {
        std::cout << "Mouse at position: " << cursorPos.x << " " << cursorPos.y << std::endl;
    }

    // If one of the mouse buttons is pressed this function will be called
    // button - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__buttons.html
    // mods - Any modifier buttons pressed
    void onMouseClicked(int button, int mods)
    {
        std::cout << "Pressed mouse button: " << button << std::endl;
    }

    // If one of the mouse buttons is released this function will be called
    // button - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__buttons.html
    // mods - Any modifier buttons pressed
    void onMouseReleased(int button, int mods)
    {
        std::cout << "Released mouse button: " << button << std::endl;
    }

private:
    Window m_window;
    Camera m_camera;
    Trackball m_trackball;
    int m_activeCamera = 0;
    bool m_showUI = true;

    // Shader for default rendering and for depth rendering
    Shader m_defaultShader;
    Shader m_shadowShader;
    Shader m_colorShader;
    Shader m_brickShader;
    Shader m_textureShader;
    // Shader m_woodShader;

    std::vector<GPUMesh> m_meshes;
    std::vector<glm::mat4> m_meshTransforms;
    std::vector<glm::vec4> m_meshColors;
    Texture m_texture;
    bool m_useMaterial { true };

    // Projection and view matrices for you to fill in and use
    glm::mat4 m_projectionMatrix = glm::perspective(glm::radians(80.0f), 1.0f, 0.1f, 30.0f);
    glm::mat4 m_viewMatrix = glm::lookAt(glm::vec3(-1, 1, -1), glm::vec3(0), glm::vec3(0, 1, 0));
    glm::mat4 m_modelMatrix { 1.0f };

    GPUMesh m_sunMesh;
    glm::vec3 m_sunPosition;
    glm::vec3 m_sunColor;
};

int main()
{
    Application app;
    app.update();

    return 0;
}
