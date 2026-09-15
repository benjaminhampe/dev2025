#pragma once
#include <FL/Fl_Hold_Browser.H>
#include <FL/fl_draw.H>

#include <de/ScanDirectory.h>
#include <de/image/Image.h>
#include <de/win32/win32_Load_Shell_Icon.h>

class DropList : public Fl_Browser
{
public:
    struct Row
    {
        de::FileInfo fileInfo;
        std::shared_ptr<de::Image> ico;   // RAII-safe shared ownership
        float progress = 0.0f;
    };

    std::vector<Row> rows;

    de::FileInfos getFileInfosA() const;

    typedef void(*FN_onListChange)(void);

    std::vector<FN_onListChange> onListChangeListeners;

    // Drag state
    int drag_index = -1;
    int hover_index = -1;
    bool dragging = false;

    const int scroll_zone = 20;
    const int scroll_speed = 4;

    DropList(int X, int Y, int W, int H, const char* L = nullptr);

    // Add row
    void addRow(std::string uri);

    void emit_onListChanged();

    // Update progress
    void set_progress(int row, float p);

    // -----------------------------
    // Custom row height
    // -----------------------------
    int item_height(void*) const override;

    void sortRows();

    void draw() override;

    // -----------------------------
    // Custom row drawing
    // -----------------------------
    void item_draw(void* v, int X, int Y, int W, int H) const override;

    // -----------------------------
    // FULL EVENT HANDLING
    // -----------------------------
    int handle(int e) override;
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
