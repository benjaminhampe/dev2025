#pragma once
#include <H3/H3_Board.h>

class H3_Game;

// =======================================================================
struct H3_Font
// =======================================================================
{
    H3_Font();
    ~H3_Font();
    void init();
    void add( std::string id, const std::string& uri );
    void addFromMemory( std::string id, const uint8_t* pData, uint64_t nBytes );
    
    de::Font get( const std::string id ) const;

    void clear();

    void save(const std::string& baseDir);

    void load();
    
    std::unordered_map<std::string, de::Font> m_cache;
};
