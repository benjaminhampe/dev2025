#include "H3_Tex.h"
#include <H3/H3_Game.h>

H3_Tex::H3_Tex() : m_driver(nullptr)
{

}

void
H3_Tex::add( H3_Game & game, eID texId, H3_Img::eID imgId,
            const de::gpu::SamplerOptions& so )
{
    if (!m_driver)
    {
        throw std::runtime_error("No driver in H3_Tex::add()");
    }

    auto img = H3_getImg( game, imgId );
    auto texName = dbStr("h3t_",u32(texId));
    auto tex = m_driver->createTexture2D( texName, *img, so );
    m_textures.push_back(tex);

    if (m_refs.find(texId) != m_refs.end())
    {
        DE_ERROR("Texture ",texId, " already exists")
    }
    m_refs[ texId ] = de::gpu::TexRef(tex);
}

void
H3_Tex::init( H3_Game & game )
{
    m_driver = game.getDriver();

    de::gpu::SamplerOptions so;
    so.setAF( 1.0f );
    so.setMin( de::gpu::SamplerOptions::Minify::Linear );
    so.setMag( de::gpu::SamplerOptions::Magnify::Linear );
    so.setWrapR( de::gpu::SamplerOptions::Wrap::Repeat );
    so.setWrapS( de::gpu::SamplerOptions::Wrap::Repeat );
    so.setWrapT( de::gpu::SamplerOptions::Wrap::Repeat );

    // gfx/wallpaper
    {
        add( game, Satan, H3_Img::Satan, so);
    }

    // gfx/base
    {
        add( game, Unknown, H3_Img::Unknown, so);
        add( game, Benni, H3_Img::Benni, so);

        de::gpu::SamplerOptions soWood;
        soWood.setAF( 1.0f );
        soWood.setMin( de::gpu::SamplerOptions::Minify::Linear ); // MipmapLinear
        soWood.setMag( de::gpu::SamplerOptions::Magnify::Linear );
        soWood.setWrapR( de::gpu::SamplerOptions::Wrap::Repeat );
        soWood.setWrapS( de::gpu::SamplerOptions::Wrap::Repeat );
        soWood.setWrapT( de::gpu::SamplerOptions::Wrap::Repeat );
        add( game, Wood, H3_Img::Wood, soWood);
        add( game, Wood2, H3_Img::Wood2, soWood);
        add( game, Wood3, H3_Img::Wood3, soWood);
        add( game, Thief, H3_Img::Thief, soWood);
        add( game, Grunge, H3_Img::Grunge, so);
        add( game, Scroll, H3_Img::Scroll, so);
        add( game, Scroll1, H3_Img::Scroll1, so);
        add( game, Scroll2, H3_Img::Scroll2, so);
    }

    // gfx/tiles
    {
        add( game, TileWater, H3_Img::TileWater, so);
        add( game, TileDesert, H3_Img::TileDesert, so);
        add( game, TileA, H3_Img::TileA, so);
        add( game, TileB, H3_Img::TileB, so);
        add( game, TileC, H3_Img::TileC, so);
        add( game, TileD, H3_Img::TileD, so);
        add( game, TileE, H3_Img::TileE, so);
    }

    // gfx/tiles2
    {
        de::gpu::SO so2;
        so2.setAF( 1.0f );
        so2.setMag( de::gpu::SO::Magnify::Linear );
        so2.setMin( de::gpu::SO::Minify::LinearMipmapLinear );
        so2.setWrapR( de::gpu::SO::Wrap::ClampToEdge );
        so2.setWrapS( de::gpu::SO::Wrap::ClampToEdge );
        so2.setWrapT( de::gpu::SO::Wrap::ClampToEdge );
        add( game, TileDesert_D, H3_Img::TileDesert, so2);
        add( game, TileA_D, H3_Img::TileA_D, so2);
        add( game, TileB_D, H3_Img::TileB_D, so2);
        add( game, TileC_D, H3_Img::TileC_D, so2);
        add( game, TileD_D, H3_Img::TileD_D, so2);
        add( game, TileE_D, H3_Img::TileE_D, so2);
    }

    // gfx/chips
    {
        add( game, Chip2, H3_Img::Chip2, so );   // 0
        add( game, Chip3, H3_Img::Chip3, so );   // 1
        add( game, Chip4, H3_Img::Chip4, so );   // 2
        add( game, Chip5, H3_Img::Chip5, so );   // 3
        add( game, Chip6, H3_Img::Chip6, so );   // 4
        add( game, Chip8, H3_Img::Chip8, so );   // 5
        add( game, Chip9, H3_Img::Chip9, so );   // 6
        add( game, Chip10, H3_Img::Chip10, so);  // 7
        add( game, Chip11, H3_Img::Chip11, so);  // 8
        add( game, Chip12, H3_Img::Chip12, so);  // 9
        add( game, ChipS, H3_Img::ChipS, so );   // 10
        add( game, ChipW, H3_Img::ChipW, so );   // 11
        add( game, Chip3v1, H3_Img::Chip3v1, so ); // 12
        add( game, ChipA, H3_Img::ChipA, so );   // 13
        add( game, ChipB, H3_Img::ChipB, so );   // 14
        add( game, ChipC, H3_Img::ChipC, so );   // 15
        add( game, ChipD, H3_Img::ChipD, so );   // 16
        add( game, ChipE, H3_Img::ChipE, so );   // 17
    }

    // gfx/ui/dice
    {
        constexpr int N = 6;
        std::array<const de::Image*,N> imgs;
        for (size_t i = 0; i < imgs.size(); ++i)
        {
            imgs[i] = H3_getImg( game, H3_Img::eID(u32(H3_Img::Dice1) + i) );
        }
        const int w = imgs[0]->w();
        const int h = imgs[0]->h();
        de::Image img( w, h * N, de::PixelFormat::R8G8B8 );
        int y = 0;
        for (const auto & p : imgs)
        {
            de::ImagePainter::drawImage(img, *p, 0, y, false);
            y += h;
        }
        //dbSaveImage(img,"H3_dice.png");
        //dbSaveImage(img,"H3_dice.webp");
        auto texDice = m_driver->createTexture2D( "h3_dice", img, so );
        m_refs[ Dice ] = de::gpu::TexRef(texDice);
        m_refs[ Dice1 ] = de::gpu::TexRef(texDice, de::Recti(1,1,    w-2,h-2));
        m_refs[ Dice2 ] = de::gpu::TexRef(texDice, de::Recti(1,1+h,  w-2,h-2));
        m_refs[ Dice3 ] = de::gpu::TexRef(texDice, de::Recti(1,1+2*h,w-2,h-2));
        m_refs[ Dice4 ] = de::gpu::TexRef(texDice, de::Recti(1,1+3*h,w-2,h-2));
        m_refs[ Dice5 ] = de::gpu::TexRef(texDice, de::Recti(1,1+4*h,w-2,h-2));
        m_refs[ Dice6 ] = de::gpu::TexRef(texDice, de::Recti(1,1+5*h,w-2,h-2));
        m_textures.push_back(texDice);
    }

    // gfx/ui/bonus
    {
        std::array<const de::Image*,2> imgs;
        imgs[0] = H3_getImg(game, H3_Img::BonusArmy );
        imgs[1] = H3_getImg(game, H3_Img::BonusRoad );
        const int w = imgs[0]->w();
        const int h = imgs[0]->h();
        int x = 0;
        int y = 0;
        de::Image img( w, h * 2, de::PixelFormat::R8G8B8A8 );
        de::ImagePainter::drawImage(img, *(imgs[0]), x, y); y += h;
        de::ImagePainter::drawImage(img, *(imgs[1]), x, y); //y += h;
        //dbSaveImage(img,"H3_bonus.png");
        //dbSaveImage(img,"H3_bonus.webp");
        auto texBonus = m_driver->createTexture2D( "h3_bonus", img, so );
        m_refs[ Bonus ] = de::gpu::TexRef(texBonus);
        m_refs[ BonusArmy ] = de::gpu::TexRef(texBonus, de::Recti(1,1,  w-2,h-2));
        m_refs[ BonusRoad ] = de::gpu::TexRef(texBonus, de::Recti(1,h+1,w-2,h-2));
        m_textures.push_back(texBonus);
    }

    // gfx/ui/card1
    {
        std::array<const de::Image*,5> imgs;
        imgs[0] = H3_getImg( game, H3_Img::CardA);
        imgs[1] = H3_getImg( game, H3_Img::CardB);
        imgs[2] = H3_getImg( game, H3_Img::CardC);
        imgs[3] = H3_getImg( game, H3_Img::CardD);
        imgs[4] = H3_getImg( game, H3_Img::CardE);
        const int w = imgs[0]->w();
        const int h = imgs[0]->h();
        int x = 0;
        int y = 0;
        de::Image img( w, h * 5, de::PixelFormat::R8G8B8 );
        de::ImagePainter::drawImage(img, *imgs[0], x, y); y += h;
        de::ImagePainter::drawImage(img, *imgs[1], x, y); y += h;
        de::ImagePainter::drawImage(img, *imgs[2], x, y); y += h;
        de::ImagePainter::drawImage(img, *imgs[3], x, y); y += h;
        de::ImagePainter::drawImage(img, *imgs[4], x, y); //y += h;
        //dbSaveImage(img,"H3_card1.png");
        //dbSaveImage(img,"H3_card1.webp");
        auto texCard1 = m_driver->createTexture2D( "h3_card1", img, so );
        m_refs[ Card1 ] = de::gpu::TexRef(texCard1);
        m_refs[ CardA ] = de::gpu::TexRef(texCard1, de::Recti(1,1,    w-2,h-2));
        m_refs[ CardB ] = de::gpu::TexRef(texCard1, de::Recti(1,1+h,  w-2,h-2));
        m_refs[ CardC ] = de::gpu::TexRef(texCard1, de::Recti(1,1+2*h,w-2,h-2));
        m_refs[ CardD ] = de::gpu::TexRef(texCard1, de::Recti(1,1+3*h,w-2,h-2));
        m_refs[ CardE ] = de::gpu::TexRef(texCard1, de::Recti(1,1+4*h,w-2,h-2));
        m_textures.push_back(texCard1);
    }

    // gfx/ui/card2
    {
        std::array<const de::Image*,3> imgs;
        imgs[0] = H3_getImg( game, H3_Img::DoBuyEventCard);
        imgs[1] = H3_getImg( game, H3_Img::EventCardKnight);
        imgs[2] = H3_getImg( game, H3_Img::EventCardPoint);
        const int w = imgs[0]->w();
        const int h = imgs[0]->h();
        int x = 0;
        int y = 0;
        de::Image img( w, h * 3, de::PixelFormat::R8G8B8A8 );
        de::ImagePainter::drawImage(img, *imgs[0], x, y); y += h;
        de::ImagePainter::drawImage(img, *imgs[1], x, y); y += h;
        de::ImagePainter::drawImage(img, *imgs[2], x, y); //y += h;
        //dbSaveImage(img,"H3_card2.png");
        //dbSaveImage(img,"H3_card2.webp");
        auto texCard2 = m_driver->createTexture2D( "h3_card2", img, so );
        m_refs[ Card2 ] = de::gpu::TexRef(texCard2);
        m_refs[ DoBuyEventCard ] = de::gpu::TexRef(texCard2, de::Recti(1,1,    w-2,h-2));
        m_refs[ EventCardKnight ]= de::gpu::TexRef(texCard2, de::Recti(1,1+h,  w-2,h-2));
        m_refs[ EventCardPoint ] = de::gpu::TexRef(texCard2, de::Recti(1,1+2*h,w-2,h-2));
        m_textures.push_back(texCard2);
    }

    // gfx/ui/do1
    {
        std::array<const de::Image*,7> imgs;
        imgs[0] = H3_getImg( game, H3_Img::DoBank);
        imgs[1] = H3_getImg( game, H3_Img::DoDice);
        imgs[2] = H3_getImg( game, H3_Img::DoEndTurn);
        imgs[3] = H3_getImg( game, H3_Img::DoPlayCard);
        imgs[4] = H3_getImg( game, H3_Img::DoTrade);
        imgs[5] = H3_getImg( game, H3_Img::DoRedo);
        imgs[6] = H3_getImg( game, H3_Img::DoUndo);
        const int w = imgs[0]->w();
        const int h = imgs[0]->h();
        int x = 0;
        int y = 0;
        de::Image img( w, h * 6, de::PixelFormat::R8G8B8A8 );
        de::ImagePainter::drawImage(img, *imgs[0], x, y); y += h;
        de::ImagePainter::drawImage(img, *imgs[1], x, y); y += h;
        de::ImagePainter::drawImage(img, *imgs[2], x, y); y += h;
        de::ImagePainter::drawImage(img, *imgs[3], x, y); y += h;
        de::ImagePainter::drawImage(img, *imgs[4], x, y); y += h;
        de::ImagePainter::drawImage(img, *imgs[5], x, y); x += w/2;
        de::ImagePainter::drawImage(img, *imgs[6], x, y);
        //dbSaveImage(img,"H3_do1.png");
        //dbSaveImage(img,"H3_do1.webp");
        auto texDo1 = m_driver->createTexture2D( "h3_do1", img, so );
        m_refs[ Do1 ]       = de::gpu::TexRef(texDo1);
        m_refs[ DoBank ]    = de::gpu::TexRef(texDo1, de::Recti(1,1,    w-2,h-2));
        m_refs[ DoDice ]    = de::gpu::TexRef(texDo1, de::Recti(1,1+h,  w-2,h-2));
        m_refs[ DoEndTurn ] = de::gpu::TexRef(texDo1, de::Recti(1,1+2*h,w-2,h-2));
        m_refs[ DoPlayCard ]= de::gpu::TexRef(texDo1, de::Recti(1,1+3*h,w-2,h-2));
        m_refs[ DoTrade ]   = de::gpu::TexRef(texDo1, de::Recti(1,1+4*h,w-2,h-2));
        m_refs[ DoRedo ]    = de::gpu::TexRef(texDo1, de::Recti(1,1+5*h,w/2-2,h-2));
        m_refs[ DoUndo ]    = de::gpu::TexRef(texDo1, de::Recti(1 + w/2,1+5*h,w/2-2,h-2));
        m_textures.push_back(texDo1);
    }

    // gfx/ui/do2
    {
        std::array<const de::Image*,6> imgs;
        imgs[0] = H3_getImg( game, H3_Img::DoBuyCity);
        imgs[1] = H3_getImg( game, H3_Img::DoBuyFarm);
        imgs[2] = H3_getImg( game, H3_Img::DoBuyRoad);
        imgs[3] = H3_getImg( game, H3_Img::DoCancel);
        imgs[4] = H3_getImg( game, H3_Img::DoRob);
        imgs[5] = H3_getImg( game, H3_Img::DoTemplate);
        const int w = imgs[0]->w();
        const int h = imgs[0]->h();
        int x = 0;
        int y = 0;
        de::Image img( w, h * 6, de::PixelFormat::R8G8B8A8 );
        de::ImagePainter::drawImage(img, *imgs[0], x, y); y += h;
        de::ImagePainter::drawImage(img, *imgs[1], x, y); y += h;
        de::ImagePainter::drawImage(img, *imgs[2], x, y); y += h;
        de::ImagePainter::drawImage(img, *imgs[3], x, y); y += h;
        de::ImagePainter::drawImage(img, *imgs[4], x, y); y += h;
        de::ImagePainter::drawImage(img, *imgs[5], x, y); //y += h;
        //dbSaveImage(img,"H3_do2.png");
        //dbSaveImage(img,"H3_do2.webp");
        auto texDo2 = m_driver->createTexture2D( "h3_do2", img, so );
        m_refs[ Do2 ]       = de::gpu::TexRef(texDo2);
        m_refs[ DoBuyCity ] = de::gpu::TexRef(texDo2, de::Recti(1,1,    w-2,h-2));
        m_refs[ DoBuyFarm ] = de::gpu::TexRef(texDo2, de::Recti(1,1+h,  w-2,h-2));
        m_refs[ DoBuyRoad ] = de::gpu::TexRef(texDo2, de::Recti(1,1+2*h,w-2,h-2));
        m_refs[ DoCancel ]  = de::gpu::TexRef(texDo2, de::Recti(1,1+3*h,w-2,h-2));
        m_refs[ DoRob ]     = de::gpu::TexRef(texDo2, de::Recti(1,1+4*h,w-2,h-2));
        m_refs[ DoTemplate ]= de::gpu::TexRef(texDo2, de::Recti(1,1+5*h,w-2,h-2));
        m_textures.push_back(texDo2);
    }

    // gfx/ui/player
    {
        constexpr int N = 16;
        std::array<const de::Image*,N> imgs;
        for (size_t i = 0; i < imgs.size(); ++i)
        {
            imgs[i] = H3_getImg( game, H3_Img::eID(u32(H3_Img::PlayU) + i) );
        }
        const int w = imgs[0]->w();
        const int h = imgs[0]->h();
        de::Image img( 4 * w, 4 * h, de::PixelFormat::R8G8B8 );

        for (int j = 0; j < 4; ++j)
        {
            for (int i = 0; i < 4; ++i)
            {
                const auto linearIndex = size_t(i) + size_t(4*j);
                const auto p = imgs[ linearIndex ];
                de::ImagePainter::drawImage(img, *p, i*w, j*h, false);
            }
        }
        //dbSaveImage(img,"H3_player.webp");
        auto texPlayer = m_driver->createTexture2D( "h3_player", img, so );
        m_refs[ Player ] = de::gpu::TexRef(texPlayer);
        for (int j = 0; j < 4; ++j)
        {
            for (int i = 0; i < 4; ++i)
            {
                const auto linearIndex = size_t(i) + size_t(4*j);
                const auto id = eID(PlayU + linearIndex);
                const auto x = i * w;
                const auto y = j * h;
                m_refs[ id ] = de::gpu::TexRef(texPlayer, de::Recti(x,y,w,h));
            }
        }
        m_textures.push_back(texPlayer);
    }

    // gfx/ui/stat
    {
        std::array<const de::Image*,5> imgs;
        imgs[0] = H3_getImg( game, H3_Img::StatCard);
        imgs[1] = H3_getImg( game, H3_Img::StatCity);
        imgs[2] = H3_getImg( game, H3_Img::StatFarm);
        imgs[3] = H3_getImg( game, H3_Img::StatVic);
        imgs[4] = H3_getImg( game, H3_Img::StatRoad);
        const int w = imgs[0]->w();
        const int h = imgs[0]->h();
        int x = 0;
        int y = 0;
        de::Image img( w, h * 5, de::PixelFormat::R8G8B8A8 );
        de::ImagePainter::drawImage(img, *imgs[0], x, y); y += h;
        de::ImagePainter::drawImage(img, *imgs[1], x, y); y += h;
        de::ImagePainter::drawImage(img, *imgs[2], x, y); y += h;
        de::ImagePainter::drawImage(img, *imgs[3], x, y); y += h;
        de::ImagePainter::drawImage(img, *imgs[4], x, y); // y += h;
        //dbSaveImage(img,"H3_stat.png");
        //dbSaveImage(img,"h3_stat.webp");
        auto texStat = m_driver->createTexture2D( "h3_stat", img, so );
        m_refs[ Stat ]     = de::gpu::TexRef(texStat);
        m_refs[ StatCard ] = de::gpu::TexRef(texStat, de::Recti(1,1,    w-2,h-2));
        m_refs[ StatCity ] = de::gpu::TexRef(texStat, de::Recti(1,1+1*h,w-2,h-2));
        m_refs[ StatFarm ] = de::gpu::TexRef(texStat, de::Recti(1,1+2*h,w-2,h-2));
        m_refs[ StatVic ]  = de::gpu::TexRef(texStat, de::Recti(1,1+3*h,w-2,h-2));
        m_refs[ StatRoad ] = de::gpu::TexRef(texStat, de::Recti(1,1+4*h,w-2,h-2));
        m_textures.push_back(texStat);
    }
}

