#pragma once
#include <H3/H3_Board.h>

// ===========================================================================
class UI_WorldCreator : public de::gui::Widget
// ===========================================================================
{
    typedef glm::vec2 V2;
    typedef glm::vec3 V3;

public:
    UI_WorldCreator( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& r );
    ~UI_WorldCreator() override;
    void draw() override;
    void resize(int x, int y, int w, int h) override;
    void onEvent(const de::Event& event) override;

    // Final config:

    H3_Cfg getCfg();
    void setCfg( const H3_Cfg& cfg );

    // PresetManager:

/*
    void setPreset( const H3_Cfg& cfg );
    H3_Cfg& getPreset();
    H3_Cfg& getPreset( const std::string& presetName );
    const std::string& getPresetName() const;
*/
protected:
    de::gui::Button* createButton(std::string txt);
    de::gui::IntField* createIntField(std::string txt, int val, int min, int max);
    void drawBoard(const de::Recti & rect);

    void resizeBoard(int minX, int minY, int maxX, int maxY);
    void resizeBoard();

    void draw2DHexOutline(int i,int j,
        uint32_t color, int width, const V2& offset, const V2& size);
private:
    H3_Game* m_game;
    int m_mouseX;
    int m_mouseY;
    int m_hoveredTileId;
    int m_selectedTileId;

    de::Recti m_rcBoard;
    de::Pos2i m_relativePosToBoard;
    bool m_bHoveredBoard;
    //float m_tileSize = 100.f;
    float m_scale;
    float m_off_x;
    float m_off_y;
    std::vector< H3_CfgTile > m_tiles;

    H3_Cfg m_cfg; // Current preset
    std::unordered_map<std::string, H3_Cfg> m_presets;

    de::gpu::TexRef m_backgroundTex;

    de::gui::Label* m_title;
    //de::gui::Label* m_presetLabel;
    de::gui::Button* m_presetPrev;
    de::gui::Button* m_presetNext;
    de::gui::Label*  m_presetName;
    de::gui::Button* m_presetNew;
    de::gui::Button* m_presetClone;
    de::gui::Button* m_presetDelete;
    de::gui::Button* m_presetExport;
    de::gui::Button* m_presetImport;
    //de::gui::Button* m_presetEdit;

    de::gui::IntField* m_edtMinX;
    de::gui::IntField* m_edtMaxX;
    de::gui::IntField* m_edtMinY;
    de::gui::IntField* m_edtMaxY;
    de::gui::IntField* m_edtVictoryPoints;
    de::gui::IntField* m_edtCardLimit;
    de::gui::IntField* m_edtBuildingSpacing;
    de::gui::IntField* m_edtSkipFirstRound;
    de::gui::IntField* m_edtNumPlayers;

    de::gui::Button* m_btnClose;
    de::gui::Button* m_btnStart;
};
