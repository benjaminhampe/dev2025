// Skeleton FLTK program

#include <FL/Fl.H>
#include <FL/Fl_Window.H>   // One include for each class of GUI object
#include <FL/Fl_Button.H>
#include <FL/Fl_Value_Output.H>

// Global pointers for the GUI objects
Fl_Window* mywindow; 
Fl_Button* grid[25];
Fl_Value_Output* xoutputbox;
Fl_Value_Output* youtputbox;

// Callback function
void mybutton_cb(Fl_Widget* w, long int data) {
    xoutputbox->value(double(data%5));
    youtputbox->value(double(data/5));
}

// Execution starts here
int main() {
    // The main window (will contain everything up to mywindow->end() )
    mywindow = new Fl_Window(290,350,"FLTK grid example");

    // Create a grid of buttons
    for ( int i = 0 ; i < 25 ; ++i )
    {
      grid[i] = new Fl_Button(20+50*(i%5), 20+50*(i/5), 50, 50 );
      grid[i]->callback(mybutton_cb, i);
    }	 

    // An output box
    xoutputbox = new Fl_Value_Output( 70, 300, 50, 30, "X =");
    youtputbox = new Fl_Value_Output(170, 300, 50, 30, "Y =");

    // Make the window visible and start processing events
    mywindow->end();
    mywindow->show();  
    return Fl::run();
}

