#pragma once
#include <de/os/Events.h>
#include <de/gpu/GPU.h>
#include <de/gpu/smesh/SMesh.h>

namespace de {
namespace gpu {

struct VideoDriver;
struct SceneManager;

// =====================================================================
struct ISceneNode
// =====================================================================
{
	typedef float T;
	
	typedef glm::vec3 V3;
    typedef glm::mat4 M4;

    // =====================================================================
    ISceneNode( SceneManager* env, ISceneNode* parent, u32 id, float x, float y, float z );
    virtual ~ISceneNode();
    virtual u32 getType() const { return 0; }
    virtual void draw();

    virtual M4 getAbsoluteTransform();

    virtual const TRSf& getTRS() const;
    virtual void setTRS( const TRSf& trs );
    virtual void setTranslate( const V3& pos )
    {
        m_trs.pos = pos;
        m_trs.update();
    }
    virtual void setRotate( const V3& rotate )
    {
        m_trs.rotate = rotate;
        m_trs.update();
    }
    virtual void setScale( const V3& scale )
    {
        m_trs.scale = scale;
        m_trs.update();
    }
    virtual TRSf getAbsoluteTRS();

    virtual Box3f getAbsoluteBoundingBox();
    virtual V3 getAbsolutePos() const;
    //virtual V3 getAbsolutePos() const = 0;
    //virtual void setPos( const V3& pos ) = 0;
    //virtual const V3& getPos() const = 0;
    //virtual V3 getAbsolutePos() const = 0;
    //virtual void setBoundingBox( const Box3f& bbox ) = 0;
    virtual void setParent( ISceneNode* parent );
    virtual ISceneNode* getParent();
    virtual void setName( const std::string & name );
    virtual const std::string & getName() const;
    virtual void setId(u32 id);
    virtual u32 getId() const;
    virtual void setVisible( bool visible );
    virtual bool isVisible() const;
    //virtual void setEnabled( bool enabled ) = 0;
    //virtual bool isHovered() const = 0;
    //virtual bool isPickable() const = 0;
    //virtual void setPickable( bool pickable ) = 0;

    virtual void clearChildren();
    virtual void addChild( ISceneNode* child );
    virtual ISceneNode* pickChild( int mx, int my );
    virtual ISceneNode* pickChild( Ray3f ray );

public:
    SceneManager* m_env;
    ISceneNode* m_parent;
    u32 m_id;
    bool m_visible;
    bool m_enabled;
    bool m_hovered;
    bool m_pickable;

    TRSf m_trs;
    Box3f m_bbox;

    std::string m_name;
    std::vector< ISceneNode* > m_children;

};

// =====================================================================
struct SMeshSceneNode : public ISceneNode
// =====================================================================
{
    // Def + Hidden impl.
    SMeshSceneNode( SceneManager* env, ISceneNode* parent, u32 id, f32 x, f32 y, f32 z );
    ~SMeshSceneNode() override;
    void draw() override;

    void setMesh( SMesh mesh ) { m_mesh = mesh; }
    const SMesh& getMesh() const { return m_mesh; }
    SMesh& getMesh() { return m_mesh; }

public:
    SMesh m_mesh;
};

// =====================================================================
struct SceneManager // 3D SceneManager
// =====================================================================
{
    SceneManager();
    ~SceneManager();
    void init( VideoDriver* driver);
    void draw();

    void clearSceneNodes();
    void addTopLevelNode( ISceneNode* node );
    ISceneNode* pick( int mx, int my );
    ISceneNode* pick( Ray3f ray );

    VideoDriver* getVideoDriver() { return m_driver; }

    bool isVisible() const { return m_visible; }
    void setVisible( bool visible ) { m_visible = visible; }

    //Camera* getCamera() { return m_camera; }
    //void setCamera( Camera* camera )
    //{
        //m_camera = camera;
        // if (m_camera)
        // {
        //     m_camera->setScreenSize()
        // }
    //}
    //void resetCamera() { m_camera = &m_camera0; }

public:
    VideoDriver* m_driver = nullptr;
    //Camera* m_camera;
    bool m_visible;
    //Camera m_camera0;
    ISceneNode* m_hoveredElement = nullptr; // Needed for events like Enter/Leave/etc...
    ISceneNode* m_lastHoveredElement = nullptr; // Needed for events like Enter/Leave/etc...
    std::vector< ISceneNode* > m_topLevelNodes;
    //glm::vec3 m_hitPos;
    //float m_hitDist;
};

} // end namespace gpu.
} // end namespace de.
