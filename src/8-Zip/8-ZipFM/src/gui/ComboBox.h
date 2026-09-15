#pragma once
#include <de/Core.h>

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Menu_Window.H>

// ===============================
// Win11 Colors
// ===============================
/*
    static Fl_Color WIN11_BORDER_NORMAL    = fl_rgb_color(200, 200, 200);
    static Fl_Color WIN11_BORDER_HOVER     = fl_rgb_color(0, 120, 215);
    static Fl_Color WIN11_BORDER_FOCUS     = fl_rgb_color(0, 120, 215);

    static Fl_Color WIN11_FILL_NORMAL      = fl_rgb_color(255, 255, 255);
    static Fl_Color WIN11_FILL_HOVER       = fl_rgb_color(245, 248, 255);

    static Fl_Color WIN11_TEXT_NORMAL      = fl_rgb_color(0, 0, 0);
    static Fl_Color WIN11_TEXT_SELECTED    = fl_rgb_color(255, 255, 255);

    static Fl_Color WIN11_POPUP_FILL       = fl_rgb_color(255, 255, 255);
    static Fl_Color WIN11_POPUP_HOVER      = fl_rgb_color(240, 244, 255);
    static Fl_Color WIN11_POPUP_SELECTED   = fl_rgb_color(0, 120, 215);
*/

#include <variant>
#include <string>


class ComboBox : public Fl_Choice
{
public:
    ComboBox(int X, int Y, int W, int H, const char *L = 0)
        : Fl_Choice(X,Y,W,H,L)
    {
    }
/*
    void draw() override
    {
        fl_push_clip(x(), y(), w(), h());

        int w2 = std::min( w(), h() );
        int w1 = w() - w2;

        Fl_Color fillColor = fl_rgb_color(224, 238, 249);
        fl_color(fillColor);
        fl_rectf(x(), y(), w1, h());

        // Draw menu item's label
        if (mvalue())
        {
            Fl_Menu_Item m = *mvalue();
            //if (active) m.activate(); else m.deactivate();

            if (m.text)
            {
                fl_color(FL_BLACK);
                fl_draw(m.text, x() + 4, y() + h() - 6);
            }
        }
        // draw arrow
        //fl_draw("@2", x() + w() - 16, y() + 2);

        fl_pop_clip();
    }
*/
    struct Item
    {
        typedef std::variant<int, std::string> UserData;

        std::string dispText;
        UserData userData;

        Item()
            : dispText{}
            , userData{ 0 } // { std::in_place_type<int>, -1 }
        {}

        Item( std::string label, UserData userDat )
            : dispText{ label }
            , userData{ userDat }
        {}

        int toInt(bool* ok = nullptr) const
        {
            if (std::holds_alternative<int>(userData))
            {
                if (ok) *ok = true;
                return std::get<int>(userData);
            }
            else
            {
                DE_ERROR("No int data")
                if (ok) *ok = false;
                return 0;
            }
        }

        std::string toString(bool* ok = nullptr) const
        {
            if (std::holds_alternative<std::string>(userData))
            {
                if (ok) *ok = true;
                return std::get<std::string>(userData);
            }
            else
            {
                DE_ERROR("No string data")
                if (ok) *ok = false;
                return {};
            }
        }
    };

    std::vector<Item> m_items;

    typedef std::function<void(int /* idx */, ComboBox*)> FN_onChange;

    FN_onChange onChange;

    void addItem(std::string label, Item::UserData userData = 0)
    {
        add(label.c_str());
        m_items.emplace_back( label, userData );
    }

    int currentIndex() const { return value(); }

    void setCurrentIndex(int index, bool bEmitChange)
    {
        const bool bChanged = index != currentIndex();

        if (!bChanged)
        {
            return; // Nothing todo.
        }

        value( index );

        // const auto& items = m_items;

        // if (m_currentIndex < 0 || m_currentIndex >= int(items.size()))
        // {
        //     m_currentText = "";
        // }
        // else
        // {
        //     m_currentText = items[m_currentIndex].dispText;
        // }

        if (bEmitChange)
        {
            if (onChange)
            {
                onChange(currentIndex(), this);
            }
            else
            {
                DE_ERROR("No onChange")
            }
        }

        // redraw();
    }

