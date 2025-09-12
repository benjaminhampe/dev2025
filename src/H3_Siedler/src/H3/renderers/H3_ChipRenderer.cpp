#include "H3_ChipRenderer.h"
#include <H3/H3_Game.h>
#include <de/image/Utils.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

namespace {

    // Used to index layer in texture2DArray
    enum eType
    {
        Chip2 = 0,
        Chip3,
        Chip4,
        Chip5,
        Chip6,
        Chip8,
        Chip9,
        Chip10,
        Chip11,
        Chip12,
        ChipS,
        ChipW,
        Chip3v1,
        ChipA,
        ChipB,
        ChipC,
        ChipD,
        ChipE,
        eTypeCount
    };

} // end namespace anonymous.


H3_ChipMeshBuffer::H3_ChipMeshBuffer()
{
    m_smesh = nullptr;
    m_vao = 0;
    m_vbo_vertices = 0;
    m_vbo_indices = 0;
    m_vbo_instanceMat = 0;
    m_vbo_instanceLayer = 0;
    // m_nVertices = 0;
    // m_nIndices = 0;
}

void H3_ChipMeshBuffer::render() const
{
    if (!m_smesh) { DE_ERROR("No smesh") return; }

    if (m_instanceMat.empty()) { return; }

    glBindVertexArray(m_vao);

    const GLenum primType = de::gpu::PrimitiveType::toOpenGL( m_smesh->primitiveType );

    if (m_smesh->indices.size() > 0)
    {
        glDrawElementsInstanced(
            primType,
            GLsizei(m_smesh->indices.size()),
            GL_UNSIGNED_INT,
            nullptr,
            m_instanceMat.size());
    }
    else
    {
        glDrawArraysInstanced(
            primType,
            0,
            GLsizei(m_smesh->indices.size()),
            m_instanceMat.size());
    }

    glBindVertexArray(0);
}

void H3_ChipMeshBuffer::initVAO()
{
    if (!m_smesh) { DE_ERROR("No smesh") return; }

    if (!m_vbo_vertices) { glGenBuffers(1, &m_vbo_vertices); }
    if (!m_vbo_indices && m_smesh->indices.size()) { glGenBuffers(1, &m_vbo_indices); }
    if (!m_vbo_instanceMat) { glGenBuffers(1, &m_vbo_instanceMat); }
    if (!m_vbo_instanceLayer) { glGenBuffers(1, &m_vbo_instanceLayer); }
    if (!m_vao) { glGenVertexArrays(1, &m_vao); }

    glBindVertexArray(m_vao);

    // vbo_vertices
    const auto & vertices = m_smesh->vertices;
    const size_t vertexCount = vertices.size();
    const size_t vertexSize = sizeof(de::gpu::S3DVertex);
    const size_t vertexBytes = vertexCount * vertexSize;
    const uint8_t* pVertices = reinterpret_cast< const uint8_t* >( vertices.data() );
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexBytes), pVertices, GL_STATIC_DRAW);
    // a_pos
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(de::gpu::S3DVertex), (void*)(0) );
    glEnableVertexAttribArray( 0 );
    // a_normal
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(de::gpu::S3DVertex), (void*)(12) );
    glEnableVertexAttribArray( 1 );
    // a_color
    glVertexAttribPointer( 2, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(de::gpu::S3DVertex), (void*)(24) );
    glEnableVertexAttribArray( 2 );
    // a_tex
    glVertexAttribPointer( 3, 2, GL_FLOAT, GL_FALSE, sizeof(de::gpu::S3DVertex), (void*)(28) );
    glEnableVertexAttribArray( 3 );

    // vbo_instanceMat:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceMat);
    //glBufferData(GL_ARRAY_BUFFER, m_instanceMat.size() * sizeof(glm::mat4), m_instanceMat.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(0));
    glVertexAttribDivisor(4, 1);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(16));
    glVertexAttribDivisor(5, 1);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(32));
    glVertexAttribDivisor(6, 1);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(48));
    glVertexAttribDivisor(7, 1);
    glEnableVertexAttribArray(7);

    // vbo_instanceLayer:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceLayer);
    //glBufferData(GL_ARRAY_BUFFER, m_instanceLayer.size() * sizeof(uint8_t), m_instanceLayer.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(8, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(uint8_t), (void*)0);
    glVertexAttribDivisor(8, 1); // Per-instance divisor
    glEnableVertexAttribArray(8);

    /*
    // vbo_instanceAngle60:
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instanceAngle60); GL_VALIDATE;
    glBufferData(GL_ARRAY_BUFFER, m_instanceAngle60.size() * sizeof(uint8_t), m_instanceAngle60.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(9, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(uint8_t), (void*)0);
    glVertexAttribDivisor(9, 1); // Per-instance divisor
    glEnableVertexAttribArray(9);
    */
    if ( m_vbo_indices )
    {
        const auto nI = m_smesh->indices.size() * sizeof(uint32_t);
        const auto pI = reinterpret_cast< const uint8_t* >( m_smesh->indices.data() );
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(nI), pI, GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (m_vbo_indices)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    GL_VALIDATE;
}


