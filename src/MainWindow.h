#pragma once

#include <QMainWindow>
#include <QVTKOpenGLNativeWidget.h>
#include <memory>
#include "SceneManager.h"

/**
 * @brief Main Application Window.
 * Responsible for UI layout and forwarding user actions to SceneManager.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onOpenFolder();
    void onToggleSlicing(bool checked);
    void onSaveScreenshot();

private:
    void setupUi(); // Manual UI construction (No.ui files for minimal deps)

    QVTKOpenGLNativeWidget* m_vtkWidget; // The bridge widget
    std::unique_ptr<SceneManager> m_sceneManager; // The VTK logic
};
