#pragma once
#include <H3/H3_Board.h>

// ===========================================================================
class UI_World : public de::gui::Widget
// ===========================================================================
{
public:
    UI_World( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& r );
    ~UI_World() override;
    void draw() override;
    void onEvent(const de::Event& event) override;
    void resizeLayout();
    void setPreset( const H3_Cfg& cfg );
    H3_Cfg& getPreset();
    // H3_Cfg& getPreset( const std::string& presetName );
    // const std::string& getPresetName() const;
protected:
private:
    H3_Game* m_game;
    float m_tileSize = 100.f;
    float m_scale = 1.f;
    float m_off_x = 0.f;
    float m_off_y = 0.f;

    H3_Cfg m_cfg;
    // de::Recti m_tileBounds;
};


// ===========================================================================
class UI_WorldCreator : public de::gui::Widget
// ===========================================================================
{
public:
    UI_WorldCreator( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& r );
    ~UI_WorldCreator() override;
    void draw() override;
    //void onEvent(const de::Event& event) override;
    void resizeLayout();
    void setPreset( const H3_Cfg& cfg );
    H3_Cfg& getPreset();
    H3_Cfg& getPreset( const std::string& presetName );
    const std::string& getPresetName() const;
protected:
    de::gui::Button* createButton(std::string txt, de::gpu::TexRef ref, de::Recti pos);
    de::gui::IntField* createIntField(std::string txt, int val, int min, int max);
    void calcTileBounds();
private:
    H3_Game* m_game;
    de::gui::Label* m_title;
    //de::gui::Label* m_presetLabel;
    de::gui::Button* m_presetPrev;
    de::gui::Button* m_presetNext;
    de::gui::Label* m_presetName;
    de::gui::Button* m_presetNew;
    de::gui::Button* m_presetClone;
    de::gui::Button* m_presetDelete;
    de::gui::Button* m_presetExport;
    de::gui::Button* m_presetImport;
    //de::gui::Button* m_presetEdit;
    //de::gui::IntField* m_edtTilesX;
    //de::gui::IntField* m_edtTilesY;
    de::gui::IntField* m_edtVictory;
    de::gui::IntField* m_edtCardLimit;
    de::gui::IntField* m_edtSpacing;
    de::gui::IntField* m_edtPlayer;
    de::gui::IntField* m_edt;
    de::gui::Button* m_btnClose;
    de::gui::Button* m_btnStart;

    UI_World* m_body;

    float m_tileSize = 100.f;

    std::unordered_map<std::string, H3_Cfg> m_presets;

    de::Recti m_tileBounds;
};