void H3_ChipMeshBuffer::uploadInstances()
{
    //collectInstances( game );

    if (m_instanceMat.empty())
    {
        return; // Nothing to upload.
    }

    // vbo_instanceMat:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceMat);
    glBufferData(GL_ARRAY_BUFFER, m_instanceMat.size() * sizeof(glm::mat4), m_instanceMat.data(), GL_STATIC_DRAW);

    // vbo_instanceLayer:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceLayer);
    glBufferData(GL_ARRAY_BUFFER, m_instanceLayer.size() * sizeof(uint8_t), m_instanceLayer.data(), GL_STATIC_DRAW);

    // vbo_instanceAngle60:
    //glBindBuffer(GL_ARRAY_BUFFER, vbo_instanceAngle60);
    //glBufferData(GL_ARRAY_BUFFER, m_instanceAngle60.size() * sizeof(uint8_t), m_instanceAngle60.data(), GL_STATIC_DRAW);

    GL_VALIDATE;
}


void H3_ChipMeshBuffer::collectEdges(const H3_Game& game)
{
    // Reset
    m_instanceMat.clear();
    m_instanceLayer.clear();
    m_instanceAngle60.clear();

    // Reserve
    const auto n = game.m_edges.size();
    m_instanceMat.reserve(n);
    m_instanceLayer.reserve(n);
    m_instanceAngle60.reserve(n);

    // Collect
    for (const auto& edge : game.m_edges)
    {
        if (!edge.visible) continue;

        de::TRSf trs;
        trs.pos = edge.trs.pos;
        trs.update();

        m_instanceMat.push_back( trs.modelMat );
        m_instanceLayer.push_back( ChipW );
        m_instanceAngle60.push_back( 0 );
    }
}

void H3_ChipMeshBuffer::collectCorners(const H3_Game& game)
{
    // Reset
    m_instanceMat.clear();
    m_instanceLayer.clear();
    m_instanceAngle60.clear();

    // Reserve
    const auto n = game.m_corners.size();
    m_instanceMat.reserve(n);
    m_instanceLayer.reserve(n);
    m_instanceAngle60.reserve(n);

    // Collect
    for (const auto& corner : game.m_corners)
    {
        if (!corner.visible) continue;

        de::TRSf trs;
        trs.pos = corner.trs.pos;
        trs.update();

        m_instanceMat.push_back( trs.modelMat );
        m_instanceLayer.push_back( ChipS );
        m_instanceAngle60.push_back( 0 );
    }
}

void H3_ChipMeshBuffer::collectTiles(const H3_Game& game)
{
    // Reset
    m_instanceMat.clear();
    m_instanceLayer.clear();
    m_instanceAngle60.clear();

    // Reserve
    const auto n = game.m_tiles.size();
    m_instanceMat.reserve(n);
    m_instanceLayer.reserve(n);
    m_instanceAngle60.reserve(n);

    // Collect
    for (const auto& tile : game.m_tiles)
    {
        int layer = -1; // Unknown tex (question mark on black background)

        if (tile.tileType.isResource())
        {
            switch( tile.chipValue )
            {
            case 2: layer = Chip2; break;
            case 3: layer = Chip3; break;
            case 4: layer = Chip4; break;
            case 5: layer = Chip5; break;
            case 6: layer = Chip6; break;
            case 8: layer = Chip8; break;
            case 9: layer = Chip9; break;
            case 10: layer = Chip10; break;
            case 11: layer = Chip11; break;
            case 12: layer = Chip12; break;
            default: break;
            }
        }
        else if (tile.tileType.isPort())
        {
            switch( tile.tileType.value )
            {
            case H3_TileType::Port3v1: layer = Chip3v1; break;
            case H3_TileType::PortA: layer = ChipA; break;
            case H3_TileType::PortB: layer = ChipB; break;
            case H3_TileType::PortC: layer = ChipC; break;
            case H3_TileType::PortD: layer = ChipD; break;
            case H3_TileType::PortE: layer = ChipE; break;
            default: break;
            }
        }

        if ( layer < 0 ) continue;

        de::TRSf trs;
        trs.pos = tile.trsChip.pos;
        trs.update();

        m_instanceMat.push_back( trs.modelMat );
        m_instanceLayer.push_back( layer );
        m_instanceAngle60.push_back( 0 );
    }

}

