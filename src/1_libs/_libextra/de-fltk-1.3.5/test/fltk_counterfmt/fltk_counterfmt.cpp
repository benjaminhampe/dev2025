// Custom formatting of text representation of Fl_Counter

// The text representation of the current value of an Fl_Counter, and all other
// valuators, is normally determined by their step() setting.  To force your own
// representation, derive a custom version of the Fl_Counter, and override the
// format() member function (as documented in the Fl_Valuator manual page).

// The "%6.3f" argument to snprintf shown here controls the formatting: in this
// case, a minimum of 6 characters, with 3 decimal digits.  Any format compatible
// with a double may be used; see, for example
// http://www.cppreference.com/wiki/c/io/printf

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Counter.H>
#include <stdio.h>  // for snprintf

class CustomCounter : public Fl_Counter {
public:
	CustomCounter(int X, int Y, int W, int H, const char* L=0): Fl_Counter(X,Y,W,H,L) {}
	int format(char* buffer) { return snprintf(buffer, 128, "%6.3f", value());}
};


Fl_Window* mainwin;
CustomCounter* ctr;

int main() {
	mainwin = new Fl_Window(300, 100, "Counter format");
	ctr = new CustomCounter(25, 25, 250, 25, "Custom");
	
	mainwin->end();
	mainwin->show();
	return Fl::run();
}
