#include "SceneManager.h"

#include <vtkSmartVolumeMapper.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkCamera.h>
#include <iostream>

SceneManager::SceneManager() {
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    // Set a neutral dark grey background for contrast
    m_renderer->SetBackground(0.1, 0.1, 0.1);
}

SceneManager::~SceneManager() {
    // VTK SmartPointers handle their own cleanup.
}

void SceneManager::setup(vtkGenericOpenGLRenderWindow* renderWindow) {
    if (!renderWindow) return;

    // Add our renderer to the provided window
    renderWindow->AddRenderer(m_renderer);

    // Set the interaction style to standard 3D trackball (rotate/zoom)
    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindow->GetInteractor()->SetInteractorStyle(style);
}

void SceneManager::loadDicomSeries(const std::string& directory) {
    // Initialize Reader
    m_dicomReader = vtkSmartPointer<vtkDICOMImageReader>::New();
    m_dicomReader->SetDirectoryName(directory.c_str());
    m_dicomReader->Update(); // Execute pipeline to load data

    // Check if data was loaded
    if (!m_dicomReader->GetOutput()) {
        std::cerr << "Error: No DICOM data found in " << directory << std::endl;
        return;
    }

    // Initialize Volume Mapper (Smart mapper chooses GPU or CPU)
    vtkNew<vtkSmartVolumeMapper> mapper;
    mapper->SetInputConnection(m_dicomReader->GetOutputPort());

    // Initialize Volume
    m_volume = vtkSmartPointer<vtkVolume>::New();
    m_volume->SetMapper(mapper);

    // Apply default visualization (Bone filter)
    applyBoneFilter();

    // Add volume to renderer
    m_renderer->AddVolume(m_volume);
    m_renderer->ResetCamera(); // Center view on data

    // Initialize Slicing Planes (Axial, Sagittal, Coronal)
    m_planeWidgets.clear();
    for (int i = 0; i < 3; i++) {
        vtkNew<vtkImagePlaneWidget> plane;
        plane->SetInputConnection(m_dicomReader->GetOutputPort());
        plane->SetPlaneOrientation(i); // 0=X, 1=Y, 2=Z
        plane->SetSliceIndex(m_dicomReader->GetOutput()->GetDimensions()[i] / 2); // Center slice
        
        // Configure interaction
        plane->SetInteractor(m_renderer->GetRenderWindow()->GetInteractor());
        plane->RestrictPlaneToVolumeOn(); // Keep plane inside data bounds
        plane->DisplayTextOn(); // Show slice number/coordinates
        plane->On(); // Enable by default
        
        m_planeWidgets.push_back(plane);
    }
    
    // Trigger initial render
    m_renderer->GetRenderWindow()->Render();
}

void SceneManager::setSlicingEnabled(bool enabled) {
    for (auto& plane : m_planeWidgets) {
        if (enabled) plane->On();
        else plane->Off();
    }
    m_renderer->GetRenderWindow()->Render();
}

void SceneManager::applyBoneFilter() {
    if (!m_volume) return;

    vtkNew<vtkVolumeProperty> property;
    property->ShadeOn(); // Enable shading for 3D effect
    property->SetInterpolationTypeToLinear();

    // Color: Map Hounsfield units to Bone colors
    vtkNew<vtkColorTransferFunction> color;
    color->AddRGBPoint(-1000, 0.0, 0.0, 0.0);   // Air
    color->AddRGBPoint(0,     0.9, 0.7, 0.7);   // Tissue
    color->AddRGBPoint(500,   1.0, 0.9, 0.8);   // Bone
    
    // Opacity: Hide everything below 200 HU
    vtkNew<vtkPiecewiseFunction> opacity;
    opacity->AddPoint(-1000, 0.0);
    opacity->AddPoint(200,   0.0); // Threshold
    opacity->AddPoint(500,   0.5);
    opacity->AddPoint(1000,  1.0); // Solid Bone

    property->SetColor(color);
    property->SetScalarOpacity(opacity);

    m_volume->SetProperty(property);
    m_renderer->GetRenderWindow()->Render();
}

void SceneManager::saveScreenshot(const std::string& filepath) {
    if (!m_renderer->GetRenderWindow()) return;

    // Capture the framebuffer
    vtkNew<vtkWindowToImageFilter> w2i;
    w2i->SetInput(m_renderer->GetRenderWindow());
    w2i->SetInputBufferTypeToRGBA();
    w2i->ReadFrontBufferOff(); 
    w2i->Update();

    // Write to PNG
    vtkNew<vtkPNGWriter> writer;
    writer->SetFileName(filepath.c_str());
    writer->SetInputConnection(w2i->GetOutputPort());
    writer->Write();
}