// =================================================================
H3_ChipRenderer::H3_ChipRenderer()
// =================================================================
{
    m_imgSize = 70;
    m_driver = nullptr;
    m_shader = nullptr;
    m_texture2DArray = 0;
    // m_vao = 0;
    // m_vbo_vertices = 0;
    // m_vbo_indices = 0;
    // m_vbo_instanceMat = 0;
    // m_vbo_instanceLayer = 0;
    // m_nVertices = 0;
    // m_nIndices = 0;
}

H3_ChipRenderer::~H3_ChipRenderer()
{

}

void H3_ChipRenderer::render( const H3_ChipMeshBuffer & drawcall )
{
    if (drawcall.m_instanceMat.empty())
    {
        return; // Nothing to draw.
    }

    if (!m_driver) { DE_ERROR("No driver") return; }

    // ==== Shader: u_projViewMat  ====
    m_driver->useShader(m_shader);

    // ==== Shader: u_projViewMat  ====
    glm::mat4 projViewMat(1.0f);
    auto camera = m_driver->getCamera();
    if (camera)
    {
        projViewMat = camera->getViewProjectionMatrix();
    }
    glUniformMatrix4fv(m_locViewProjMat, 1, GL_FALSE, glm::value_ptr( projViewMat ));

    // ==== Shader: u_tex  ====
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture2DArray);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glUniform1i(m_locTex, 0);

    // ==== RenderStates  ====
    de::gpu::State state;
    state.culling = de::gpu::Culling::disabled();
    state.blend = de::gpu::Blend::disabled();
    m_driver->setState( state );

    drawcall.render();

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    GL_VALIDATE;
}

void
H3_ChipRenderer::init( H3_Game & game )
{
    m_driver = game.getDriver();

    initShader();
    initTexture( game );
}

void H3_ChipRenderer::initShader()
{
    if (!m_driver) { DE_ERROR("No driver") return; }

    if (m_shader) { return; } // Nothing todo.

    const std::string shaderName = "ChipShader";

    const std::string vs = R"(
        layout(location = 0) in vec3 a_pos;
        layout(location = 1) in vec3 a_normal;
        layout(location = 2) in lowp vec4 a_color;
        layout(location = 3) in vec2 a_texCoord;
        layout(location = 4) in mat4 a_instanceMat; // (mat4 spans 4 attribute locations)
        layout(location = 8) in float a_instanceLayer; // index into texture2DArray

        uniform mat4 u_projViewMat;

        out vec3 v_pos;
        out vec3 v_normal;
        out vec2 v_texCoord;
        out vec4 v_color;
        flat out int v_texLayer;

        void main()
        {
            v_pos = a_pos;
            v_normal = a_normal;
            v_color = clamp( vec4( a_color ) / 255.0, vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
            v_texCoord = a_texCoord;
            v_texLayer = int(a_instanceLayer);
            gl_Position = u_projViewMat * a_instanceMat * vec4(a_pos, 1.0);
        }
    )";

    const std::string fs = R"(

        out vec4 fragColor;

        in vec3 v_pos;
        in vec3 v_normal;
        in vec4 v_color;
        in vec2 v_texCoord;
        flat in int v_texLayer;

        uniform sampler2DArray u_tex; // Array of 2D diffuseMaps

        void main()
        {
            fragColor = texture(u_tex, vec3(v_texCoord, v_texLayer)) * v_color;
        }
    )";

    m_shader = m_driver->createShader( shaderName, vs, fs );

    m_locViewProjMat = glGetUniformLocation(m_shader->id, "u_projViewMat");
    m_locTex = glGetUniformLocation(m_shader->id, "u_tex");
}

