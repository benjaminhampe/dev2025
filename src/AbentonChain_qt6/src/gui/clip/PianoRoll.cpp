#include "PianoRoll.h"
#include <App.h>

PianoRoll::PianoRoll(QWidget* parent )
   : QWidget( parent )
   , m_playTimerId( 0 )
   , m_drawTimerId( 0 )
   , m_clip{nullptr}
{
    setObjectName( "PianoRoll" );
    setMouseTracking( true );
    setContentsMargins(0,0,0,0);
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    reset();
}

PianoRoll::~PianoRoll()
{
    // stopPlayTimer();
}

void PianoRoll::setClip(de::session::Clip* clip )
{
    if (m_clip == clip)
    {
        return; // Nothing todo
    }

    setUpdatesEnabled( false );
    reset();
    m_clip = clip;
    updateLayout();
    setUpdatesEnabled( true );
}

void
PianoRoll::reset()
{
    m_skin.zoomX = 1.0f;
    m_skin.zoomY = 1.0f;
    m_scrollX = 0;
    m_scrollY = 0;

    m_pixelsPerSecond = 250.0;
    m_secondsPerPixel = 1.0 / m_pixelsPerSecond;

    m_isPlaying = false;
    m_isBeatSync = false;
    m_bpm = 120.0f;
    m_ppq = 960;

    // X-Axis is time 't' ( complicated ) in [s], [bars], [ticks] and [beats]
    m_beatBeg = 0;
    m_beatEnd = 16;  // show 4 bars eats
    m_beatNow = 0;

    m_loopNow = 0;
    m_loopCount = 0;
    m_loopTimeBeg = 0;
    m_loopTimeEnd = int64_t(1000 * 1000 * 1000) * 2; // 2 sec

    // m_isOverPianoBar = false;
    // m_isOverBeatGrid = false;
    m_mx = 0;
    m_my = 0;
    m_detectedMidiNote = -1;
    m_hoveredNote.reset();
    m_selectedNote.reset();

    m_dragMode = 0;
    m_dragStartX = 0;
    m_dragStartY = 0;

}



void PianoRoll::applySkin()
{
    const auto& skin = App::instance()->getSkin();
    m_skin.zoom = skin.zoom;
    m_skin.panelColor = skin.panelColor;
    m_skin.pianobarColor = skin.panelColor.lighter(10);
    m_skin.timelineColor = skin.panelColor.lighter(25);
    m_skin.bodyColor = skin.panelColor.lighter(50);
    m_skin.gridColorX = skin.panelColor.darker(25);
    m_skin.gridColorY = skin.panelColor.darker(50);
    m_skin.yWhite = QColor(255,255,255);
    m_skin.yBlack = QColor(0,0,0);

/*
    m_windowColor = skin.windowColor;
    m_textColor = skin.textColor;
    m_activeColor = skin.symbolColorActive;
    m_radius = (m_baseRadius * skin.zoom) / 100;
    m_padding = (m_basePadding * skin.zoom) / 100;
    m_buttonHeight = (m_baseButtonHeight * skin.zoom) / 100;

    // QColor m_panelColor(128,128,128);
    // QColor m_contentColor(255,255,255);
    // QColor m_symbolColor(255,128,0);
    // QColor m_focusColor(32,32,32);
*/
    updateLayout();
}

void PianoRoll::updateLayout()
{
    m_skin.keyW = (32 * m_skin.zoom * m_skin.zoomX)/100;
    m_skin.keyH = (10 * m_skin.zoom * m_skin.zoomY)/100;

    m_skin.pianobarW = (64 * m_skin.zoom)/100;
    m_skin.timelineH = (16 * m_skin.zoom)/100;

    m_skin.fontKey = de::Font5x8(m_skin.keyH/10,m_skin.keyH/10,0,0,1,1);

    int w = width();
    int h = height();

    int pW = m_skin.pianobarW;
    int tH = m_skin.timelineH;

    m_skin.r_corner = QRect(0,0,pW,tH);
    m_skin.r_pianobar = QRect(0,tH,pW,h-tH);
    m_skin.r_timeline = QRect(pW,0,w-pW,tH);
    m_skin.r_body = QRect(pW,tH,w-pW,h-tH);

    update();
}

void PianoRoll::resizeEvent(QResizeEvent* e)
{
    const int w = e->size().width();
    const int h = e->size().height();
    if (w < 1) return;
    if (h < 1) return;
    updateLayout();
}

