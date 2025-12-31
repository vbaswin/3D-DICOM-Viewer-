#include "MainWindow.h"
#include <QMenuBar>
#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent) 
    : QMainWindow(parent), m_sceneManager(std::make_unique<SceneManager>()) 
{
    setupUi();
    
    // Initialize the VTK Render Window
    // We create a generic window wrapper that QVTKOpenGLNativeWidget can manage
    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    m_vtkWidget->SetRenderWindow(renderWindow);
    
    // Initialize the scene
    m_sceneManager->setup(renderWindow);
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi() {
    // 1. Central Widget Setup
    QWidget* central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0); // Full window
    
    m_vtkWidget = new QVTKOpenGLNativeWidget(central);
    layout->addWidget(m_vtkWidget);
    setCentralWidget(central);

    // 2. Menu Bar Construction
    QMenu* fileMenu = menuBar()->addMenu("&File");
    
    QAction* openAction = fileMenu->addAction("Open DICOM &Folder...");
    connect(openAction, &QAction::triggered, this, &MainWindow::onOpenFolder);
    
    QAction* screenshotAction = fileMenu->addAction("&Save Screenshot...");
    connect(screenshotAction, &QAction::triggered, this, &MainWindow::onSaveScreenshot);
    
    QAction* exitAction = fileMenu->addAction("E&xit");
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // 3. Toolbar Construction (Quick Access)
    QToolBar* toolbar = addToolBar("Tools");
    
    QAction* sliceAction = toolbar->addAction("Slicing Tools");
    sliceAction->setCheckable(true);
    sliceAction->setChecked(true); // Enabled by default
    connect(sliceAction, &QAction::toggled, this, &MainWindow::onToggleSlicing);
}

void MainWindow::onOpenFolder() {
    QString dir = QFileDialog::getExistingDirectory(
        this, "Select DICOM Directory", QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        
    if (!dir.isEmpty()) {
        m_sceneManager->loadDicomSeries(dir.toStdString());
    }
}

void MainWindow::onToggleSlicing(bool checked) {
    m_sceneManager->setSlicingEnabled(checked);
}

void MainWindow::onSaveScreenshot() {
    QString fileName = QFileDialog::getSaveFileName(
        this, "Save View", QDir::homePath(), "Images (*.png)");
        
    if (!fileName.isEmpty()) {
        m_sceneManager->saveScreenshot(fileName.toStdString());
    }
}