void H3_ChipRenderer::initTexture( H3_Game & game )
{
    if (!m_driver) { DE_ERROR("No driver") return; }

    if (m_texture2DArray) { return; } // Nothing todo.

    std::vector<de::Image> m_images;
    m_images.reserve( 18 );

    auto addImage = [&]( H3_Img::eID id )
    {
        m_images.emplace_back();
        de::Image & img = m_images.back();

        img = *H3_getImg( game, id );

        if (img.pixelFormat() == de::PixelFormat::R8G8B8)
        {
            img = img.convert(de::PixelFormat::R8G8B8A8);
        }

        int w = img.w();
        int h = img.h();
        if (w < h)
        {
            int x = 0;
            int y = (h-w)/2;
            img = de::ImageCutter::copyRegionRect(img,de::Recti(x,y,w,w));
        }
        else if (h < w)
        {
            int x = (w-h)/2;
            int y = 0;
            img = de::ImageCutter::copyRegionRect(img,de::Recti(x,y,h,h));
        }

        if ( m_imgSize != img.w() )
        {
            img = de::ImageScaler::resizeBicubic(img, m_imgSize, m_imgSize);
        }

        //dbSaveImage(img, dbStrJoin("chip_",uint32_t(id),".webp"));
    };

    addImage( H3_Img::Chip2);    // 0
    addImage( H3_Img::Chip3);    // 1
    addImage( H3_Img::Chip4);    // 2
    addImage( H3_Img::Chip5);    // 3
    addImage( H3_Img::Chip6);    // 4
    addImage( H3_Img::Chip8);    // 5
    addImage( H3_Img::Chip9);    // 6
    addImage( H3_Img::Chip10);   // 7
    addImage( H3_Img::Chip11);   // 8
    addImage( H3_Img::Chip12);   // 9
    addImage( H3_Img::ChipS);    // 10
    addImage( H3_Img::ChipW);    // 11
    addImage( H3_Img::Chip3v1);  // 12
    addImage( H3_Img::ChipA);    // 13
    addImage( H3_Img::ChipB);    // 14
    addImage( H3_Img::ChipC);    // 15
    addImage( H3_Img::ChipD);    // 16
    addImage( H3_Img::ChipE);    // 17

    const int n = m_images.size();

    de::Image bigImage( m_imgSize, m_imgSize * n, de::PixelFormat::R8G8B8A8 );

    int y = 0;
    for (size_t i = 0; i < m_images.size(); ++i)
    {
        de::ImagePainter::drawImage(bigImage, m_images[i], 0, y, false);
        y += m_imgSize;
    }

    de::replaceColor(bigImage,0x00000000, 0xFFFFFFFF);

    //dbSaveImage(bigImage,"H3_chips.webp");

    glGenTextures(1, &m_texture2DArray);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture2DArray);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, m_imgSize, m_imgSize, n, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, bigImage.data());
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    GL_VALIDATE;
}

