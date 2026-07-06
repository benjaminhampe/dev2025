#pragma once

/*



class RenderThread
{
public:
    RenderThread(QWindow* window)
        : window(window)
    {
        running = true;
        thread = std::thread(&RenderThread::run, this);
    }

    ~RenderThread()
    {
        running = false;
        if (thread.joinable())
            thread.join();
    }

    // hier schiebst du State rein (atomar / lockfrei)
    void submitState(const RenderState& s)
    {
        state.store(s, std::memory_order_release);
    }

private:
    QWindow* window;
    std::thread thread;
    std::atomic<bool> running { false };
    std::atomic<RenderState> state; // oder Pointer auf State

    void run()
    {
        // 1. WGL‑Context in diesem Thread erstellen
        HDC hdc = GetDC(reinterpret_cast<HWND>(window->winId()));
        HGLRC glctx = wglCreateContext(hdc);
        wglMakeCurrent(hdc, glctx);

        // 2. GLEW hier initialisieren
        glewInit();

        // 3. Render‑Loop
        while (running)
        {
            RenderState s = state.load(std::memory_order_acquire);

            // GL‑Zeug
            glViewport(0, 0, window->width(), window->height());
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // dein Draw‑Code basierend auf s
            drawScene(s);

            SwapBuffers(hdc);
        }

        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(glctx);
        ReleaseDC(reinterpret_cast<HWND>(window->winId()), hdc);
    }
};

🧱 So sieht das in Qt aus (QWindow)
1. Maus‑Events bleiben wie immer
cpp

void MyWindow::mousePressEvent(QMouseEvent* e)
{
    cameraState.mouseDown = true;
    cameraState.lastPos = e->pos();
}

void MyWindow::mouseReleaseEvent(QMouseEvent* e)
{
    cameraState.mouseDown = false;
}

void MyWindow::mouseMoveEvent(QMouseEvent* e)
{
    if (!cameraState.mouseDown)
        return;

    QPoint delta = e->pos() - cameraState.lastPos;
    cameraState.lastPos = e->pos();

    // Kamera-Update im UI-Thread
    cameraState.yaw   += delta.x() * 0.01f;
    cameraState.pitch += delta.y() * 0.01f;

    // Lockfrei an Render-Thread übergeben
    sharedCameraState.store(cameraState, std::memory_order_release);
}

Wichtig:

Keine GL‑Calls hier.
Nur Mathe.
🧱 2. Render‑Thread liest den Camera‑State
cpp

void RenderThread::run()
{
    // WGL Context erstellen
    HDC hdc = GetDC(hwnd);
    HGLRC ctx = wglCreateContext(hdc);
    wglMakeCurrent(hdc, ctx);

    glewInit();

    while (running)
    {
        CameraState cam = sharedCameraState.load(std::memory_order_acquire);

        // View-Matrix aus CameraState berechnen
        glm::mat4 view = glm::lookAt(
            cam.position,
            cam.position + cam.forwardVector(),
            cam.upVector()
        );

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        drawScene();

        SwapBuffers(hdc);
    }

    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(ctx);
    ReleaseDC(hwnd, hdc);
}

🎯 Warum das klickfrei ist
✔ UI‑Thread macht nur Mathe

→ keine GPU‑Stalls
→ keine GL‑Flushes
→ keine Context‑Switches
✔ Render‑Thread hat eigenen WGL‑Context

→ keine Qt‑Interaktion
→ keine MessageManager‑Locks
→ keine Host‑Blockaden
✔ Audio‑Thread bleibt komplett unberührt

→ keine Klicks mehr
🎯 Wie du die Camera‑Daten lockfrei übergibst
Option A: std::atomic<CameraState>

Wenn CameraState trivially copyable ist → perfekt.
Option B: Double‑Buffering
cpp

CameraState bufferA, bufferB;
std::atomic<CameraState*> front = &bufferA;
CameraState* back = &bufferB;

UI‑Thread schreibt in back, dann:
cpp

front.store(back, std::memory_order_release);
std::swap(back, front.load());

Render‑Thread liest aus front.
Option C: Ringbuffer (lockfrei)

Wenn du viele Events pro Frame hast.
🎯 Was du nicht tun darfst

    QWindow::update() im Audio‑Thread

    QWindow::repaint() im Render‑Thread

    QOpenGLContext mischen mit WGL

    GL‑Calls im UI‑Thread

    GL‑Calls im Audio‑Thread

    MessageManagerLock irgendwo im DSP‑Pfad

    glFinish() oder glFlush() im UI‑Thread


*/