#pragma once
#include <vector>
#include <deque>

#include <jansson.h>

#include <rack_common.hpp>
#include <rack_math.hpp>
#include <rack_color.hpp>
#include <plugin/Model.hpp>


namespace rack {


namespace app {
struct ModuleWidget;
struct CableWidget;
} // namespace app


/** Action history for UI undo/redo */
namespace history {

/** An undo action with an inverse redo action.

Pointers to Modules, Params, etc. are not allowed in Actions because the object they refer to may be deleted and restored.
Instead, use moduleIds, etc.
*/
struct RACK_DLL_API Action {
    /** Name of the action, lowercase. Used in the phrase "Undo ..." */
    std::string name;
    virtual ~Action() {}
    virtual void undo() {}
    virtual void redo() {}
};


template <class TAction>
struct InverseAction : TAction {
    void undo() override {
        TAction::redo();
    }
    void redo() override {
        TAction::undo();
    }
};


/** Batches multiple actions into one */
struct RACK_DLL_API ComplexAction : Action {
    /** Ordered by time occurred. Undoing will replay them backwards. */
    std::vector<Action*> actions;
    ~ComplexAction();
    void undo() override;
    void redo() override;
    void push(Action* action);
    bool isEmpty();
};


/** An action operating on a module.
Subclass this to create your own custom actions for your module.
*/
struct RACK_DLL_API ModuleAction : Action {
    int64_t moduleId = -1;
};


struct RACK_DLL_API ModuleAdd : ModuleAction {
    plugin::Model* model = NULL;
    math::Vec pos;
    json_t* moduleJ = NULL;
    ModuleAdd() {
        name = "add module";
    }
    ~ModuleAdd();
    void setModule(app::ModuleWidget* mw);
    void undo() override;
    void redo() override;
};


struct RACK_DLL_API ModuleRemove : InverseAction<ModuleAdd> {
    ModuleRemove() {
        name = "remove module";
    }
};


struct RACK_DLL_API ModuleMove : ModuleAction {
    math::Vec oldPos;
    math::Vec newPos;
    void undo() override;
    void redo() override;
    ModuleMove() {
        name = "move module";
    }
};


struct RACK_DLL_API ModuleBypass : ModuleAction {
    bool bypassed = false;
    void undo() override;
    void redo() override;
    ModuleBypass() {
        name = "bypass module";
    }
};


struct RACK_DLL_API ModuleChange : ModuleAction {
    json_t* oldModuleJ = NULL;
    json_t* newModuleJ = NULL;
    ModuleChange() {
        name = "change module";
    }
    ~ModuleChange();
    void undo() override;
    void redo() override;
};


struct RACK_DLL_API ParamChange : ModuleAction {
    int paramId = -1;
    float oldValue = 0.f;
    float newValue = 0.f;
    void undo() override;
    void redo() override;
    ParamChange() {
        name = "change parameter";
    }
};


struct RACK_DLL_API CableAdd : Action {
    int64_t cableId = -1;
    int64_t inputModuleId = -1;
    int inputId = -1;
    int64_t outputModuleId = -1;
    int outputId = -1;
    NVGcolor color = color::BLACK_TRANSPARENT;
    void setCable(app::CableWidget* cw);
    bool isCable(app::CableWidget* cw) const;
    void undo() override;
    void redo() override;
    CableAdd() {
        name = "add cable";
    }
};


struct RACK_DLL_API CableRemove : InverseAction<CableAdd> {
    CableRemove() {
        name = "remove cable";
    }
};


struct RACK_DLL_API CableColorChange : Action {
    int64_t cableId = -1;
    NVGcolor newColor = color::BLACK_TRANSPARENT;
    NVGcolor oldColor = color::BLACK_TRANSPARENT;
    void setCable(app::CableWidget* cw);
    void undo() override;
    void redo() override;
    CableColorChange() {
        name = "change cable color";
    }
};


struct RACK_DLL_API State {
    struct Internal;
    Internal* internal;

    std::deque<Action*> actions;
    int actionIndex;
    /** Action index of saved patch state. */
    int savedIndex;

    PRIVATE State();
    PRIVATE ~State();
    PRIVATE void clear();
    void push(Action* action);
    void undo();
    void redo();
    bool canUndo();
    bool canRedo();
    std::string getUndoName();
    std::string getRedoName();
    void setSaved();
    bool isSaved();
};


} // namespace history
} // namespace rack
