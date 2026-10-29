#pragma once
#include <de/gpu/VideoDriver.h>
// #include <GL/glew.h>
// #include <vector>
// #include <string>
// #include <stdexcept>
// #include <algorithm>
// #include <cmath>

namespace de {
namespace gpu {

std::string TextureFormat_str(uint32_t internalFormat);

struct Tex2DArray
{
    uint32_t id;
    int32_t w;          // Same width for all images.
    int32_t h;     // Same height for all images.
    int32_t layers;
    int32_t mipLevels;
    uint32_t gpuFormat;
    SamplerOptions so;

    Tex2DArray();
    ~Tex2DArray();

    // Verschiebe-Semantik (Move)
    // Tex2DArray(Tex2DArray&& other) noexcept;
    // Tex2DArray& operator=(Tex2DArray&& other) noexcept;
    Tex2DArray(Tex2DArray&& other) = delete;
    Tex2DArray& operator=(Tex2DArray&& other) = delete;
    Tex2DArray(const Tex2DArray&) = delete;
    Tex2DArray& operator=(const Tex2DArray&) = delete;

    bool empty() const { return id == 0; }
    bool create(const std::string& name, const std::vector<const Image*>& images, const SamplerOptions& so);
    void destroy();

    std::string str() const;

    /**
     * Erstellt das Textur-Array. Berechnet automatisch die maximal möglichen Mipmap-Stufen.
     */
    // static Tex2DArray
    // create(const std::string& name, const std::vector<const Image*>& images, const SamplerOptions& so);

    /**
     * Diese Funktion wird kurz vor dem Rendern aufgerufen.
     * Sie setzt die Filteroptionen und generiert die Mipmaps NUR DANN, wenn sie gebraucht werden.
     */
    // void applySamplerOptions(const SamplerOptions& options, uint32_t slot = 0);
};

} // end namespace gpu.
} // end namespace de.