void PianoRoll::paintEvent( QPaintEvent* event )
{
    const int w = width();
    const int h = height();
    if (w < 1) return;
    if (h < 1) return;

    QPainter dc( this );
    dc.fillRect( m_skin.r_corner, m_skin.panelColor );
    dc.fillRect( m_skin.r_pianobar, m_skin.panelColor );
    dc.fillRect( m_skin.r_timeline, m_skin.timelineColor );
    dc.fillRect( m_skin.r_body, m_skin.bodyColor );

    drawPianoBar( dc, m_skin.r_pianobar );
    drawTimeline( dc, m_skin.r_timeline );
    drawNotes( dc, m_skin.r_body );
#if 0
    // Draw PianoBar + filled background of notes
    //int x = 0;
    int y = 0;

    int keyStart = m_scrollY/m_skin.keyH;
    int keyCount = height()/m_skin.keyH;

    // Draw Time Grid ( 250ms )
    //y = m_topHeight;
    dc.setPen( QPen( QColor( 155,155,155) ) );
    dc.setBrush( Qt::NoBrush );
    for ( int i = 0; i < 4*10; ++i )
    {
        int x = sec2pix( 0.25 * i );
        int y1 = 0;
        int y2 = h - 1;
        dc.drawLine( x, y1, x, y2 );
    }

    // Draw Time Grid ( 1s )
    //y = m_topHeight;
    dc.setPen( QPen( QColor( 200,200,200) ) );
    dc.setBrush( Qt::NoBrush );
    for ( int i = 0; i < 10; ++i )
    {
        int x = sec2pix( i );
        int y1 = 0;
        int y2 = h - 1;
        dc.drawLine( x, y1, x, y2 );
    }

    for ( int i = 0; i < keyCount; ++i )
    {
        int midiNote = keyStart + i;
        drawNotes(midiNote, y);
        y += m_skin.keyH;
    }

    // Draw all visible notes
    y = 0;
    for ( int i = 0; i < m_keyCount; ++i )
    {
        int keyIndex = m_keyStart + i;
        if ( keyIndex < 0 || keyIndex >= int( m_keys.size() ) )
        {
            DE_ERROR("keyIndex = ",keyIndex)
        }
        else
        {
        dc.setPen( Qt::NoPen );
        Key & key = m_keys[ keyIndex ];
        for ( int j = 0; j < int( key.m_notes.size() ); ++j )
        {
        Note const & note = key.m_notes[ j ];
        int x1 = time2pixel( note.timeBeg );
        int x2 = time2pixel( note.timeEnd );
        dc.setBrush( QBrush( toQColor( note.color ) ) );
        dc.drawRect( x1, y, x2-x1+1, m_keyHeight );
        }
    }

    // End row
    y += m_keyHeight;
    }


   // [VisualDebug] Draw detected key index as full row rect.
   if ( m_isOverPianoBar )
   {
      //DE_DEBUG( "isOverPianoBar")
      dc.setPen( QPen( QColor(255,100,100) ) );
      dc.setBrush( Qt::NoBrush );
      QRect m_rectPianoBar( 0, m_topHeight, m_keyWidth, h-1-m_topHeight );
      dc.drawRect( m_rectPianoBar );
   }
   if ( m_isOverBeatGrid )
   {
      //DE_DEBUG( "isOverBeatGrid")
      dc.setPen( QPen( QColor(200,200,255) ) );
      dc.setBrush( Qt::NoBrush );
      QRect m_rectBeatGrid( m_keyWidth, m_topHeight, w-1-m_keyWidth, h-1-m_topHeight );
      dc.drawRect( m_rectBeatGrid );
   }

   if ( m_detectedKeyIndex > -1 )
   {
      //auto nsg = de::midi::MidiTools::noteStr( m_keys[ m_detectedKeyIndex ].midiNote );
      //DE_DEBUG( "m_detectedKeyIndex = ", m_detectedKeyIndex, ", ", nsg )
      dc.setPen( QPen( QColor(255,180,55) ) );
      dc.setBrush( Qt::NoBrush );
      QRect m_rectKeyIndex( 0, m_topHeight + (( m_detectedKeyIndex - m_keyStart ) * m_keyHeight), w-1, m_keyHeight-1 );
      dc.drawRect( m_rectKeyIndex );
   }

   // Draw Loop Start Time
   int xs = time2pixel( m_loopTimeStart );
   int xe = time2pixel( m_loopTimeEnd );
   int xt = time2pixel( m_loopTime );

   dc.setBrush( Qt::NoBrush );

   dc.setPen( QPen( QColor( 0,200,0 ) ) );
   dc.drawLine( xs, m_topHeight, xs, h );

   dc.setPen( QPen( QColor( 220,0,0 ) ) );
   dc.drawLine( xe, m_topHeight, xe, h );

   dc.setPen( QPen( QColor( 220,180,20 ) ) );
   dc.drawLine( xt, m_topHeight, xt, h );

   if ( m_dragMode == 1 ) // drawing
   {
      dc.setPen( Qt::NoPen );
      int x1 = m_dragStartX;
      int y1 = m_topHeight + (m_detectedKeyIndex - m_keyStart) * m_keyHeight;
      int x2 = m_mouseX;
      int y2 = y1 + m_keyHeight;
      dc.setBrush( QBrush( QColor(255,100,100) ) );
      dc.drawRect( x1, y1, x2-x1+1, y2-y1 );
   }

#endif
}

