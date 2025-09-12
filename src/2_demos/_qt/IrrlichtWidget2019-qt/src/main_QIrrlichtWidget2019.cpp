// ============================================================================
/// @author Benjamin Hampe (c) 2019 <benjaminhampe@gmx.de>
// ============================================================================

// Separated outside because moc compiler wants one Q_OBJECT macromagic per file.
#include "MainWindow.hpp"

int main(int argc, char** argv)
{   
	QApplication app(argc, argv);

	MainWindow mainWindow( nullptr );
   mainWindow.setWindowIcon( QIcon(":/winico") ); // Compile in res/resource.qrc, see CMakeLists.txt
	mainWindow.show();

   return app.exec();
}
