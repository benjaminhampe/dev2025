#include "ClipEditor.h"
#include <App.h>
#include <de_fontawesome.h>
#include <QChar>

ClipEditor::ClipEditor(QWidget* parent )
    : QWidget( parent )
    , m_clip{ nullptr }
{
    setObjectName( "ClipEditor" );
    setMouseTracking( true );

    m_pianoRoll = new PianoRoll(this);
    m_btnRecord = new QPushButton(QChar(static_cast<ushort>(fa::circle)),this);
    m_btnRecord->setMinimumSize(32,32);
    m_btnRecord->setMaximumSize(64,64);
    m_btnRecord->setCheckable(true);
    m_btnRecord->setChecked(false);

    App::instance()->getMidiCentral().registerListener(this);

    applySkin();

    connect(m_btnRecord, &QPushButton::toggled, this, [&](bool checked)
        {
            QChar c = QChar(static_cast<ushort>(checked ? fa::square : fa::circle));
            m_btnRecord->setText(c);

            if (checked)
            {
                m_midiClock.restart();
            }
        });
}

void ClipEditor::setClip(de::session::Clip* clip)
{
    if (m_clip != clip)
    {
        m_clip = clip;
        m_pianoRoll->setClip(clip);
        update();
    }
}

void ClipEditor::applySkin()
{
    const auto& skin = App::instance()->getSkin();
    m_zoom = skin.zoom;
    m_margin = (8 * m_zoom)/100;
    m_btnSize = (48 * m_zoom)/100;
    m_windowColor = skin.windowColor;
    m_panelColor = skin.panelColor;
    updateLayout();
}

void ClipEditor::updateLayout()
{
    const int w = width();
    const int h = height();
    if (w < 1) return;
    if (h < 1) return;

    int m = 2*m_margin; // inner margin
    int dx = std::max(w - 2*m,0);
    int dy = std::max(h - 2*m,0);
    m_btnRecord->setGeometry(m,m,m_btnSize,m_btnSize);
    m_pianoRoll->setGeometry(m + m_btnSize,m,dx,dy);
    update();
}

void ClipEditor::resizeEvent(QResizeEvent* e)
{
    const int w = e->size().width();
    const int h = e->size().height();
    if (w < 1) return;
    if (h < 1) return;
    updateLayout();
}

void ClipEditor::paintEvent( QPaintEvent* event )
{
    const int w = width();
    const int h = height();
    if (w < 1) return;
    if (h < 1) return;
    QPainter dc( this );
    dc.fillRect(rect(), m_windowColor );

    int m = m_margin;
    int dx = std::max(0, w - 2*m);
    int dy = std::max(0, h - 2*m);
    if (dx > 0 && dy > 0)
    {
        dc.setRenderHint(QPainter::Antialiasing);
        dc.setPen(Qt::NoPen);
        dc.setBrush(QBrush(m_panelColor));
        dc.drawRoundedRect(QRect(m,m,dx,dy),m,m);
    }
}

void ClipEditor::onMidiMessage(double pts, const de::midi::MidiMessage& msg)
{

}

