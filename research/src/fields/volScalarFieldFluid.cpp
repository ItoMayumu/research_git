#include<iostream>
#include<vector>

#include "./volScalarFieldFluid.h"

volVectorField::volVectorField(std::string name, const fvMesh& mesh)
    :name_(name), mesh_(mesh), internalField_(mesh.size(),{0.0,0.0,0.0}){}

const Vector3& volVectorField::operator[](int i) const{
    return internalField_[i];
}

volVectorField& volVectorField::operator=(const Vector3& value){
    std::fill(internalField_.begin(), internalField_.end(), value);
    return *this;
}

#include <fstream>
#include <iomanip> // 数値をきれいに書き出すため

void volVectorField::writeVTK(const std::string& filename) const {
    std::ofstream os(filename);
    if (!os.is_open()) return;

    os << "# vtk DataFile Version 3.0" << std::endl;
    os << "Vector Field Data" << std::endl;
    os << "ASCII" << std::endl;
    os << "DATASET STRUCTURED_POINTS" << std::endl;

    os << "DIMENSIONS " << mesh_.Nx() << " " << mesh_.Ny() << " " << mesh_.Nz() << std::endl;
    os << "ORIGIN 0 0 0" << std::endl;
    os << "SPACING " << mesh_.dx() << " " << mesh_.dy() << " " << mesh_.dz() << std::endl;

    os << "POINT_DATA " << mesh_.size() << std::endl;
    os << "VECTORS " << name_ << " double" << std::endl;

    for (int i = 0; i < mesh_.size(); ++i) {
        os << std::fixed << std::setprecision(6)
           << internalField_[i].u << " " 
           << internalField_[i].v << " " 
           << internalField_[i].w << std::endl;
    }

    os.close();
}
