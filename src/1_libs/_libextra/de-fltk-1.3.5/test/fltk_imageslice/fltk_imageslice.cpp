// A specialised box that draws a portion of an image

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_Window.H>

class MyBox; // forward declaration 

Fl_Window* mainwin;
MyBox* box1;
MyBox* box2;
Fl_JPEG_Image* myimage;  

// This class is just an Fl_Box that draws a fragment of an image
class MyBox : public Fl_Box {
public:
	MyBox(int X, int Y, int W, int H,   // position and size of the box
	      int xorigin, int yorigin,     // origin of image fragment to use
	      const char *L=0)  // optional label
	     : Fl_Box(X,Y,W,H,L), xo(xorigin), yo(yorigin) {}
private:
	int xo, yo;
	void draw() {
		myimage->draw(x(),y(),w(),h(), xo, yo);
	}
};


int main() {
	mainwin = new Fl_Window(300, 300, "Image slicing");
	myimage = new Fl_JPEG_Image("concertina.jpg");
	
	box1 = new MyBox(10, 10, 100, 100, 0, 0);
	box1->box(FL_UP_BOX);
	box2 = new MyBox(120, 110, 100, 100, 50, 50);
	box2->box(FL_UP_BOX);
	
	mainwin->end();
	mainwin->show();
	return Fl::run();
}
