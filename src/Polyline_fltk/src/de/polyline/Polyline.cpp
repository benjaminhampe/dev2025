#include <de/polyline/Polyline.h>





#if 0


   /// @brief generate a mesh from lines
   template < typename GET_POINTCOUNT,
            typename GET_POINT,
            typename GET_VERTEXCOUNT,
            typename ADD_VERTEX3D,
            typename ADD_INDEXED_TRIANGLE
         >
   static bool
   asIndexedTriangles(
         GET_POINTCOUNT const &  getPointCount,
         GET_POINT const &       getPoint,
         GET_VERTEXCOUNT const & getVertexCount,
         ADD_VERTEX3D const &    addVertex3D,
         ADD_INDEXED_TRIANGLE const &  addIndexedTriangle,
         float32_t const              lineWidth )
   {
      std::stringstream debugStream; //
      auto & n = getPointCount();
      auto & v = getVertexCount();
      debugStream << __FUNCTION__ << " :: Begin with pointCount(" << n << "), vertexCount(" << v << "):\n";
      if ( n < 2 )
      {
         if ( n == 1 )
         {
            debugStream << __FUNCTION__ << " :: INFORMATION-LOSS!\n";
         }
         return false; // nothing todo, empty input
      }

      // [SegmentAB]
      //
      //    AL-----BL   A,B are stored, and their normal displacements +-nAB.
      //    |     /|
      //    |    / |
      //    A------B
      //    |  /   |
      //    | /    |
      //    AR-----BR
      //
      auto addLineSegment = [ & ] ( glm::vec3 const & A, glm::vec3 const & B, uint32_t color )
      {
         auto v = getVertexCount();
         auto nAB = getNormal2D( glm::vec2( B ) - glm::vec2( A ) ) * lineWidth;      // Compute normal nAB
         addVertex3D( A, -nAB, color );                  // 0 - AL - First vertex of first segment[0]
         addVertex3D( A, nAB, varyColor( color, 30 ) );  // 1 - AR - Second vertex of first segment[0]
         addVertex3D( B, -nAB, varyColor( color, 30 ) ); // 2 - BL - First vertex of first segment[0]
         addVertex3D( B, nAB, varyColor( color, 30 ) );  // 3 - BR - Second vertex of first segment[0]
         addIndexedTriangle( v, v + 1, v + 2 );    // ccw 012 - AL,AR,BL
         addIndexedTriangle( v + 1, v + 3, v + 2 ); // ccw 132 - AR,BR,BL
         std::cout << "addSegment( A:" << A << ", B:" << B << ", v:" << v << ", nAB:" << nAB << " )\n";
      };

      // Add Line Join between points A,B,C with possible type Bevel, Miter or Round )
      auto addLineJoin = [ & ] ( glm::vec3 const & A, glm::vec3 const & B, glm::vec3 const & C, uint32_t const color )
      {
         LineJoin::ELineJoin lineJoin = LineJoin::Bevel;
         auto v = getVertexCount();
         //auto nAB = getNormal2D( glm::vec2( B ) - glm::vec2( A ) ) * lineWidth;
         //auto nBC = getNormal2D( glm::vec2( C ) - glm::vec2( B ) ) * lineWidth;
         auto phi = getAngleABC( A, B, C );    // Compute angle between vector AB and BC
         uint32_t BL = v - 2;
         uint32_t BR = v - 1;
         uint32_t CL = v ;
         uint32_t CR = v + 1;
         std::cout << "addLineJoin( v:" << v << ", " << lineJoin << ", phi:" << phi << ", A:" << A << ", B:" << B << ", C:" << C << " , BL:" << BL << ", BR:" << BR << ", CL:" << CL << ", CR:" << CR << "\n";
         // Add Right PolyLine Join ( Bevel, Miter or Round )
         //
         // turn left ( math positive, ccw )
         /**
         //     DL---C----DR
         //      \    \    \        // SL = B - 0.5 ( nAB + nBC );
         //       \    \    \       // CR = B + nBC;
         //        \    \    \      // BR = B + nAB;
         //         \    \    \
         // AL-------SL   \    \
         // |    ----  -   \    \
         // |----        -  \    \
         // A----------------B----CR
         // |--------        |    /
         // |        --------|  /
         // AR---------------BR/
         */
         if ( phi < 0.0f ) // turn left ( math positive, ccw )
         {
            std::cout << "Polyline turns left, adds line join right, BR(" << BR << "), CR(" << CR << ")\n";
            if ( lineJoin == LineJoin::Bevel )
            {
               addIndexedTriangle( B, BR, CR ); // LineJoin 1. Bevel Triangle [B,BR,CR] ccw
            }
         }
         // Add Left PolyLine Join ( Bevel, Miter or Round )
         /**
         // turn right ( math negative, cw )
         //
         // AL---------------BL\       AL,AR,BL + AR,BL,
         // |        --------|  \
         // |--------        |    \
         // A----------CR----B----CL
         // |----      /    /|   /
         // |    ---- / -  / |  /
         // AR-------/----/--BR/
         //         /    /    /        // SR = B + 0.5 ( nAB + nBC );
         //        /    /    /         // BL = B - nBC;
         //       /    /    /          // CL = B - nAB;
         //      /    /    /
         //     DR---C----DL
         */
         else
         {
            std::cout << "Polyline turns right, adds line join left, BL(" << BL << "), CL(" << CL << ")\n";
            if ( lineJoin == LineJoin::Bevel )
            {
               addIndexedTriangle( B, BL, CL ); // LineJoin 1. Bevel Triangle [B,BL,CL] ccw
            }
         }
      };

      //
      // Local variables:
      //
      glm::vec3 A,B,C; // We also need normals for both line-segments AB and BC
      glm::vec2 nAB,nBC;
      float32_t phi;                   // The angle between AB and BC in degrees, used to determine side of line-join and intersection to compute
      A = getPoint( 0 );    // Fetch A
      B = getPoint( 1 );    // Fetch B
      //nAB = getNormal2D( B - A ) * lineWidth;      // Compute normal nAB
      addLineSegment( A, B, varyColor( 0xFF2020E0, 60 ) );

      uint32_t processedSegments = 0; // num_discarded_points = getPointCount() - (processedSegments + 1)v d
      for ( size_t i = 2; i < n; ++i )
      {
         C = getPoint( i + 1 ); // Fetch C
         nAB = getNormal2D( B - A ) * lineWidth;      // Compute normal nAB
         phi = getAngleABC( A, B, C );    // Compute angle between vector AB and BC
         #ifdef USE_POINT_SKIPPING
         // if (parallel) then combine both segments by skipping this one.
         if ( std::abs( phi ) < 0.00001f || std::abs( phi - Math::TWO_PI ) < 0.00001f )
         {
            B = C;                            // Skip current segment
            nAB = getNormal2D( B - A ) * lineWidth; // Update normal nAB
            debugStream << __FUNCTION__ << " :: Skip parallel point(" << i << ")\n";
            continue;
         }
         #endif
         uint32_t color = 0xFFEF20EF;
         if ( phi < 0.0f )
         {
            color = 0xFFFF8620;
         }
         addLineJoin( A, B, C, color );

         // Segment BC
         color = 0xFFEFEF20;
         if ( i == n - 1 )
         {
            color = randomColor();
         }
         addLineSegment( A, B, color );

         // prepare NEXT:
         A = B;
         B = C;
         nAB = nBC;
         ++processedSegments; // this leads to +1 than actual segments
      }
      return true;
   }

    // =========================================================================




    /// @brief generate a mesh from lines
    template <  bool CLOCK_WISE,
                bool USE_POINT_SKIPPING,
                bool USE_VECTOR_ADDITION,
                typename ADD_VERTEX3D,
                typename FNO_ADD_INDEX,
                typename GET_POINT,
                typename GET_POINTCOUNT>
    static bool
    asTriangleStrip(
            ADD_VERTEX3D const &  addScalableVertex3D,
            FNO_ADD_INDEX const &      addIndex,
            GET_POINT const &      getPoint,
            GET_POINTCOUNT const & getPointCount,
            float32_t const            lineWidth )
    {
        size_t const n = getPointCount();

        STATIC_LOG_DEBUG("n(",n,"), cw(", CLOCK_WISE, "), pskip(", USE_POINT_SKIPPING, "), vadd(", USE_VECTOR_ADDITION, "), lwidth(", lineWidth, "):\n" )

        //      ( n == 0 ) for empty input we simply abort
        //      ( n == 1 ) for a single point we atleast create LineCaps
        //      ( n == 2 ) for a single line-segment we create 2 LineCaps and a hexagon: TODO: only build rect, but need specialized handling outside the main loop
        //      ( n >= 3 ) a normal polyline ABC with 2 LineCaps, 2 hexagons and 1 LineJoin

        if ( n == 0 )
        {
            MITER_POLYLINE_LOG_ERROR( "getPointCount() == 0, abort.\n" )
            return false; // nothing todo, empty input
        }

        else if ( n == 1 )
        {
            MITER_POLYLINE_LOG_ERROR( "n(1), NOT IMPLEMENTED, INFORMATION-LOSS, should create 2 LineCaps for a single point.\n" )
            return false; // nothing todo, empty input
        }

        //
        // Local variables:
        //
        glm::vec2 A,B,C,nAB,nBC, SL, SR; // We also need normals for both line-segments AB and BC
        float32_t phi;                   // The angle between AB and BC in degrees, used to determine side of line-join and intersection to compute
        uint32_t processedPoints = 0; // num_discarded_points = getPointCount() - (processedSegments + 1)

#ifdef USE_MITER_DEBUG_LOGGING
        float32_t t,s;                                                  // for debugging and limitation of values
        float32_t min_t = std::numeric_limits< float32_t >::max();      // for debugging intersection functions
        float32_t max_t = std::numeric_limits< float32_t >::lowest();   // for debugging intersection functions
        float32_t min_s = std::numeric_limits< float32_t >::max();      // for debugging intersection functions
        float32_t max_s = std::numeric_limits< float32_t >::lowest();   // for debugging intersection functions
        std::stringstream ss;
        glm::vec2 vAB, vBC; // DEBUG vectors
        glm::vec2 dAB, dBC; // DEBUG directions
        float32_t lAB, lBC; // DEBUG lengths
#endif
        //<SEGMENT id="Start">
        A = getPoint( 0 );    // Fetch A
        B = getPoint( 1 );    // Fetch B
        nAB = Math::getNormal2D( B - A ) * lineWidth;      // Compute normal nAB
        if (CLOCK_WISE)
        {
            addScalableVertex3D( A - nAB );    // AL - First vertex of first segment[0]
            addScalableVertex3D( A + nAB );    // AR - Second vertex of first segment[0]
        }
        else
        {
            addScalableVertex3D( A + nAB );    // AR
            addScalableVertex3D( A - nAB );    // AL
        }
        addIndex( 2*processedPoints );
        addIndex( 2*processedPoints + 1 );
        ++processedPoints;

#ifdef USE_MITER_DEBUG_LOGGING
        vAB = B - A;
        dAB = glm::normalize(vAB);
        lAB = glm::length(vAB);
        MITER_POLYLINE_LOG_DEBUG( "Segment[0] A(", A, "), B(", B, "), n(", nAB, "), l(", lAB, "), d(", dAB, ")\n" )
#endif
        // </SEGMENT>

        //<SEGMENT id="Middle">
        for ( size_t i = 1; i < n - 1; ++i )
        {
            C = getPoint( i + 1 );                  // Fetch C

            // Compute angle between vector AB and BC
            //phi = Math::getAngleABC( A, B, C );
            {
                glm::vec2 const AB = B - A;
                glm::vec2 const BC = C - B;
                phi = static_cast< float32_t >( atan2( BC.y, BC.x ) - atan2( AB.y, AB.x ) );
             }

#ifdef USE_MITER_DEBUG_LOGGING
            vAB = B - A;
            dAB = glm::normalize(vAB);
            lAB = glm::length(vAB);
            MITER_POLYLINE_LOG_DEBUG( "Segment[",i,"] A(",A,"),B(",B,"),C(",C,"), nAB(",nAB,"), phi(", Math::toDegrees(phi), ")\n" )
#endif
            if ( USE_POINT_SKIPPING )
            {
                // if (parallel) then combine both segments by skipping this one.
                if ( std::abs( phi ) < 0.1f )
                {
                    B = C;                            // Skip current segment
                    nAB = Math::getNormal2D( B - A ) * lineWidth; // Update normal nAB
                    MITER_POLYLINE_LOG_DEBUG( "skip parallel point ", i, "\n")
                    continue;
                }
            }

            // ... continue computations ...
            nBC = Math::getNormal2D( C - B ) * lineWidth;   // Compute normal nBC

            if ( USE_VECTOR_ADDITION )
            {
                glm::vec2 const nABC = glm::normalize( nAB + nBC );
                SL = B - nABC;
                SR = B + nABC;
            }
            else
            {
                // [LEFT] intersection must exist, or we skip the segment
                bool const hasLeft = IntersectionTool2D::intersectLine32( SL, A - nAB, B - nAB, B - nBC, C - nBC );
                #ifdef USE_MITER_DEBUG_LOGGING
                bool const hasLeft = IntersectionTool2D::intersectLineEx64( SL, A - nAB, B - nAB, B - nBC, C - nBC, t, s );
                min_t = std::min( min_t, t );
                max_t = std::max( max_t, t );
                min_s = std::min( min_s, s );
                max_s = std::max( max_s, s );
                #endif

                bool const hasRight = IntersectionTool2D::intersectLine32( SR, A + nAB, B + nAB, B + nBC, C + nBC );
                #ifdef USE_MITER_DEBUG_LOGGING
                bool const hasRight = IntersectionTool2D::intersectLineEx64( SR, A + nAB, B + nAB, B + nBC, C + nBC, t, s );
                min_t = std::min( min_t, t );
                max_t = std::max( max_t, t );
                min_s = std::min( min_s, s );
                max_s = std::max( max_s, s );

                ss.str("");
                ss << "t(" << t << ", " << min_t << ", " << max_t << "), "
                   << "s(" << s << ", " << min_s << ", " << max_s << ")";

                if ( !hasLeft && !hasRight)
                {
                    MITER_POLYLINE_LOG_ERROR( "Segment[", i, "/", n-1, "] has NO LEFT AND RIGHT intersection with ", ss.str(), ", skip point.\n" )
                    assert( false );
                }
                if ( !hasLeft )
                {
                    MITER_POLYLINE_LOG_ERROR( "Segment[", i, "/", n-1, "] has NO LEFT intersection with ", ss.str(), ", skip point.\n" )
                }
                if ( !hasRight )
                {
                    MITER_POLYLINE_LOG_ERROR( "Segment[", i, "/", n-1, "] has NO RIGHT intersection with ", ss.str(), ", skip point.\n" )
                }
                #endif
                if ( !hasLeft )
                {
                    B = C;                            // Skip current segment
                    nAB = Math::getNormal2D( B - A ) * lineWidth; // Update normal nAB
                    continue;
                }
                if ( !hasRight )
                {
                    B = C;                            // Skip current segment
                    nAB = Math::getNormal2D( B - A ) * lineWidth; // Update normal nAB
                    continue;
                }
            }

            // [ADD VERTICES]
            // ... now the last segment [i-1] has all 4 vertices and current segment has already 2 vertices ...
            if (CLOCK_WISE)
            {
                addScalableVertex3D( SL ); // SL
                addScalableVertex3D( SR ); // SR
            }
            else
            {
                addScalableVertex3D( SR ); // SR
                addScalableVertex3D( SL ); // SL
            }

            // [ADD INDICES]
            /**
            //    -AL----BL                A,B are actually not stored as vertices
            //    |\    |\
            //    | \   | \
            //    A  \  B  \
            //    |   \ |   \
            //    |    \|    \
            //    -AR----BR----CR
            */
            addIndex( 2*processedPoints );
            addIndex( 2*processedPoints + 1 );

            // prepare NEXT:
            A = B;
            B = C;
            nAB = nBC;
            ++processedPoints;
        }
        // </SEGMENT>

        // <SEGMENT id="End-Segment">
        // [ADD VERTICES]
        // ... now add the last two missing vertices of the current [i] segment ...
        if (CLOCK_WISE)
        {
            addScalableVertex3D( B - nAB ); // DL
            addScalableVertex3D( B + nAB ); // DR
        }
        else
        {
            addScalableVertex3D( B + nAB ); // DR
            addScalableVertex3D( B - nAB ); // DL
        }
        // [ADD INDICES]
        addIndex( 2*processedPoints );
        addIndex( 2*processedPoints + 1 );
        ++processedPoints;

        // statistics
        #ifdef USE_MITER_DEBUG_LOGGING
        ss.str("");
        ss << "t(" << t << ", " << min_t << ", " << max_t << "), "
           << "s(" << s << ", " << min_s << ", " << max_s << ")";
        MITER_POLYLINE_LOG_DEBUG( "[END] processedPoints(", processedPoints, ") of (", n, ") with ", ss.str(), "\n")
        #endif
        return true;
    }





