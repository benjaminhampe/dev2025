#pragma once
#include <QDialog>
#include <QImage>
#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QThread>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

// ============================================================================
class QStringInputDialog : public QDialog
// ============================================================================
{
   Q_OBJECT
public:
   QStringInputDialog( QWidget* parent = 0 )
      : QDialog(parent)
   {
      setObjectName( "QStringInputDialog" );
      setContentsMargins( 0,0,0,0 );
      setModal( true );

      m_lblInfo = new QLabel("StringInputDialog", this );
      m_lblInfo->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

      m_edit = new QLineEdit("", this );
      m_edit->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

      m_btnOk = new QPushButton( "OK", this );
      m_btnOk->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

      m_btnCancel = new QPushButton( "Cancel", this );
      m_btnCancel->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

      auto h = new QHBoxLayout();
      h->addWidget( m_btnOk );
      h->addWidget( m_btnCancel );

      auto v = new QVBoxLayout();
      v->addWidget( m_lblInfo );
      v->addWidget( m_edit );
      v->addLayout( h );

      setLayout( v);

      connect(m_btnOk, SIGNAL(clicked()), this, SLOT(accept()));
      connect(m_btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
   }

   ~QStringInputDialog() override
   {}

   QString dialogResult() const { return m_edit->text(); }

signals:
public slots:
   void setInfoText( QString msg ) { m_lblInfo->setText( msg ); }
   void setInfoFont( QFont font ) { m_lblInfo->setFont( font ); }
   void setEditText( QString msg ) { m_edit->setText( msg ); }
protected:
private:
   QLabel* m_lblInfo;
   QLineEdit* m_edit;
   QPushButton* m_btnOk;
   QPushButton* m_btnCancel;
};
