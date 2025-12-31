#pragma once

#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkDICOMImageReader.h>
#include <vtkVolume.h>
#include <vtkImagePlaneWidget.h>
#include <vector>
#include <string>

/**
 * @brief Manages the VTK pipeline, data loading, and visualization state.
 * Adheres to SRP: Handles 3D logic, isolated from UI.
 */
class SceneManager {
public:
    SceneManager();
    ~SceneManager();

    // Connects the abstract render window (from Qt) to the VTK renderer
    void setup(vtkGenericOpenGLRenderWindow* renderWindow);

    // Loads DICOM series from a directory and initializes volume/slicers
    void loadDicomSeries(const std::string& directory);

    // Toggles the visibility of the 3 orthogonal slicing planes
    void setSlicingEnabled(bool enabled);

    // Applies the specific "Bone" transfer function filter
    void applyBoneFilter();

    // Captures the current view and saves to disk
    void saveScreenshot(const std::string& filepath);

private:
    vtkSmartPointer<vtkRenderer> m_renderer;
    vtkSmartPointer<vtkDICOMImageReader> m_dicomReader;
    vtkSmartPointer<vtkVolume> m_volume;
    
    // Store widgets for X, Y, Z slicing planes
    std::vector<vtkSmartPointer<vtkImagePlaneWidget>> m_planeWidgets;

    // Internal helper to configure default visualization properties
    void setupVolumeProperties();
};
