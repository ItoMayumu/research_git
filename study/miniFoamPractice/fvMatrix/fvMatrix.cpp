#include "fvMatrix.h"
#include <vector>

fvMatrix::fvMatrix(volScalarField& psi)
    : psi_(psi)
    {
        int nCells = psi_.size();
        aP_.resize(nCells, 0.0);
        aE_.resize(nCells, 0.0);
        aW_.resize(nCells, 0.0);
        aN_.resize(nCells, 0.0);
        aS_.resize(nCells, 0.0);
        aT_.resize(nCells, 0.0);
        aB_.resize(nCells, 0.0);
        source_.resize(nCells, 0.0);
    }

std::vector<double>& fvMatrix::aP(){
    return aP_;
}

std::vector<double>& fvMatrix::aE(){
    return aE_;
}

std::vector<double>& fvMatrix::aW(){
    return aW_;
}



std::vector<double>& fvMatrix::aN() {
    return aN_;
}

std::vector<double>& fvMatrix::aS() {
    return aS_;
}

std::vector<double>& fvMatrix::aT() {
    return aT_;
}

std::vector<double>& fvMatrix::aB() {
    return aB_;
}

std::vector<double>& fvMatrix::source() {
    return source_;
}

void fvMatrix::solve(){
    int N = psi_.size();
    int Nx = psi_.mesh().Nx();
    int Ny = psi_.mesh().Ny();
    int Nz = psi_.mesh().Nz();

    int Nxy = Nx * Ny;
    int maxIter = 1000;
    double tolerance  = 1e-6;

    for(int iter=0;iter<maxIter;++iter){
        if(std::abs(aP_[0]) < 1e-12) continue; 
        // Avoid division by zero

        
        
    }
}