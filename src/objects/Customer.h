#pragma once
#include "Human.h"
#include <vector>
#include <glm/glm.hpp>

enum class CustomerState {
    Entering,
    Browsing,
    GoingToCounter,
    Exiting,
    WaitOut
};

class Customer{
private:
    CustomerState m_state = CustomerState::Entering;
    size_t m_currentWaypoint = 0;
    float m_waitTimer = 0.0f; // to track pause duration at counter

    // approach outer door and cross threshold into store
    std::vector<glm::vec3> m_entrancePath = {
        glm::vec3(-7.5f, 1.45f, 0.0f),  // outside front dor
        glm::vec3(-1.0f, 1.45f, 0.0f)   // a bit further
    };

    // circular path around the store aisles
    std::vector<glm::vec3> m_browsePath = {
        glm::vec3(-1.0f, 1.45f,  0.0f),  //center
        glm::vec3(0.0f, 1.45f,  3.4f),  // right down
        glm::vec3(3.2f, 1.45f,  2.8f),  // right up
        glm::vec3(3.2f, 1.45f, -3.2f),  // left up
        glm::vec3(0.25f, 1.45f, -3.2f),   // left down
        glm::vec3(0.25f, 1.45f,  0.0f)   // a bit towards right to not collide with the thing
    };

    // walk to inner door -> step outside door and stop
    std::vector<glm::vec3> m_exitPath = {
        glm::vec3(-3.8f, 1.45f, 0.0f),  // Inside door position
        glm::vec3(-5.2f, 1.45f, 0.0f),  // Outside door frame
        glm::vec3(-7.5f, 1.45f, 0.0f)   // Parking lot exit point
    };

    bool moveToWaypoint(Human& human, glm::vec3 target, float timeInSeconds, float deltaTime);


public:
    Customer() = default;
    void update(Human& customer, Human& cashier, float timeInSeconds, float deltaTime);

    void comeInside(Human& custoemr, Human& cashier, float timeInSeconds, float deltaTime);
    void goAroundStore(Human& customer, float timeInSeconds, float deltaTime);
    void goToCounter(Human& customer, Human& cashier, float timeInSeconds, float deltaTime);
    void exitStore(Human& customer, float timeInSeconds, float deltaTime);
    void waitOutside(Human& customer, Human& cashier, float timeInSeconds, float deltaTime);


    CustomerState getState() const { return m_state; }
};