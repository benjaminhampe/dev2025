#pragma once
#include <de/gpu/Camera.h>

/**
 * Berechnet den Schnittpunkt einer Geraden mit der Ebene y = 0.
 *
 * @param linePoint  Ein Punkt auf der Geraden (z.B. Kameraposition)
 * @param lineDir    Der Richtungsvektor der Geraden (muss nicht normiert sein)
 * @return           Der Schnittpunkt als glm::dvec3 oder std::nullopt, falls kein eindeutiger Schnittpunkt existiert.
 */
std::optional<glm::dvec3>
intersectWithYZeroPlane(const glm::dvec3& linePoint, const glm::dvec3& lineDir);

float
computeCameraAngleY_in_degrees(de::gpu::Camera* camera);