void PianoRoll::drawPianoBar(QPainter & dc, QRect pos)
{
    int x = m_skin.r_pianobar.x();
    int y = m_skin.r_pianobar.y();
    int w = m_skin.r_pianobar.width();
    int h = m_skin.r_pianobar.height();

    int keyStart = m_scrollY/m_skin.keyH;
    int keyCount = h/m_skin.keyH;

    for ( int i = 0; i < keyCount; ++i )
    {
        int midiNote = keyStart + i;
        if ( midiNote >= 0 && midiNote < 128 )
        {
            continue;
        }

        QRect r( x, y, w, m_skin.keyH );

        // QColor fillColor = QColor(255,255,255);// white
        // QColor penColor = QColor(205,205,205); // light white
        // QColor textColor = QColor(2,2,2);      // xor
        // if ( key.isBlack )
        // {
        //     fillColor = QColor(2,2,2); // black
        //     penColor = QColor(65,65,65); // light black
        //     textColor = QColor(255,255,255); // xor
        // }

        // if (key.semi == 0) // C key more blue
        // {
        //     fillColor = QColor(230,230,240);
        // }

        int oktave = 0;
        int semitone = 0;
        de::midi::MidiTools::decompose(midiNote,oktave,semitone);
        bool isBlack = de::midi::MidiTools::isBlackPianoKey(semitone);
        QColor fillColor = isBlack ? QColor(2,2,2) : QColor(255,255,255);
        QColor penColor = isBlack ? QColor(65,65,65) : QColor(205,205,205);
        QColor textColor = isBlack ? QColor(255,255,255) : QColor(2,2,2);

        drawKey( dc, r, fillColor, penColor );

        // Draw key text
        auto s = de::midi::MidiTools::noteStr( midiNote );
        auto t = QString::fromStdString( s );
        auto tx = x + m_skin.keyW - 6;
        auto ty = y + m_skin.keyH/2;
        auto textAlign = de::Align::MiddleRight;
        drawText( m_skin.fontKey, dc, tx, ty, t, textColor, textAlign );

        // /// Draw note backgrounds
        // fillColor = m_editColorWhite;
        // //strokeColor = QColor(220,220,220);
        // if ( key.isBlack )
        // {
        // fillColor = m_editColorBlack;
        // //strokeColor = QColor(150,150,150);
        // }
        // //dc.setPen( QPen( strokeColor ) );
        // dc.fillRect( QRect(m_keyWidth, y, w-1-m_keyWidth, m_keyHeight), fillColor );

        // if ( key.semi == 0 ) // every C draw blue line below
        // {
        // dc.setPen( QPen( QColor(100,100,255) ) );
        // dc.setBrush( Qt::NoBrush );
        // int y = r.y() + r.height() - 1;
        // dc.drawLine( m_keyWidth, y, w-1-m_keyWidth, y );
        // }

        // End row
        y += m_skin.keyH;
    }

}

void PianoRoll::drawTimeline(QPainter & dc, QRect pos)
{
    int x = pos.x();
    int y = pos.y();
    int w = pos.width();
    int h = height();

    dc.setBrush( Qt::NoBrush );
    dc.setPen( m_skin.gridColorX );

    for ( int i = 0; i < 10; ++i )
    {
        int x = sec2pix( i );
        int y1 = 0;
        int y2 = h - 1;
        dc.drawLine( x, y1, x, y2 );
    }
}

