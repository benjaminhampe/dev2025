#include "eBT_PlanetRenderer.h"

void eBT_PlanetRenderer::init( de::gl::Bridge* driver )
{
    m_meshSphere128x64.create(128,64);
    m_meshSphere128x64.upload();

    std::string vsSrc = getVertexShaderText();
    std::string tcsSrc = getTesselationControlShaderText();
    std::string tesSrc = getTesselationEvaluationShaderText();
    std::string fsSrc = getFragmentShaderText();
    GLuint vs = de::gpu::GT_compileShader("eBT_Planet_vs", GL_VERTEX_SHADER, vsSrc);
    GLuint tcs = de::gpu::GT_compileShader("eBT_Planet_tcs", GL_TESS_CONTROL_SHADER, tcsSrc);
    GLuint tes = de::gpu::GT_compileShader("eBT_Planet_tes", GL_TESS_EVALUATION_SHADER, tesSrc);
    GLuint fs = de::gpu::GT_compileShader("eBT_Planet_fs", GL_FRAGMENT_SHADER, fsSrc);

    programId = glCreateProgram();
    glAttachShader(programId, vs);
    glAttachShader(programId, tcs);
    glAttachShader(programId, tes);
    glAttachShader(programId, fs);
    glLinkProgram(programId);

    u_modelPos = glGetUniformLocation(programId, "u_modelPos");
    u_modelScale = glGetUniformLocation(programId, "u_modelScale");

    u_modelMatrix = glGetUniformLocation(programId, "u_modelMatrix");
    u_viewMatrix = glGetUniformLocation(programId, "u_viewMatrix");
    u_projectionMatrix = glGetUniformLocation(programId, "u_projectionMatrix");
    u_cameraPos = glGetUniformLocation(programId, "u_cameraPos");
    u_diffuseMap = glGetUniformLocation(programId, "u_diffuseMap");
    //u_diffuseMapSize = glGetUniformLocation(programId, "u_diffuseMapSize");
    u_bumpMap = glGetUniformLocation(programId, "u_bumpMap");
    u_bumpScale = glGetUniformLocation(programId, "u_bumpScale");

    u_maxDist = glGetUniformLocation(programId, "u_maxDist");
    u_maxTess = glGetUniformLocation(programId, "u_maxTess");

}

void eBT_PlanetRenderer::draw( const Body& body )
{
    if (!m_driver)
    {
        DE_ERROR("No driver")
        return;
    }

    if (!body.diffuseMap.textureId)
    {
        DE_ERROR("No diffuseMap for body ", body.name)
        return;
    }

    if (!body.bumpMap.textureId)
    {
        DE_ERROR("No bumpMap for body ", body.name)
        return;
    }

    auto camera = m_driver->getCamera();

    glUseProgram(programId);
    const auto modelMatrix = glm::mat4(1.0f);
    const auto viewMatrix = glm::mat4(camera->getViewMatrix());
    const auto projMatrix = glm::mat4(camera->getProjectionMatrix());
    const auto cameraPos = glm::vec3(camera->getPos());
    glUniformMatrix4fv(u_modelMatrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(u_viewMatrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(u_projectionMatrix, 1, GL_FALSE, glm::value_ptr(projMatrix));
    glUniform3fv(u_cameraPos, 1, glm::value_ptr(cameraPos));

    glUniform3fv(u_modelPos, 1, glm::value_ptr(body.pos));
    glUniform3fv(u_modelScale, 1, glm::value_ptr(body.size));

    glUniform1f(u_maxDist, 50.0f);
    glUniform1f(u_maxTess, 64.0f);

    // [Uniform] u_diffuseMap
    int stage = 0;
    glActiveTexture(GL_TEXTURE0 + stage);
    glBindTexture(body.diffuseMap.targetTyp, body.diffuseMap.textureId);
    glBindSampler(stage, body.diffuseMap.samplerId);
    glUniform1i(u_diffuseMap, stage);
    //glm::vec2 diffuseMapSize( diffuseMap.tex->w(), diffuseMap.tex->h());
    //glUniform2fv(u_diffuseMapSize, 1, glm::value_ptr(diffuseMapSize));

    // [Uniform] u_bumpMap
    stage = 1;
    glActiveTexture(GL_TEXTURE0 + stage);
    glBindTexture(body.bumpMap.targetTyp, body.bumpMap.textureId);
    glBindSampler(stage, body.bumpMap.samplerId);
    glUniform1i(u_bumpMap, stage);
    glUniform1f(u_bumpScale, body.bumpScale);


    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Wireframe mode
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glFrontFace(GL_CCW);

    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);

    m_meshSphere128x64.draw();
    // glPatchParameteri(GL_PATCH_VERTICES, 3);
    // glBindVertexArray(vao);
    // glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, 0);

}

std::string 
eBT_PlanetRenderer::getVertexShaderText()
{
    const char* vsSrc = R"(
        #version 450 core

        //layout(location = 0) in vec3 aPos;
        layout(location = 0) in vec3 aNormal;
        layout(location = 1) in vec2 aTexCoord;

        out vec3 vPosition;
        out vec3 vNormal;
        out vec2 vTexCoord;
        out vec3 vFragPos;

        uniform vec3 u_modelPos;
        uniform vec3 u_modelScale;
        uniform mat4 u_modelMatrix;

        void main()
        {
            vNormal = aNormal;
            vec3 aPos = aNormal * u_modelScale + u_modelPos;
            vPosition = aPos;
            vTexCoord = aTexCoord;
            vec4 worldPos = u_modelMatrix * vec4(aPos, 1.0);
            vFragPos = worldPos.xyz;
        }
    )";
    
    return vsSrc;
}

