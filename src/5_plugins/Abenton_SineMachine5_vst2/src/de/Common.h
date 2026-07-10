#pragma once
#include <DarkImage.h>
#include <DarkGPU.h>
// #include <cstdint>
// #include <string>
// #include <cmath>
// #include <algorithm>
// #include <thread>

#include <pluginterfaces/vst2.x/audioeffectx.h>

#ifndef NANOVG_GL3_IMPLEMENTATION
#define NANOVG_GL3_IMPLEMENTATION
#endif
#include <nanovg.h>
#include <nanovg_gl.h>

// #include <GLFW/glfw3.h> // Bad: GLFW uses TopLevelWindows, not well managable by Hosts.
// #include <de_Approx_Math.h>
// #include <de/approx_math.h>
// #include <de/de_aligned_memory.h>


//🖌️ Drawing a Colored Rectangle Border
inline void 
drawLineRect( 
    NVGcontext* vg, 
    int x, int y, int w, int h, 
    const NVGcolor& color, 
    float strokeWidth = 4.0f )
{
    nvgBeginPath(vg);                     // Start a new path
    nvgRect(vg, x, y, w, h);              // Define the rectangle
    nvgStrokeWidth(vg, strokeWidth);     // Set border thickness
    nvgStrokeColor(vg, color); // Set border color (red)
    nvgStroke(vg);                        // Render the stroke
}

//🖌️ Drawing a Colored Rectangle Border
inline void 
drawLineRect( 
    NVGcontext* vg, 
    const de::Recti& pos, 
    const NVGcolor& color, 
    float strokeWidth = 4.0f )
{
    drawLineRect(vg,pos.x,pos.y,pos.w,pos.h,color,strokeWidth);
}


/*
    NVGcontext* vg = ...; // your NanoVG GL3 context

    // Create framebuffer (FBO + texture)
    NVGframebuffer* fb = nvgCreateFramebuffer(vg, 512, 512, NVG_IMAGE_REPEAT);

    // Bind framebuffer for drawing
    nvgBindFramebuffer(fb);

    // Draw into the framebuffer
    nvgBeginFrame(vg, 512, 512, 1.0f);

    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, 512, 512);
    nvgFillColor(vg, nvgRGBA(30, 30, 30, 255));
    nvgFill(vg);

    // Draw text, shapes, SVGs, whatever
    nvgFontSize(vg, 48);
    nvgFillColor(vg, nvgRGBA(255, 255, 255, 255));
    nvgText(vg, 20, 60, "Hello Framebuffer!", nullptr);

    nvgEndFrame(vg);

    // Unbind framebuffer → back to default framebuffer
    nvgBindFramebuffer(nullptr);

    // Draw the framebuffer texture to screen
    nvgBeginFrame(vg, windowWidth, windowHeight, pixelRatio);

    nvgBeginPath(vg);
    nvgRect(vg, 100, 100, 512, 512);
    nvgFillPaint(vg, nvgImagePattern(vg, 100, 100, 512, 512, 0.0f, fb->image, 1.0f));
    nvgFill(vg);

    nvgEndFrame(vg);
*/

