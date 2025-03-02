#include "object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"

Mesh* object2D::CreateSquare(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* object2D::CreateTrapezoid(const std::string& name, glm::vec3 leftBottomCorner, float baseWidth, float topWidth, float height, glm::vec3 color)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(baseWidth, 0, 0), color),
        VertexFormat(corner + glm::vec3((baseWidth - topWidth) / 2, height, 0), color),
        VertexFormat(corner + glm::vec3((baseWidth + topWidth) / 2, height, 0), color)
    };

    std::vector<unsigned int> indices = { 0, 1, 2, 1, 3, 2 };

    Mesh* trapezoid = new Mesh(name);
    trapezoid->InitFromData(vertices, indices);
    return trapezoid;
}

Mesh* object2D::CreateRectangle(const std::string& name, glm::vec3 leftBottomCorner, float width, float height, glm::vec3 color)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(width, 0, 0), color),
        VertexFormat(corner + glm::vec3(width, height, 0), color),
        VertexFormat(corner + glm::vec3(0, height, 0), color)
    };

    std::vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0 };

    Mesh* rectangle = new Mesh(name);
    rectangle->InitFromData(vertices, indices);
    return rectangle;
}

Mesh* object2D::CreateArc(const std::string& name, float radius, glm::vec3 center, glm::vec3 color, bool fill)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    int numSegments = 100;
    float angleStep = M_PI / numSegments;

    vertices.emplace_back(center, color);

    for (int i = 0; i <= numSegments; ++i) {
        float angle = i * angleStep;
        glm::vec3 point = center + glm::vec3(radius * cos(angle), radius * sin(angle), 0);
        vertices.emplace_back(point, color);
        indices.push_back(i);
    }

    if (fill) {
        for (int i = 1; i <= numSegments; ++i) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }
    }
    else {
        for (int i = 1; i <= numSegments; ++i) {
            indices.push_back(i);
            indices.push_back(i + 1);
        }
    }

    Mesh* arc = new Mesh(name);
    arc->SetDrawMode(fill ? GL_TRIANGLES : GL_LINE_STRIP);
    arc->InitFromData(vertices, indices);
    return arc;
}

Mesh* object2D::CreateCircle(const std::string& name, glm::vec3 center, float radius, glm::vec3 color, int numSegments) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    vertices.emplace_back(center, color);

    for (int i = 0; i < numSegments; ++i) {
        float angle = 2.0f * glm::pi<float>() * i / numSegments;
        float x = center.x + radius * cos(angle);
        float y = center.y + radius * sin(angle);
        vertices.emplace_back(glm::vec3(x, y, 0), color);

        if (i < numSegments - 1) {
            indices.push_back(0);            
            indices.push_back(i + 1);        
            indices.push_back(i + 2);        
        }
        else {
            indices.push_back(0);            
            indices.push_back(i + 1);        
            indices.push_back(1);           
        }
    }

    Mesh* circle = new Mesh(name);
    circle->InitFromData(vertices, indices);
    return circle;
}

Mesh* object2D::CreateRectangleOutline(const std::string& name,glm::vec3 leftBottomCorner,float width,float height,glm::vec3 color){
    std::vector<VertexFormat> vertices = {
        VertexFormat(leftBottomCorner, color),
        VertexFormat(leftBottomCorner + glm::vec3(width, 0, 0), color),
        VertexFormat(leftBottomCorner + glm::vec3(width, height, 0), color),
        VertexFormat(leftBottomCorner + glm::vec3(0, height, 0), color)
    };

    std::vector<unsigned int> indices = { 0, 1, 2, 3, 0 };

    Mesh* rectangleOutline = new Mesh(name);
    rectangleOutline->SetDrawMode(GL_LINE_LOOP);
    rectangleOutline->InitFromData(vertices, indices);
    return rectangleOutline;
}
