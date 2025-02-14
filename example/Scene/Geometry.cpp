#include "Geometry.hpp"

#include <numbers>

#pragma region "Cube index and vertex data"

std::vector<uint32_t> Cube::sCubeIndices = {
     0,  1,  2,  2,  3,  0,
     4,  5,  6,  6,  7,  4,
     8,  9, 10, 10, 11,  8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20,
};

std::vector<BasicVertex> Cube::sCubeVertices = {
    {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
    {{-1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
    {{1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
    {{1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
    {{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
    {{1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
    {{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-1.0f, -1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{-1.0f, -1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{-1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
    {{-1.0f, 1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
    {{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{1.0f, 1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
    {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
    {{-1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
    {{1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
    {{1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
    {{-1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
    {{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
    {{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
};

#pragma endregion

Cube::Cube(const Params& params)
: Geometry()
{
    mVertices    = generateVertices(params.scale);
    mVertexCount = static_cast<uint32_t>(mVertices.size());

    mIndices     = sCubeIndices;
    mIndexCount  = static_cast<uint32_t>(sCubeIndices.size());

    mName = "Cube";
}

std::vector<BasicVertex> Cube::generateVertices(const float scale)
{
    std::vector<BasicVertex> vertices = Cube::sCubeVertices;
    for (auto& vertex : vertices)
    {
        vertex.position *= scale;
    }
    return vertices;
}

Sphere::Sphere(const Params& params)
: Geometry()
{
    mVertices    = generateVertices(params.tesselationX, params.tesselationY, params.radius);
    mVertexCount = static_cast<uint32_t>(mVertices.size());

    mIndices     = generateIndices(params.tesselationX, params.tesselationY);
    mIndexCount  = static_cast<uint32_t>(mIndices.size());

    mName = "Sphere";
}

std::vector<BasicVertex> Sphere::generateVertices(const int32_t stackCount, const int32_t sectorCount, const float radius)
{
    std::vector<glm::vec3> vertices, normals;
    std::vector<glm::vec2> uvs;

    float sector_step = 2.0f * std::numbers::pi_v<float> / (float) sectorCount;
    float stack_step  = std::numbers::pi_v<float> / (float) stackCount;
    float sector_angle, stack_angle;

    float r_inv = 1.0f / radius;

    for (int32_t i = 0; i <= stackCount; i++)
    {
        float x, y, z, xy;
        stack_angle = std::numbers::pi_v<float> / 2 - (float) i * stack_step;
        xy = radius * cosf(stack_angle);
        z = radius * sinf(stack_angle);

        for (int32_t j = 0; j <= sectorCount; j++)
        {
            sector_angle = (float) j * sector_step;

            x = xy * cosf(sector_angle);
            y = xy * sinf(sector_angle);

            vertices.emplace_back(x, y, z);
            normals.emplace_back(x * r_inv, y * r_inv, z * r_inv);
            uvs.emplace_back((float) j / (float) sectorCount, (float) i / (float) stackCount);
        }
    }

    std::vector<BasicVertex> result;
    for (uint32_t i = 0; i < vertices.size(); i++)
    {
        result.push_back({ vertices[i], normals[i], uvs[i] });
    }
    return result;
}

std::vector<uint32_t> Sphere::generateIndices(const int32_t stackCount, const int32_t sectorCount)
{
    std::vector<uint32_t> indices;
    uint32_t k1, k2;

    for (int32_t i = 0; i < stackCount; i++)
    {
        k1 = i * (sectorCount + 1);
        k2 = k1 + sectorCount + 1;

        for (int32_t j = 0; j < sectorCount; j++, k1++, k2++)
        {
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if (i != (stackCount - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
    return indices;
}
