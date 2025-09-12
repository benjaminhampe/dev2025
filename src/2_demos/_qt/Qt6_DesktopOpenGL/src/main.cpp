#include "ClipHexagon.h"
#include "widget.h"

#include <QApplication>



int main(int argc, char *argv[])
{
	// Define width and height of the heightmap
    int w = 100, h = 100;
    std::vector<double> heightmap(w * h, 1.0);  // Single vector to represent the heightmap

    // Generate hexagon vertices
    int cx = 50, cy = 50, radius = 30;
    auto polygon = generateHexagon(cx, cy, radius);

    // Clip heightmap to polygon and return spans
    auto spans = clipHeightmapToPolygon(heightmap, w, h, polygon);

    // Output spans (for example, save to file or visualize)
    for (const auto& span : spans) {
        std::cout << "y: " << span.y << ", x1: " << span.x1 << ", x2: " << span.x2 << std::endl;
    }

    //return 0;
	
    QApplication a(argc, argv);
    Widget testWidget;
    testWidget.show();
    testWidget.setWindowIcon(QIcon(":/winico"));
    return a.exec();
}