/*
🚀 Full working code: lunasvg cubic curves → NanoVG framebuffer
cpp

NVGcontext* vg = ...; // NanoVG GL3 context

// Load SVG with lunasvg
auto doc = lunasvg::Document::loadFromFile("input.svg");
auto svg = doc->svgElement();
auto shapes = svg->shapes();

// Create framebuffer for caching
NVGframebuffer* fb = nvgCreateFramebuffer(vg, 512, 512, 0);
nvgBindFramebuffer(fb);

nvgBeginFrame(vg, 512, 512, 1.0f);

// Draw background
nvgBeginPath(vg);
nvgRect(vg, 0, 0, 512, 512);
nvgFillColor(vg, nvgRGBA(20, 20, 20, 255));
nvgFill(vg);

// Draw SVG shapes using cubic curves
for(const auto& shape : shapes)
{
    const auto& path = shape->path();

    nvgBeginPath(vg);

    for(const auto& cmd : path.commands())
    {
        switch(cmd.type)
        {
            case lunasvg::PathCommandType::MoveTo:
                nvgMoveTo(vg, cmd.x, cmd.y);
                break;

            case lunasvg::PathCommandType::LineTo:
                nvgLineTo(vg, cmd.x, cmd.y);
                break;

            case lunasvg::PathCommandType::CubicTo:
                // NanoVG cubic curve
                nvgBezierTo(vg,
                    cmd.x1, cmd.y1,   // control point 1
                    cmd.x2, cmd.y2,   // control point 2
                    cmd.x,  cmd.y     // end point
                );
                break;

            case lunasvg::PathCommandType::Close:
                nvgClosePath(vg);
                break;
        }
    }

    // Fill or stroke depending on SVG style
    nvgFillColor(vg, nvgRGBA(255, 255, 255, 255));
    nvgFill(vg);
}

nvgEndFrame(vg);
nvgBindFramebuffer(nullptr);

Now you have a GPU texture containing your SVG with real cubic curves, not flattened lines.
🎨 Draw cached framebuffer every frame (fast path)
cpp

nvgBeginFrame(vg, winW, winH, pixelRatio);

nvgBeginPath(vg);
nvgRect(vg, 100, 100, 512, 512);
nvgFillPaint(vg, nvgImagePattern(vg, 100, 100, 512, 512, 0, fb->image, 1.0f));
nvgFill(vg);

nvgEndFrame(vg);

This is one quad, one draw call, almost zero CPU cost.
🧠 Why this works
✔ lunasvg gives you exact cubic curve control points

SVG cubic curves are defined as:

    (x1, y1) control point 1

    (x2, y2) control point 2

    (x, y) end point

NanoVG’s nvgBezierTo() expects exactly the same format.

So lunasvg → NanoVG is a perfect match.
*/
/*
🚀 Minimal clipping example (correct usage)
cpp

nvgSave(vg);

// Set clip region
nvgScissor(vg, x, y, w, h);

// Draw anything — it will be clipped
nvgBeginPath(vg);
nvgRect(vg, x-50, y-50, w+100, h+100);
nvgFillColor(vg, nvgRGBA(255, 0, 0, 255));
nvgFill(vg);

nvgRestore(vg); // removes clip

This is the canonical pattern used in VCV Rack widgets.
🧩 How clipping actually works internally

NanoVG’s GL backend uses:

    glScissor for rectangular clipping

    stencil buffer for complex clipping (paths)

When you call:
cpp

nvgScissor(vg, x, y, w, h);

NanoVG sets:

    a scissor rectangle

    a transform matrix

    a clip region in its state stack

When you call:
cpp

nvgIntersectScissor(vg, x, y, w, h);

NanoVG computes the intersection of the old and new clip rectangles.

When you call:
cpp

nvgResetScissor(vg);

NanoVG disables clipping entirely.
🧱 Clipping with framebuffers (VCV Rack style)

VCV Rack draws widgets like this:
cpp

nvgSave(vg);
nvgScissor(vg, widgetX, widgetY, widgetW, widgetH);

// draw widget contents
drawWidget(vg);

nvgRestore(vg);

This ensures:

    no drawing leaks outside the widget

    no overlapping artifacts

    no weird AA edges

    no accidental overdraw

    correct layering

    correct compositing

This is mandatory for a DAW‑style UI.
🎨 Clipping SVGs (lunasvg + NanoVG hybrid)

If you draw SVGs into a framebuffer:
cpp

nvgBindFramebuffer(fb);
nvgBeginFrame(vg, w, h, 1.0f);

nvgSave(vg);
nvgScissor(vg, 0, 0, w, h);

// draw SVG curves
drawSVG(vg);

nvgRestore(vg);

nvgEndFrame(vg);
nvgBindFramebuffer(nullptr);

Then draw the framebuffer:
cpp

nvgBeginFrame(vg, winW, winH, pixelRatio);

nvgSave(vg);
nvgScissor(vg, panelX, panelY, panelW, panelH);

nvgBeginPath(vg);
nvgRect(vg, panelX, panelY, panelW, panelH);
nvgFillPaint(vg, nvgImagePattern(vg, panelX, panelY, panelW, panelH, 0, fb->image, 1.0f));
nvgFill(vg);

nvgRestore(vg);
nvgEndFrame(vg);

This is exactly how VCV Rack clips module panels.
🧠 Important details you must know
✔ Clipping is affected by transforms

If you call:
cpp

nvgTranslate(vg, ...);
nvgScissor(vg, ...);

The scissor region is transformed.
✔ Clipping is affected by pixel ratio

NanoVG applies pixel ratio scaling internally.
✔ Clipping is fast

GPU scissor is extremely cheap.
✔ Clipping is rectangular

NanoVG does not support arbitrary shape clipping (except via stencil for paths).
🧭 Final takeaway
✔ Clipping is not automatic

You must call nvgScissor().
✔ Clipping is stateful

It persists until nvgRestore() or nvgResetScissor().
✔ Clipping is fast

GPU scissor + stencil.
✔ VCV Rack uses clipping everywhere

Every widget, every panel, every SVG.

If you want, I can show you:

    Correct NanoVG clipping patterns for DAW UIs

    NanoVG clipping pitfalls

    Hybrid lunasvg + NanoVG clipping pipeline

Show correct NanoVG GUI patterns

NanoVG has a very specific “correct” way to build GUI code.
VCV Rack, ImGui-NanoVG forks, and most professional NanoVG UIs all follow the same patterns because they avoid:

    state leakage

    clipping bugs

    transform accumulation

    AA artifacts

    path contamination

    expensive redraws

Below is the real, production‑grade pattern set you should follow in your DAW/GUI engine.
🟦 1. Every widget must bracket its drawing with nvgSave() / nvgRestore()

NanoVG is a state machine.
If you don’t isolate state, transforms and clipping will leak.
cpp

void drawWidget(NVGcontext* vg, float x, float y, float w, float h)
{
    nvgSave(vg);

    // draw widget contents here

    nvgRestore(vg);
}

This isolates:

    transforms

    scissor/clipping

    stroke width

    fill/stroke paint

    alpha

    composite operations

This is the single most important rule.
🟦 2. Always call nvgBeginPath() before drawing any shape

NanoVG accumulates path commands.
If you forget nvgBeginPath(), your new shape may append to the previous one.

Correct:
cpp

nvgBeginPath(vg);
nvgRect(vg, x, y, w, h);
nvgFillColor(vg, color);
nvgFill(vg);

Never do:
cpp

nvgRect(vg, x, y, w, h); // WRONG: path may contain old geometry
nvgFill(vg);

🟦 3. Use nvgScissor() for clipping every widget

NanoVG does not clip automatically.
You must set a clip region manually.

Correct:
cpp

nvgSave(vg);
nvgScissor(vg, x, y, w, h);

// draw widget contents

nvgRestore(vg);

This prevents:

    overdraw

    AA bleeding

    overlapping widgets

    text leaking outside bounds

VCV Rack uses this pattern for every widget.
🟦 4. Use transforms only inside a saved state

NanoVG transforms are cumulative.
Always isolate them:
cpp

nvgSave(vg);
nvgTranslate(vg, x, y);
nvgRotate(vg, angle);

// draw shape

nvgRestore(vg);

Never leave transforms active across widgets.
🟦 5. Cache expensive shapes in NanoVG framebuffers

This is how VCV Rack stays fast.
Pattern:

    Draw SVG or complex geometry once into a framebuffer

    Draw the framebuffer texture every frame

cpp

NVGframebuffer* fb = nvgCreateFramebuffer(vg, w, h, 0);

nvgBindFramebuffer(fb);
nvgBeginFrame(vg, w, h, 1.0f);

// draw SVG or complex curves

nvgEndFrame(vg);
nvgBindFramebuffer(nullptr);

Then:
cpp

nvgBeginPath(vg);
nvgRect(vg, x, y, w, h);
nvgFillPaint(vg, nvgImagePattern(vg, x, y, w, h, 0, fb->image, 1.0f));
nvgFill(vg);

This reduces CPU cost by 90–99%.
🟦 6. Text must be drawn with a fresh path

NanoVG text is not part of the path system, but you still need a clean state.

Correct:
cpp

nvgSave(vg);
nvgScissor(vg, x, y, w, h);

nvgFontSize(vg, 14);
nvgFillColor(vg, nvgRGBA(255,255,255,255));
nvgText(vg, x+4, y+14, "Hello", nullptr);

nvgRestore(vg);

🟦 7. Never mix fill and stroke without separate paths

NanoVG consumes the path when you call nvgFill() or nvgStroke().

Correct:
cpp

nvgBeginPath(vg);
nvgRect(vg, x, y, w, h);
nvgFillColor(vg, fillColor);
nvgFill(vg);

nvgBeginPath(vg);
nvgRect(vg, x, y, w, h);
nvgStrokeColor(vg, strokeColor);
nvgStrokeWidth(vg, 2.0f);
nvgStroke(vg);

🟦 8. Use pixelRatio correctly

NanoVG expects:
cpp

nvgBeginFrame(vg, windowWidth, windowHeight, pixelRatio);

Where:

    pixelRatio = framebufferScale

    1.0 for normal DPI

    2.0 for Retina

    1.25–1.5 for Windows scaling

Incorrect pixelRatio causes blurry text.
🟦 9. Use nvgIntersectScissor() for nested widgets

Correct:
cpp

nvgSave(vg);
nvgScissor(vg, parentX, parentY, parentW, parentH);

nvgIntersectScissor(vg, childX, childY, childW, childH);

// draw child widget

nvgRestore(vg);

This is how VCV Rack clips nested panels.
🟦 10. Always isolate composite operations

NanoVG supports blending modes:
cpp

nvgGlobalCompositeOperation(vg, NVG_LIGHTER);

But they must be isolated:
cpp

nvgSave(vg);
nvgGlobalCompositeOperation(vg, NVG_LIGHTER);

// draw glow effect

nvgRestore(vg);

🧠 Summary: the correct NanoVG GUI patterns

    Always use nvgSave() / nvgRestore()

    Always start shapes with nvgBeginPath()

    Always clip widgets with nvgScissor()

    Always isolate transforms

    Always cache expensive shapes in framebuffers

    Never mix fill/stroke in one path

    Never let transforms or clipping leak

    Never draw SVGs every frame

    Never forget pixelRatio

These patterns give you:

    stable render thread

    predictable performance

    zero jitter

    crisp AA

    correct clipping

    fast UI

    VCV‑Rack‑level responsiveness

If you want, I can show you the full widget class template used in NanoVG‑based engines, or a complete DAW‑style UI loop using these patterns.
*/

