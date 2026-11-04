#include "H3_Camera.h"

std::optional<glm::dvec3>
intersectWithYZeroPlane(const glm::dvec3& linePoint, const glm::dvec3& lineDir)
{
    // Wenn die Richtung parallel zur y=0 Ebene ist (also dir.y == 0),
    // gibt es entweder keinen oder unendlich viele Schnittpunkte.
    // Ein kleiner Schwellenwert (Epsilon) fängt Fließkomma-Ungenauigkeiten ab.
    if (std::abs(lineDir.y) < 1e-9)
    {
        return std::nullopt;
    }

    // Strahlensatz / Lineare Interpolation für die y-Ebene:
    // linePoint.y + t * lineDir.y = 0  =>  t = -linePoint.y / lineDir.y
    double t = -linePoint.y / lineDir.y;

    // Schnittpunkt berechnen: P = Start + t * Richtung
    return linePoint + t * lineDir;
}

float
computeCameraAngleY_in_degrees(de::gpu::Camera* camera)
{
    auto camPos = camera->getPos();
    auto camTar = camera->getTarget();
    auto camDir = camTar - camPos;

    auto S = intersectWithYZeroPlane( camPos, camDir);
    if (S)
    {
        camTar = *S; // Ground is intersection point
    }
    else
    {
        camTar.y = 0.0; // Make it ground instead.
    }

    camDir = camTar - camPos; // Update

    double cylinderCamAngleY = atan2(camDir.z, camDir.x); // - (0.5 * M_PI);

    return de::Math::wrapAngle0_360(glm::degrees(cylinderCamAngleY));

/*
    de::TRSf trs;
    trs.pos = tile.trsChip.pos;
    trs.rotate.y = phi;
    trs.update();

    m_instanceMat.push_back( trs.modelMat );

    //DE_DEBUG("phi = ",phi, ", instances = ",m_instanceMat.size())
    if (m_instanceMat.size())
    {
    // DE_DEBUG("instances[0].",m_instanceMat.front()[0])
    // DE_DEBUG("instances[1].",m_instanceMat.front()[1])
    // DE_DEBUG("instances[2].",m_instanceMat.front()[2])
    // DE_DEBUG("instances[3].",m_instanceMat.front()[3])

        // 1. Matrix in Quaternion umwandeln
    glm::quat rotationQuat = glm::quat_cast(m_instanceMat.front());

    // 2. Yaw (Y-Winkel) aus dem Quaternion extrahieren (in Radianten)
    float yawRad = glm::yaw(rotationQuat);

    // Optional: In Grad umwandeln
    float yawDeg = glm::degrees(yawRad);

    DE_DEBUG("yawRad = ",yawRad, ", yawDeg = ",yawDeg)
    }
*/
}