void PianoRoll::drawNotes(QPainter & dc, QRect pos)
{
    int x = pos.x();
    int y = pos.y();
    int w = pos.width();
    int h = pos.height();

    int keyStart = m_scrollY/m_skin.keyH;
    int keyCount = height()/m_skin.keyH;

    dc.setPen( Qt::NoPen );
    for ( int i = 0; i < keyCount; ++i )
    {
        int midiNote = keyStart + i;
        if ( midiNote >= 0 && midiNote < 128 )
        {
            const auto & notes = m_clip->m_notes[ midiNote ];
            for ( int j = 0; j < int( notes.size() ); ++j )
            {
                const auto & note = notes[ j ];
                int x1 = sec2pix( note.ppqNoteOn );
                int x2 = sec2pix( note.ppqNoteOff );
                dc.setBrush( QBrush( toQColor( note.color ) ) );
                dc.drawRect( x1, y, x2-x1+1, m_skin.keyH );
            }
        }
        y += m_skin.keyH;
    }

/*
   // [VisualDebug] Draw detected key index as full row rect.
   if ( m_isOverPianoBar )
   {
      //DE_DEBUG( "isOverPianoBar")
      dc.setPen( QPen( QColor(255,100,100) ) );
      dc.setBrush( Qt::NoBrush );
      QRect m_rectPianoBar( 0, m_topHeight, m_keyWidth, h-1-m_topHeight );
      dc.drawRect( m_rectPianoBar );
   }
   if ( m_isOverBeatGrid )
   {
      //DE_DEBUG( "isOverBeatGrid")
      dc.setPen( QPen( QColor(200,200,255) ) );
      dc.setBrush( Qt::NoBrush );
      QRect m_rectBeatGrid( m_keyWidth, m_topHeight, w-1-m_keyWidth, h-1-m_topHeight );
      dc.drawRect( m_rectBeatGrid );
   }

   if ( m_detectedKeyIndex > -1 )
   {
      //auto nsg = de::midi::MidiTools::noteStr( m_keys[ m_detectedKeyIndex ].midiNote );
      //DE_DEBUG( "m_detectedKeyIndex = ", m_detectedKeyIndex, ", ", nsg )
      dc.setPen( QPen( QColor(255,180,55) ) );
      dc.setBrush( Qt::NoBrush );
      QRect m_rectKeyIndex( 0, m_topHeight + (( m_detectedKeyIndex - m_keyStart ) * m_keyHeight), w-1, m_keyHeight-1 );
      dc.drawRect( m_rectKeyIndex );
   }

   // Draw Loop Start Time
   int xs = time2pixel( m_loopTimeStart );
   int xe = time2pixel( m_loopTimeEnd );
   int xt = time2pixel( m_loopTime );

   dc.setBrush( Qt::NoBrush );

   dc.setPen( QPen( QColor( 0,200,0 ) ) );
   dc.drawLine( xs, m_topHeight, xs, h );

   dc.setPen( QPen( QColor( 220,0,0 ) ) );
   dc.drawLine( xe, m_topHeight, xe, h );

   dc.setPen( QPen( QColor( 220,180,20 ) ) );
   dc.drawLine( xt, m_topHeight, xt, h );

   if ( m_dragMode == 1 ) // drawing
   {
      dc.setPen( Qt::NoPen );
      int x1 = m_dragStartX;
      int y1 = m_topHeight + (m_detectedKeyIndex - m_keyStart) * m_keyHeight;
      int x2 = m_mouseX;
      int y2 = y1 + m_keyHeight;
      dc.setBrush( QBrush( QColor(255,100,100) ) );
      dc.drawRect( x1, y1, x2-x1+1, y2-y1 );
   }
*/
}

