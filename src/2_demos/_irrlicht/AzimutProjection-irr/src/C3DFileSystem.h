#pragma once
#include <irrlicht.h>
#include <vector>

namespace irr {

/*
struct FileItem
{
    core::aabbox3d<f32> m_bbox;
    core::stringw m_label;
    core::stringw m_uri; // unique, absolute, key
    u64 m_byteSize;
    u32 m_flags; // 0 = none, 1 = file, 2 = directory
};

struct FileList
{
    std::vector<FileItem> m_files;
};

struct FileSys
{
    std::vector<FileItem> m_files;
};
*/

// ===========================================================================
struct DrawTools
// ===========================================================================
{
    static void draw3D( video::IVideoDriver* driver, scene::SMeshBuffer* p )
    {
        if (!p)
        {
            return; // Nothing todo.
        }

        u32 const n = p->getIndexCount() > 0 ? p->getIndexCount() : p->getVertexCount();
        scene::E_PRIMITIVE_TYPE primType = p->getPrimitiveType();
        u32 primCount = 0;
        switch (primType)
        {
            case scene::EPT_TRIANGLES: primCount = n / 3; break;
            case scene::EPT_POINTS: primCount = n; break;
            case scene::EPT_LINES: primCount = n / 2; break;
            default: break;
        }

        driver->setMaterial(p->getMaterial());

        driver->drawVertexPrimitiveList(p->getVertices(),
                                        p->getVertexCount(),
                                        p->getIndices(),
                                        primCount,
                                        p->getVertexType(),
                                        primType,
                                        p->getIndexType()       );
    }

    static scene::SMeshBuffer createTetragonMeshBuffer()
    {
        scene::SMeshBuffer tetragon;
        tetragon.PrimitiveType = scene::EPT_TRIANGLES;
        tetragon.Material.Wireframe = false;
        tetragon.Material.Lighting = false;
        tetragon.Vertices.reallocate( 4 );
        tetragon.Vertices.set_used( 0 );
        tetragon.Vertices.push_back(video::S3DVertex(0,0,10,   1,1,0, video::SColor(255,0,255,255), 0, 1));
        tetragon.Vertices.push_back(video::S3DVertex(10,0,-10, 1,0,0, video::SColor(255,255,0,255), 1, 1));
        tetragon.Vertices.push_back(video::S3DVertex(0,20,0,   0,1,1, video::SColor(255,255,255,0), 1, 0));
        tetragon.Vertices.push_back(video::S3DVertex(-10,0,-10,0,0,1, video::SColor(255,0,255,0), 0, 0));
        tetragon.Vertices.push_back(video::S3DVertex(-10,0,-10,0,0,1, video::SColor(255,0,255,0), 0, 0));
        tetragon.recalculateBoundingBox();
        return tetragon;
    }
};

// ===========================================================================
class Folder3D : public scene::ISceneNode
// ===========================================================================
{
    core::aabbox3d<f32> m_bbox;
    core::stringw m_dirName;
    io::IFileList* m_fileList = nullptr;
    scene::SMeshBuffer m_tetragon;
public:
    Folder3D( scene::ISceneNode* parent, scene::ISceneManager* smgr, s32 id )
        : scene::ISceneNode(parent, smgr, id)
        , m_fileList(nullptr)
    {
        m_tetragon = DrawTools::createTetragonMeshBuffer();
        setDir("C:\\");
    }

    void render() override
    {
        video::IVideoDriver* driver = SceneManager->getVideoDriver();

        core::matrix4 const M0 = driver->getTransform(video::ETS_WORLD);

        driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);

        if (m_fileList)
        {
            for ( u32 i = 0; i < m_fileList->getFileCount(); ++i )
            {
                io::path uri = m_fileList->getFileName(i);
                u64 byteSize = m_fileList->getFileSize(i);

                core::matrix4 T;
                T.setTranslation(core::vector3df(0,20*i,0));
                core::matrix4 Mtrs = AbsoluteTransformation * T;


                driver->setTransform(video::ETS_WORLD, Mtrs);
                DrawTools::draw3D(driver, &m_tetragon);

                driver->draw3DLine(core::vector3df(0,0,0),core::vector3df(100,0,0),video::SColor(255,250,230,210) );
            }
        }

        driver->setTransform(video::ETS_WORLD, M0);
    }

    void setDir( core::stringw const & dirName )
    {
        if (m_fileList)
        {
            m_fileList->drop();
            m_fileList = nullptr;
        }

        if (m_dirName == dirName)
        {
            return;
        }

        auto fs = SceneManager->getFileSystem();
        m_dirName = fs->getAbsolutePath(m_dirName);

        bool ok = fs->changeWorkingDirectoryTo( m_dirName );

        io::IFileList* fl = fs->createFileList();
        for ( u32 i = 0; i < fl->getFileCount(); ++i )
        {
            io::path uri = fl->getFileName(i);
            u64 byteSize = fl->getFileSize(i);
        }

        m_dirName = dirName;
    }

    virtual void
    OnRegisterSceneNode()
    {
        if (IsVisible) SceneManager->registerNodeForRendering(this);
        ISceneNode::OnRegisterSceneNode();
    }


    virtual core::aabbox3d<f32> const &
    getBoundingBox() const { return m_bbox; }

    virtual u32
    getMaterialCount() const { return 1; }

    virtual video::SMaterial&
    getMaterial(u32 i) { return m_tetragon.Material; }
};

} // end namespace irr














































/*
class C3DFileSystem : public scene::ISceneNode
{
    core::aabbox3d<f32> m_bbox;


    scene::SMeshBuffer m_tetragon;
    video::S3DVertex m_tetragonVertices[4];
    u16 m_tetragonIndices[] = { 0,2,3, 2,1,3, 1,0,3, 2,0,1	};
    video::SMaterial m_tetragonMaterial;
public:
    C3DFileSystem( scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id )
        : scene::ISceneNode(parent, mgr, id)
    {
        m_tetragonMaterial.Wireframe = false;
        m_tetragonMaterial.Lighting = false;
        m_tetragonVertices[0] = video::S3DVertex(0,0,10, 1,1,0, video::SColor(255,0,255,255), 0, 1);
        m_tetragonVertices[1] = video::S3DVertex(10,0,-10, 1,0,0, video::SColor(255,255,0,255), 1, 1);
        m_tetragonVertices[2] = video::S3DVertex(0,20,0, 0,1,1, video::SColor(255,255,255,0), 1, 0);
        m_tetragonVertices[3] = video::S3DVertex(-10,0,-10, 0,0,1, video::SColor(255,0,255,0), 0, 0);
        m_bbox.reset(m_tetragonVertices[0].Pos);
        for (s32 i = 1; i < 4; ++i)
        {
            m_bbox.addInternalPoint( m_tetragonVertices[i].Pos );
        }
    }

    virtual void
    OnRegisterSceneNode()
    {
        if (IsVisible) SceneManager->registerNodeForRendering(this);
        ISceneNode::OnRegisterSceneNode();
    }

    virtual void
    render()
    {
        video::IVideoDriver* driver = SceneManager->getVideoDriver();

        driver->setMaterial(m_tetragonMaterial);
        driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
        driver->drawVertexPrimitiveList(
            &m_tetragonVertices[0],
            4,
            &m_indices[0], 4, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
    }

    virtual core::aabbox3d<f32> const &
    getBoundingBox() const { return m_bbox; }

    virtual u32
    getMaterialCount() const { return 1; }

    virtual video::SMaterial&
    getMaterial(u32 i) { return m_tetragonMaterial; }
};

*/
