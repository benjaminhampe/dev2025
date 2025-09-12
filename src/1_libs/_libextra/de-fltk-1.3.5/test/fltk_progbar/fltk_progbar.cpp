// Illustrate progress bar and Fl::flush()

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Progress.H>

// Global pointers for the GUI objects
Fl_Window* mywindow;
Fl_Button* startbut;
Fl_Output* myoutputbox;
Fl_Progress* progbar;

// Callback function.  This represents a time-consuming operation, during which
// we want to update the progress bar to show that something is happening.
// The flush() calls are needed, otherwise the GUI will not be updated until
// the callback has finished, so the progress bar updates will not be seen until
// too late.
void start_cb(Fl_Widget* w, long int data) {
    myoutputbox->value("Started");
    Fl::flush();  
    for (int i = 0; i < 1000000; i++) {
        if (i % 100000 == 0) {
            progbar->value(i);  // every so often, bump the progbar
            Fl::flush();
        }
        // waste some time
        for (volatile int j=0; j<1000; j++) {}
    }
    myoutputbox->value("Finished");
    progbar->value(1000000);
}

int main() {
    mywindow = new Fl_Window(300,130,"Progress bar demo");
    
    startbut = new Fl_Button(125, 10, 50, 30, "Start");
    startbut->callback(start_cb);
    
    myoutputbox = new Fl_Output(50, 50, 200, 30);
    myoutputbox->value("Waiting");
    
    progbar = new Fl_Progress(50, 100, 200, 25);
    progbar->selection_color(FL_BLUE);
    progbar->minimum(0.0);
    progbar->maximum(1000000.0);
    
    mywindow->end();
    mywindow->show();  
    return Fl::run();
}

