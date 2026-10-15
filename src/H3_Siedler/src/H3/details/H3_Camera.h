#pragma once
#include <de/gpu/Camera.h>

/**
 * Berechnet den Schnittpunkt einer Geraden mit der Ebene y = 0.
 *
 * @param linePoint  Ein Punkt auf der Geraden (z.B. Kameraposition)
 * @param lineDir    Der Richtungsvektor der Geraden (muss nicht normiert sein)
 * @return           Der Schnittpunkt als glm::dvec3 oder std::nullopt, falls kein eindeutiger Schnittpunkt existiert.
 */
inline std::optional<glm::dvec3>
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


inline float
computeCameraAngleXZ(de::gpu::Camera* camera)
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

    double cylinderCamAngleY = atan2(camDir.z, camDir.x); // - de::Math::PI64;
    cylinderCamAngleY = de::Math::wrapAngleToRange0_2pi(cylinderCamAngleY);

    return cylinderCamAngleY;
}
