#include "volScalarField.h"
#include <fstream>
#include <iostream>
#include <cmath>


volScalarField::volScalarField(std::string name, const fvMesh& mesh, double initialValue)
    : name_(name), mesh_(mesh)
    {
        internalField_.assign(mesh.size(), initialValue);
    }

std::string volScalarField::name() const {
    return name_;
}

const fvMesh& volScalarField::mesh() const {
    return mesh_;
}


const double& volScalarField::operator[](int i) const {
    return internalField_[i];
}

int volScalarField::size() const {
    return internalField_.size();
}

double& volScalarField::operator[](int i){
    return internalField_[i];
}

volScalarField& volScalarField::operator=(double value){
    for (int i=0; i < size(); ++i){
        internalField_[i] = value;
    }


    return *this;
}

void volScalarField::writeVTK(const std::string& filename) const {
    std::ofstream os(filename);
    if (!os.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }

    int Nx = mesh_.Nx();
    int Ny = mesh_.Ny();
    int Nz = mesh_.Nz();
    double dx = mesh_.dx();
    double dy = mesh_.dy();
    double dz = mesh_.dz();

    // 1. VTKファイルのヘッダー
    os << "# vtk DataFile Version 3.0\n";
    os << "miniFOAM volScalarField Output: " << name_ << "\n";
    os << "ASCII\n";
    os << "DATASET STRUCTURED_POINTS\n";

    // 2. 空間のマス目の数
    os << "DIMENSIONS " << Nx << " " << Ny << " " << Nz << "\n";
    
    // 3. 原点と1マスのサイズ
    os << "ORIGIN 0.0 0.0 0.0\n";
    os << "SPACING " << dx << " " << dy << " " << dz << "\n";

    // 4. 物理量データの合図
    os << "POINT_DATA " << size() << "\n";
    os << "SCALARS " << name_ << " double 1\n";
    os << "LOOKUP_TABLE default\n";

    // 5. 全マスのデータを書き出す
    for (int i = 0; i < size(); ++i) {
        os << internalField_[i] << "\n";
    }

    std::cout << "Successfully saved VTK file: " << filename << std::endl;
}