de::gpu::TexRef
H3_getDiceTex(const H3_Game& game, const int diceValue )
{
    switch (diceValue)
    {
    case 1: return game.getTex(H3_Tex::Dice1, "H3_getDiceTex.1");
    case 2: return game.getTex(H3_Tex::Dice2, "H3_getDiceTex.2");
    case 3: return game.getTex(H3_Tex::Dice3, "H3_getDiceTex.3");
    case 4: return game.getTex(H3_Tex::Dice4, "H3_getDiceTex.4");
    case 5: return game.getTex(H3_Tex::Dice5, "H3_getDiceTex.5");
    case 6: return game.getTex(H3_Tex::Dice6, "H3_getDiceTex.6");
    default: return game.getTex(H3_Tex::Unknown, "H3_getDiceTex.Unknown");
    }
}

H3_Tex::eID
H3_convertTileType2Tex( const H3_TileType tt )
{
    switch ( tt )
    {
        case H3_TileType::Desert: return H3_Tex::TileDesert;
        case H3_TileType::Water: return H3_Tex::TileWater;
        case H3_TileType::TileA: return H3_Tex::TileA;
        case H3_TileType::TileB: return H3_Tex::TileB;
        case H3_TileType::TileC: return H3_Tex::TileC;
        case H3_TileType::TileD: return H3_Tex::TileD;
        case H3_TileType::TileE: return H3_Tex::TileE;
        case H3_TileType::Port3v1: return H3_Tex::TileWater; // return Port3v1;
        case H3_TileType::PortA: return H3_Tex::TileWater; // return PortA;
        case H3_TileType::PortB: return H3_Tex::TileWater; // return PortB;
        case H3_TileType::PortC: return H3_Tex::TileWater; // return PortC;
        case H3_TileType::PortD: return H3_Tex::TileWater; // return PortD;
        case H3_TileType::PortE: return H3_Tex::TileWater; // return PortE;
        default: return H3_Tex::Unknown;
    }
}

