#pragma once
#include <gui/Base.h>

class DropList : public Fl_Browser
{
public:
    struct Row {
        std::string uri;
        std::shared_ptr<de::Image> ico;   // RAII-safe shared ownership
        float progress = 0.0f;
        int typ = 0;
    };

    std::vector<Row> rows;

    FileNamesA getFileNamesA() const
    {
        FileNamesA fileNames;
        fileNames.reserve(rows.size());

        for (const auto& row : rows)
        {
            fileNames.push_back(row.uri);
        }
        return fileNames;
    }

    typedef void(*FN_onListChange)(void);

    std::vector<FN_onListChange> onListChangeListeners;

    // Drag state
    int drag_index = -1;
    int hover_index = -1;
    bool dragging = false;

    const int scroll_zone = 20;
    const int scroll_speed = 4;

    DropList(int X, int Y, int W, int H, const char* L = nullptr)
        : Fl_Browser(X, Y, W, H, L) {
        type(FL_MULTI_BROWSER);
    }

    // Add row
    void addRow(std::string uri)
    {
        if (uri.size() > 4096)
        {
            DE_DEBUG("Uri too big")
            return;
        }

        uri = make_posix_path(uri);

        if (uri.empty())
        {
            DE_DEBUG("Empty")
            return;
        }

        // Only add unique paths:
        auto found = std::find_if( rows.begin(), rows.end(),
                [&] (const Row& cached)
                {
                #ifdef _WIN32
                return de::StringUtil::makeLower(cached.uri)
                    == de::StringUtil::makeLower(uri);
                #else
                    return cached.uri == uri;
                #endif
                });
        if (found != rows.end())
        {
            DE_DEBUG("Already added: ", uri)
            return;
        }

        bool bFile = dbExistFile(uri);
        bool bDir = dbExistDirectory(uri);
        if (!bFile && !bDir)
        {
            DE_DEBUG("Not a dir or file: ", uri)
            return;
        }

        int ico_w = item_height(nullptr) - 4;

        Row row;
        row.uri = uri;
        row.ico = load_shell_icon(uri, ico_w, ico_w);
        row.progress = 0.0f;
        row.typ = (bFile) ? 0 : 5;
        rows.emplace_back( std::move(row) );

        sortRows();

        add(uri.c_str());

        DE_OK("Added: ",uri)

        emit_onListChanged();
    }

    void emit_onListChanged()
    {
        for (auto & listener : onListChangeListeners)
        {
            listener();
        }
    }

    // Update progress
    void set_progress(int row, float p) {
        rows[row - 1].progress = std::clamp(p, 0.0f, 1.0f);
        redraw();
    }

    // -----------------------------
    // Custom row height
    // -----------------------------
    int item_height(void*) const override
    {
        //return textsize() + 8;
        return 24 * Fl::screen_scale(0);
    }

    void sortRows()
    {
        std::sort(rows.begin(), rows.end(),
            [](const Row& a, const Row& b)
            {
                if (a.typ != b.typ)
                    return a.typ > b.typ;
#ifdef _WIN32
                return de::StringUtil::makeLower(a.uri)
                     < de::StringUtil::makeLower(b.uri);
#else
                return a.uri < b.uri;
#endif
            });
    }

    void draw() override
    {
        Fl_Browser::draw();

        if (rows.empty())
        {
            fl_color(FL_BLACK);

            int x = this->x();
            int y = this->y();
            int w = this->w();
            int h = this->h();
            auto s = "Drop files and folders here...";
            fl_draw(s, x + 20, y + h/2 + textsize()/2);
        }

    }

    // -----------------------------
    // Custom row drawing
    // -----------------------------
    void item_draw(void* v, int X, int Y, int W, int H) const override
    {
        int index = lineno(v);        // <-- FIXED
        // DE_DEBUG("index = ",index)
        const Row& r = rows.at(index - 1);

        fl_color(selected(index) ? fl_rgb_color(180,200,255) : FL_WHITE);
        fl_rectf(X, Y, W, H);

        int x = X;
        int y = Y;

        if (r.ico)
        {
            Fl_RGB_Image fl_img(r.ico->data(),
                                r.ico->w(),
                                r.ico->h(),
                                r.ico->bytesPerPixel());
            fl_img.draw(x + 2, y + 2);
            x += r.ico->w() + 4;
        }

        fl_color(FL_BLACK);
        fl_draw(r.uri.c_str(), x, y + H/2 + textsize()/2);

        if (r.progress > 0.0f)
        {
            int bar_x = X + 32;
            int bar_y = Y + H - 6;
            int bar_w = W - 40;
            int bar_h = 4;

            fl_color(fl_rgb_color(200,200,200));
            fl_rectf(bar_x, bar_y, bar_w, bar_h);

            fl_color(fl_rgb_color(0,160,0));
            fl_rectf(bar_x, bar_y, int(bar_w * r.progress), bar_h);

            fl_color(fl_rgb_color(80,80,80));
            fl_rect(bar_x, bar_y, bar_w, bar_h);
        }
    }

