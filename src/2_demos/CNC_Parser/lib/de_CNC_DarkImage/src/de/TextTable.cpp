/////////////////////////////////////////////////
/// @author Benjamin Hampe <benjaminhampe@gmx.de>
/////////////////////////////////////////////////

#include <de/TextTable.hpp>

namespace de {

// static
std::string
TextTable::spaces( size_t n )
{
   std::ostringstream s;
   for ( size_t i = 0; i < n; ++i )
   {
      s << ' ';
   }
   return s.str();
}

TextTable::TextTable()
{
   rows.resize( 1 ); // Always room for a header line
}

void
TextTable::setHeader( TextTableRow row )
{
   rows[ 0 ] = row;
}

void
TextTable::setRowCount( size_t expectedRows )
{
   rows.reserve( 1 + expectedRows );
}

void
TextTable::addRow( TextTableRow row )
{
   rows.emplace_back( std::move( row ) );
}

uint32_t
TextTable::computeColumnCount() const
{
   uint32_t n = 0;
   for ( size_t y = 0; y < rows.size(); ++y )
   {
      n = std::max( n, uint32_t( rows[ y ].size() ) );
   }
   return n;
}

uint32_t
TextTable::computeColumnWidth( size_t x ) const
{
   size_t w = 0;
   for ( size_t y = 0; y < rows.size(); ++y )
   {
      TextTableRow const & row = rows[ y ];
      if ( row.size() > x )
      {
         w = std::max( w, row[ x ].text.size() );
      }
   }
   return uint32_t( w );
}

std::vector< std::string >
TextTable::toStringVector() const
{
   std::vector< std::string > lines;
   lines.reserve( rows.size() );

   // Compute max width of each column ( to be able to format the table )
   uint32_t const colCount = computeColumnCount();
   std::vector< uint32_t > cw_maxima;
   cw_maxima.reserve( colCount );
   for ( uint32_t c = 0; c < colCount; ++c )
   {
      cw_maxima.emplace_back( computeColumnWidth( c ) );
   }

   // Print table row by row.
   for ( size_t y = 0; y < rows.size(); ++y )
   {
      TextTableRow const & row = rows[ y ];

      std::ostringstream s;

      if ( y > 0 )
      {
         s << (y-1);
         if ( y-1 < 10 ) s << " ";
         if ( y-1 < 100 ) s << " ";
      }
      else {
         s << "No.";
      }

      for ( size_t x = 0; x < row.size(); ++x )
      {
         TextTableCell const & col = row[ x ];
         s << "|";
         s << col.text;
         size_t w1 = col.text.size();
         size_t w2 = size_t( cw_maxima[ x ] );
         if ( w1 < w2 )
         {
            s << spaces( w2 - w1 );
         }
      }

      lines.emplace_back( std::move( s.str() ) );
   }

   return lines;
}

} // end namespace de
