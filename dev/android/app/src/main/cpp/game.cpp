#define OLC_PGE3_APPLICATION
#include "olcPixelGameEngine3.h"

#include <string>
#include <sstream>
#include <android/log.h>

#include "linalg.h"
using namespace linalg::aliases;

#define DEG2RAD(x) ((x) * (M_PI / 180.0f))

static std::vector<std::string> SplitString(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

struct Vertex {
    float4 position, color;
    float2 texCoord;
    float3 normal;
};

class Example : public olc::PixelGameEngine
{
public:
    Example()
    {

    }

protected:
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    GLuint vao, vbo, ebo;

    olc::gpu::Shader_GLSL33 shader;

    float4x4 projMatrix;
    float4 rotation{ 0.0f, 0.0f, 0.0f, 1.0f };

    float yRot = 0.0f;


public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
    {
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glGenVertexArrays(1, &vao);

        LoadModel("monkey.obj");

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t), indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);

        projMatrix = linalg::perspective_matrix<float>(
            DEG2RAD(60.0f),
            static_cast<float>(ScreenSize().x) / static_cast<float>(ScreenSize().y),
            0.01f, 600.0f
        );

        std::string vss = R"(
layout (location = 3) in vec3 aNorm;
out vec3 vNorm;
out vec3 oWPos;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main() {
    vec4 worldPos = modelMatrix * vec4(aPos.xyz, 1.0);
    oWPos = worldPos.xyz;

    mat3 normalMatrix = mat3(modelMatrix);
    vNorm = normalize(normalMatrix * aNorm);

    gl_Position = projMatrix * viewMatrix * worldPos;

    oTex = aTex;
}
)";
        std::string fss = R"(
in vec3 vNorm;
in vec3 oWPos;
void main() {
    vec3 N = normalize(vNorm);

    vec3 L = normalize(-oWPos);

    float diffuse = max(dot(N, L), 0.0);
    pixel = vec4(oTex.x, oTex.y, 0.0, 1.0);
}
)";

        shader.SetVertexShaderSource(
            olc::gpu::Shader::VS_DefaultHeader() + vss
        );
        shader.SetPixelShaderSource(
            olc::gpu::Shader::PS_DefaultHeader() + fss
        );

        std::string result = shader.Compile();
        __android_log_print(
            ANDROID_LOG_ERROR,
            "SHADER COMPILER",
            "%s", result.c_str()
        );

        shader.CreateUniform("modelMatrix");
        shader.CreateUniform("viewMatrix");
        shader.CreateUniform("projMatrix");

		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear whole screen
		draw.Clear(olc::Colour::VERY_DARK_BLUE);

        draw.SetShader(shader);

        float4x4 modelMat = linalg::rotation_matrix(rotation);
        float4x4 viewMat = linalg::translation_matrix(float3{ 0.0f, 0.0f, -5.0f });

        glUniformMatrix4fv(shader.GetUniform("modelMatrix"), 1, GL_FALSE, &modelMat.x.x);
        glUniformMatrix4fv(shader.GetUniform("viewMatrix"), 1, GL_FALSE, &viewMat.x.x);
        glUniformMatrix4fv(shader.GetUniform("projMatrix"), 1, GL_FALSE, &projMatrix.x.x);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);

        rotation = linalg::rotation_quat({ 0.0f, 1.0f, 0.0f }, yRot);
        yRot += fElapsedTime;

		// Successful frame
		return true;
	}

    void LoadModel(const char* sFileName) {
        using VertexIDs = std::tuple<int, int, int>;
        using Face = std::array<VertexIDs, 3>;

        auto modelFileRaw = GetHost()->OpenTextFile(sFileName);
        auto modelStream = std::stringstream(modelFileRaw);

        std::vector<float3> positions, normals;
        std::vector<float2> texCoords;
        std::vector<Face> faces;

        std::string line;
        while (std::getline(modelStream, line)) {
            std::stringstream lineStream(line);

            std::string type; lineStream >> type;
            if (type == "v") {
                float3 position;
                lineStream >> position.x >> position.y >> position.z;
                positions.push_back(position);
            } else if (type == "vn") {
                float3 normal;
                lineStream >> normal.x >> normal.y >> normal.z;
                normals.push_back(normal);
            } else if (type == "vt") {
                float2 texCoord;
                lineStream >> texCoord.x >> texCoord.y;
                texCoords.push_back(texCoord);
            } else if (type == "f") {
                // Assume 3 vertices per face
                // handle all OBJ cases: v or v/vt or v/vt/vn or v//vn

                std::string v1, v2, v3;
                lineStream >> v1 >> v2 >> v3;

                auto parseVert = [](std::string vert) -> VertexIDs  {
                    auto spl = SplitString(vert, '/');
                    std::string v = "1", vt = "1", vn = "1";

                    if (spl.size() == 1) {
                        v = spl[0];
                    } else if (spl.size() == 2) {
                        v = spl[0];
                        vt = spl[1];
                    } else if (spl.size() == 3) {
                        v = spl[0];
                        if (spl[1].empty()) {
                            vn = spl[2];
                        } else {
                            vt = spl[1];
                            vn = spl[2];
                        }
                    }

                    return { std::stoi(v)-1, std::stoi(vt)-1, std::stoi(vn)-1 };
                };

                faces.push_back({ parseVert(v1), parseVert(v2), parseVert(v3) });
            }
        }

        vertices.clear();
        indices.clear();

        uint16_t index = 0;
        for (auto face : faces) {
            for (int i = 0; i < 3; i++) {
                auto vert = face[i];
                auto pos = positions[std::get<0>(vert)];
                auto tex = texCoords[std::get<1>(vert)];
                auto norm = normals[std::get<2>(vert)];

                vertices.push_back({ { pos.x, pos.y, pos.z, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, tex, norm });
                indices.push_back(index++);
            }
        }
    }
};

int main(int argc, char** argv) {
    __android_log_print(ANDROID_LOG_INFO, "PGE3 Game", "Started native_main");

    // Construct demo application
    Example demo;

    // with a pixel size of 4x4 actual screen pixels
    if (demo.Construct({ 270, 548 }, { 4, 4 })) {
        demo.Start();
    }

    return 0;
}
