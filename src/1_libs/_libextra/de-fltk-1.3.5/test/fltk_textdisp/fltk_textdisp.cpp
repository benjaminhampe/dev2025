#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Button.H>


Fl_Window* mainwin;
Fl_Window* instwin;
Fl_Text_Display* instructions;
Fl_Text_Buffer* instbuff;
Fl_Button* showbut;
Fl_Button* insthidebut;


void show_instructions(Fl_Widget* w) {
	instwin->show();
}

void hide_instructions(Fl_Widget* w) {
	instwin->hide();
}

int main() {
	// Instruction window
	instwin = new Fl_Window (300, 500, "Text");
	instructions = new Fl_Text_Display(25,25,250,450, "Instructions");
	instbuff = new Fl_Text_Buffer();
	instructions->buffer(instbuff);
	instbuff->loadfile("fltk_textdisp.cpp");  // use a file of text here
	// remove <cr> symbols that Windows may have inserted
	int pos = 0;
	while (instbuff->findchar_forward(pos, '\r', &pos)) {
		instbuff->remove(pos, pos+1);
	}
	insthidebut = new Fl_Button(100,475,100,25,"Hide");
	insthidebut->callback(hide_instructions);
	instwin->end();
	
	// main window
	mainwin = new Fl_Window (200,200, "Text");
	showbut = new Fl_Button(25, 100, 150, 25, "Show instructions");
	showbut->callback(show_instructions);
	
	mainwin->end();
	mainwin->show();
	return Fl::run();
}