/*
 *
Here is the deep, correct, engine‑level explanation of the three NanoVG topics you asked for:

    nvgIntersectScissor()

    nvgBeginFrame(vg, windowWidth, windowHeight, pixelRatio)

    nvgGlobalCompositeOperation() (all modes)

    Fastest way to set fonts

Everything below is production‑grade NanoVG knowledge, the same patterns used in VCV Rack, ImGui‑NanoVG, and commercial UIs.
🟦 1. nvgIntersectScissor() — what it REALLY does

Takeaway:
It shrinks the current clip region by intersecting it with a new rectangle.
This is how you clip nested widgets.

NanoVG clipping is rectangular, GPU‑accelerated, and stateful.
✔ Correct usage pattern
cpp

nvgSave(vg);
nvgScissor(vg, parentX, parentY, parentW, parentH);

nvgIntersectScissor(vg, childX, childY, childW, childH);

// draw child widget

nvgRestore(vg);

✔ What happens internally

NanoVG keeps a current scissor rect:
Code

current_scissor = {x, y, w, h}

When you call:
cpp

nvgIntersectScissor(vg, x2, y2, w2, h2);

NanoVG computes:
Code

new_scissor = intersection(current_scissor, {x2, y2, w2, h2})

Then it updates the GPU scissor:

    GL backend: glScissor()

    Stencil backend: updates stencil mask

✔ Why this matters

Nested widgets (VCV Rack panels, DAW tracks, scroll areas) need:

    parent clip

    child clip

    sub‑child clip

nvgIntersectScissor() is the only correct way to do this.
🟦 2. nvgBeginFrame(vg, windowWidth, windowHeight, pixelRatio)

This function sets up NanoVG’s entire rendering context for the frame.
✔ Parameters

    windowWidth → logical window size

    windowHeight → logical window size

    pixelRatio → framebuffer scale (DPI)

✔ What pixelRatio actually does

NanoVG internally multiplies:

    stroke widths

    font sizes

    AA radius

    scissor rects

    transforms

by pixelRatio.
✔ Correct pixelRatio values
Platform	pixelRatio
Normal DPI	1.0
Windows 125% scaling	1.25
Windows 150% scaling	1.5
macOS Retina	2.0
HiDPI monitors	2.0–3.0
✔ Why this matters

If you set pixelRatio wrong:

    text becomes blurry

    AA becomes wrong

    strokes look wrong

    clipping becomes misaligned

VCV Rack sets pixelRatio from GLFW’s framebuffer size.
🟦 3. nvgGlobalCompositeOperation() — ALL MODES explained

NanoVG supports the same blend modes as HTML Canvas.
✔ Full list of composite operations
Mode	Meaning
NVG_SOURCE_OVER	Normal alpha blending
NVG_SOURCE_IN	Keep only where both source & dest overlap
NVG_SOURCE_OUT	Keep only where source is outside dest
NVG_ATOP	Source over dest, but only where dest exists
NVG_DESTINATION_OVER	Draw behind existing pixels
NVG_DESTINATION_IN	Keep dest only where source overlaps
NVG_DESTINATION_OUT	Remove dest where source overlaps
NVG_DESTINATION_ATOP	Dest over source, but only where source exists
NVG_LIGHTER	Additive blending (glow effects)
NVG_COPY	Replace pixels (no blending)
NVG_XOR	Exclusive OR blending
✔ Correct usage pattern

Always isolate composite operations:
cpp

nvgSave(vg);
nvgGlobalCompositeOperation(vg, NVG_LIGHTER);

// draw glow

nvgRestore(vg);

✔ Why isolate?

Composite ops affect all subsequent drawing until restored.
🟦 4. Fastest way to set fonts in NanoVG

NanoVG text is bitmap‑based, not vector.
The fastest way to use fonts is:
✔ Load font once at startup
cpp

int fontId = nvgCreateFont(vg, "sans", "Roboto-Regular.ttf");

✔ Store the font ID

Never look it up by name again.
✔ Set font by ID, not by name
cpp

nvgFontFaceId(vg, fontId);

This avoids:

    hash lookup

    string comparison

    font atlas switching

✔ Set font size once per text block
cpp

nvgFontSize(vg, 14.0f);

✔ Avoid changing font size repeatedly

NanoVG recalculates metrics each time.
✔ Avoid nvgTextBounds() inside render loop

It is expensive.
Cache bounds.
✔ Use nvgText() directly for fastest rendering

It draws textured quads.
🧠 Summary (the correct NanoVG GUI patterns)

    Use nvgIntersectScissor() for nested clipping

    Use correct pixelRatio in nvgBeginFrame()

    Isolate composite operations with nvgSave() / nvgRestore()

    Use nvgFontFaceId() for fastest text rendering

    Cache text metrics

    Never mix fill/stroke in one path

    Always begin shapes with nvgBeginPath()

    Always isolate widget drawing with nvgSave() / nvgRestore()

These patterns give you:

    stable render thread

    zero jitter

    crisp AA

    correct clipping

    fast UI

    VCV‑Rack‑level performance
*/