/*
void H3_ChipRenderer::collectInstances(const H3_Game& game)
{
    if (!m_driver) { DE_ERROR("No driver") return; }

    const auto & g_tiles = game.m_tiles;
    const auto & g_corners = game.m_corners;
    const auto & g_edges = game.m_edges;

    const size_t nTiles = g_tiles.size();
    const size_t nCorners = g_corners.size();
    const size_t nEdges = g_edges.size();
    const size_t n = nTiles + nCorners + nEdges;

    //========================================================================
    // CPU Memory:
    //========================================================================

    // Instance Data (Transform Matrices)
    m_instanceMat.clear();
    m_instanceMat.reserve(n);
    m_instanceLayer.clear();
    m_instanceLayer.reserve(n);
    m_instanceAngle60.clear();
    m_instanceAngle60.reserve(n);

    enum eType
    {
        Chip2 = 0, Chip3, Chip4, Chip5, Chip6, Chip8, Chip9, Chip10, Chip11, Chip12,
        ChipS, ChipW, Chip3v1, ChipA, ChipB, ChipC, ChipD, ChipE, eTypeCount
    };

    // Collect Corners
    if (game.m_bCornersVisible)
    {
        for (const auto& corner : g_corners)
        {
            //if (corner.enabled)
            if (corner.building_type == 0)
            {
                m_instanceMat.push_back( corner.trs.trs );
                m_instanceLayer.push_back( ChipS );
                m_instanceAngle60.push_back( 0 );
            }
        }
    }

    // Collect Edges
    if (game.m_bEdgesVisible)
    {
        for (const auto& edge : g_edges)
        {
            //if (edge.enabled)
            if (edge.owner == 0)
            {
                m_instanceMat.push_back( edge.trs.trs );
                m_instanceLayer.push_back( ChipW );
                m_instanceAngle60.push_back( 0 );
            }
        }
    }

    // Collect TileChips
    for (const auto& tile : g_tiles)
    {
        int layer = -1; // Unknown tex (question mark on black background)

        if (tile.tileType.isResource())
        {
            switch( tile.chipValue )
            {
            case 2: layer = Chip2; break;
            case 3: layer = Chip3; break;
            case 4: layer = Chip4; break;
            case 5: layer = Chip5; break;
            case 6: layer = Chip6; break;
            case 8: layer = Chip8; break;
            case 9: layer = Chip9; break;
            case 10: layer = Chip10; break;
            case 11: layer = Chip11; break;
            case 12: layer = Chip12; break;
            default: break;
            }
        }
        else if (tile.tileType.isPort())
        {
            switch( tile.tileType.value )
            {
            case H3_TileType::Port3v1: layer = Chip3v1; break;
            case H3_TileType::PortA: layer = ChipA; break;
            case H3_TileType::PortB: layer = ChipB; break;
            case H3_TileType::PortC: layer = ChipC; break;
            case H3_TileType::PortD: layer = ChipD; break;
            case H3_TileType::PortE: layer = ChipE; break;
            default: break;
            }
        }

        if ( layer < 0 ) continue;

        m_instanceMat.push_back( tile.trsChip.trs );

        m_instanceLayer.push_back( layer );

        m_instanceAngle60.push_back( 0 );
    }

}

void H3_ChipRenderer::upload(const H3_Game& game)
{
    if (!m_driver) { DE_ERROR("No driver") return; }

    collectInstances( game );

    //========================================================================
    // GPU Memory:
    //========================================================================
    m_nVertices = game.m_chipMesh.vertices.size();
    m_nIndices = game.m_chipMesh.indices.size();

    if (!m_vbo_vertices)
    {
        glGenBuffers(1, &m_vbo_vertices);
        //DE_TRACE("vbo_vertices = ",m_vbo_vertices)
    }

    if (!m_vbo_indices && game.m_chipMesh.indices.size())
    {
        glGenBuffers(1, &m_vbo_indices);
        //DE_TRACE("vbo_indices = ",m_vbo_indices)
    }

    if (!m_vbo_instanceMat)
    {
        glGenBuffers(1, &m_vbo_instanceMat);
        //DE_TRACE("vbo_instanceMat = ",m_vbo_instanceMat)
    }

    if (!m_vbo_instanceLayer)
    {
        glGenBuffers(1, &m_vbo_instanceLayer);
        //DE_TRACE("vbo_instanceLayer = ",m_vbo_instanceLayer)
    }

    if (!m_vao)
    {
        glGenVertexArrays(1, &m_vao);
        //DE_TRACE("m_vao = ",m_vao)
    }

    glBindVertexArray(m_vao);

    // vbo_vertices
    const auto & vertices = game.m_chipMesh.vertices;
    const size_t vertexCount = vertices.size();
    const size_t vertexSize = sizeof(de::gpu::S3DVertex);
    const size_t vertexBytes = vertexCount * vertexSize;
    const uint8_t* pVertices = reinterpret_cast< const uint8_t* >( vertices.data() );
    //std::cout << "Upload " << vertexCount << " vertices" << std::endl;
    //std::cout << "Upload " << vertexBytes << " bytes" << std::endl;
    //std::cout << "Upload " << vertexSize << " stride" << std::endl;
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexBytes), pVertices, GL_STATIC_DRAW);
    // a_pos
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(de::gpu::S3DVertex), (void*)(0) );
    glEnableVertexAttribArray( 0 );
    // a_normal
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(de::gpu::S3DVertex), (void*)(12) );
    glEnableVertexAttribArray( 1 );
    // a_color
    glVertexAttribPointer( 2, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(de::gpu::S3DVertex), (void*)(24) );
    glEnableVertexAttribArray( 2 );
    // a_tex
    glVertexAttribPointer( 3, 2, GL_FLOAT, GL_FALSE, sizeof(de::gpu::S3DVertex), (void*)(28) );
    glEnableVertexAttribArray( 3 );

    // vbo_instanceMat:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceMat);
    glBufferData(GL_ARRAY_BUFFER, m_instanceMat.size() * sizeof(glm::mat4), m_instanceMat.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(0));
    glVertexAttribDivisor(4, 1);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(16));
    glVertexAttribDivisor(5, 1);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(32));
    glVertexAttribDivisor(6, 1);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(48));
    glVertexAttribDivisor(7, 1);
    glEnableVertexAttribArray(7);

    // vbo_instanceLayer:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceLayer);
    glBufferData(GL_ARRAY_BUFFER, m_instanceLayer.size() * sizeof(uint8_t), m_instanceLayer.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(8, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(uint8_t), (void*)0);
    glVertexAttribDivisor(8, 1); // Per-instance divisor
    glEnableVertexAttribArray(8);

    // vbo_instanceAngle60:
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instanceAngle60); GL_VALIDATE;
    glBufferData(GL_ARRAY_BUFFER, m_instanceAngle60.size() * sizeof(uint8_t), m_instanceAngle60.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(9, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(uint8_t), (void*)0);
    glVertexAttribDivisor(9, 1); // Per-instance divisor
    glEnableVertexAttribArray(9);

    if ( m_vbo_indices )
    {
        const auto & indices = game.m_chipMesh.indices;
        const size_t indexCount = indices.size();
        const size_t indexSize = sizeof(uint32_t);
        const size_t indexBytes = indexCount * indexSize;
        const uint8_t* pIndices = reinterpret_cast< const uint8_t* >( indices.data() );

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indexBytes), pIndices, GL_STATIC_DRAW);

        //std::cout << "Upload " << vertexCount << " vertices" << std::endl;
        //std::cout << "Upload " << vertexBytes << " bytes" << std::endl;
        //std::cout << "Upload " << vertexSize << " stride" << std::endl;
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (m_vbo_indices)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    GL_VALIDATE;
}


void H3_ChipRenderer::uploadInstances(const H3_Game& game)
{
    collectInstances( game );

    if (m_instanceMat.empty())
    {
        return; // Nothing to upload.
    }

    // vbo_instanceMat:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceMat);
    glBufferData(GL_ARRAY_BUFFER, m_instanceMat.size() * sizeof(glm::mat4), m_instanceMat.data(), GL_STATIC_DRAW);

    // vbo_instanceLayer:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceLayer);
    glBufferData(GL_ARRAY_BUFFER, m_instanceLayer.size() * sizeof(uint8_t), m_instanceLayer.data(), GL_STATIC_DRAW);

    // vbo_instanceAngle60:
    //glBindBuffer(GL_ARRAY_BUFFER, vbo_instanceAngle60);
    //glBufferData(GL_ARRAY_BUFFER, m_instanceAngle60.size() * sizeof(uint8_t), m_instanceAngle60.data(), GL_STATIC_DRAW);

    GL_VALIDATE;
}

void H3_ChipRenderer::collectInstances(const H3_Game& board)
{
    if (!m_driver) { DE_ERROR("No driver") return; }

    enum eType
    {
    Chip2 = 0, Chip3, Chip4, Chip5, Chip6, Chip8, Chip9, Chip10, Chip11, Chip12,
    ChipS, ChipW, Chip3v1, ChipA, ChipB, ChipC, ChipD, ChipE, eTypeCount
    };

    const auto & g_tiles = board.m_tiles;
    const auto & g_corners = board.m_corners;
    const auto & g_edges = board.m_edges;

    const size_t nTiles = g_tiles.size();
    const size_t nCorners = g_corners.size();
    const size_t nEdges = g_edges.size();
    const size_t n = nTiles + nCorners + nEdges;

    //========================================================================
    // CPU Memory:
    //========================================================================

    // Instance Data (Transform Matrices)
    m_instanceMat.clear();
    m_instanceMat.reserve(n);
    m_instanceLayer.clear();
    m_instanceLayer.reserve(n);
    m_instanceAngle60.clear();
    m_instanceAngle60.reserve(n);

    // Collect Corners
    for (const auto& corner : g_corners)
    {
        m_instanceMat.push_back( corner.trs.trs );
        m_instanceLayer.push_back( ChipS );
        m_instanceAngle60.push_back( 0 );
    }

    // Collect Edges
    for (const auto& edge : g_edges)
    {
        m_instanceMat.push_back( edge.trs.trs );
        m_instanceLayer.push_back( ChipW );
        m_instanceAngle60.push_back( 0 );
    }

    // Collect TileChips
    for (const auto& tile : g_tiles)
    {
        int layer = -1; // Unknown tex (question mark on black background)

        if (tile.tileType.isResource())
        {
            switch( tile.chipValue )
            {
                case 2: layer = Chip2; break;
                case 3: layer = Chip3; break;
                case 4: layer = Chip4; break;
                case 5: layer = Chip5; break;
                case 6: layer = Chip6; break;
                case 8: layer = Chip8; break;
                case 9: layer = Chip9; break;
                case 10: layer = Chip10; break;
                case 11: layer = Chip11; break;
                case 12: layer = Chip12; break;
                default: break;
            }
        }
        else if (tile.tileType.isPort())
        {
            switch( tile.tileType.value )
            {
                case H3_TileType::Port3v1: layer = Chip3v1; break;
                case H3_TileType::PortA: layer = ChipA; break;
                case H3_TileType::PortB: layer = ChipB; break;
                case H3_TileType::PortC: layer = ChipC; break;
                case H3_TileType::PortD: layer = ChipD; break;
                case H3_TileType::PortE: layer = ChipE; break;
                default: break;
            }
        }

        if ( layer < 0 ) continue;

        m_instanceMat.push_back( tile.trsChip.trs );

        m_instanceLayer.push_back( layer );

        m_instanceAngle60.push_back( 0 );
    }

    //========================================================================
    // GPU Memory:
    //========================================================================
    m_nVertices = board.m_chipMesh.vertices.size();
    m_nIndices = board.m_chipMesh.indices.size();

    if (!m_vbo_vertices)
    {
        glGenBuffers(1, &m_vbo_vertices);
        //DE_TRACE("vbo_vertices = ",m_vbo_vertices)
    }

    if (!m_vbo_indices && board.m_chipMesh.indices.size())
    {
        glGenBuffers(1, &m_vbo_indices);
        //DE_TRACE("vbo_indices = ",m_vbo_indices)
    }

    if (!m_vbo_instanceMat)
    {
        glGenBuffers(1, &m_vbo_instanceMat);
        //DE_TRACE("vbo_instanceMat = ",m_vbo_instanceMat)
    }

    if (!m_vbo_instanceLayer)
    {
        glGenBuffers(1, &m_vbo_instanceLayer);
        //DE_TRACE("vbo_instanceLayer = ",m_vbo_instanceLayer)
    }

if (!m_vao)
{
    glGenVertexArrays(1, &m_vao);
    //DE_TRACE("m_vao = ",m_vao)
}

glBindVertexArray(m_vao);

// vbo_vertices
const auto & vertices = board.m_chipMesh.vertices;
const size_t vertexCount = vertices.size();
const size_t vertexSize = sizeof(de::gpu::S3DVertex);
const size_t vertexBytes = vertexCount * vertexSize;
const uint8_t* pVertices = reinterpret_cast< const uint8_t* >( vertices.data() );
//std::cout << "Upload " << vertexCount << " vertices" << std::endl;
//std::cout << "Upload " << vertexBytes << " bytes" << std::endl;
//std::cout << "Upload " << vertexSize << " stride" << std::endl;
glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices);
glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexBytes), pVertices, GL_STATIC_DRAW);
// a_pos
glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(de::gpu::S3DVertex), (void*)(0) );
glEnableVertexAttribArray( 0 );
// a_normal
glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(de::gpu::S3DVertex), (void*)(12) );
glEnableVertexAttribArray( 1 );
// a_color
glVertexAttribPointer( 2, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(de::gpu::S3DVertex), (void*)(24) );
glEnableVertexAttribArray( 2 );
// a_tex
glVertexAttribPointer( 3, 2, GL_FLOAT, GL_FALSE, sizeof(de::gpu::S3DVertex), (void*)(28) );
glEnableVertexAttribArray( 3 );

// vbo_instanceMat:
glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceMat);
glBufferData(GL_ARRAY_BUFFER, m_instanceMat.size() * sizeof(glm::mat4), m_instanceMat.data(), GL_STATIC_DRAW);
glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(0));
glVertexAttribDivisor(4, 1);
glEnableVertexAttribArray(4);
glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(16));
glVertexAttribDivisor(5, 1);
glEnableVertexAttribArray(5);
glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(32));
glVertexAttribDivisor(6, 1);
glEnableVertexAttribArray(6);
glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(48));
glVertexAttribDivisor(7, 1);
glEnableVertexAttribArray(7);

// vbo_instanceLayer:
glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceLayer);
glBufferData(GL_ARRAY_BUFFER, m_instanceLayer.size() * sizeof(uint8_t), m_instanceLayer.data(), GL_STATIC_DRAW);
glVertexAttribPointer(8, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(uint8_t), (void*)0);
glVertexAttribDivisor(8, 1); // Per-instance divisor
glEnableVertexAttribArray(8);

if ( m_vbo_indices )
{
    const auto & indices = board.m_chipMesh.indices;
    const size_t indexCount = indices.size();
    const size_t indexSize = sizeof(uint32_t);
    const size_t indexBytes = indexCount * indexSize;
    const uint8_t* pIndices = reinterpret_cast< const uint8_t* >( indices.data() );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indexBytes), pIndices, GL_STATIC_DRAW);

    //std::cout << "Upload " << vertexCount << " vertices" << std::endl;
    //std::cout << "Upload " << vertexBytes << " bytes" << std::endl;
    //std::cout << "Upload " << vertexSize << " stride" << std::endl;
}

glBindVertexArray(0);
glBindBuffer(GL_ARRAY_BUFFER, 0);

if (m_vbo_indices)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GL_VALIDATE;
}

void H3_ChipRenderer::initTexture( H3_Game & game )
{
    if (!m_driver) { DE_ERROR("No driver") return; }


    std::string mediaDir = "media/H3/gfx/chip/";
    addImage("chip2", mediaDir + "chip_2.png" );    // 0
    addImage("chip3", mediaDir + "chip_3.png" );    // 1
    addImage("chip4", mediaDir + "chip_4.png" );    // 2
    addImage("chip5", mediaDir + "chip_5.png" );    // 3
    addImage("chip6", mediaDir + "chip_6.png" );    // 4
    addImage("chip8", mediaDir + "chip_8.png" );    // 5
    addImage("chip9", mediaDir + "chip_9.png" );    // 6
    addImage("chip10",mediaDir + "chip_10.png");    // 7
    addImage("chip11",mediaDir + "chip_11.png");    // 8
    addImage("chip12",mediaDir + "chip_12.png");    // 9
    addImage("chipS", mediaDir + "chip_s.png" );    // 10
    addImage("chipW", mediaDir + "chip_w.png" );    // 11
    addImage("chip3v1",mediaDir+ "chip_3v1.png" );  // 12
    addImage("chipA", mediaDir + "chip_a.jpg" );    // 13
    addImage("chipB", mediaDir + "chip_b.png" );    // 14
    addImage("chipC", mediaDir + "chip_c.jpg" );    // 15
    addImage("chipD", mediaDir + "chip_d.jpg" );    // 16
    addImage("chipE", mediaDir + "chip_e.jpg" );    // 17

    de::Image m_image;
    m_image.resize( m_image_size, m_image_size * int(m_images.size()) );

    int y = 0;
    for (size_t i = 0; i < m_images.size(); ++i)
    {
        de::ImagePainter::drawImage(m_image, m_images[i], 0, y, false);
        y += m_image_size;
    }

    //dbSaveImage(m_image,"H3_chips.png");


    // Create textures:
    if (!m_texture2DArray)
    {
        const int w = m_image_size;
        const int h = m_image_size;
        const int layers = m_images.size();

        glGenTextures(1, &m_texture2DArray);
        GL_VALIDATE;
        glActiveTexture(GL_TEXTURE0);
        GL_VALIDATE;
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture2DArray);
        GL_VALIDATE;
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, w, h, layers, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.data());
        GL_VALIDATE;
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GL_VALIDATE;
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GL_VALIDATE;
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        GL_VALIDATE;
    }
}

*/
