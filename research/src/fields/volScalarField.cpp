#include "volScalarField.h"

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