void ClipEditor::onShortMidiMessage(double pts, const de::midi::ShortMidiMessage& msg)
{
    if (!m_btnRecord->isChecked())
    {
        return;
    }

    if (!m_clip)
    {
        DE_WARN("No clip to record to")
        return;
    }

    const int64_t ns = std::llround(1.0e9 * m_midiClock.now()); // Clock is restarted every callback call.

    // HOPEFULLY that fixes missing NoteOff events:
    // Pianos work ok without that, but monophonic synth are
    // beasts on a higher level...
    // const int deltaFrames = std::clamp( int(dt * m_sampleRate),
    //                                     int(0),
    //                                     int(m_blockSize) - 10);

    const uint8_t command = msg.m_status & 0xF0;
    const uint8_t channel = msg.m_status & 0x0F;

    if (command == 0x90 && msg.m_data2 != 0) // ✔ Note On
    {
        int midiNote = static_cast<int16_t>(msg.m_data1 & 0x7F);
        int velocity = static_cast<float>(msg.m_data2 & 0x7F);
        m_clip->noteOn(ns,channel,midiNote,velocity);
    }
    else if (command == 0x80 ||
            (command == 0x90 && msg.m_data2 == 0)) // ✔ Note Off
    {
        int midiNote = static_cast<int16_t>(msg.m_data1 & 0x7F);
        int velocity = static_cast<float>(msg.m_data2 & 0x7F);
        m_clip->noteOff(ns,channel,midiNote,velocity);
    }
    else if (command == 0xA0) // ✔ Polyphonic Aftertouch: kPolyPressureEvent
    {
        // e.type = Steinberg::Vst::Event::kPolyPressureEvent;
        // e.polyPressure.channel  = channel;
        // e.polyPressure.pitch    = static_cast<int16_t>(msg.m_data1 & 0x7F);
        // e.polyPressure.pressure = static_cast<float>(msg.m_data2 & 0x7F) / 127.0f;   // normalize
        // e.polyPressure.noteId   = -1; // unless you track note IDs
        // return e;
    }
    else if (command == 0xB0) // ✔ CC Control Change
    {
        // e.type = Steinberg::Vst::Event::kLegacyMIDICCOutEvent;
        // e.midiCCOut.channel     = channel;
        // e.midiCCOut.controlNumber = msg.m_data1; // 64 = sustain on/off
        // e.midiCCOut.value       = static_cast<int8_t>(msg.m_data2 & 0x7F); // [0-127]
        // e.midiCCOut.value2      = static_cast<int8_t>(msg.m_data3 & 0x7F); // [0-127]
        // return e;
    }
#if 0
    else if (command == 0xC0) // ❌ 0xC0–0xCF — Program Change :: Legacy
    {
        return std::nullopt;
    }
    else if (command == 0xD0) // ❌ 0xD0–0xDF — Channel Pressure
    {
        return std::nullopt;
    }
    else if (command == 0xE0) // ❌ 0xE0–0xEF — Pitch Bend
    {
        return std::nullopt;
    }
    else if (command == 0xF0) // ❌ 0xF0–0xFF — System Messages
    {
        // SysEx should NOT be done with ShortMidiMessage here!
        return std::nullopt;

        // Status Meaning           How?        Sent continuously?
        // F0     SysEx Start       kDataEvent  No
        // F1     MTC QuarterFrame	No          No
        // F2     Song Pos Pointer  No          No
        // F3     Song Select       No          No
        // F4     Undefined         No          No
        // F5     Undefined         No          No
        // F6     Tune Request      No          No
        // F7     SysEx End         kDataEvent  No
        // F8     Timing Clock      No          Yes (24 ticks per quarter note)
        // F9     Undefined         No          No
        // FA     Start             No          Once
        // FB     Continue          No          Once
        // FC     Stop              No          Once
        // FD     Undefined         No          No
        // FE     Active Sensing    No          Some devices send it every ~300ms
        // FF     System Reset      No          Rare
    }
    else
    {
        DE_ERROR("Unsupported midi message ", msg.str())
        return std::nullopt;
    }
#endif

}

#if 0

void
ClipEditor::hideEvent( QHideEvent* event )
{
   QWidget::hideEvent( event );
}

void
ClipEditor::showEvent( QShowEvent* event )
{
   //DE_DEBUG("")
   QWidget::showEvent( event );
   //event->accept();
}


void
ClipEditor::mouseMoveEvent( QMouseEvent* event )
{
   m_mouseX = event->x();
   m_mouseY = event->y();

   m_isOverPianoBar = isMouseOverPianoBar();
   m_isOverBeatGrid = isMouseOverBeatGrid();
   m_detectedKeyIndex = getKeyIndexFromMousePos();
   m_hoveredNote = computeHoveredNote();

   if ( m_dragMode == 1 ) // drawing
   {
//      int sy = m_mouseY - m_dragStartY;
//      DE_DEBUG("StretchY(",sy,")")
   }
   if ( m_dragMode == 2 ) // stretch y
   {
      int sy = m_mouseY - m_dragStartY;
      DE_DEBUG("StretchY(",sy,")")
   }

   update();
   QWidget::mouseMoveEvent( event );
}

void
ClipEditor::wheelEvent( QWheelEvent* event )
{
   int wheel = event->angleDelta().y();
   DE_DEBUG("MouseWheel = ",wheel )

   if ( wheel >= 1 )
   {
      m_keyStart = std::clamp( m_keyStart - 3, 0, 125 );
      update();
   }
   else if ( wheel <= 1 )
   {
      m_keyStart = std::clamp( m_keyStart + 3, 0, 125 );
      update();
   }

   QWidget::wheelEvent( event );

//   if ( m_driver )
//   {
//      de::SEvent post;
//      post.type = de::EET_MOUSE_EVENT;
//      post.mouseEvent.m_Flags = de::SMouseEvent::Wheel;
//      post.mouseEvent.m_X = event->x();
//      post.mouseEvent.m_Y = event->y();
//      post.mouseEvent.m_Wheel = event->angleDelta().y(); //    //QPoint delta = event->pixelDelta();
//      // DE_DEBUG("post.mouseEvent.m_Wheel = ",post.mouseEvent.m_Wheel)
//      m_driver->postEvent( post );
//   }

   //event->accept();
}