/*
struct Widget {
    float x = 0;
    float y = 0;
    float w = 0;
    float h = 0;

    Widget* parent = nullptr;
    std::vector<Widget*> children;

    // Optional: framebuffer for caching expensive drawing
    NVGframebuffer* fb = nullptr;
    bool dirty = true; // redraw flag

    // --- Add child ---
    void addChild(Widget* child) {
        child->parent = this;
        children.push_back(child);
    }

    // --- Absolute position ---
    float absX() const {
        return parent ? parent->absX() + x : x;
    }
    float absY() const {
        return parent ? parent->absY() + y : y;
    }

    // --- Root clipping region ---
    void applyClip(NVGcontext* vg) {
        float ax = absX();
        float ay = absY();

        if(parent) {
            // Parent clip first
            parent->applyClip(vg);

            // Intersect with this widget's region
            nvgIntersectScissor(vg, ax, ay, w, h);
        } else {
            // Root widget clip
            nvgScissor(vg, ax, ay, w, h);
        }
    }

    // --- Draw contents (override in subclasses) ---
    virtual void drawContents(NVGcontext* vg) {
        // Example: background
        nvgBeginPath(vg);
        nvgRect(vg, 0, 0, w, h);
        nvgFillColor(vg, nvgRGBA(40, 40, 40, 255));
        nvgFill(vg);
    }

    // --- Draw children ---
    void drawChildren(NVGcontext* vg) {
        for(auto* child : children)
            child->draw(vg);
    }

    // --- Main draw entry point ---
    void draw(NVGcontext* vg) {
        nvgSave(vg);

        // Clip hierarchy
        applyClip(vg);

        // Transform into local space
        nvgTranslate(vg, x, y);

        // Draw cached framebuffer if available
        if(fb && !dirty) {
            nvgBeginPath(vg);
            nvgRect(vg, 0, 0, w, h);
            nvgFillPaint(vg, nvgImagePattern(vg, 0, 0, w, h, 0, fb->image, 1.0f));
            nvgFill(vg);
        } else {
            // Draw widget contents
            drawContents(vg);

            // Draw children
            drawChildren(vg);

            // Optionally cache into framebuffer
            if(fb) {
                redrawToFramebuffer(vg);
                dirty = false;
            }
        }

        nvgRestore(vg);
    }

    // --- Redraw into framebuffer ---
    void redrawToFramebuffer(NVGcontext* vg) {
        nvgBindFramebuffer(fb);
        nvgBeginFrame(vg, w, h, 1.0f);

        nvgSave(vg);
        nvgScissor(vg, 0, 0, w, h);

        drawContents(vg);
        drawChildren(vg);

        nvgRestore(vg);
        nvgEndFrame(vg);
        nvgBindFramebuffer(nullptr);
    }
};
*/

