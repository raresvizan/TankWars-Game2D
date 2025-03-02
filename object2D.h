#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object2D
{
    Mesh* CreateSquare(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
    Mesh* CreateRectangle(const std::string& name, glm::vec3 leftBottomCorner, float width, float height, glm::vec3 color);
	Mesh* CreateTrapezoid(const std::string& name, glm::vec3 leftBottomCorner, float baseWidth, float topWidth, float height, glm::vec3 color);
	Mesh* CreateArc(const std::string& name, float radius, glm::vec3 center, glm::vec3 color, bool fill = false);
	Mesh* CreateCircle(const std::string& name, glm::vec3 center, float radius, glm::vec3 color, int numSegments);
	Mesh* CreateRectangleOutline(const std::string& name, glm::vec3 leftBottomCorner, float width, float height, glm::vec3 color);
}