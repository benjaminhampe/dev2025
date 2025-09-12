/////////////////////////////////////////////////
/// @author Benjamin Hampe <benjaminhampe@gmx.de>
/////////////////////////////////////////////////

#pragma once
#include <de/Console.hpp>
#include <sstream>
#include <vector>

namespace de {

   /// @brief A table cell
   struct TextTableCell
   {
      std::string text;

      TextTableCell() : text()
      {}

      TextTableCell( std::string text_ ) : text( text_ )
      {}
   };

   /// @brief A table row is a vector of cells.
   typedef std::vector< TextTableCell > TextTableRow;

   /// @brief A table is a vector of rows.
   struct TextTable
   {
      std::string name;
      std::vector< TextTableRow > rows; // a row represents one instance of a plane struct

      /// @brief Repeat char ' ' n-times and return as single string.
      ///        Simple tool used to fill voids when formatting the table.
      static std::string
      spaces( size_t n );

      TextTable();

      /// @brief Set header line
      void
      setHeader( TextTableRow row );

      /// @brief Set expected table size ( row count excluding the first header row == num data rows )
      void
      setRowCount( size_t expectedRows );

      /// @brief Append a row to the table
      void
      addRow( TextTableRow row );

      /// @brief Compute max col count of all rows
      uint32_t
      computeColumnCount() const;

      /// @brief Compute max width of all rows of specified column 'x'.
      uint32_t
      computeColumnWidth( size_t x ) const;

      /// @brief Prints the entire table into a vector of strings.
      std::vector< std::string >
      toStringVector() const;
   };

} // end namespace de

