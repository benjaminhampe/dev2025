// Dragging boxes around a window

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>

class MovingBox;  // forward declaration of my special box variant

// global pointers for the gui widgets
MovingBox *box1;
MovingBox *box2;
Fl_Box *box3;
Fl_Window *mainwin;

// This class is just an Fl_Box that responds to drag-related events
class MovingBox : public Fl_Box {
public:
    MovingBox(int X, int Y, int W, int H, const char *L) : Fl_Box(X,Y,W,H,L) {}
private:
    int handle(int event) {
        static int startpos[2] = {0, 0};
        static int offset[2] = {0, 0};
        
        switch (event ) {
            case FL_PUSH:
                // record the starting position (top left of box)
                startpos[0] = x(); 
                startpos[1] = y();
                // record where in the box the cursor was when button was pushed
                offset[0] = x() - Fl::event_x();
                offset[1] = y() - Fl::event_y();
                return(1);
            case FL_DRAG:
                // while dragging, update the position of the box to be under
                // the cursor 
                position(Fl::event_x() + offset[0], Fl::event_y() + offset[1]);
                mainwin->redraw();
                return(1);
            case FL_RELEASE:
                // on button up, decide whether the box should go back to its
                // start position, or be left where it was dragged
                if (y() > 400) {
                    // move it back
                    position(startpos[0], startpos[1]);
                    mainwin->redraw();
                }
                return(1);
        }
        // If not used for moving, offer the event to the parent
        return Fl_Box::handle(event);
    }
};

int main() {
	// Use a double-buffered window to prevent all the redraws causing flicker
	mainwin = new Fl_Double_Window(600,600,"Box dragging");
	mainwin->color(FL_WHITE);
	box1 = new MovingBox(20,20,100,25, "One");
	box1->box(FL_OVAL_BOX);
	box1->color(FL_RED);
	box2 = new MovingBox(380,20,100,25, "Two");
	box2->box(FL_OVAL_BOX);
	box2->color(FL_BLUE);
	box3 = new Fl_Box(200, 400, 200, 25, 
	    "You can drag the blobs, but they won't stay below this notice");
	mainwin->end();
	mainwin->show();  
	
	return Fl::run();
}


