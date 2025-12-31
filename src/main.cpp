#include <QApplication>
#include <QSurfaceFormat>
#include <QVTKOpenGLNativeWidget.h>
#include "MainWindow.h"

int main(int argc, char** argv) {
    // PRE-INIT: Set the default OpenGL format to be compatible with VTK.
    // This ensures correct depth buffer size and OpenGL profile (Core vs Compatibility).
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    QApplication app(argc, argv);

    // Initialize the main window
    MainWindow window;
    window.setWindowTitle("Interactive 3D Anatomy Viewer");
    window.resize(1280, 800); // 16:10 aspect ratio for standard displays
    window.show();

    return app.exec();
}
