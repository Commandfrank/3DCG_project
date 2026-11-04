#include "Customer.h"
#include <iostream>

// moves character toward target waypoint. returns true when waypoint is reached.
bool Customer::moveToWaypoint(Human& human, glm::vec3 target, float timeInSeconds, float deltaTime) {
    glm::vec3 currentPos = human.getPosition(); //returns world postion
    glm::vec3 direction = target - currentPos;
    direction.y = 0.0f;

    float distance = glm::length(direction);
    if (distance < 0.35f) return true;

    float targetAngle = glm::degrees(std::atan2(direction.x, direction.z));
    float currentAngle = human.getRotation().y;
    float angleDiff = std::abs(targetAngle - currentAngle);

    if (angleDiff > 180.0f) angleDiff = 360.0f - angleDiff;

    // turn to face waypoint
    human.turn(deltaTime, 180.0f, targetAngle);

    // only step forward if oriented reasonably close to target angle (prevents sideways/backwards jitter)
    if (angleDiff < 30.0f) human.walk(timeInSeconds, deltaTime, 1.0f, 2.7f);// 1.2f);

    return false;
}

void Customer::update(Human& customer, Human& cashier, float timeInSeconds, float deltaTime) {
    switch (m_state) {
    case CustomerState::Entering:
        comeInside(customer, cashier, timeInSeconds, deltaTime);
        break;
    case CustomerState::Browsing:
        goAroundStore(customer, timeInSeconds, deltaTime);
        break;
    case CustomerState::GoingToCounter:
        goToCounter(customer, cashier, timeInSeconds, deltaTime);
        break;
    case CustomerState::Exiting:
        exitStore(customer, timeInSeconds, deltaTime);
        break;
    case CustomerState::WaitOut:
        waitOutside(customer, cashier, timeInSeconds, deltaTime);
        break;
    }
}


void Customer::comeInside(Human& customer, Human& cashier, float timeInSeconds, float deltaTime) {
    if (m_currentWaypoint >= m_entrancePath.size()) {
        cashier.resetPose();
        m_state = CustomerState::Browsing;
        m_currentWaypoint = 0;
        return;
    }
    
    cashier.waveHand(timeInSeconds);
    if (moveToWaypoint(customer, m_entrancePath[m_currentWaypoint], timeInSeconds, deltaTime)) 
        m_currentWaypoint++;
}

void Customer::goAroundStore(Human& customer, float timeInSeconds, float deltaTime) {
    if (m_browsePath.empty()) return;

    // After finishing browsing path loop, move to counter state
    if (m_currentWaypoint >= m_browsePath.size()) {
        m_state = CustomerState::GoingToCounter;
        m_currentWaypoint = 0;
        return;
    }

    if (moveToWaypoint(customer, m_browsePath[m_currentWaypoint], timeInSeconds, deltaTime)) 
        m_currentWaypoint++;
}

void Customer::goToCounter(Human& customer, Human& cashier, float timeInSeconds, float deltaTime) {
    glm::vec3 counterPosition = cashier.getPosition() + glm::vec3(0.0f, 0.0f, 2.0f); // Stand in front of counter
    if (m_waitTimer == 0.0f) {//move to counter if not arrived yet
        bool reachedCounter = moveToWaypoint(customer, counterPosition, timeInSeconds, deltaTime);
        if (!reachedCounter) return; // Still walking to the counter
    }

    // counter reached, face cashier and interact for 2 seconds
    glm::vec3 dirToCashier = cashier.getPosition() - customer.getPosition();
    float angleToCashier = glm::degrees(std::atan2(dirToCashier.x, dirToCashier.z));
    customer.turn(deltaTime, 180.0f, angleToCashier);

    customer.resetPose();
    customer.waveHand(timeInSeconds);
    cashier.waveHand(timeInSeconds);

    m_waitTimer += deltaTime;
    if (m_waitTimer >= 2.0f) {
        customer.resetPose();
        cashier.resetPose();
        m_state = CustomerState::Exiting;
        m_currentWaypoint = 0;
        m_waitTimer = 0.0f;
    }
}

void Customer::exitStore(Human& customer, float timeInSeconds, float deltaTime) {
    if (m_currentWaypoint < m_exitPath.size()) {
        if (moveToWaypoint(customer, m_exitPath[m_currentWaypoint], timeInSeconds, deltaTime)) 
            m_currentWaypoint++;
        return;
    }

    customer.resetPose();
    m_state = CustomerState::WaitOut;
    m_currentWaypoint = 0;
    m_waitTimer = 0.0f;
}

void Customer::waitOutside(Human& customer, Human& cashier, float timeInSeconds, float deltaTime) {
    customer.resetPose();

    m_waitTimer += deltaTime;
    if (m_waitTimer >= 5.0f) {
        m_state = CustomerState::Entering;
        m_currentWaypoint = 0;
        m_waitTimer = 0.0f;
    }
}