// Image demo

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/FL_Box.H>
#include <FL/Fl_JPEG_Image.H>

// Global pointers for the GUI objects
Fl_Window *mywindow;
Fl_Button *spiderbutton;
Fl_Button *dogbutton;
Fl_Box *mypicturebox;
Fl_JPEG_Image *startimg;
Fl_JPEG_Image *spiderimg;
Fl_JPEG_Image *dogimg;

// Callback function
void mybutton_cb(Fl_Widget * w, long int data)
{
	if (data == 8) {
		mypicturebox->image(spiderimg);
		mypicturebox->redraw();
	}
	if (data == 4) {
		mypicturebox->image(dogimg);
		mypicturebox->redraw();
	}
}
int main()
{

	// The main window
	mywindow = new Fl_Window(300, 220, "FLTK image demo");

	// Two buttons, sharing one callback
	spiderbutton = new Fl_Button(50, 50, 50, 30, "Spider");
	dogbutton = new Fl_Button(200, 50, 50, 30, "Dog");
	spiderbutton->callback(mybutton_cb, 8);
	dogbutton->callback(mybutton_cb, 4);

	// Load some images to use later
	startimg = new Fl_JPEG_Image("startimg.jpg");
	spiderimg = new Fl_JPEG_Image("spider.jpg");
	dogimg = new Fl_JPEG_Image("dog1.jpg");

	// A box for the image
	mypicturebox = new Fl_Box(100, 100, 100, 100);

	// Give it some initial contents
	mypicturebox->image(startimg);

	// Make the window visible and start processing events
	mywindow->end();
	mywindow->show();
	return Fl::run();
}