    Item currentData() const
    {
        const auto& items = m_items;

        int index = currentIndex();
        if (index < 0 || index >= int(items.size()))
        {
            return {};
        }

        return items[ index ];
    }

    std::string currentText() const
    {
        const auto& items = m_items;

        int index = currentIndex();
        if (index < 0 || index >= int(items.size()))
        {
            return {};
        }

        return items[ index ].dispText;
    }

};




#if 0


class ComboBox : public Fl_Widget
{
public:
    struct Item
    {
        typedef std::variant<int, std::string> UserData;

        std::string dispText;
        UserData userData;

        Item()
            : dispText{}
            , userData{ 0 } // { std::in_place_type<int>, -1 }
        {}

        Item( std::string label, UserData userDat )
            : dispText{ label }
            , userData{ userDat }
        {}

        int toInt(bool* ok = nullptr) const
        {
            if (std::holds_alternative<int>(userData))
            {
                if (ok) *ok = true;
                return std::get<int>(userData);
            }
            else
            {
                DE_ERROR("No int data")
                if (ok) *ok = false;
                return 0;
            }
        }

        std::string toString(bool* ok = nullptr) const
        {
            if (std::holds_alternative<std::string>(userData))
            {
                if (ok) *ok = true;
                return std::get<std::string>(userData);
            }
            else
            {
                DE_ERROR("No string data")
                if (ok) *ok = false;
                return {};
            }
        }
    };

    // ============================
    class Popup : public Fl_Menu_Window
    // ============================
    {
    public:
        ComboBox* m_owner = nullptr;

        bool m_bHovered;

        int m_lineHeight;

        int m_hoverIndex;

        Popup(int X, int Y, int W, int H, ComboBox* o)
            : Fl_Menu_Window(X, Y, W, H)
            , m_owner(o)
            , m_bHovered{ false }
            , m_lineHeight{ int(22 * Fl::screen_scale(0)) }
            , m_hoverIndex{ -1 }
        {
            clear_border();
            DE_DEBUG("rect(",X,",",Y,",",W,",",H,")")
        }

        ~Popup() override
        {
            DE_DEBUG("Died")
        }

        void draw() override
        {
            DE_DEBUG("rect(",x(),",",y(),",",w(),",",h(),"), "
                    "m_hoverIndex(",m_hoverIndex,")")

            fl_color(FL_WHITE);
            fl_rectf(x(), y(), w(), h());

            m_lineHeight = 22 * Fl::screen_scale(0);

            int cx = x();
            int cy = y();

            for (size_t i = 0; i < m_owner->m_items.size(); ++i)
            {
                const auto& item = m_owner->m_items[i];

                if (int(i) == m_hoverIndex)
                {
                    Fl_Color cellbg = fl_rgb_color(240, 244, 255);
                    fl_color(cellbg);
                    fl_rectf(cx, cy, w(), m_lineHeight);
                }

                int tx = cx + 4;
                int ty = cy + 16;
                fl_color(FL_BLACK);
                fl_draw(item.dispText.c_str(), tx, ty);
                cy += m_lineHeight;
            }

            #if 0
            Fl_Color WIN11_POPUP_FILL       = fl_rgb_color(255, 255, 255);
            Fl_Color WIN11_POPUP_HOVER      = fl_rgb_color(240, 244, 255);
            Fl_Color WIN11_POPUP_SELECTED   = fl_rgb_color(0, 120, 215);
            Fl_Color WIN11_TEXT_NORMAL      = fl_rgb_color(0, 0, 0);
            Fl_Color WIN11_TEXT_SELECTED    = fl_rgb_color(255, 255, 255);

            fl_push_clip(0, 0, w(), h());
            fl_color(WIN11_POPUP_FILL);
            fl_rectf(0, 0, w(), h());

            fl_font(FL_HELVETICA, 14);
            int lh = fl_height() + 8;

            for (int i = 0; i < m_items.size(); ++i)
            {
                int y = i * lh;

                Fl_Color fill = WIN11_POPUP_FILL;
                Fl_Color text = WIN11_TEXT_NORMAL;

                if (i == m_hoverIndex)
                {
                    fill = WIN11_POPUP_HOVER;
                }

                if (i == m_selectedIndex)
                {
                    fill = WIN11_POPUP_SELECTED;
                    text = WIN11_TEXT_SELECTED;
                }

                fl_color(fill);
                fl_rectf(0, y, w(), lh);

                fl_color(text);
                fl_draw(m_items[i].dispText.c_str(), 8, y + lh - 6);
            }

            fl_pop_clip();
            #endif
        }

        int handle(int e) override
        {
            switch (e)
            {
                case FL_MOVE:
                {
                    int hoverIndex = m_hoverIndex;
                    int row = Fl::event_y() / m_lineHeight;
                    if (row >= 0 && row < (int)m_owner->m_items.size())
                    {
                        m_hoverIndex = row;
                    }
                    else
                    {
                        m_hoverIndex = -1;
                    }

                    redraw();
                    break;
                }
                case FL_ENTER:
                {
                    m_bHovered = true;
                    redraw();
                    break;
                }
                case FL_LEAVE:
                {
                    m_bHovered = false;
                    redraw();
                    break;
                }
                case FL_PUSH:
                {
                    int row = Fl::event_y() / m_lineHeight;
                    if (row >= 0 && row < (int)m_owner->m_items.size())
                    {
                        m_hoverIndex = row;
                    }
                    else
                    {
                        m_hoverIndex = -1;
                    }
                    m_owner->setCurrentIndex(m_hoverIndex, true);
                    m_owner->m_popup = nullptr;
                    //hide();
                    delete this;
                    return 1;
                }
            }

            return Fl_Window::handle(e);
        }
    };

