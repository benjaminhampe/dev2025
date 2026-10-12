#include "Draw3D.h"
#include <de/gpu/SkyboxRenderer.h>

struct Draw3D
{
    de::gpu::Camera camera;
};

Draw3D g_d3d;

void Draw3D_init()
{
    SkyboxRenderer_init();
}

void Draw3D_draw( int w, int h )
{
    auto camera = &g_d3d.camera;
    camera->setScreenSize(w,h);
    camera->update();

    SkyboxRenderer_draw( &g_d3d.camera );
}

void Draw3D_destroy()
{
    SkyboxRenderer_destroy();
}
