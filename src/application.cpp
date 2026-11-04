//#include "Image.h"
#include <framework/trackball.h>
#include "mesh.h"

#include "texture.h"
#include "camera/camera.h"
#include "objects/PrimitiveFactory.h"
#include "objects/GasStation.h"
#include "objects/Human.h"
#include "objects/Customer.h"

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
    std::unique_ptr<Human> cashier;
    std::unique_ptr<Human> customer;
    Customer customerController;

    Application()
        : m_window("Final Project", glm::ivec2(1024, 1024), OpenGLVersion::GL41)
        , m_camera {&m_window, glm::vec3(0.0f, 1.1f, 0.9f), -glm::vec3(1.2f, 1.1f, 0.9f) }  // Default camera position for now
        , m_trackball(&m_window, glm::radians(50.0f), 18.0f, glm::radians(35.0f), glm::radians(45.0f))
        , m_texture(RESOURCE_ROOT "resources/checkerboard.png")
        , m_sunMesh(PrimitiveFactory::createSphere())
    {
        cashier = std::make_unique<Human>();
        cashier->setPosition(glm::vec3(-3.8f, 1.45f, -3.0f));
        customer = std::make_unique<Human>();
        customer->setPosition(glm::vec3(-7.5f, 1.45f, 0.0f));
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

        // --- Initialize Shadow Map Framebuffer ---
        glGenFramebuffers(1, &m_shadowFramebuffer);
        glGenTextures(1, &m_shadowTex);

        glBindTexture(GL_TEXTURE_2D, m_shadowTex);
        // Use depth component to store depth values from the light's perspective
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOWTEX_WIDTH, SHADOWTEX_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // Clamp to border ensures areas outside the shadow map are fully lit
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFramebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowTex, 0);

        // Tell OpenGL we are only storing depth, no color
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

        } catch (ShaderLoadingException e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void update()
    {
        float lastFrameTime = static_cast<float>(glfwGetTime());
        int dummyInteger = 0; // Initialized to 0
        while (!m_window.shouldClose()) {
            // This is your game loop
            // Put your real-time logic and rendering in here
            m_window.updateInput();
            float timeInSeconds = static_cast<float>(glfwGetTime());
            float deltaTime = timeInSeconds - lastFrameTime;
            lastFrameTime = timeInSeconds;

            float orbitSpeed = timeInSeconds * 0.5f;
            float orbitRadius = 15.0f;
            m_sunPosition = glm::vec3(std::sin(orbitSpeed) * orbitRadius, std::cos(orbitSpeed) * orbitRadius, 1.0f); // straight orbit

            if (m_sunPosition.y > 0.0f) {
                m_sunColor = glm::vec3(1.0f, 0.95f, 0.8f); // Warm daytime sun
            } else {
                m_sunColor = glm::vec3(0.1f, 0.2f, 0.4f); // Cool night sky
            }

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
                ImGui::Separator();
                ImGui::Checkbox("Enable Sun (Directional Light)", &m_enableSun);
                ImGui::Checkbox("Enable Interior Lights (Point Lights)", &m_enableInteriorLights);

                ImGui::InputInt("This is an integer input", &dummyInteger); // Use ImGui::DragInt or ImGui::DragFloat for larger range of numbers.
                ImGui::Text("Value is: %i", dummyInteger); // Use C printf formatting rules (%i is a signed integer)
                ImGui::Checkbox("Use material if no texture", &m_useMaterial);
                ImGui::End();
            }
            glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

            const glm::mat4 mvpMatrix = m_projectionMatrix * m_viewMatrix * m_modelMatrix;


            customerController.update(*customer, *cashier, timeInSeconds, deltaTime);
            
            //human.movePosition(deltaTime, 3.0f, glm::vec3(1.0f, 0.0f, 1.0f));//diagonal
            cashier->draw(m_defaultShader, mvpMatrix);
            customer->draw(m_defaultShader, mvpMatrix);






            // Normals should be transformed differently than positions (ignoring translations + dealing with scaling):
            // https://paroj.github.io/gltut/Illumination/Tut09%20Normal%20Transformation.html
            // const glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(m_modelMatrix));

            // shadow mapping setup
            glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 30.0f);
            glm::mat4 lightView = glm::lookAt(m_sunPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 lightVP = lightProjection * lightView;

            // Pass 1: Render Shadow Map (Depth from Sun)
            m_shadowShader.bind();
            glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFramebuffer);
            glViewport(0, 0, SHADOWTEX_WIDTH, SHADOWTEX_HEIGHT);
            glClear(GL_DEPTH_BUFFER_BIT); // Only clear depth

            for (size_t i = 0; i < m_meshes.size(); ++i) {
                bool isTransparent = (i < m_meshColors.size() && m_meshColors[i].a < 0.99f);
                if (isTransparent) continue;

                glm::mat4 modelMatrix = m_modelMatrix * m_meshTransforms[i];
                glm::mat4 lightMVP = lightVP * modelMatrix;

                glUniformMatrix4fv(m_shadowShader.getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(lightMVP));
                m_meshes[i].draw(m_shadowShader);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // Pass 2: Render Main Scene from Camera
            glm::ivec2 windowSize = m_window.getWindowSize();
            glViewport(0, 0, windowSize.x, windowSize.y);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_shadowTex);
            glActiveTexture(GL_TEXTURE0);

            for (size_t i = 0; i < m_meshes.size(); ++i) {
                glm::mat4 modelMatrix = m_modelMatrix * m_meshTransforms[i];
                const glm::mat4 mvpMatrix = m_projectionMatrix * m_viewMatrix * modelMatrix;
                const glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));

                glm::mat4 lightMVP = lightVP * modelMatrix;

                Shader* currentShader;

                if (i >= 1 && i <= 4) {
                    currentShader = &m_brickShader;
                } else {
                    currentShader = &m_colorShader;
                }

                currentShader->bind();
                glUniformMatrix4fv(currentShader->getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
                glUniformMatrix3fv(currentShader->getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));
                glUniformMatrix4fv(currentShader->getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
                glUniformMatrix4fv(currentShader->getUniformLocation("lightMVP"), 1, GL_FALSE, glm::value_ptr(lightMVP));
                glUniform1i(currentShader->getUniformLocation("texShadow"), 1); // Slot 1

                glUniform3fv(currentShader->getUniformLocation("cameraPos"), 1, glm::value_ptr(m_camera.cameraPos()));
                glUniform3fv(currentShader->getUniformLocation("sunPos"), 1, glm::value_ptr(m_sunPosition));
                glUniform3fv(currentShader->getUniformLocation("sunColor"), 1, glm::value_ptr(m_sunColor));

                glUniform1i(currentShader->getUniformLocation("enableSun"), m_enableSun ? 1 : 0);
                glUniform1i(currentShader->getUniformLocation("enableInteriorLights"), m_enableInteriorLights ? 1 : 0);

                glUniform3fv(currentShader->getUniformLocation("pointLightPositions"), 1, glm::value_ptr(m_pointLightPositions[0]));
                glUniform3fv(currentShader->getUniformLocation("pointLightColors"), 1, glm::value_ptr(m_pointLightColors[0]));
                glUniform3fv(currentShader->getUniformLocation("spotlightDirections"), 1, glm::value_ptr(m_spotlightDirections[0]));

                if (currentShader == &m_colorShader && i < m_meshColors.size()) {
                    glUniform4fv(currentShader->getUniformLocation("baseColor"), 1, glm::value_ptr(m_meshColors[i]));
                }


                bool isMetal = false;
                float roughness = 0.5f;
                bool isTransparent = (i < m_meshColors.size() && m_meshColors[i].a < 0.99f);

                if (i == 8) {
                    isMetal = true;
                    roughness = 0.2f;
                } else if (i == 4) {
                    isMetal = false;
                    roughness = 0.6f;
                } else if (isTransparent) {
                    isMetal = false;
                    roughness = 0.05f;
                } else if (currentShader == &m_brickShader) {
                    isMetal = false;
                    roughness = 0.5f;
                }

                glUniform1f(currentShader->getUniformLocation("isMetal"), isMetal ? 1.0f : 0.0f);
                glUniform1f(currentShader->getUniformLocation("roughness"), roughness);

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

            if (m_enableSun) {
                m_sunMesh.draw(m_colorShader);
            }

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

    GLuint m_shadowFramebuffer;
    GLuint m_shadowTex;
    const int SHADOWTEX_WIDTH = 2048;
    const int SHADOWTEX_HEIGHT = 2048;

    bool m_enableSun = true;
    bool m_enableInteriorLights = true;

    glm::vec3 m_pointLightPositions[1] = {
        glm::vec3(0.0f, 4.8f, 0.0f)     // Overhead fixture position
    };
    glm::vec3 m_pointLightColors[1] = {
        glm::vec3(1.0f, 0.7f, 0.9f)     // Warm light color
    };
    // Add spotlight directions (pointing straight down into the store)
    glm::vec3 m_spotlightDirections[1] = {
        glm::vec3(0.0f, -1.0f, 0.0f)    // Aimed straight down
    };

    // Define 4 interior point lights (one in center, 3 near walls/corners)
    // glm::vec3 m_pointLightPositions[4] = {
    //     glm::vec3(0.0f, 4.8f, 0.0f),    // Center overhead
    //     glm::vec3(-4.0f, 4.0f, -4.0f),  // Back left corner
    //     glm::vec3(4.0f, 4.0f, -4.0f),   // Back right corner
    //     glm::vec3(4.0f, 4.0f, 4.0f)     // Front right corner
    // };
    // glm::vec3 m_pointLightColors[4] = {
    //     glm::vec3(1.0f, 0.9f, 0.7f), // Warm white
    //     glm::vec3(1.0f, 0.9f, 0.7f),
    //     glm::vec3(1.0f, 0.9f, 0.7f),
    //     glm::vec3(1.0f, 0.9f, 0.7f)
    // };

};

int main()
{
    Application app;
    app.update();

    return 0;
}