/*
void PianoRoll::drawNotes(int midiNote, int yStart)
{
    const int w = width();
    const int h = height();
    if (w < 1) return;
    if (h < 1) return;

    //m_pixelPerBeat = w / m_beatCount;

    QPainter dc( this );
    dc.fillRect( rect(), m_skin.panelColor );

    // Draw PianoBar + filled background of notes
    //int x = 0;
    int y = 0;

    int keyStart = m_scrollY/m_skin.keyH;
    int keyCount = height()/m_skin.keyH;

    for ( int i = 0; i < keyCount; ++i )
    {
        int keyIndex = keyStart + i;
        if ( keyIndex >= 0 && keyIndex < 128 )
        {
            std::vector<ClipNote> & notes = m_[ keyIndex ];

            // Get key-rect
            QRect r( 0, y, m_keyWidth, m_keyHeight );

            // Draw key, determine colors ( black or white key )
            QColor fillColor = QColor(255,255,255);// white
            QColor penColor = QColor(205,205,205); // light white
            QColor textColor = QColor(2,2,2);      // xor
            if ( key.isBlack )
            {
            fillColor = QColor(2,2,2); // black
            penColor = QColor(65,65,65); // light black
            textColor = QColor(255,255,255); // xor
            }

            if (key.semi == 0) // C key more blue
            {
            fillColor = QColor(230,230,240);
            }

            drawKey( dc, r, fillColor, penColor );

            // Draw key text
            if ( m_keyHeight >= 9 )
            {
            auto s = de::midi::MidiTools::noteStr( key.midiNote );
            auto t = QString::fromStdString( s );
            auto tx = r.x() + m_keyWidth - 6;
            auto ty = r.y() + m_keyHeight/2;
            auto textAlign = de::Align::MiddleRight;
            m_font5x8.drawText( dc, tx, ty, t, textColor, textAlign );
            }

            /// Draw note backgrounds
            fillColor = m_editColorWhite;
            //strokeColor = QColor(220,220,220);
            if ( key.isBlack )
            {
            fillColor = m_editColorBlack;
            //strokeColor = QColor(150,150,150);
            }
            //dc.setPen( QPen( strokeColor ) );
            dc.fillRect( QRect(m_keyWidth, y, w-1-m_keyWidth, m_keyHeight), fillColor );

            if ( key.semi == 0 ) // every C draw blue line below
            {
            dc.setPen( QPen( QColor(100,100,255) ) );
            dc.setBrush( Qt::NoBrush );
            int y = r.y() + r.height() - 1;
            dc.drawLine( m_keyWidth, y, w-1-m_keyWidth, y );
            }
        }
        // End row
        y += m_skin.keyH;
    }

   // Draw Time Grid ( 250ms )
   //y = m_topHeight;
   dc.setPen( QPen( QColor( 155,155,155) ) );
   dc.setBrush( Qt::NoBrush );
   for ( int i = 0; i < 4*10; ++i )
   {
      int x = time2pixel( int64_t( 250*1000*1000 ) * i );
      int y1 = m_topHeight;
      int y2 = h - 1;
      dc.drawLine( x, y1, x, y2 );
   }

   // Draw Time Grid ( 1s )
   //y = m_topHeight;
   dc.setPen( QPen( QColor( 200,200,200) ) );
   dc.setBrush( Qt::NoBrush );
   for ( int i = 0; i < 10; ++i )
   {
      int x = time2pixel( int64_t( 1000000000 ) * i );
      int y1 = m_topHeight;
      int y2 = h - 1;
      dc.drawLine( x, y1, x, y2 );
   }
   // Draw all visible notes
   y = m_topHeight;
   for ( int i = 0; i < m_keyCount; ++i )
   {
      int keyIndex = m_keyStart + i;
      if ( keyIndex < 0 || keyIndex >= int( m_keys.size() ) )
      {
         DE_ERROR("keyIndex = ",keyIndex)
      }
      else
      {
         dc.setPen( Qt::NoPen );
         Key & key = m_keys[ keyIndex ];
         for ( int j = 0; j < int( key.m_notes.size() ); ++j )
         {
            Note const & note = key.m_notes[ j ];
            int x1 = time2pixel( note.timeBeg );
            int x2 = time2pixel( note.timeEnd );
            dc.setBrush( QBrush( toQColor( note.color ) ) );
            dc.drawRect( x1, y, x2-x1+1, m_keyHeight );
         }
      }

      // End row
      y += m_keyHeight;
   }


   // [VisualDebug] Draw detected key index as full row rect.
   if ( m_isOverPianoBar )
   {
      //DE_DEBUG( "isOverPianoBar")
      dc.setPen( QPen( QColor(255,100,100) ) );
      dc.setBrush( Qt::NoBrush );
      QRect m_rectPianoBar( 0, m_topHeight, m_keyWidth, h-1-m_topHeight );
      dc.drawRect( m_rectPianoBar );
   }
   if ( m_isOverBeatGrid )
   {
      //DE_DEBUG( "isOverBeatGrid")
      dc.setPen( QPen( QColor(200,200,255) ) );
      dc.setBrush( Qt::NoBrush );
      QRect m_rectBeatGrid( m_keyWidth, m_topHeight, w-1-m_keyWidth, h-1-m_topHeight );
      dc.drawRect( m_rectBeatGrid );
   }

   if ( m_detectedKeyIndex > -1 )
   {
      //auto nsg = de::midi::MidiTools::noteStr( m_keys[ m_detectedKeyIndex ].midiNote );
      //DE_DEBUG( "m_detectedKeyIndex = ", m_detectedKeyIndex, ", ", nsg )
      dc.setPen( QPen( QColor(255,180,55) ) );
      dc.setBrush( Qt::NoBrush );
      QRect m_rectKeyIndex( 0, m_topHeight + (( m_detectedKeyIndex - m_keyStart ) * m_keyHeight), w-1, m_keyHeight-1 );
      dc.drawRect( m_rectKeyIndex );
   }

   // Draw Loop Start Time
   int xs = time2pixel( m_loopTimeStart );
   int xe = time2pixel( m_loopTimeEnd );
   int xt = time2pixel( m_loopTime );

   dc.setBrush( Qt::NoBrush );

   dc.setPen( QPen( QColor( 0,200,0 ) ) );
   dc.drawLine( xs, m_topHeight, xs, h );

   dc.setPen( QPen( QColor( 220,0,0 ) ) );
   dc.drawLine( xe, m_topHeight, xe, h );

   dc.setPen( QPen( QColor( 220,180,20 ) ) );
   dc.drawLine( xt, m_topHeight, xt, h );

   if ( m_dragMode == 1 ) // drawing
   {
      dc.setPen( Qt::NoPen );
      int x1 = m_dragStartX;
      int y1 = m_topHeight + (m_detectedKeyIndex - m_keyStart) * m_keyHeight;
      int x2 = m_mouseX;
      int y2 = y1 + m_keyHeight;
      dc.setBrush( QBrush( QColor(255,100,100) ) );
      dc.drawRect( x1, y1, x2-x1+1, y2-y1 );
   }
}


void
PianoRoll::play( int mode )
{
   m_isPlaying = true;
   m_timeStart = dbTimeInNanoseconds();
   //m_step = 0;
   startPlayTimer();
}

void
PianoRoll::stop()
{
   m_isPlaying = false;
   m_timeStart = dbTimeInNanoseconds();
   //m_step = 0;
   stopPlayTimer();
}

void
PianoRoll::setSynth( int i, de::audio::IDspChainElement* synth )
{
   if ( i < 0 || i >= int(m_synths.size()) ) return;
   m_synths[ i ] = synth;
}

void
PianoRoll::sendNote( de::audio::Note const & note )
{
   for ( auto synth : m_synths )
   {
      if ( !synth ) continue;
      synth->sendNote( note );
   }
}

void
PianoRoll::startPlayTimer()
{
   stopPlayTimer();
   m_playTimerId = startTimer( 1, Qt::CoarseTimer );
   m_drawTimerId = startTimer( 40, Qt::CoarseTimer );
}

void
PianoRoll::stopPlayTimer()
{
   if ( m_playTimerId )
   {
      killTimer( m_playTimerId );
      m_playTimerId = 0;
   }

   if ( m_drawTimerId )
   {
      killTimer( m_drawTimerId );
      m_drawTimerId = 0;
   }
}

void
PianoRoll::timerEvent( QTimerEvent* event )
{
   if ( event->timerId() == m_drawTimerId )
   {
      update();
   }

   if ( event->timerId() == m_playTimerId )
   {
      m_time = dbTimeInNanoseconds() - m_timeStart;
      m_loopTime = m_loopTimeStart
                 + m_time % m_loopTimeRange;

//      double timeStart = m_periodStep * m_step;
//      if ( t >= timeStart-0.5 )
//      {
//         auto & item = m_items[ m_step ];
//         if ( item.enabled->isChecked() && !item.isPlaying )
//         {
//            item.isPlaying = true;
//            de::audio::Note note;
//            note.m_midiNote = item.note->value();
//            note.m_velocity = 0; //item.velocity->value();
//            note.m_detune = 0; //item.detune->value();
//            sendNote( midiNote, detune, velocity );
//         }

//         m_step++;

//      }

//      if ( m_step >= m_step_count )
//      {
//         for ( auto & item : m_items)
//         {
//            item.isPlaying = false;
//         }
//         m_step = 0;
//      }
   }
   // if ( event->timerId() == m_timerId && m_isPlaying )
   // {
      //
   // }
}

*/

