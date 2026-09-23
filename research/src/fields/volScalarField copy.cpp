#include "volScalarField copy.h"
#include <fstream>
#include <iostream>
#include <cmath>


volScalarField::volScalarField(std::string name, const fvMeshWrite& mesh, double initialValue)
    : name_(name), mesh_(mesh)
    {
        internalField_.assign(mesh.size(), initialValue);
    }

std::string volScalarField::name() const {
    return name_;
}

const fvMeshWrite& volScalarField::mesh() const{
    return mesh_;
}

int volScalarField::size() const{
    return internalField_.size();
}

double& volScalarField::operator[](int i){
    return internalField_[i];
}

const double& volScalarField::operator[](int i) const{
    return internalField_[i];
}
volScalarField& volScalarField::operator=(double value) {
    for (int i = 0; i < size(); ++i) {
        internalField_[i] = value;
    }
    return *this;
}