    // ============================
    // ComboBox: Members
    // ============================
    typedef std::function<void(int /* idx */, ComboBox*)> FN_onChange;

    FN_onChange onChange;

    bool m_bHovered;

    int m_currentIndex;

    std::string m_currentText;

    std::vector<Item> m_items;

    Popup* m_popup;

    // =====================================
    ComboBox(int X, int Y, int W, int H)
    // =====================================
        : Fl_Widget(X, Y, W, H)
        , m_bHovered{ false }
        , m_currentIndex{ -1 }
        , m_popup{ nullptr }
    {}

    int handle(int e) override
    {
        switch (e)
        {
        case FL_ENTER:
            m_bHovered = true;
            redraw();
            break;
        case FL_LEAVE:
            m_bHovered = false;
            redraw();
            break;
        case FL_PUSH:
            open_popup();
            redraw();
            break;
        }

        return Fl_Widget::handle(e);
    }

    void draw() override
    {
        fl_draw_box(FL_DOWN_BOX, x(), y(), w(), h(), FL_WHITE);

        if (m_currentText.size())
        {
            fl_color(FL_BLACK);
            fl_draw(m_currentText.c_str(), x()+4, y()+h()-6);
        }

#if 0
        fl_push_clip(x(), y(), w(), h());

        int w2 = std::min( w(), h() );
        int w1 = w() - w2;

        Fl_Color fillColor = fl_rgb_color(224, 238, 249);
        fl_color(fillColor);
        fl_rectf(x(), y(), w1, h());

        if (m_currentText.size())
        {
            fl_color(FL_BLACK);
            fl_draw(m_currentText.c_str(), x() + 4, y() + h() - 6);
        }

        // draw arrow
        //fl_draw("@2", x() + w() - 16, y() + 2);

/*
        // drawButton()
        Fl_Color fill = m_hover ? WIN11_FILL_HOVER : WIN11_FILL_NORMAL;

        fl_color(fill);
        fl_rectf(x(), y(), w(), h());

        fl_color(WIN11_BORDER_NORMAL);
        fl_rect(x(), y(), w(), h());

        fl_color(WIN11_TEXT_NORMAL);
        int cx = x() + w()/2;
        int cy = y() + h()/2;
        fl_polygon(cx-4, cy-2, cx+4, cy-2, cx, cy+4);
*/
        // fl_pop_clip();
#endif
    }

    void open_popup()
    {
        int item_h = 22;
        int H = item_h * m_items.size();

        if (!m_popup)
        {
            m_popup = new Popup(x(), y()+5+h(), w(), 400, this);
            m_popup->show();
        }
        //Fl::grab(popup);
    }


    void add(std::string label, Item::UserData userData = 0)
    {
        m_items.emplace_back( label, userData );
    }

    int currentIndex() const { return m_currentIndex; }