void
PianoRoll::hideEvent( QHideEvent* event )
{
   QWidget::hideEvent( event );
}

void
PianoRoll::showEvent( QShowEvent* event )
{
   //DE_DEBUG("")
   QWidget::showEvent( event );
   //event->accept();
}


bool
PianoRoll::isMouseOverPianoBar() const
{
    return isMouseOver( QPoint(m_mx, m_my), m_skin.r_pianobar );
}

bool
PianoRoll::isMouseOverBeatGrid() const
{
    return isMouseOver( QPoint(m_mx, m_my), m_skin.r_timeline );
}

int
PianoRoll::getKeyIndexFromMousePos() const
{
    // int w = width();
    // int h = height();
    // // Check canvas
    // if ( w < 1 || h < 1 ) return -1;
    // // Check coords
    // if ( m_mouseX < 0 || m_mouseX > w - 1 ) return -1;
    // if ( m_mouseY < 0 || m_mouseY > h - 1 ) return -1;
    // // compute key index, the higher mouse_y,
    // // then the higher key index, but the lower the freq.

    // int keyIndex = m_keyStart + (m_my-m_topHeight) / m_keyHeight;
    int keyIndex = (m_my - m_skin.timelineH) / m_skin.keyH;
    if (keyIndex < 0 || keyIndex > 127)
    {
        // DE_ERROR("No valid keyIndex = ",keyIndex)
        return -1;
    }
    return keyIndex;
}

PianoRoll::SelNote
PianoRoll::computeHoveredNote() const
{
//   if ( !m_clip )
//   {
//      return {};
//   }

    int keyIndex = m_detectedMidiNote;
    if ( keyIndex < 0 || keyIndex > 127 )
    {
        DE_ERROR("No m_detectedKeyIndex = ",keyIndex)
        return {};
    }

    const auto & notes = m_clip->m_notes[ m_detectedMidiNote ];
    for ( size_t i = 0; i < notes.size(); ++i )
    {
        const auto & note = notes[ i ];
        int a = sec2pix( note.ppqNoteOn );
        int b = sec2pix( note.ppqNoteOff );
        if (m_mx >= a && m_mx < b)
        {
            SelNote sel;
            sel.keyIndex = m_detectedMidiNote;
            sel.noteIndex = i;
            return sel;
        }
    }

    return {};
}

void
PianoRoll::mouseMoveEvent( QMouseEvent* event )
{
   m_mx = event->x();
   m_my = event->y();

   // m_isOverPianoBar = isMouseOverPianoBar();
   // m_isOverBeatGrid = isMouseOverBeatGrid();
   m_detectedMidiNote = getKeyIndexFromMousePos();
   m_hoveredNote = computeHoveredNote();

   if ( m_dragMode == 1 ) // drawing
   {
//      int sy = m_mouseY - m_dragStartY;
//      DE_DEBUG("StretchY(",sy,")")
   }
   if ( m_dragMode == 2 ) // stretch y
   {
      int sy = m_my - m_dragStartY;
      DE_DEBUG("StretchY(",sy,")")
   }

   update();
   QWidget::mouseMoveEvent( event );
}

