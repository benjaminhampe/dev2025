#pragma once
#include <H3/H3_Board.h>

void H3_draw2DRect( H3_Game & game, const de::Recti& pos, const uint32_t color = 0xffffffff, const de::gpu::TexRef& tex = de::gpu::TexRef() );
void H3_drawDebugRect( H3_Game & game, const de::Recti& pos, const uint32_t color, const de::Font5x8& font );
void H3_draw2DRoundRect( H3_Game & game, const de::Recti& pos, const glm::ivec2& r, const uint32_t color, int tess = 13 );
void H3_draw2DRoundRect( H3_Game & game, const de::Recti& pos, const glm::ivec2& r, const de::gpu::TexRef& tex, const uint32_t color = 0xffffffff, int tess = 13 );
void H3_drawCircle( H3_Game & game, const de::Recti& pos, const uint32_t color = 0xFFFFFFFF, const de::gpu::TexRef& tex = de::gpu::TexRef(), int num = -1, int tess = 33 );
void H3_drawHexagon(H3_Game & game, const de::Recti& pos, const uint32_t color = 0xFFFFFFFF, const de::gpu::TexRef& tex = de::gpu::TexRef(), int num = -1 );
void H3_drawDice( H3_Game & game, const de::Recti& pos, int r, int dice );
void H3_drawDice( H3_Game & game, const de::Recti& pos, int r, int dice1, int dice2 );

void H3_drawText( H3_Game & game, int x, int y, const std::string& msg, const uint32_t color,
                 const de::Align align, const de::Font5x8 & font,
                 uint32_t bgColor = 0x80000000, int padding = 5);
void H3_drawText( H3_Game & game, int x, int y, const std::string& msg, const uint32_t color,
                 const de::Align align, const de::Font & font,
                uint32_t bgColor = 0x80000000, int padding = 5 );

void UI_Stats_update( H3_Game & game );
void UI_Stats_render( H3_Game & game );
// void UI_Stats_setVisible( H3_Game & game, bool bVisible );
// void UI_Stats_hide( H3_Game & game );
// void UI_Stats_show( H3_Game & game );

void UI_drawDlgLooseCards( H3_Game & game );
void UI_drawRoundStats( H3_Game & game);
void UI_drawStatNum( H3_Game & game, int img, de::Recti pos, int value, int icon_width = 0 );
void UI_drawPlayerStats( H3_Game & game, int px, int py, int playerId );
void UI_drawInfoPanelTile( H3_Game & game, de::Recti pos, const u32 tileId );
void UI_drawInfoPanelCorner( H3_Game & game, de::Recti pos, const u32 cornerId );
void UI_drawInfoPanelEdge( H3_Game & game, de::Recti pos, const u32 edgeId );