#endif



#if 0

        C = getPoint2D( i + 1 );            // Fetch C
        phi = Math::getAngleABC( A, B, C ); // Compute angle between vector AB and BC

        // Skip segment if next segment is parallel to us, and combine both segments by skipping this one.
        if ( std::abs( phi ) < 0.01f )
        {
            B = C;                                         // Skip current point B
            nAB = Math::getNormal2D( B - A ) * lineWidth;  // Update normal nAB with new B
            continue;
        }

        // ... continue computations ...
        nBC = Math::getNormal2D( C - B ) * lineWidth;  // Compute normal nBC

        // LEFT & RIGHT intersection must exist, or we skip the segment
        bool const hasLeft = IntersectionTool2D::intersectLine32( SL, A - nAB, B - nAB, B - nBC, C - nBC );
        bool const hasRight = IntersectionTool2D::intersectLine32( SR, A + nAB, B + nAB, B + nBC, C + nBC );

        if ( !hasLeft || !hasRight)
        {
            B = C;                            // Skip current segment
            nAB = Math::getNormal2D( B - A ) * lineWidth; // Update normal nAB
            //TEXTURED_MITER_POLYLINE_LOG_ERROR( "Segment[", i, "/", n-1, "] has NO intersection, skip point.\n" )
            continue;
        }

        addPrimitive( B, SL, SR, accumLength, randomColor() );

      /*
        // turn left ( math positive, ccw )
        //
        //     DL---C----DR
        //      \    \    \
        //       \    \    \
        //        \    \    \
        //         \    \    \
        // AL-------SL   \    \
        // |    ----  -   \    \
        // |----        -  \    \
        // A----------------B----CR
        // |--------        |    /
        // |        --------|  /
        // AR---------------BR/
        //
        // SL = B - 0.5 ( nAB + nBC );
        // CR = B + nBC;
        // BR = B + nAB;
        //


        if ( phi < 0.0f )     // turn left ( math positive, ccw )
        {
            // if ( IntersectionTool2D::intersectLine32( SL, A - nAB, B - nAB, B - nBC, C - nBC ) )

            glm::vec2 const BR = B + nAB;
            glm::vec2 const CR = B + nBC;
            //addHexagon( A, KL, KR, B, QL, BR, color, colorL, colorR );
//            dc.addVertex( B,  color,  glm::vec2( 0.5f, 0.5f ) ); // v0 + 3
//            dc.addVertex( QL, colorL, glm::vec2( 0.5f, 0.0f ) ); // v0 + 4
//            dc.addVertex( QR, colorR, glm::vec2( 0.5f, 1.0f ) ); // v0 + 5
            //addLineJoin( dc, LineJoin::BEVEL, B, CR, BR, phi );
//            dc.addTriangle2D( B, CR, BR, POLYLINE_COLOR_LINE_JOIN_BEVEL );
//            QR = CR;

        }

        // turn right ( math negative, cw )
        //
        // AL---------------BL\
        // |        --------|  \
        // |--------        |    \
        // A----------------B----CL
        // |----        -  /    /
        // |    ----  -   /    /
        // AR-------SR   /    /
        //         /    /    /
        //        /    /    /
        //       /    /    /
        //      /    /    /
        //     DR---C----DL
        //
        // SR = B + 0.5 ( nAB + nBC );
        // BL = B - nBC;
        // CL = B - nAB;
        //
        else
        {
            // Compute RIGHT intersection ( TODO: Only compute this when using miter join )
            if ( IntersectionTool::intersectRay2D( S, A + nAB, B + nAB, B + nBC, C + nBC ) )
            {
                QR = S;
            }
            glm::vec2 const BL = B - nAB;
            glm::vec2 const CL = B - nBC;
            //addHexagon( A, KL, KR, B, BL, QR, color, colorL, colorR );
            dc.addVertex2D( B,  color,  glm::vec2( 0.5f, 0.5f ) ); // v0 + 3
            dc.addVertex2D( QL, colorL, glm::vec2( 0.5f, 0.0f ) ); // v0 + 4
            dc.addVertex2D( QR, colorR, glm::vec2( 0.5f, 1.0f ) ); // v0 + 5
            //addLineJoin( dc, LineJoin::BEVEL, B, BL, CL, phi );
            dc.addTriangle2D( B, BL, CL, POLYLINE_COLOR_LINE_JOIN_BEVEL );
            QL = CL;
        }
*/
        // prepare NEXT:
        accumLength += glm::length( C - B );
        A = B;
        B = C;
        nAB = nBC;
    }

#endif