void
ClipEditor::mousePressEvent( QMouseEvent* event )
{
//   m_X = event->x();
//   m_Y = event->y();
//   m_Wheel = 0.0f;
   m_mouseX = event->x();
   m_mouseY = event->y();


   int x = event->x();
   int y = event->y();

   if ( m_dragMode > 0 )
   {
      DE_ERROR("Already dragging ", m_dragMode)
   }
   else
   {
      m_dragStartX = x;
      m_dragStartY = y;
      //m_selectedNote = m_hoveredNote;

      if ( m_isOverBeatGrid )
      {
         m_dragMode = 1;
         DE_DEBUG("DrawMode ", m_dragMode)
      }

      if ( m_isOverPianoBar )
      {
         m_dragMode = 2;
         DE_DEBUG("StretchMode ", m_dragMode)
      }
      update();
   }

   QWidget::mousePressEvent( event );
}

void
ClipEditor::mouseReleaseEvent( QMouseEvent* event )
{
   m_mouseX = event->x();
   m_mouseY = event->y();

//   m_isOverPianoBar = isMouseOverPianoBar();
//   m_isOverBeatGrid = isMouseOverBeatGrid();

   // Finish Drawing
   if ( m_dragMode == 1 && m_detectedKeyIndex > -1 )
   {
      auto a = pixel2time( m_dragStartX );
      auto b = pixel2time( m_mouseX );
      if ( a > b )
      {
         std::swap( a, b );
      }
      if ( a < b )
      {
         Note note;
         note.timeBeg = a;
         note.timeEnd = b;
         note.color = de::randomColorRGB();
         Key & key = m_keys[ m_detectedKeyIndex ];
         key.m_notes.emplace_back( std::move( note ) );
         DE_DEBUG("Added Note ")
      }
   }

   // Finish StretchY
   if ( m_dragMode == 2 && m_isOverPianoBar )
   {
      int sy = m_mouseY - m_dragStartY;
      DE_DEBUG("StretchY Final(",sy,")")
      m_keyHeight = std::clamp( m_keyHeight + sy/4, 2, 48 );
   }

   // Reset all
   m_dragMode = 0;

   // Enqueue ( async ) request for redraw.
   update();

   QWidget::mouseReleaseEvent( event );
}


void
ClipEditor::keyPressEvent( QKeyEvent* event )
{
   //DE_DEBUG("KeyPress(",event->key(),")")

//   if ( m_driver )
//   {
//      de::SEvent post;
//      post.type = de::EET_KEY_EVENT;
//      post.keyEvent.Key = de::KEY_UNKNOWN;
//      post.keyEvent.Flags = de::SKeyEvent::Pressed;
//      if ( event->modifiers() & Qt::ShiftModifier ) { post.keyEvent.Flags |= de::SKeyEvent::Shift; }
//      if ( event->modifiers() & Qt::ControlModifier ) { post.keyEvent.Flags |= de::SKeyEvent::Ctrl; }
//      if ( event->modifiers() & Qt::AltModifier ) { post.keyEvent.Flags |= de::SKeyEvent::Alt; }
//      parseQtKey( event, post );
//      m_driver->postEvent( post );
//   }

   QWidget::keyPressEvent( event );
}

void
ClipEditor::keyReleaseEvent( QKeyEvent* event )
{
   //DE_DEBUG("KeyRelease(",event->key(),")")

//   if ( m_driver )
//   {
//      de::SEvent post;
//      post.type = de::EET_KEY_EVENT;
//      post.keyEvent.Key = de::KEY_UNKNOWN;
//      post.keyEvent.Flags = de::SKeyEvent::None;
//      if ( event->modifiers() & Qt::ShiftModifier ) { post.keyEvent.Flags |= de::SKeyEvent::Shift; }
//      if ( event->modifiers() & Qt::ControlModifier ) { post.keyEvent.Flags |= de::SKeyEvent::Ctrl; }
//      if ( event->modifiers() & Qt::AltModifier ) { post.keyEvent.Flags |= de::SKeyEvent::Alt; }
//      parseQtKey( event, post );
//      m_driver->postEvent( post );
//   }

//   event->accept();
   QWidget::keyReleaseEvent( event );
}

#endif