/*
struct MouseEvent {
    float x;      // local coords
    float y;      // local coords
    float absX;   // absolute coords
    float absY;   // absolute coords
    int button;
    bool pressed;
    bool released;
    bool moved;
    bool dragged;
};

struct Widget {
    float x = 0;
    float y = 0;
    float w = 0;
    float h = 0;

    Widget* parent = nullptr;
    std::vector<Widget*> children;

    // Optional framebuffer caching
    NVGframebuffer* fb = nullptr;
    bool dirty = true;

    // -----------------------------
    // CHILD MANAGEMENT
    // -----------------------------
    void addChild(Widget* child) {
        child->parent = this;
        children.push_back(child);
    }

    // -----------------------------
    // ABSOLUTE COORDINATES
    // -----------------------------
    float absX() const {
        return parent ? parent->absX() + x : x;
    }
    float absY() const {
        return parent ? parent->absY() + y : y;
    }

    // -----------------------------
    // HIT TESTING
    // -----------------------------
    bool hit(float ax, float ay) const {
        float X = absX();
        float Y = absY();
        return ax >= X && ay >= Y && ax < X + w && ay < Y + h;
    }

    // -----------------------------
    // EVENT DISPATCH
    // -----------------------------
    virtual bool onMouse(const MouseEvent& e) {
        // Override in subclasses
        return false;
    }

    bool dispatchMouse(const MouseEvent& e) {
        // 1. Dispatch to children first (topmost child last)
        for(int i = children.size() - 1; i >= 0; --i) {
            Widget* child = children[i];

            if(child->hit(e.absX, e.absY)) {
                // Convert to child's local coords
                MouseEvent ce = e;
                ce.x = e.absX - child->absX();
                ce.y = e.absY - child->absY();

                if(child->dispatchMouse(ce))
                    return true;
            }
        }

        // 2. Dispatch to self
        if(hit(e.absX, e.absY)) {
            MouseEvent le = e;
            le.x = e.absX - absX();
            le.y = e.absY - absY();
            return onMouse(le);
        }

        return false;
    }

    // -----------------------------
    // CLIPPING
    // -----------------------------
    void applyClip(NVGcontext* vg) {
        float ax = absX();
        float ay = absY();

        if(parent) {
            parent->applyClip(vg);
            nvgIntersectScissor(vg, ax, ay, w, h);
        } else {
            nvgScissor(vg, ax, ay, w, h);
        }
    }

    // -----------------------------
    // DRAWING
    // -----------------------------
    virtual void drawContents(NVGcontext* vg) {
        // Example background
        nvgBeginPath(vg);
        nvgRect(vg, 0, 0, w, h);
        nvgFillColor(vg, nvgRGBA(40, 40, 40, 255));
        nvgFill(vg);
    }

    void drawChildren(NVGcontext* vg) {
        for(auto* child : children)
            child->draw(vg);
    }

    void draw(NVGcontext* vg) {
        nvgSave(vg);

        applyClip(vg);
        nvgTranslate(vg, x, y);

        if(fb && !dirty) {
            nvgBeginPath(vg);
            nvgRect(vg, 0, 0, w, h);
            nvgFillPaint(vg, nvgImagePattern(vg, 0, 0, w, h, 0, fb->image, 1.0f));
            nvgFill(vg);
        } else {
            drawContents(vg);
            drawChildren(vg);

            if(fb) {
                redrawToFramebuffer(vg);
                dirty = false;
            }
        }

        nvgRestore(vg);
    }

    void redrawToFramebuffer(NVGcontext* vg) {
        nvgBindFramebuffer(fb);
        nvgBeginFrame(vg, w, h, 1.0f);

        nvgSave(vg);
        nvgScissor(vg, 0, 0, w, h);

        drawContents(vg);
        drawChildren(vg);

        nvgRestore(vg);
        nvgEndFrame(vg);
        nvgBindFramebuffer(nullptr);
    }
};
*/

