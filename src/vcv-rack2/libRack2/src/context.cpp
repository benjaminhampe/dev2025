#include <rack_context.hpp>
#include <window/Window.hpp>
#include <rack_patch.hpp>
#include <engine/Engine.hpp>
#include <app/Scene.hpp>
#include <rack_history.hpp>
#include <rack_midiloopback.hpp>


namespace rack {


Context::~Context() {
    // Deleting NULL is safe in C++.

    // Set pointers to NULL so other objects will segfault when attempting to access them

    RK_INFO("Deleting window");
    delete window;
    window = NULL;

    RK_INFO("Deleting patch manager");
    delete patch;
    patch = NULL;

    RK_INFO("Deleting scene");
    delete scene;
    scene = NULL;

    RK_INFO("Deleting event state");
    delete event;
    event = NULL;

    RK_INFO("Deleting history state");
    delete history;
    history = NULL;

    RK_INFO("Deleting engine");
    delete engine;
    engine = NULL;

    RK_INFO("Deleting MIDI loopback");
    delete midiLoopbackContext;
    midiLoopbackContext = NULL;
}


static thread_local Context* threadContext = NULL;

Context* RACK_DLL_CALL contextGet() {
    return threadContext;
}

// Apple's clang incorrectly compiles this function when -O2 or higher is enabled.
#ifdef ARCH_MAC
__attribute__((optnone))
#endif
void RACK_DLL_CALL contextSet(Context* context) {
    threadContext = context;
}


} // namespace rack
