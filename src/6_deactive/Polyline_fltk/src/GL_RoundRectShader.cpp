#include "GL_RoundRectShader.h"
#include "GL_Util.h"

void GL_RoundRectShader::draw(
        int screenW,
        int screenH,
        int x,
        int y,
        int w,
        int h,
        int radius,
        float r,
        float g,
        float b,
        float a,
        float aaWidth )
{
    if (!m_shaderId)
    {
        // Triangle (CCW) A: v0 → v1 → v2
        // Triangle (CCW) B: v2 → v1 → v3

        // v2 (0,1) ---------------------- v3 (1,1)
        //      ^                         |
        //      | Edge 2                  | Edge 3
        //      |                         |
        //      |                         |
        //      |                         |
        // v0 (0,0) ---------------------- v1 (1,0)
        //      Edge 0        --->        Edge 1

        auto vs = R"(#version 330 core

            out vec2 v_tex;

            uniform vec2 u_screenSize; // framebuffer size in pixels
            uniform vec2 u_rectPos;   // rounded rect in pixels
            uniform vec2 u_rectSize;     // rounded rect width/height in pixels

            void main()
            {
/*
                const vec2 positions[4] = vec2[](
                    vec2(-1.0, -1.0),
                    vec2( 1.0, -1.0),
                    vec2(-1.0,  1.0),
                    vec2( 1.0,  1.0)
                );

                const vec2 texcoords[4] = vec2[](
                    vec2(0.0, 0.0),
                    vec2(1.0, 0.0),
                    vec2(0.0, 1.0),
                    vec2(1.0, 1.0)
                );

                gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
                v_tex = texcoords[gl_VertexID];
*/
                // gl_VertexID:
                // 0: (0,0)
                // 1: (1,0)
                // 2: (0,1)
                // 3: (1,1)

                // Compute corner offset WITHOUT branching
                vec2 corner = vec2(
                    float(gl_VertexID & 1),
                    float((gl_VertexID >> 1) & 1)
                );

                // Pixel position of this vertex
                vec2 px = u_rectPos + corner * u_rectSize;

                // Convert pixel → NDC
                vec2 ndc = (px / u_screenSize) * 2.0 - 1.0;

                gl_Position = vec4(ndc, 0.0, 1.0);
                v_tex = corner;
            }
        )";

        auto fs = R"(#version 330 core

            out vec4 FragColor;

            in vec2 v_tex;

            //uniform vec2 u_screenSize; // framebuffer size in pixels
            uniform vec4 u_fillColor; // fill color
            //uniform vec2 u_rectPos;   // rounded rect in pixels
            uniform vec2 u_rectSize;     // rounded rect width/height in pixels
            uniform float u_rectRadius;  // rounded rect corner radius in pixels
            uniform float u_aaWidth;     // AA width in pixels, e.g. 1.0–2.0
            //uniform vec2 u_circleCenter;  // circle params in pixels
            //uniform float u_circleRadius; // circle params in pixels

            float sdRoundedRect(vec2 p, vec2 b, float r)
            {
                // p: position relative to rect center (pixels)
                // b: half-size (pixels)
                vec2 q = abs(p) - b + vec2(r);
                return length(max(q, 0.0)) - r;
            }

            //float sdCircle(vec2 p, float r) { return length(p) - r; }

            void main()
            {
                vec2 p = v_tex * u_rectSize - u_rectSize * 0.5;
                float d = sdRoundedRect(p, u_rectSize * 0.5, u_rectRadius);

                float alpha = 1.0 - smoothstep(0.0, u_aaWidth, d);
                //if (alpha <= 0.0) discard;

                FragColor = vec4(u_fillColor.rgb, u_fillColor.a * alpha);
            /*
                vec2 fragCoord = v_tex * u_screenSize; // convert in pixels

                // ----- choose shape -----

                // Rounded rectangle
                vec2 rectCenter = u_rectPos + u_rectSize * 0.5;
                vec2 pRect = fragCoord - rectCenter;
                vec2 halfSize = u_rectSize * 0.5;
                float d = sdRoundedRect(pRect, halfSize, u_rectRadius);

                // Circle (if you want circle instead, replace d with this)
                // vec2 pCircle = fragCoord - circleCenter;
                // float d = sdCircle(pCircle, circleRadius);

                // anti-aliased alpha from signed distance
                float alpha = 1.0 - smoothstep(0.0, u_aaWidth, d);

                // if (alpha <= 0.0)
                //     discard;

                FragColor = vec4(u_fillColor.rgb, u_fillColor.a * alpha);
            */
            }
        )";

        m_shaderId = GL_Util::createShader("GL_RoundRectShader",vs,fs);
        u_screenSize = glGetUniformLocation(m_shaderId, "u_screenSize");
        u_fillColor = glGetUniformLocation(m_shaderId, "u_fillColor");
        u_rectPos = glGetUniformLocation(m_shaderId, "u_rectPos");
        u_rectSize = glGetUniformLocation(m_shaderId, "u_rectSize");
        u_rectRadius = glGetUniformLocation(m_shaderId, "u_rectRadius");
        u_aaWidth = glGetUniformLocation(m_shaderId, "u_aaWidth");
        //glGetUniformLocation(m_shaderId, "circleCenter");
        //glGetUniformLocation(m_shaderId, "circleRadius");
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
    }
/*
    AlphaBlending = SourceColor*SourceAlpha + DestColor*(1-SourceAlpha)
    AdditiveBlending = SourceColor*1 + DestColor*1
    case SourceOver:      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); break;
    case DestinationOver: glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE); break;
    case Clear:           glBlendFunc(GL_ZERO, GL_ZERO); break;
    case Source:          glBlendFunc(GL_ONE, GL_ZERO); break;
    case Destination:     glBlendFunc(GL_ZERO, GL_ONE); break;
    case SourceIn:        glBlendFunc(GL_DST_ALPHA, GL_ZERO); break;
    case DestinationIn:   glBlendFunc(GL_ZERO, GL_SRC_ALPHA); break;
    case SourceOut:       glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ZERO); break;
    case DestinationOut:  glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA); break;
    case SourceAtop:      glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
    case DestinationAtop: glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_SRC_ALPHA); break;
    case Xor:   glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
    case Plus:  glBlendFunc(GL_ONE, GL_ONE); break;

*/
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(m_shaderId);
    glUniform2f(u_screenSize, screenW, screenH);
    glUniform4f(u_fillColor, r,g,b,a);
    glUniform2f(u_rectPos, x, y);
    glUniform2f(u_rectSize, w, h);
    glUniform1f(u_rectRadius, radius);
    //glUniform2f(u_circleCenter, width*0.5f, height*0.5f);
    //glUniform1f(u_circleRadius, 120.0f);
    glUniform1f(u_aaWidth, aaWidth);
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
