#pragma once
#include <vector>
#include <glm/glm.hpp>

class BezierPath {
private:
    std::vector<glm::vec3> m_controlPoints;

    static glm::vec3 evaluateCubicBezier(const glm::vec3& p0, const glm::vec3& p1,
        const glm::vec3& p2, const glm::vec3& p3,
        float t) {
        float u = 1.0f- t;
        float tt = t*t;
        float uu = u*u;
        float uuu = uu*u;
        float ttt = tt*t;

        glm::vec3 p = uuu * p0;           // (1-t)^3 * P0
        p += 3.0f * uu * t * p1;         // 3*(1-t)^2 * t * P1
        p += 3.0f * u * tt * p2;         // 3*(1-t) * t^2 * P2
        p += ttt * p3;                   // t^3 * P3

        return p;
    }

public:
    BezierPath() = default;

    BezierPath(const std::vector<glm::vec3>& controlPoints)
        : m_controlPoints(controlPoints) {
    }

    // num of cubic segments
    size_t getSegmentCount() const {
        if (m_controlPoints.size() < 4) return 0;
        return (m_controlPoints.size() - 1) / 3;
    }

    // evaluate overall path at parametric distance u in range [0, numSegments]
    glm::vec3 getPoint(float globalT) const {
        size_t numSegments = getSegmentCount();
        if (numSegments == 0) return glm::vec3(0.0f);

        // Clamp global parameter
        if (globalT <= 0.0f) return m_controlPoints.front();
        if (globalT >= (float)numSegments) return m_controlPoints.back();

        size_t segmentIdx = static_cast<size_t>(globalT);
        if (segmentIdx >= numSegments) segmentIdx = numSegments - 1;

        float localT = globalT - static_cast<float>(segmentIdx);
        size_t baseIdx = segmentIdx * 3;

        return evaluateCubicBezier(
            m_controlPoints[baseIdx],
            m_controlPoints[baseIdx + 1],
            m_controlPoints[baseIdx + 2],
            m_controlPoints[baseIdx + 3],
            localT
        );
    }

    const std::vector<glm::vec3>& getControlPoints() const { return m_controlPoints; }

    // Generates line vertex positions for rendering the curve
    std::vector<glm::vec3> generateLineVertices(int samplesPerSegment = 20) const {
        std::vector<glm::vec3> vertices;
        size_t numSegments = getSegmentCount();
        if (numSegments == 0) return vertices;

        for (size_t i = 0; i < numSegments; ++i) {
            for (int step = 0; step <= samplesPerSegment; ++step) {
                float localT = static_cast<float>(step) / samplesPerSegment;
                float globalT = static_cast<float>(i) + localT;
                vertices.push_back(getPoint(globalT));
            }
        }
        return vertices;
    }
};