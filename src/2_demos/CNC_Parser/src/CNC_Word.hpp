#pragma once
#include <CNC_Util.hpp>

namespace de {
namespace cnc {

// All CNC commands have the (simplified benni) form <char><double>,
// e.g. G0, F1000.1, N0001. In G0 the 0 is given as double,
// the parser listener can then round to integer if wanted.
// This simplifies everything. No need to parse different value type.

// No spaces shall be between <char> and <double>, algo is not robust for that!!!
// As far as ive seen there are never spaces between G and 00 in G00
// and never spaces between X and -1 in X-1. But i can be completly mistaken yet.
// Its hard to find references that document such important syntax details.
// Maybe it differs on every CNC machine. Smallest common denominator would be no spaces.

// This class decomposes multiline text into tokenized lines.
// Each CNC command shall not have any space between <char> and <double>
// Normally a line contains only one move command G0,G1,G2,G3,G4, but its not really an error if otherwise.
// A token is [commandList,comment] = pair<vector<string>, string >
// The commandList is a vector of commands.

// its kinda bad 9 byte alignment in memory ( char=1, double=8 ).
// But this way its not memory wasting. We have enough compute power.
// So a token can look like ( vector<string>{"G0","X0","Y100.1"}, "Comment: Fast move to x0,y100.1" )

// Parser output

// A decomposed (ready to interpret) cnc word = <char><double>,
// Each CNC word has the form (=syntax) struct word { char key; double value; }
// decomposes text G21 into word('G',21.0)
// G21 = <char><double> G17 G90 F100 M03 S1000
struct CNC_Word
{
   enum eState
   {
      Untouched = 0,
      Skipped,
      Parsed,
      Malformed,
      Fatal
   };

   // Util operating on CNC_Word::eState
   static u32
   getStateColor( u8 state )
   {
      switch (state)
      {
      case Untouched: return 0xFF8D8D8D; // comment color
      case Skipped: return 0xFFDDDDDD; // comment color
      case Parsed: return 0xFF00AD00; // success color
      case Malformed: return 0xFF50BDEF; // warn malfunction color
      case Fatal: return 0xFF5050DF; // error/fatal color
      default:return 0xFFFF00FF; // debug/anomaly violett
      }
   }

   // Util operating on CNC_Word::eState
   static std::string
   getStateName( u8 state )
   {
      switch (state)
      {
      case Untouched: return "untouched"; // comment color
      case Skipped: return "skipped"; // comment color
      case Parsed: return"parsed"; // success color
      case Malformed: return"malformed"; // warn malfunction color
      case Fatal: return"fatal"; // error/fatal color
      default: return"unknown"; // debug/anomaly violett
      }
   }

   f64 m_value;
   char m_key;
   u8 m_state;
   u16 m_dummy;
   u32 m_color;

   CNC_Word()
      : m_value(0.0), m_key('\0'), m_state(Untouched), m_color(getStateColor(Untouched))
   {}

   f64 d() const { return m_value; }
   s32 i() const { return s32(m_value); }

   bool
   isWord() const
   {
      return CNC_Util::isWord(m_key);
   }

   std::string
   toString() const
   {
      std::ostringstream s;
      s << char(m_key) << m_value;
      return s.str();
   }

   std::string
   toHtml() const
   {
      std::string tag_name = std::string("word_") + getStateName(m_state);
      std::ostringstream o;
      //o << "<" << tag_name << ">" << toString() << "</" << tag_name << ">";
      o << "<span style=\"color:" << HtmlUtil::rgb( m_color ) << "\">" << toString() << "</span>";
      return o.str();
   }
};


} // end namespace cnc
} // end namespace de