de::gpu::TexRef
H3_getTileTex(const H3_Game& game, const H3_TileType tt )
{
    return game.getTex(H3_convertTileType2Tex( tt ), "H3_getTileTex" );
}

de::gpu::TexRef
H3_getChipTex(const H3_Game& game, const int diceValue, const H3_TileType tt )
{
         if ( tt == H3_TileType::PortA ) { return game.getTex(H3_Tex::ChipA, "H3_getChipTex.A" ); }
    else if ( tt == H3_TileType::PortB ) { return game.getTex(H3_Tex::ChipB, "H3_getChipTex.B" ); }
    else if ( tt == H3_TileType::PortC ) { return game.getTex(H3_Tex::ChipC, "H3_getChipTex.C" ); }
    else if ( tt == H3_TileType::PortD ) { return game.getTex(H3_Tex::ChipD, "H3_getChipTex.D" ); }
    else if ( tt == H3_TileType::PortE ) { return game.getTex(H3_Tex::ChipE, "H3_getChipTex.E" ); }
    else if ( tt == H3_TileType::Port3v1 ){return game.getTex(H3_Tex::Chip3v1, "H3_getChipTex.3v1" ); }
    else
    {
        switch ( diceValue )
        {
      //case 0: return game.getTex(H3_Tex::Chip0, "H3_getChipTex.0" );
        case 2: return game.getTex(H3_Tex::Chip2, "H3_getChipTex.2" );
        case 3: return game.getTex(H3_Tex::Chip3, "H3_getChipTex.3" );
        case 4: return game.getTex(H3_Tex::Chip4, "H3_getChipTex.4" );
        case 5: return game.getTex(H3_Tex::Chip5, "H3_getChipTex.5" );
        case 6: return game.getTex(H3_Tex::Chip6, "H3_getChipTex.6" );
      //case 7: return game.getTex(H3_Tex::Unknown );
        case 8: return game.getTex(H3_Tex::Chip8, "H3_getChipTex.8" );
        case 9: return game.getTex(H3_Tex::Chip9, "H3_getChipTex.9" );
        case 10: return game.getTex(H3_Tex::Chip10, "H3_getChipTex.10" );
        case 11: return game.getTex(H3_Tex::Chip11, "H3_getChipTex.11" );
        case 12: return game.getTex(H3_Tex::Chip12, "H3_getChipTex.12" );
        default: return game.getTex(H3_Tex::Unknown, "H3_getChipTex.Unknown" );
        }
    }
}

de::gpu::TexRef
H3_getPlayerTex(const H3_Game& game, int playerIndex0_F )
{
    if (playerIndex0_F < 0)
    {
        playerIndex0_F = 0;
    }
    else if (playerIndex0_F > 15)
    {
        playerIndex0_F = 15;
    }

    u32 id = playerIndex0_F + static_cast<u32>(H3_Tex::PlayU);

    return game.getTex( static_cast<H3_Tex::eID>(id), "H3_getPlayerTex");
}
