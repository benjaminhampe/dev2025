#pragma once
#include <de/Common.h>

inline void drawTextButton(NVGcontext* vg, de::Recti pos, std::string label)
{
    const int x = pos.x;
    const int y = pos.y;
    const int w = pos.w;
    const int h = pos.h;
    float m_cornerRadius = 6.0f;
    //std::string m_label = "=";

    bool m_hovered = false;
    bool m_pressed = false;

    NVGcolor m_baseColor     = nvgRGBA(70, 70, 70, 255);
    NVGcolor m_hoverColor    = nvgRGBA(100, 100, 100, 255);
    NVGcolor m_pressColor    = nvgRGBA(150, 60, 60, 255);
    NVGcolor m_textColor     = nvgRGBA(255, 255, 255, 255);

    NVGcolor color =
        m_pressed ? m_pressColor :
        m_hovered ? m_hoverColor : m_baseColor;

    // Background
    nvgBeginPath(vg);
    nvgFillColor(vg, color);
    nvgRoundedRect(vg, x, y, w, h, m_cornerRadius);
    nvgFill(vg);

    // Text
    nvgFillColor(vg, m_textColor);
    nvgFontFace(vg, "ShareTechMonoRegular");
    nvgFontSize(vg, 48.0f);  // in pixels
    nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);

    // float tw = nvgTextBounds(vg, 0, 0, m_label.c_str(), nullptr, nullptr);
    // float tx = x + (w - tw) * 0.5f;
    // float ty = y + (h * 0.5f) + 6.0f; // vertical centering

    nvgText(vg, x + w/2, y + h/2, label.c_str(), nullptr);
}


struct NVG_4ColorRect
{
    int m_img = 0;

    void init(NVGcontext* vg)
    {
        unsigned char pixels[2 * 2 * 4] =
        {
            100,   0,   0, 255, // top-left
              0, 100,   0, 255, // top-right
             50,  50, 155, 255, // bottom-left
            100,   0, 100, 255  // bottom-right
        };

        m_img = nvgCreateImageRGBA(vg, 2, 2, 0, pixels);
    }

    void draw(NVGcontext* vg, const de::Recti pos)
    {
        NVGpaint paint = nvgImagePattern(
            vg,
            pos.x, pos.y,           // position of rect
            pos.w, pos.h,           // size of rect
            0.0f,           // rotation
            m_img,            // the 2×2 gradient texture
            1.0f            // alpha
        );

        nvgBeginPath(vg);
        nvgRect(vg, pos.x, pos.y, pos.w, pos.h);
        nvgFillPaint(vg, paint);
        nvgFill(vg);
    }
};

