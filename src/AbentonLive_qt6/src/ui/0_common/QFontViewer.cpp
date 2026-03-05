#include "QFontViewer.h"
#include <QResizeEvent>
QFontViewer::QFontViewer( QWidget* parent )
   : QWidget(parent)
   , m_isDisplayDirty( true )
   , m_imgScaling( true )
   , m_imgPreserveAspectWoH( false )
{
   setObjectName( "QFontViewer" );
   setContentsMargins( 0,0,0,0 );
}

QFontViewer::~QFontViewer()
{
}