std::string 
eBT_PlanetRenderer::getTesselationControlShaderText()
{
    const char* tcsSrc = R"(
        #version 450 core

        layout(vertices = 3) out;

        in vec3 vPosition[];
        in vec3 vNormal[];
        in vec3 vFragPos[];
        in vec2 vTexCoord[];

        out vec3 tcPosition[];
        out vec3 tcNormal[];
        out vec2 tcTexCoord[];

        uniform float u_maxDist; // 50
        uniform float u_maxTess; // 64

        uniform mat4 u_viewMatrix;
        uniform vec3 u_cameraPos;

        void main()
        {
            tcPosition[gl_InvocationID] = vPosition[gl_InvocationID];
            tcNormal[gl_InvocationID] = vNormal[gl_InvocationID];
            tcTexCoord[gl_InvocationID] = vTexCoord[gl_InvocationID];

            float cameraDist = length(vFragPos[gl_InvocationID] - u_cameraPos);
            float t = clamp(cameraDist / u_maxDist, 0.0, 1.0);
            float tessLevel = mix(u_maxTess, 1.0, t );

            gl_TessLevelOuter[0] = tessLevel;
            gl_TessLevelOuter[1] = tessLevel;
            gl_TessLevelOuter[2] = tessLevel;
            gl_TessLevelInner[0] = tessLevel;
        }
    )";
    
    return tcsSrc;
}

std::string 
eBT_PlanetRenderer::getTesselationEvaluationShaderText()
{
    const char* tesSrc = R"(
        #version 450 core

        layout(triangles, equal_spacing, cw) in;

        in vec3 tcPosition[];
        in vec3 tcNormal[];
        in vec2 tcTexCoord[];

        out vec3 tePosition;
        out vec3 teNormal;
        out vec2 teTexCoord;

        uniform mat4 u_modelMatrix;
        uniform mat4 u_viewMatrix;
        uniform mat4 u_projectionMatrix;

        uniform sampler2D u_bumpMap;
        uniform float u_bumpScale;

        void main()
        {
            vec3 b = gl_TessCoord; // barycentric

            vec3 pos = b.x * tcPosition[0]
                     + b.y * tcPosition[1]
                     + b.z * tcPosition[2];

            vec2 tex = b.x * tcTexCoord[0]
                     + b.y * tcTexCoord[1]
                     + b.z * tcTexCoord[2];

            vec3 nrm = normalize(b.x * tcNormal[0] +
                                 b.y * tcNormal[1] +
                                 b.z * tcNormal[2]);

            float bumpHeight = u_bumpScale * texture(u_bumpMap, tex).r;

            pos += nrm * bumpHeight;

            tePosition = pos;
            teNormal = nrm;
            teTexCoord = tex;
            gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vec4(pos, 1.0);
        }
    )";

    return tesSrc;
}

std::string 
eBT_PlanetRenderer::getFragmentShaderText()
{
    /*
    const char* fsSrc = R"(
        #version 450 core

        out vec4 fragColor;

        in vec3 teNormal;
        in vec2 teTexCoord;

        uniform sampler2D u_diffuseMap;
        uniform vec2 u_diffuseMapSize; // z. B. vec2(512.0, 512.0)

        void main()
        {
            fragColor = texture(u_diffuseMap, teTexCoord);
            //fragColor = vec4(teTexCoord.x, teTexCoord.y, 0.5, 1.0);
        }
    )";
    */
    
    const char* fsKubischeInterpolationSrc = R"(
        #version 450 core

        out vec4 fragColor;

        in vec3 teNormal;
        in vec2 teTexCoord;

        uniform sampler2D u_diffuseMap;
        uniform vec2 u_diffuseMapSize; // z. B. vec2(512.0, 512.0)

        // Bicubic Gewichtung
        vec4 cubic(float v)
        {
            float v2 = v * v;
            float v3 = v2 * v;
            return vec4(
                v * (-v2 + 2.0 * v - 1.0),
                3.0 * v3 - 5.0 * v2 + 2.0,
                -3.0 * v3 + 4.0 * v2 + v,
                v3 - v2
            ) * 0.5;
        }

        // Bicubic Sampling
        vec4 sampleBicubic(sampler2D tex, vec2 uv, vec2 texSize)
        {
            vec2 coord = uv * texSize - 0.5;
            vec2 f = fract(coord);
            coord -= f;

            vec4 xc = cubic(f.x);
            vec4 yc = cubic(f.y);

            vec4 result = vec4(0.0);
            for (int j = 0; j < 4; ++j) {
                for (int i = 0; i < 4; ++i) {
                    vec2 offset = (coord + vec2(i - 1.0, j - 1.0)) / texSize;
                    result += texture(tex, offset) * xc[i] * yc[j];
                }
            }
            return result;
        }

        void main()
        {
            // Get texture size
            vec2 u_diffuseMapSize = textureSize(u_diffuseMap);

            // Zoom-Level bestimmen
            float ddx = dFdx(teTexCoord).x;
            float ddy = dFdy(teTexCoord).y;
            float len = length(vec2(ddx, ddy));
            float zoomFactor = len * u_diffuseMapSize.x;

            // Schwellenwert: ab Zoom > 1.5 Texel pro Fragment → bicubic
            bool useBicubic = zoomFactor < 1.5;

            vec4 color = useBicubic
                ? sampleBicubic(u_diffuseMap, teTexCoord, u_diffuseMapSize)
                : texture(u_diffuseMap, teTexCoord);

            fragColor = color;
        }
    )";
    
    return fsKubischeInterpolationSrc;
}