    void setCurrentIndex(int index, bool bEmitChange)
    {
        const bool bChanged = index != m_currentIndex;

        if (!bChanged)
        {
            return; // Nothing todo.
        }

        const auto& items = m_items;

        m_currentIndex = index;

        if (m_currentIndex < 0 || m_currentIndex >= int(items.size()))
        {
            m_currentText = "";
        }
        else
        {
            m_currentText = items[m_currentIndex].dispText;
        }

        if (bEmitChange)
        {
            if (onChange)
            {
                onChange(m_currentIndex, this);
            }
            else
            {
                DE_ERROR("No onChange")
            }
        }

        redraw();
    }

    Item currentData() const
    {
        const auto& items = m_items;

        if (m_currentIndex < 0 || m_currentIndex >= int(items.size()))
        {
            return {};
        }

        return items[ m_currentIndex ];
    }

    std::string currentText() const
    {
        const auto& items = m_items;

        if (m_currentIndex < 0 || m_currentIndex >= int(items.size()))
        {
            return {};
        }

        return items[ m_currentIndex ].dispText;
    }

};

struct ComboBox_Item
{
    typedef std::variant<int, std::string> UserData;

    std::string dispText;
    UserData userData;

    ComboBox_Item()
        : dispText{}
        , userData{ 0 } // { std::in_place_type<int>, -1 }
    {}

    ComboBox_Item( std::string label, UserData userDat )
        : dispText{ label }
        , userData{ userDat }
    {}

    int toInt(bool* ok = nullptr) const
    {
        if (std::holds_alternative<int>(userData))
        {
            if (ok) *ok = true;
            return std::get<int>(userData);
        }
        else
        {
            DE_ERROR("No int data")
            if (ok) *ok = false;
            return 0;
        }
    }

    std::string toString(bool* ok = nullptr) const
    {
        if (std::holds_alternative<std::string>(userData))
        {
            if (ok) *ok = true;
            return std::get<std::string>(userData);
        }
        else
        {
            DE_ERROR("No string data")
            if (ok) *ok = false;
            return {};
        }
    }
};


// ===========================================
class ComboBox_Popup : public Fl_Menu_Window
// ===========================================
{
public:
    std::vector<ComboBox_Item> m_items;
    int m_hoverIndex = -1;
    int m_selectedIndex = -1;

    typedef std::function<void(int, void*)> FN_onSelect;

    FN_onSelect onSelect = nullptr;

    ComboBox_Popup(int X, int Y, int W, int H)
        : Fl_Menu_Window(X,Y,W,H)
    {
        clear_border();
    }

    void move(int X, int Y)
    {
        DE_DEBUG("x(",X,"), y(",Y,")")
        resize(X,Y,w(),h());
    }

    void draw() override
    {
        Fl_Color WIN11_POPUP_FILL       = fl_rgb_color(255, 255, 255);
        Fl_Color WIN11_POPUP_HOVER      = fl_rgb_color(240, 244, 255);
        Fl_Color WIN11_POPUP_SELECTED   = fl_rgb_color(0, 120, 215);
        Fl_Color WIN11_TEXT_NORMAL      = fl_rgb_color(0, 0, 0);
        Fl_Color WIN11_TEXT_SELECTED    = fl_rgb_color(255, 255, 255);

        fl_push_clip(0, 0, w(), h());
        fl_color(WIN11_POPUP_FILL);
        fl_rectf(0, 0, w(), h());

        fl_font(FL_HELVETICA, 14);
        int lh = fl_height() + 8;

        for (int i = 0; i < m_items.size(); ++i)
        {
            int y = i * lh;

            Fl_Color fill = WIN11_POPUP_FILL;
            Fl_Color text = WIN11_TEXT_NORMAL;

            if (i == m_hoverIndex)
            {
                fill = WIN11_POPUP_HOVER;
            }

            if (i == m_selectedIndex)
            {
                fill = WIN11_POPUP_SELECTED;
                text = WIN11_TEXT_SELECTED;
            }

            fl_color(fill);
            fl_rectf(0, y, w(), lh);

            fl_color(text);
            fl_draw(m_items[i].dispText.c_str(), 8, y + lh - 6);
        }

        fl_pop_clip();
    }

    int handle(int e) override
    {
        fl_font(FL_HELVETICA, 14);
        int lh = fl_height() + 8;

        switch (e) {
        case FL_MOVE:
        case FL_DRAG: {
            int idx = Fl::event_y() / lh;
            m_hoverIndex = (idx >= 0 && idx < m_items.size()) ? idx : -1;
            redraw();
            return 1;
        }
        case FL_PUSH: {
            int idx = Fl::event_y() / lh;
            if (idx >= 0 && idx < m_items.size()) {
                m_selectedIndex = idx;
                redraw();
            }
            return 1;
        }

        case FL_RELEASE: {
            int idx = Fl::event_y() / lh;
            if (idx >= 0 && idx < m_items.size())
            {
                m_selectedIndex = idx;
                if (onSelect)
                {
                    onSelect(idx, this);
                }
            }
            Fl::grab(0);
            hide();
            return 1;
        }
        }

        return Fl_Menu_Window::handle(e);
    }
};

