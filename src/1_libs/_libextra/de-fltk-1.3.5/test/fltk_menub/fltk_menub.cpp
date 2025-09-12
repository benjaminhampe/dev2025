// FLTK menu demo

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <sstream>

#ifdef _WIN32

    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif
    #include <windows.h>
    #include <FL/x.H>

    #include "res/resource.h"

    void
    setWindowTitle(HWND m_hWnd, std::string const& title)
    {
        // SendMessage instead of SetText for cases where HWND was created in a different thread
        DWORD_PTR dwResult;
        SendMessageTimeoutA(
            m_hWnd,
            WM_SETTEXT,
            0,
            reinterpret_cast<LPARAM>(title.c_str()),
            SMTO_ABORTIFHUNG,
            2000,
            &dwResult
        );
    }

    void
    setWindowIcon(HWND m_hWnd, int iRessourceID, HINSTANCE m_hInstance = nullptr)
    {
        if (!m_hWnd)
        {
            return;
        }

        if (m_hInstance == nullptr)
        {
            m_hInstance = GetModuleHandle(nullptr);
        }

        HICON hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(iRessourceID));
        if (hIcon)
        {
            LONG_PTR ptr = reinterpret_cast<LONG_PTR>(hIcon);
            SetClassLongPtr(m_hWnd, GCLP_HICON, ptr);
            SetClassLongPtr(m_hWnd, GCLP_HICONSM, ptr);
        }
    }

#endif

// Global pointers for the GUI objects
Fl_Window* mywindow;
Fl_Menu_Bar* mymenubar;
Fl_Menu_Button* mymenubut;
Fl_Choice* mychoice;
Fl_Button* mygetbutton1;
Fl_Button* mygetbutton2;
Fl_Button* mygetbutton3;
Fl_Box* messagebox;

// ===========================================================================
struct StringUtil
// ===========================================================================
{
    template< typename ... T >
    static std::string Join(T const & ... t)
    {
        std::ostringstream o;
        (void)std::initializer_list<int>{ (o << t, 0)... };
        return o.str();
    }
};

//// Convert anything to a C-string, starting with a user-supplied string ("note")
//template<typename T> std::string
//makestr(char* note, const T& i) 
//{
//	std::ostringstream os;
//	os << note << i << std::ends;
//	return os.str();
//}
//

// Callbacks for the main menu - one for each option.
void file_open_cb(Fl_Widget* w, void* data) {
    (void)w;
    (void)data;
	messagebox->label("File Open selected");
}

void file_save_cb(Fl_Widget* w, void* data) {
    (void)w;
    (void)data;
	messagebox->label("File Save selected");
}

void file_print_cb(Fl_Widget* w, void* data) {
    (void)w;
    (void)data;
	messagebox->label("File Print selected");
}

void edit_find_cb(Fl_Widget* w, void* data) {
    (void)w;
    (void)data;
	messagebox->label("Edit Find selected");
}

// Callback for the menu button.  Here (just to show the alternative)
// we use a single callback for all the options, and pass an integer
// via the data parameter to tell it which was chosen.

void compiler_menu_cb(Fl_Widget* w, void* data) {
    (void)w;
    //(void)data;
	messagebox->label(
        StringUtil::Join("Compiler option ", (uint64_t)data).c_str());
}

// Callbacks that are activated by the "get" buttons, but which read the
// latest setting of the Fl_Choice.  We do not bother to give the Fl_Choice
// its own callback, since it does not require any immediate action.
// We have three callbacks here just to show what information can be
// retrieved from the menu - generally you would only need one.

void getchoice_value_cb(Fl_Widget* w, void* data) {
    (void)w;
    (void)data;
	// value() is an integer giving the position in the menu (0=top)
	messagebox->label(
        StringUtil::Join("Meal position ", mychoice->value()).c_str());
}

void getchoice_userdata_cb(Fl_Widget* w, void* data) {
    (void)w;
    (void)data;
    // mvalue() gets the menu item, and user_data() the data we associated with it
	messagebox->label(
        StringUtil::Join("Meal data ", (uint64_t)mychoice->mvalue()->user_data()).c_str());
}

void getchoice_label_cb(Fl_Widget* w, void* data) {
    (void)w;
    (void)data;
    // mvalue()->label() gets the label we gave the menu item
	messagebox->label(
        StringUtil::Join("Meal label ", mychoice->mvalue()->label() ).c_str());
}

// Execution starts here
int main() {
    mywindow = new Fl_Window(400,180,"FLTK menu demos");
	
	// Both Fl_Menu_Bar and Fl_Menu_button are generally used to select
	// some immediate action.  See the FLTK help for menu shortcuts,
	// separators and callback parameters.
	
	// Fl_Menu_Bar is the classic top-of-window drop-down menu
	mymenubar = new Fl_Menu_Bar(0, 0, 400, 25);
	mymenubar->add("File/Open", 0, file_open_cb);
	mymenubar->add("File/Save", 0, file_save_cb);
	mymenubar->add("File/Print", 0, file_print_cb);
	mymenubar->add("Edit/Find", 0, edit_find_cb);
	
	// Fl_Menu_Button is a stand-alone version of a menu. 
	mymenubut = new Fl_Menu_Button(20, 50, 120, 25, "Compiler");
	mymenubut->add("Crash (1)", 0, compiler_menu_cb, (void*)1);
	mymenubut->add("Burn (2)", 0, compiler_menu_cb, (void*)2);
	mymenubut->add("Explode (42)", 0, compiler_menu_cb, (void*)42);
	
	// Fl_Choice is generally used to allow a data item to be selected and
	// displayed for later use.  We could set a callback to be called whenever
	// the value is changed, but in this case we'll wait until a later callback
	// asks the Fl_Choice for its current value	   
	mychoice = new Fl_Choice(130, 100, 100, 25, "Choose meal:");
	mychoice->add("Chicken", 0, 0, (void*)7);
	mychoice->add("Beef", 0, 0, (void*)4);
	mychoice->add("Surprise",0, 0, (void*)8);
	mychoice->value(1); // set the initial choice
	
	mygetbutton1 = new Fl_Button(250, 75, 130, 25, "Get meal position");
	mygetbutton1->callback(getchoice_value_cb);
	mygetbutton2 = new Fl_Button(250, 100, 130, 25, "Get meal data");
	mygetbutton2->callback(getchoice_userdata_cb);
	mygetbutton3 = new Fl_Button(250, 125, 130, 25, "Get meal label");
	mygetbutton3->callback(getchoice_label_cb);
	         	     	       
	messagebox = new Fl_Box(50, 150, 200, 25);

    mywindow->show();  

#ifdef _WIN32
    HWND hWnd = (HWND)fl_xid(mywindow);
    setWindowIcon(hWnd,aaaa);
#endif
    return Fl::run();
}