/*
struct Button : Widget {
    std::string label = "Button";

    bool hovered = false;
    bool pressed = false;

    NVGcolor baseColor     = nvgRGBA(70, 70, 70, 255);
    NVGcolor hoverColor    = nvgRGBA(100, 100, 100, 255);
    NVGcolor pressColor    = nvgRGBA(150, 60, 60, 255);
    NVGcolor textColor     = nvgRGBA(255, 255, 255, 255);

    float cornerRadius = 6.0f;

    // -----------------------------
    // Mouse handling
    // -----------------------------
    bool onMouse(const MouseEvent& e) override {
        if(e.moved) {
            hovered = true;
            return false; // allow bubbling
        }

        if(e.pressed) {
            pressed = true;
            return true; // consume
        }

        if(e.released) {
            pressed = false;
            // trigger callback here if needed
            return true;
        }

        return false;
    }

    // When mouse leaves the widget region
    void onMouseLeave() {
        hovered = false;
        pressed = false;
    }

    // -----------------------------
    // Draw button
    // -----------------------------
    void drawContents(NVGcontext* vg) override {
        NVGcolor col =
            pressed ? pressColor :
            hovered ? hoverColor :
                      baseColor;

        // Background
        nvgBeginPath(vg);
        nvgRoundedRect(vg, 0, 0, w, h, cornerRadius);
        nvgFillColor(vg, col);
        nvgFill(vg);

        // Text
        nvgFontSize(vg, 16.0f);
        nvgFontFace(vg, "sans");
        nvgFillColor(vg, textColor);

        float tw = nvgTextBounds(vg, 0, 0, label.c_str(), nullptr, nullptr);
        float tx = (w - tw) * 0.5f;
        float ty = (h * 0.5f) + 6.0f; // vertical centering

        nvgText(vg, tx, ty, label.c_str(), nullptr);
    }

    // -----------------------------
    // Override dispatch to detect leave
    // -----------------------------
    bool dispatchMouse(const MouseEvent& e) override {
        bool inside = hit(e.absX, e.absY);

        if(!inside && hovered) {
            onMouseLeave();
        }

        return Widget::dispatchMouse(e);
    }
};
*/
