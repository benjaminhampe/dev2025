#pragma once
#include <cstdint>
#include <sstream>

namespace de {
namespace audio {

int const EGenreCount = 126;

const char* const EGenreStrings[] =
{
   "Blues", "Classic Rock", "Country", "Dance", "Disco",						// 0 - 4
   "Funk", "Grunge", "Hip-Hop", "Jazz", "Metal",								// 5 - 9
   "New Age", "Oldies", "Other", "Pop", "R&B",									// 10 - 14
   "Rap", "Reggae", "Rock", "Techno", "Industrial",							// 15 - 19
   "Alternative", "Ska", "Death Metal", "Pranks", "Soundtrack",				// 20 - 24
   "Euro-Techno", "Ambient", "Trip-Hop", "Vocal", "Jazz+Funk",					// 25 - 29
   "Fusion", "Trance",	"Classical", "Instrumental", "Acid",					// 30 - 34
   "House", "Game", "Sound Clip", "Gospel", "Noise",							// 35 - 39
   "Alt. Rock", "Bass", "Soul", "Punk", "Space",								// 40 - 44
   "Meditative", "Instrumental Pop", "Instrumental Rock", "Ethnic", "Gothic",	// 45 - 49
   "Darkwave",	"Techno-Industrial", "Electronic", "Pop-Folk", "Eurodance",		// 50 - 54
   "Dream", "Southern Rock", "Comedy", "Cult", "Gangsta",						// 55 - 59
   "Top 40", "Christian Rap", "Pop/Funk", "Jungle", "Native American",			// 60 - 64
   "Cabaret", "New Wave", "Psychadelic", "Rave", "Showtunes",					// 65 - 69
   "Trailer", "Lo-Fi", "Tribal", "Acid Punk", "Acid Jazz",						// 70 - 74
   "Polka", "Retro", "Musical", "Rock & Roll", "Hard Rock",					// 75 - 79
   "Folk",	"Folk/Rock", "National Folk", "Swing", "Fusion",					// 80 - 84
   "Bebob", "Latin", "Revival", "Celtic", "Bluegrass",							// 85 - 89
   "Avantgarde", "Gothic Rock", "Progressive Rock", "Psychedelic Rock", "Symphonic Rock",	// 90 - 94
   "Slow Rock", "Big Band", "Chorus", "Easy Listening", "Acoustic",			// 95 - 99
   "Humour", "Speech", "Chanson", "Opera", "Chamber Music",					// 100 - 104
   "Sonata", "Symphony", "Booty Bass", "Primus", "Porn Groove",				// 105 - 109
   "Satire", "Slow Jam", "Club", "Tango", "Samba",								// 110 - 114
   "Folklore", "Ballad", "Power Ballad", "Rhythmic Soul", "Freestyle",			// 115 - 119
   "Duet", "Punk Rock", "Drum Solo", "A capella", "Euro-House",				// 120 - 124
   "Dance Hall" 																// 125
};

inline std::string
EGenre_toString( int genre )
{
   if (genre >= 0 && genre < EGenreCount)
      return EGenreStrings[genre];
   else
      return "";
}

} // end namespace audio
} // end namespace de