// ===============================
// Arrow Button
// ===============================
class ComboBox : public Fl_Widget
{
public:

    bool m_hover;

    ComboBox_Popup* m_popup;

    typedef std::function<void()> FN_onPress;

    FN_onPress onPress;

    typedef std::function<void(int /* idx */, ComboBox*)> FN_onChange;

    FN_onChange onChange;

    int m_currentIndex = -1;

    std::string m_currentText;

    ComboBox(int X, int Y, int W, int H)
        : Fl_Widget(X, Y, W, H)
        , m_hover{ false }
        , m_popup{ nullptr }
        , onChange{ nullptr }
        , m_currentText{}
    {
        m_popup = new ComboBox_Popup(X, Y + H, 1.5f * W, 400);

        onPress =
            [this]()
            {
                int px = x();
                int py = y() + h();

                m_popup = new ComboBox_Popup(px, py, 1.5f * w(), 400);
                //m_popup->move(px,py);
                m_popup->show();
            };

        m_popup->onSelect =
            [this] (int idx, void* userData)
            {
                setCurrentIndex(idx, true);
            };
    }

    void add(std::string label, ComboBox_Item::UserData userData = 0)
    {
        m_popup->m_items.emplace_back( label, userData );
    }

    int currentIndex() const { return m_currentIndex; }

    void setCurrentIndex(int index, bool bEmitChange)
    {
        const bool bChanged = index != m_currentIndex;

        if (!bChanged)
        {
            return; // Nothing todo.
        }

        const auto& items = m_popup->m_items;

        m_currentIndex = index;

        if (m_currentIndex < 0 || m_currentIndex >= int(items.size()))
        {
            m_currentText = "";
        }
        else
        {
            m_currentText = items[m_currentIndex].dispText;
        }

        if (bEmitChange)
        {
            if (onChange)
            {
                onChange(m_currentIndex, this);
            }
            else
            {
                DE_ERROR("No onChange")
            }
        }

        redraw();
    }

    ComboBox_Item currentData() const
    {
        const auto& items = m_popup->m_items;

        if (m_currentIndex < 0 || m_currentIndex >= int(items.size()))
        {
            return {};
        }

        return items[ m_currentIndex ];
    }

    std::string currentText() const
    {
        const auto& items = m_popup->m_items;

        if (m_currentIndex < 0 || m_currentIndex >= int(items.size()))
        {
            return {};
        }

        return items[ m_currentIndex ].dispText;
    }

    void draw() override
    {
        fl_push_clip(x(), y(), w(), h());

        int w2 = std::min( w(), h() );
        int w1 = w() - w2;

        Fl_Color fillColor = fl_rgb_color(224, 238, 249);
        fl_color(fillColor);
        fl_rectf(x(), y(), w1, h());

        if (m_currentText.size())
        {
            fl_color(FL_BLACK);
            fl_draw(m_currentText.c_str(), x() + 4, y() + h() - 6);
        }

        // draw arrow
        //fl_draw("@2", x() + w() - 16, y() + 2);

/*
        // drawButton()
        Fl_Color fill = m_hover ? WIN11_FILL_HOVER : WIN11_FILL_NORMAL;

        fl_color(fill);
        fl_rectf(x(), y(), w(), h());

        fl_color(WIN11_BORDER_NORMAL);
        fl_rect(x(), y(), w(), h());

        fl_color(WIN11_TEXT_NORMAL);
        int cx = x() + w()/2;
        int cy = y() + h()/2;
        fl_polygon(cx-4, cy-2, cx+4, cy-2, cx, cy+4);
*/
        // fl_pop_clip();
    }

