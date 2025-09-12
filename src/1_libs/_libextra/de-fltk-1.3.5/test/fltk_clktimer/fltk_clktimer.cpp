// Using Fl_Clock_Output to display a Countdown-like clock
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Clock.H>

Fl_Window* mainwin;
Fl_Button* but;
Fl_Clock_Output* clk;
const int timeallowed = 10 ;  // 10 seconds before expiry


// Alter the else clause to determine what happens when the timer runs out
void advance_clock(void*) {
	int timenow  = clk->second() + 1;
	clk->value(0,0,timenow);
	if (timenow < timeallowed) {
		Fl::repeat_timeout(1.0, advance_clock);
	} else {
		clk->label("Expired!");
	}
}


// Call this function to (re)start the clock from zero
void start_clock(Fl_Widget*) {
	Fl::remove_timeout(advance_clock);  // in case it is already running
	clk->value(0,0,0);
	clk->label("Running");
	Fl::add_timeout(1.0, advance_clock);
}


int main() {
	mainwin = new Fl_Window (200, 200, "Clocktest");
	clk = new Fl_Clock_Output(50,50,100,100, "Waiting");
	but = new Fl_Button(50,175,100,20,"Start");
	but->callback(start_clock);
	
	mainwin->end();
	mainwin->show();
	return Fl::run();
}
