#pragma once
#include "SMeshTool.hpp"

namespace de {
namespace gpu {

struct GUI;
struct VideoDriver;

struct GUIElement
{
   GUIElement( GUI* gui, GUIElement* parent = nullptr, int id = -1 )
      : m_gui(gui)
      , m_parent(parent)
      , m_id(id)
   {
      if ( m_parent )
      {
         m_parent->addChild( this );
      }

      updateAbsolutePosition();
   }

   virtual ~GUIElement()
   {
      removeAll();
      // delete all animators
      // m_TriangleSelector->drop();
   }

   virtual void render() = 0;

   virtual glm::ivec2
   getPosition() const { return m_position; }

   virtual void
   setPosition( glm::ivec2 pos ) { m_position = pos; }

   virtual void
   setPosition( int x, int y ) { m_position = glm::ivec2(x,y); }

   virtual glm::ivec2
   getAbsolutePosition() const { return m_absolutePosition; }

   virtual void
   updateAbsolutePosition()
   {
      m_absolutePosition = m_parent ? m_parent->getAbsolutePosition() + getPosition() : getPosition();
   }

   virtual GUIElement*
   getParent() const { return m_parent; }
   virtual void
   setParent( GUIElement* parent )
   {
      m_parent = parent;
      //      for ( auto & child : m_children )
      //      {
      //         if ( child ) { child->setParent( parent ) }
      //      }
   }
   virtual void
   removeAll()
   {
      for (GUIElement* child : m_children)
      {
         if ( child )
         {
            child->setParent( nullptr );
            //delete node;
            //node->drop();
         }
      }
      m_children.clear();
   }

   // Removes a child from this scene node.
   virtual bool
   removeChild( GUIElement* child )
   {
      if ( !child ) return false;
      for ( size_t i = 0; i < m_children.size(); ++i )
      {
         if ( m_children[ i ] == child )
         {
            m_children[ i ]->m_parent = nullptr;
            m_children.erase( m_children.begin() + i );
            return true;
         }
      }
      return false;
   }


   // Adds a child to this scene node.
   void addChild( GUIElement* child )
   {
      if ( !child || ( child == this ) ) return;
      child->setParent( nullptr ); // remove from old parent
      m_children.push_back( child );
      child->setParent( this );
   }


   virtual std::vector< GUIElement* > const & getChildren() const { return m_children; }
   virtual std::vector< GUIElement* >& getChildren() { return m_children; }

   GUI* m_gui;
   GUIElement* m_parent;
   int m_id;
   std::vector< GUIElement* > m_children;
   glm::ivec2 m_size;
   glm::ivec2 m_position;
   glm::ivec2 m_absolutePosition;
};

struct GUIButton : public GUIElement
{
   GUIButton( GUI* gui, GUIElement* parent = nullptr, int id = -1 );
   void render() override;
};

struct GUIText : public GUIElement
{
   GUIText( GUI* gui, GUIElement* parent = nullptr, int id = -1 );
   void render() override;
};


struct GUIEnvironment
{
   DE_CREATE_LOGGER("de.gpu.GUIEnvironment")
   VideoDriver* m_driver;
   std::vector< GUIElement* > m_elements;

   GUIEnvironment( VideoDriver* driver ) : m_driver( driver )
   {

   }

   ~GUIEnvironment()
   {
      clearElements();
   }

   void
   render()
   {
      if ( !m_driver ) { DE_ERROR("No driver") return; }
      for ( size_t i = 0; i < m_elements.size(); ++i )
      {
         auto item = m_elements[ i ];
         if ( !item ) continue;
         item->render();
      }
   }

   void clearElements()
   {
      for( size_t i = 0; i < m_elements.size(); ++i )
      {
         if (m_elements[ i ])
         {
            delete m_elements[ i ];
         }
      }
      m_elements.clear();
   }

   VideoDriver* getVideoDriver() { return m_driver; }

   GUIElement* addPanel( std::string label, GUIElement* parent = nullptr, int id = -1 );
   GUIButton* addButton( std::string label, GUIElement* parent = nullptr, int id = -1 );
   GUIText* addText( std::string label, GUIElement* parent = nullptr, int id = -1 );
};



} // end namespace gpu.
} // end namespace de.
