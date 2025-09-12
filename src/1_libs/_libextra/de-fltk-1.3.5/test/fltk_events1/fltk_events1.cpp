// Handling mouse clicks in a window

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <iostream>

using namespace std;


// This class is just an Fl_Window with a custom event handler added.

class SpecialWindow : public Fl_Window {
  public:
	// The ctor just makes a plain window of the right size 
	SpecialWindow(int w, int h, const char* title) : Fl_Window(w,h, title) {}
	
  private:
	// fltk calls handle() with events the window might want. 
	// Here we play with the events  
	int handle(int event) {
		if (event == FL_PUSH) {
			cout << "button " << Fl::event_button() << " push at x=" << Fl::event_x()
			     << ", y=" << Fl::event_y() << endl;
			return 1;   // Signifies we used the event
		} else if (event == FL_RELEASE) {
			cout << "button " << Fl::event_button() << " release at x=" << Fl::event_x()
			     << ", y=" << Fl::event_y() << endl;
			return 1;   // Signifies we used the event
		} else {
			// maybe our parent wants the event instead
			return Fl_Window::handle(event); // pass event to parent
		}
	}
};


// global pointers for the gui widgets

Fl_Box *box1;
Fl_Box *box2;
Fl_Box *box3;
SpecialWindow *mainwin;

int main() {
	mainwin = new SpecialWindow(600,600,"Event catcher");
	box1 = new Fl_Box(20,20,100,25, "One");	 
	box2 = new Fl_Box(380,20,100,25, "Two");
	box3 = new Fl_Box(200, 400, 200, 25,
	    "Check the console to see button events caught by the window");
	mainwin->end();
	mainwin->show();  
	
	return Fl::run();
}