void
PianoRoll::wheelEvent( QWheelEvent* event )
{
    int wheel = event->angleDelta().y();
    DE_DEBUG("MouseWheel = ",wheel )

    if ( wheel >= 1 )
    {
        m_scrollY = std::clamp( m_scrollY - 30, 0, m_skin.keyH * 128 );
        update();
    }
    else if ( wheel <= 1 )
    {
        m_scrollY = std::clamp( m_scrollY + 30, 0, m_skin.keyH * 128 );
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

void PianoRoll::mousePressEvent( QMouseEvent* event )
{
    //   m_X = event->x();
    //   m_Y = event->y();
    //   m_Wheel = 0.0f;
    m_mx = event->x();
    m_my = event->y();

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

        if ( isMouseOver( m_mx, m_my, m_skin.r_timeline ) )
        {
            m_dragMode = 1;
            DE_DEBUG("DrawMode ", m_dragMode)
        }

        if ( isMouseOver( m_mx, m_my, m_skin.r_pianobar) )
        {
            m_dragMode = 2;
            DE_DEBUG("StretchMode ", m_dragMode)
        }
        update();
    }

    QWidget::mousePressEvent( event );
}

void PianoRoll::mouseReleaseEvent( QMouseEvent* event )
{
    m_mx = event->x();
    m_my = event->y();

    //   m_isOverPianoBar = isMouseOverPianoBar();
    //   m_isOverBeatGrid = isMouseOverBeatGrid();

    // Finish Drawing
    if (m_clip && (m_dragMode == 1) && inRange(m_detectedMidiNote,0,127))
    {
        auto a = pix2sec( m_dragStartX );
        auto b = pix2sec( m_mx );
        if ( a > b )
        {
            std::swap( a, b );
        }
        if ( a < b )
        {
            de::session::ClipNote note;
            note.ppqNoteOn = a;
            note.ppqNoteOff = b;
            note.color = de::randomColorRGB();
            auto & notes = m_clip->m_notes[ m_detectedMidiNote ];
            notes.emplace_back( std::move(note) );
            DE_DEBUG("Added Note ")
        }
    }

    // Finish StretchY
    if ( m_dragMode == 2 )
    {
        int sy = m_my - m_dragStartY;
        DE_DEBUG("StretchY Final(",sy,")")
        // m_keyHeight = std::clamp( m_keyHeight + sy/4, 2, 48 );
        m_skin.keyH = std::clamp( m_skin.keyH + sy/4, 2, 48 );
    }

    // Reset all
    m_dragMode = 0;

    // Enqueue ( async ) request for redraw.
    update();

    QWidget::mouseReleaseEvent( event );
}


void
PianoRoll::keyPressEvent( QKeyEvent* event )
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
PianoRoll::keyReleaseEvent( QKeyEvent* event )
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

#if 0


void PianoRoll::drawNotes(int midiNote, int yStart)
{
    const int w = width();
    const int h = height();
    if (w < 1) return;
    if (h < 1) return;

    //m_pixelPerBeat = w / m_beatCount;

    QPainter dc( this );
    dc.fillRect( rect(), m_skin.panelColor );

    // Draw PianoBar + filled background of notes
    //int x = 0;
    int y = 0;

    int keyStart = m_scrollY/m_skin.keyH;
    int keyCount = height()/m_skin.keyH;

    for ( int i = 0; i < keyCount; ++i )
    {
        int keyIndex = keyStart + i;
        if ( keyIndex >= 0 && keyIndex < 128 )
        {
            std::vector<ClipNote> & notes = m_[ keyIndex ];

            // Get key-rect
            QRect r( 0, y, m_keyWidth, m_keyHeight );

            // Draw key, determine colors ( black or white key )
            QColor fillColor = QColor(255,255,255);// white
            QColor penColor = QColor(205,205,205); // light white
            QColor textColor = QColor(2,2,2);      // xor
            if ( key.isBlack )
            {
            fillColor = QColor(2,2,2); // black
            penColor = QColor(65,65,65); // light black
            textColor = QColor(255,255,255); // xor
            }

            if (key.semi == 0) // C key more blue
            {
            fillColor = QColor(230,230,240);
            }

            drawKey( dc, r, fillColor, penColor );

            // Draw key text
            if ( m_keyHeight >= 9 )
            {
            auto s = de::midi::MidiTools::noteStr( key.midiNote );
            auto t = QString::fromStdString( s );
            auto tx = r.x() + m_keyWidth - 6;
            auto ty = r.y() + m_keyHeight/2;
            auto textAlign = de::Align::MiddleRight;
            m_font5x8.drawText( dc, tx, ty, t, textColor, textAlign );
            }

            /// Draw note backgrounds
            fillColor = m_editColorWhite;
            //strokeColor = QColor(220,220,220);
            if ( key.isBlack )
            {
            fillColor = m_editColorBlack;
            //strokeColor = QColor(150,150,150);
            }
            //dc.setPen( QPen( strokeColor ) );
            dc.fillRect( QRect(m_keyWidth, y, w-1-m_keyWidth, m_keyHeight), fillColor );

            if ( key.semi == 0 ) // every C draw blue line below
            {
            dc.setPen( QPen( QColor(100,100,255) ) );
            dc.setBrush( Qt::NoBrush );
            int y = r.y() + r.height() - 1;
            dc.drawLine( m_keyWidth, y, w-1-m_keyWidth, y );
            }
        }
        // End row
        y += m_skin.keyH;
    }

   // Draw Time Grid ( 250ms )
   //y = m_topHeight;
   dc.setPen( QPen( QColor( 155,155,155) ) );
   dc.setBrush( Qt::NoBrush );
   for ( int i = 0; i < 4*10; ++i )
   {
      int x = time2pixel( int64_t( 250*1000*1000 ) * i );
      int y1 = m_topHeight;
      int y2 = h - 1;
      dc.drawLine( x, y1, x, y2 );
   }

   // Draw Time Grid ( 1s )
   //y = m_topHeight;
   dc.setPen( QPen( QColor( 200,200,200) ) );
   dc.setBrush( Qt::NoBrush );
   for ( int i = 0; i < 10; ++i )
   {
      int x = time2pixel( int64_t( 1000000000 ) * i );
      int y1 = m_topHeight;
      int y2 = h - 1;
      dc.drawLine( x, y1, x, y2 );
   }
   // Draw all visible notes
   y = m_topHeight;
   for ( int i = 0; i < m_keyCount; ++i )
   {
      int keyIndex = m_keyStart + i;
      if ( keyIndex < 0 || keyIndex >= int( m_keys.size() ) )
      {
         DE_ERROR("keyIndex = ",keyIndex)
      }
      else
      {
         dc.setPen( Qt::NoPen );
         Key & key = m_keys[ keyIndex ];
         for ( int j = 0; j < int( key.m_notes.size() ); ++j )
         {
            Note const & note = key.m_notes[ j ];
            int x1 = time2pixel( note.timeBeg );
            int x2 = time2pixel( note.timeEnd );
            dc.setBrush( QBrush( toQColor( note.color ) ) );
            dc.drawRect( x1, y, x2-x1+1, m_keyHeight );
         }
      }

      // End row
      y += m_keyHeight;
   }


   // [VisualDebug] Draw detected key index as full row rect.
   if ( m_isOverPianoBar )
   {
      //DE_DEBUG( "isOverPianoBar")
      dc.setPen( QPen( QColor(255,100,100) ) );
      dc.setBrush( Qt::NoBrush );
      QRect m_rectPianoBar( 0, m_topHeight, m_keyWidth, h-1-m_topHeight );
      dc.drawRect( m_rectPianoBar );
   }
   if ( m_isOverBeatGrid )
   {
      //DE_DEBUG( "isOverBeatGrid")
      dc.setPen( QPen( QColor(200,200,255) ) );
      dc.setBrush( Qt::NoBrush );
      QRect m_rectBeatGrid( m_keyWidth, m_topHeight, w-1-m_keyWidth, h-1-m_topHeight );
      dc.drawRect( m_rectBeatGrid );
   }

   if ( m_detectedKeyIndex > -1 )
   {
      //auto nsg = de::midi::MidiTools::noteStr( m_keys[ m_detectedKeyIndex ].midiNote );
      //DE_DEBUG( "m_detectedKeyIndex = ", m_detectedKeyIndex, ", ", nsg )
      dc.setPen( QPen( QColor(255,180,55) ) );
      dc.setBrush( Qt::NoBrush );
      QRect m_rectKeyIndex( 0, m_topHeight + (( m_detectedKeyIndex - m_keyStart ) * m_keyHeight), w-1, m_keyHeight-1 );
      dc.drawRect( m_rectKeyIndex );
   }

   // Draw Loop Start Time
   int xs = time2pixel( m_loopTimeStart );
   int xe = time2pixel( m_loopTimeEnd );
   int xt = time2pixel( m_loopTime );

   dc.setBrush( Qt::NoBrush );

   dc.setPen( QPen( QColor( 0,200,0 ) ) );
   dc.drawLine( xs, m_topHeight, xs, h );

   dc.setPen( QPen( QColor( 220,0,0 ) ) );
   dc.drawLine( xe, m_topHeight, xe, h );

   dc.setPen( QPen( QColor( 220,180,20 ) ) );
   dc.drawLine( xt, m_topHeight, xt, h );

   if ( m_dragMode == 1 ) // drawing
   {
      dc.setPen( Qt::NoPen );
      int x1 = m_dragStartX;
      int y1 = m_topHeight + (m_detectedKeyIndex - m_keyStart) * m_keyHeight;
      int x2 = m_mouseX;
      int y2 = y1 + m_keyHeight;
      dc.setBrush( QBrush( QColor(255,100,100) ) );
      dc.drawRect( x1, y1, x2-x1+1, y2-y1 );
   }
}

#endif
