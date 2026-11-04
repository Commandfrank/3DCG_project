#include "Customer.h"
#include <iostream>
#include <cmath>

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

void Customer::update(Human& customer, Human& cashier, bool bezier, float timeInSeconds, float deltaTime) {
    switch (m_state) {
    case CustomerState::Entering:
        comeInside(customer, cashier, bezier, timeInSeconds, deltaTime);
        break;
    case CustomerState::Browsing:
        goAroundStore(customer, bezier, timeInSeconds, deltaTime);
        break;
    case CustomerState::GoingToCounter:
        goToCounter(customer, cashier, bezier, timeInSeconds, deltaTime);
        break;
    case CustomerState::Exiting:
        exitStore(customer, bezier, timeInSeconds, deltaTime);
        break;
    case CustomerState::WaitOut:
        waitOutside(customer, cashier, bezier, timeInSeconds, deltaTime);
        break;
    }
}


void Customer::comeInside(Human& customer, Human& cashier, bool bezier, float timeInSeconds, float deltaTime) {
    cashier.waveHand(timeInSeconds);

    bool reachedEnd = bezier
        ? moveAlongBezier(customer, m_entrancePath, timeInSeconds, deltaTime)
        : (moveToWaypoint(customer, m_entrancePath[m_currentWaypoint], timeInSeconds, deltaTime) && (++m_currentWaypoint >= m_entrancePath.size()));

    if (reachedEnd) {
        cashier.resetPose();
        m_state = CustomerState::Browsing;
        m_currentWaypoint = 0;
        m_bezierT = 0.0f;
    }
}

void Customer::goAroundStore(Human& customer, bool bezier, float timeInSeconds, float deltaTime) {
if (m_browsePath.empty()) return;

    bool reachedEnd = bezier
        ? moveAlongBezier(customer, m_browsePath, timeInSeconds, deltaTime)
        : (moveToWaypoint(customer, m_browsePath[m_currentWaypoint], timeInSeconds, deltaTime) && (++m_currentWaypoint >= m_browsePath.size()));

    if (reachedEnd) {
        m_state = CustomerState::GoingToCounter;
        m_currentWaypoint = 0;
        m_bezierT = 0.0f;
    }
}

void Customer::goToCounter(Human& customer, Human& cashier, bool bezier, float timeInSeconds, float deltaTime) {
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

void Customer::exitStore(Human& customer, bool bezier, float timeInSeconds, float deltaTime) {
    bool reachedEnd = bezier
        ? moveAlongBezier(customer, m_exitPath, timeInSeconds, deltaTime)
        : (moveToWaypoint(customer, m_exitPath[m_currentWaypoint], timeInSeconds, deltaTime) && (++m_currentWaypoint >= m_exitPath.size()));

    if (reachedEnd) {
        customer.resetPose();
        m_state = CustomerState::WaitOut;
        m_currentWaypoint = 0;
        m_bezierT = 0.0f;
        m_waitTimer = 0.0f;
    }
}

void Customer::waitOutside(Human& customer, Human& cashier, bool bezier, float timeInSeconds, float deltaTime) {
    customer.resetPose();

    m_waitTimer += deltaTime;
    if (m_waitTimer >= 5.0f) {
        m_state = CustomerState::Entering;
        m_currentWaypoint = 0;
        m_waitTimer = 0.0f;
    }
}

////////////////////////////////////////////////bezier/////////////////////////////////

glm::vec3 Customer::evaluateCubicBezier(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t) {
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    return uuu * p0 + 3.0f * uu * t * p1 + 3.0f * u * tt * p2 + ttt * p3;
}

glm::vec3 Customer::evaluateCubicBezierDerivative(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t) {
    float u = 1.0f - t;
    return 3.0f * u * u * (p1 - p0) + 6.0f * u * t * (p2 - p1) + 3.0f * t * t * (p3 - p2);
}

bool Customer::moveAlongBezier(
    Human& human,
    const std::vector<glm::vec3>& path,
    float timeInSeconds,
    float deltaTime)
{
    if (path.size() < 2) return true;

    size_t lastIdx = path.size() - 1;

    if (m_currentWaypoint >= lastIdx) {
        return true;
    }

    glm::vec3 p0 = path[m_currentWaypoint];
    glm::vec3 p3 = path[m_currentWaypoint + 1];

    glm::vec3 dir = p3 - p0;
    dir.y = 0.0f;

    float segmentLen = glm::length(dir);

    if (segmentLen < 0.001f) {
        human.setPosition(p3);
        m_bezierT = 0.0f;
        m_currentWaypoint++;

        return m_currentWaypoint >= lastIdx;
    }

    // perpendicular direction for the curve.
    glm::vec3 perpendicular(-dir.z, 0.0f, dir.x);
    perpendicular = glm::normalize(perpendicular);

    // curve strength
    float curveAmount = segmentLen * 0.2f;

    glm::vec3 p1 = p0 + dir * 0.33f + perpendicular * curveAmount;
    glm::vec3 p2 = p0 + dir * 0.66f + perpendicular * curveAmount;

    float speed = 2.7f;

    m_bezierT += (speed * deltaTime) / segmentLen;

    if (m_bezierT >= 1.0f) m_bezierT = 1.0f;

    glm::vec3 targetPos = evaluateCubicBezier(p0, p1, p2, p3, m_bezierT);
    glm::vec3 tangent = evaluateCubicBezierDerivative(p0, p1, p2, p3, m_bezierT);
    tangent.y = 0.0f;
    human.setPosition(targetPos);
    if (glm::length(tangent) > 0.001f) {
        float targetAngle = glm::degrees(std::atan2(tangent.x, tangent.z));
        human.turn(deltaTime, 360.0f, targetAngle);
    }

    human.walk(timeInSeconds, deltaTime, 1.0f, 0.0f);

    // next segment only after reaching the endpoint.
    if (m_bezierT >= 1.0f) {
        m_bezierT = 0.0f;
        m_currentWaypoint++;

        return m_currentWaypoint >= lastIdx;
    }

    return false;
}