/*
🎯 Correct way: create a 2×2 texture with your 4 corner colors and draw it as a paint

NanoVG supports image patterns with arbitrary textures.
If you upload a 2×2 RGBA texture with your corner colors, NanoVG will bilinearly interpolate across the entire rectangle.
✔ Corner layout inside the 2×2 texture
Code

(0,0) top-left     = colorTL
(1,0) top-right    = colorTR
(0,1) bottom-left  = colorBL
(1,1) bottom-right = colorBR

NanoVG will interpolate between these automatically.
🧩 Step 1 — Create the 2×2 gradient texture

unsigned char pixels[2 * 2 * 4] = {
    // row 0: top-left, top-right
    colorTL.r, colorTL.g, colorTL.b, colorTL.a,
    colorTR.r, colorTR.g, colorTR.b, colorTR.a,

    // row 1: bottom-left, bottom-right
    colorBL.r, colorBL.g, colorBL.b, colorBL.a,
    colorBR.r, colorBR.g, colorBR.b, colorBR.a,
};

int img = nvgCreateImageRGBA(vg, 2, 2, 0, pixels);

🧩 Step 2 — Draw the rectangle using an image pattern
cpp

NVGpaint paint = nvgImagePattern(
    vg,
    x, y,           // position of rect
    w, h,           // size of rect
    0.0f,           // rotation
    img,            // the 2×2 gradient texture
    1.0f            // alpha
);

nvgBeginPath(vg);
nvgRect(vg, x, y, w, h);
nvgFillPaint(vg, paint);
nvgFill(vg);

This produces a perfect 4‑corner gradient.
🧩 Why this works

NanoVG’s image sampling is bilinear.
A 2×2 texture is the smallest possible bilinear field.
Each pixel becomes a corner color, and interpolation fills the entire area.

This is exactly how professional engines implement multi‑corner gradients.
🧩 Alternative: custom shader (only if you use NanoVG GL backend)

If you need dynamic gradients without textures, you can patch NanoVG’s GL backend and supply your own fragment shader.
But the 2×2 texture method is simpler, faster, and fully portable.
🧩 Visual intuition
🧩 Want the version that supports arbitrary quad warping (not just rectangles)?

I can give you:

    4‑corner gradient for arbitrary convex quads

    Perspective‑correct interpolation

    NanoVG GL shader patch

    Dynamic gradient animation











🧩 Step 1 — Add a custom shader to NanoVG GL backend

Inside nanovg_gl.h / nanovg_gl.c, add a new shader program:
cpp

static const char* quadFragShader =
"#version 330 core\n"
"in vec2 uv;\n"
"uniform vec4 colorTL;\n"
"uniform vec4 colorTR;\n"
"uniform vec4 colorBL;\n"
"uniform vec4 colorBR;\n"
"out vec4 outColor;\n"
"void main() {\n"
"    vec4 top = mix(colorTL, colorTR, uv.x);\n"
"    vec4 bottom = mix(colorBL, colorBR, uv.x);\n"
"    outColor = mix(top, bottom, uv.y);\n"
"}\n";

This is true bilinear interpolation.
Vertex shader:
cpp

static const char* quadVertShader =
"#version 330 core\n"
"layout(location = 0) in vec2 pos;\n"
"layout(location = 1) in vec2 inUV;\n"
"out vec2 uv;\n"
"uniform mat4 uMatrix;\n"
"void main() {\n"
"    uv = inUV;\n"
"    gl_Position = uMatrix * vec4(pos.xy, 0.0, 1.0);\n"
"}\n";

Compile and link these into a GL program (progQuad).
🧩 Step 2 — Draw your own quad geometry

You provide the quad’s four corners in screen space:
cpp

struct Quad {
    NVGpoint p0; // top-left
    NVGpoint p1; // top-right
    NVGpoint p2; // bottom-right
    NVGpoint p3; // bottom-left
};

Build the vertex buffer (two triangles)
cpp

struct Vertex {
    float x, y;
    float u, v;
};

Vertex verts[6] = {
    {p0.x, p0.y, 0.0f, 0.0f},
    {p1.x, p1.y, 1.0f, 0.0f},
    {p2.x, p2.y, 1.0f, 1.0f},

    {p0.x, p0.y, 0.0f, 0.0f},
    {p2.x, p2.y, 1.0f, 1.0f},
    {p3.x, p3.y, 0.0f, 1.0f},
};

Upload to GL (VBO).
🧩 Step 3 — Bind shader + uniforms
cpp

glUseProgram(progQuad);

glUniform4fv(glGetUniformLocation(progQuad, "colorTL"), 1, &colorTL.r);
glUniform4fv(glGetUniformLocation(progQuad, "colorTR"), 1, &colorTR.r);
glUniform4fv(glGetUniformLocation(progQuad, "colorBL"), 1, &colorBL.r);
glUniform4fv(glGetUniformLocation(progQuad, "colorBR"), 1, &colorBR.r);

glUniformMatrix4fv(glGetUniformLocation(progQuad, "uMatrix"),
                   1, GL_FALSE, nvgCurrentTransform(vg));

🧩 Step 4 — Draw the quad
cpp

glBindVertexArray(quadVAO);
glDrawArrays(GL_TRIANGLES, 0, 6);

This produces a perfect 4‑corner gradient across any quad.
🧩 Why this works

NanoVG’s normal paints only support:

    linear gradients

    radial gradients

    image patterns

None of these can produce a 4‑corner bilinear gradient on arbitrary geometry.

By drawing your own quad and using a custom shader:

    You get full control

    You get true bilinear interpolation

    You can warp the quad arbitrarily

    You can animate the gradient

    You can apply perspective transforms

This is exactly how professional UI engines implement multi‑corner gradients.
🧩 Want the version that supports perspective‑correct interpolation?

I can give you:

    Homogeneous UV interpolation

    Quad → triangle mapping with Jacobian correction

    Perspective‑correct bilinear blending

    NanoVG GL backend patch for perspective transforms

Choose your next step:
perspective gradient
Show NanoVG GL backend shader patch for corner gradient
NanoVG GL backend: minimal 4‑corner gradient patch

Below is a surgical patch that adds a new shader mode to NanoVG’s GL backend for a 4‑corner gradient fill, without touching the public NanoVG API.
1. Extend shader type enum

In nanovg_gl.h (or equivalent), add a new shader type:
c

typedef enum GLNVGshaderType {
    NSVG_SHADER_WINDINGCLEAR = 0,
    NSVG_SHADER_WINDINGCALC  = 1,
    NSVG_SHADER_FILLSOLID    = 2,
    NSVG_SHADER_FILLGRAD     = 3,
    NSVG_SHADER_FILLIMG      = 4,
    NSVG_SHADER_TEXT         = 5,
    NSVG_SHADER_FILL4CORNER  = 6, // <-- new
} GLNVGshaderType;

2. Extend fragment uniforms with 4 corner colors

In GLNVGfragUniforms (same file), add four extra colors:
c

typedef struct GLNVGfragUniforms {
    float scissorMat[12];
    float paintMat[12];
    NVGcolor innerCol;
    NVGcolor outerCol;
    float extent[2];
    float radius;
    float feather;
    float strokeMult;
    float strokeThr;
    int   texType;
    int   tex;
    // --- add this ---
    NVGcolor cornerTL;
    NVGcolor cornerTR;
    NVGcolor cornerBL;
    NVGcolor cornerBR;
} GLNVGfragUniforms;

Make sure uniform buffer size / upload code includes these fields.
3. Add uniform locations for the new colors

In the uniform location enum (e.g. GLNVGuniformLoc):
c

typedef enum GLNVGuniformLoc {
    GLNVG_LOC_SCISSORMAT,
    GLNVG_LOC_PAINTMAT,
    GLNVG_LOC_INNERCOL,
    GLNVG_LOC_OUTERCOL,
    GLNVG_LOC_EXTENT,
    GLNVG_LOC_RADIUS,
    GLNVG_LOC_FEATHER,
    GLNVG_LOC_STROKEMULT,
    GLNVG_LOC_STROKETHR,
    GLNVG_LOC_TEX,
    GLNVG_LOC_TYPE,
    // --- new ---
    GLNVG_LOC_CORNER_TL,
    GLNVG_LOC_CORNER_TR,
    GLNVG_LOC_CORNER_BL,
    GLNVG_LOC_CORNER_BR,
} GLNVGuniformLoc;

And in glnvg__getUniforms():
c

gl->shader.loc[GLNVG_LOC_CORNER_TL] = glGetUniformLocation(gl->shader.prog, "cornerTL");
gl->shader.loc[GLNVG_LOC_CORNER_TR] = glGetUniformLocation(gl->shader.prog, "cornerTR");
gl->shader.loc[GLNVG_LOC_CORNER_BL] = glGetUniformLocation(gl->shader.prog, "cornerBL");
gl->shader.loc[GLNVG_LOC_CORNER_BR] = glGetUniformLocation(gl->shader.prog, "cornerBR");

4. Patch the fragment shader

In the GL fragment shader source (the unified NanoVG shader), add uniforms and a branch for the new type.

Add uniforms:
glsl

uniform vec4 cornerTL;
uniform vec4 cornerTR;
uniform vec4 cornerBL;
uniform vec4 cornerBR;

Inside main() where it switches on type (the shader mode):
glsl

if (type == NSVG_SHADER_FILL4CORNER) {
    // uv is the local paint coordinate in [0,1]x[0,1]
    vec2 uv01 = clamp(uv, 0.0, 1.0);

    vec4 top    = mix(cornerTL, cornerTR, uv01.x);
    vec4 bottom = mix(cornerBL, cornerBR, uv01.x);
    vec4 col    = mix(top, bottom, uv01.y);

    fragColor = col * scissor; // apply scissor / coverage as NanoVG does
    return;
}

Leave all other branches untouched.
5. Upload corner colors from the backend

In the GL backend code where it prepares GLNVGfragUniforms for a fill call (e.g. glnvg__convertPaint()), add something like:
c

static void glnvg__convertPaint(GLNVGcontext* gl, GLNVGfragUniforms* frag,
                                NVGpaint* paint, NVGscissor* scissor,
                                float width, float fringe, float strokeThr)
{
    // existing NanoVG setup...
    // innerCol / outerCol / extent / paintMat / etc.

    // For 4-corner gradient, you decide how to encode colors in NVGpaint.
    // Example: use innerCol/outerCol + image or custom fields.
    // Here we assume you stuffed them into paint->innerColor/outerColor
    // and some extra storage; adapt to your own scheme.

    frag->cornerTL = paint->cornerTL; // custom extension in NVGpaint
    frag->cornerTR = paint->cornerTR;
    frag->cornerBL = paint->cornerBL;
    frag->cornerBR = paint->cornerBR;
}

And when uploading uniforms (non‑UBO path):
c

glUniform4fv(gl->shader.loc[GLNVG_LOC_CORNER_TL], 1, (const float*)&frag->cornerTL);
glUniform4fv(gl->shader.loc[GLNVG_LOC_CORNER_TR], 1, (const float*)&frag->cornerTR);
glUniform4fv(gl->shader.loc[GLNVG_LOC_CORNER_BL], 1, (const float*)&frag->cornerBL);
glUniform4fv(gl->shader.loc[GLNVG_LOC_CORNER_BR], 1, (const float*)&frag->cornerBR);

6. Select the new shader type for your quad/rect

Where the backend chooses type for a fill call (usually in glnvg__fill()):
c

int type = NSVG_SHADER_FILLSOLID;
// ...
if (isFourCornerGradientPaint(paint)) // your own condition
    type = NSVG_SHADER_FILL4CORNER;

glUniform1i(gl->shader.loc[GLNVG_LOC_TYPE], type);

You then draw your rect/quad as usual; the fragment shader will do the 4‑corner interpolation based on uv.

If you want, I can sketch a minimal NVGpaint extension and a helper like nvgFourCornerGradient(vg, x, y, w, h, tl, tr, bl, br) that plugs cleanly into this patch.
*/