    int handle(int e) override
    {
        switch (e)
        {
        case FL_ENTER:
            m_hover = true;
            redraw();
            break;
        case FL_LEAVE:
            m_hover = false;
            redraw();
            break;
        case FL_PUSH:
            if (onPress)
                onPress();
            redraw();
            break;
        }

        return Fl_Widget::handle(e);
    }
};

class ComboBox : public Fl_Widget {
public:
    std::vector<std::string> items;
    std::string value;

    ComboBox(int X, int Y, int W, int H)
        : Fl_Widget(X, Y, W, H) {}

    int handle(int e) override {
        if (e == FL_PUSH) {
            open_popup();
            return 1;
        }
        return 0;
    }

    void draw() override {
        fl_draw_box(FL_DOWN_BOX, x(), y(), w(), h(), FL_WHITE);
        fl_color(FL_BLACK);
        fl_draw(value.c_str(), x()+4, y()+h()-6);
    }

    void open_popup() {
        int item_h = 22;
        int H = item_h * items.size();

        Fl_Menu_Window* popup = new Fl_Menu_Window(w(), H);
        popup->clear_border();
        popup->begin();
        popup->end();

        popup->resize(x(), y()+h(), w(), H);
        popup->show();
        Fl::grab(popup);

        popup->callback([](Fl_Widget* w, void* v){
            ComboBox* self = (ComboBox*)v;

            int my = Fl::event_y() - w->y();
            int idx = my / 22;

            if (idx >= 0 && idx < (int)self->items.size()) {
                self->value = self->items[idx];
                self->redraw();
            }

            Fl::grab(0);
            w->hide();
            delete w;
        }, this);

        Fl::add_timeout(0.01, [](void* v){
            ((ComboBox*)v)->draw_popup();
        }, this);
    }

    void draw_popup() {
        Fl_Menu_Window* popup = (Fl_Menu_Window*)Fl::grab();
        if (!popup) return;

        popup->make_current();
        fl_color(FL_WHITE);
        fl_rectf(0, 0, popup->w(), popup->h());

        fl_color(FL_BLACK);
        int y = 0;
        for (auto& s : items) {
            fl_draw(s.c_str(), 4, y + 16);
            y += 22;
        }

        popup->redraw();
    }
};

//======================================================
class ComboBox : public Fl_Group {
public:
    Fl_Input* input;
    Fl_Button* arrow;
    std::vector<std::string> items;

    ComboBox(int X, int Y, int W, int H)
        : Fl_Group(X, Y, W, H)
    {
        box(FL_NO_BOX);

        input = new Fl_Input(X, Y, W - H, H);
        arrow = new Fl_Button(X + W - H, Y, H, H, "@2");

        arrow->callback([](Fl_Widget* w, void* v){
            ((ComboBox*)v)->open_popup();
        }, this);

        end();
    }

    void set_items(const std::vector<std::string>& list) {
        items = list;
    }

    void open_popup() {
        // Popup jedes Mal neu erzeugen
        auto* popup = new Fl_Menu_Window(w(), items.size() * 22);
        popup->clear_border();
        popup->begin();
        popup->end();

        int X = arrow->x();
        int Y = arrow->y() + arrow->h();
        popup->resize(X, Y, w(), items.size() * 22);

        popup->show();
        Fl::grab(popup);

        popup->callback([](Fl_Widget* w, void* v){
            auto* self = (ComboBox*)v;
            int my = Fl::event_y() - w->y();
            int idx = my / 22;

            if (idx >= 0 && idx < (int)self->items.size()) {
                self->input->value(self->items[idx].c_str());
            }

            Fl::grab(0);
            w->hide();
            delete w; // Popup zerstören
        }, this);

        Fl::add_timeout(0.01, [](void* v){
            ((ComboBox*)v)->draw_popup();
        }, this);
    }

    void draw_popup() {
        Fl_Menu_Window* popup = (Fl_Menu_Window*)Fl::grab();
        if (!popup) return;

        popup->make_current();
        fl_color(FL_WHITE);
        fl_rectf(0, 0, popup->w(), popup->h());

        fl_color(FL_BLACK);
        int y = 0;
        for (auto& s : items) {
            fl_draw(s.c_str(), 4, y + 16);
            y += 22;
        }

        popup->redraw();
    }
};

#endif