    // -----------------------------
    // FULL EVENT HANDLING
    // -----------------------------
    int handle(int e) override {
        switch (e) {

        // -----------------------------
        // Drag & Drop (file drop)
        // -----------------------------
        case FL_DND_ENTER:
        case FL_DND_DRAG:
        case FL_DND_RELEASE:
            return 1;

        case FL_PASTE:
        {
            const char* t = Fl::event_text();
            if (t)
            {
                std::string s(t);
                std::stringstream ss(s);
                std::string line;

                while (std::getline(ss, line))
                {
                    addRow(line);
                }
            }
            return 1;
        }

        // -----------------------------
        // Keyboard → delete selected rows
        // -----------------------------
        case FL_KEYDOWN: {
            if (Fl::event_key() == FL_Delete)
            {
                bool bRemovedSomething = false;
                for (int i = size(); i >= 1; --i)
                {
                    if (selected(i))
                    {
                        rows.erase(rows.begin() + (i - 1));
                        remove(i);

                        bRemovedSomething = true;
                    }
                }

                if (bRemovedSomething)
                {
                    emit_onListChanged();
                }
                return 1;
            }
            break;
        }
/*
        // -----------------------------
        // Mouse push → begin drag
        // -----------------------------
        case FL_PUSH: {
            //int idx = find_line(Fl::event_y());
            int idx = lineno(find_line(Fl::event_y()));
            if (idx >= 1) {
                drag_index = idx;
                dragging = true;
            }
            return 1;
        }

        // -----------------------------
        // Mouse drag → reorder + auto-scroll
        // -----------------------------
        case FL_DRAG: {
            if (dragging && drag_index >= 1) {
                //hover_index = find_line(Fl::event_y());
                hover_index = lineno(find_line(Fl::event_y()));

                if (hover_index >= 1)
                    select(hover_index, 1);

                int my = Fl::event_y();

                // Auto-scroll up
                if (my < y() + scroll_zone)
                    position(position() - scroll_speed);

                // Auto-scroll down
                if (my > y() + h() - scroll_zone)
                    position(position() + scroll_speed);

                return 1;
            }
            break;
        }

        // -----------------------------
        // Mouse release → finalize reorder
        // -----------------------------
        case FL_RELEASE: {
            if (dragging && drag_index >= 1) {
                //int index = lineno(v);   // 1-based index
                //int drop_index = find_line(Fl::event_y());
                int drop_index = lineno(find_line(Fl::event_y()));


                if (drop_index >= 1 && drop_index != drag_index) {
                    Row r = rows[drag_index - 1];

                    rows.erase(rows.begin() + (drag_index - 1));
                    rows.insert(rows.begin() + (drop_index - 1), r);

                    // Rebuild browser text list
                    clear();
                    for (auto& row : rows)
                        add(row.uri.c_str());
                }
            }

            dragging = false;
            drag_index = -1;
            hover_index = -1;
            return 1;
        }
*/

        default: break;
        }

        return Fl_Browser::handle(e);
    }


};

/*
class DropList : public Fl_Hold_Browser
{
public:
    DropList(int X, int Y, int W, int H, const char *L = 0)
        : Fl_Hold_Browser(X, Y, W, H, L)
    {
        type(FL_MULTI_BROWSER); // allow multi-selection
    }

    void add_dropped_path(const std::string& path)
    {
        Fl_Image* icon = nullptr;

    #ifdef _WIN32
        icon = load_shell_icon(path);
    #elif __APPLE__
        icon = load_nsimage_icon(path);
    #else
        icon = is_directory(path)
            ? load_xdg_icon("folder")
            : load_xdg_icon("text-x-generic");
    #endif

        add(icon, path.c_str());
    }

    int handle(int e) override
    {
        switch (e)
        {
            case FL_DND_ENTER:
            case FL_DND_DRAG:
            case FL_DND_RELEASE:
                return 1;

            case FL_KEYDOWN:
            {
                int key = Fl::event_key();
                if (key == FL_Delete)
                {
                    for (int i = size(); i >= 1; --i)
                        if (selected(i))
                            remove(i);
                    return 1;
                }
                break;
            }

            case FL_PASTE:
            {
                const char *t = Fl::event_text();
                if (!t) return 1;

                // FLTK gives dropped paths separated by newlines
                std::string s(t);
                size_t pos = 0;
                while ((pos = s.find('\n')) != std::string::npos)
                {
                    std::string path = s.substr(0, pos);
                    if (!path.empty())
                        // add(path.c_str());
                        add(load_shell_icon(path), path.c_str());
                    s.erase(0, pos + 1);
                }
                if (!s.empty())
                    add(s.c_str());

                return 1;
            }
        }
        return Fl_Hold_Browser::handle(e);
    }
};

int main() {
    Fl_Window win(400, 300, "Drop List");
    DropList list(10, 10, 380, 280);
    win.end();
    win.show();
    return Fl::run();
}
*/
