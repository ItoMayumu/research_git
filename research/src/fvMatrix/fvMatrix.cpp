#include "fvMatrix.h"
#include <iostream>
#include <cmath>


fvMatrix::fvMatrix(volScalarField& psi)
    : psi_(psi)
    {
        int nCells = psi_.size();
        aC_.resize(nCells, 0.0);
        aE_.resize(nCells, 0.0);
        aW_.resize(nCells, 0.0);
        aN_.resize(nCells, 0.0);
        aS_.resize(nCells, 0.0);
        aT_.resize(nCells, 0.0);
        aB_.resize(nCells, 0.0);
        source_.resize(nCells, 0.0);
    }

std::vector<double>& fvMatrix::aC() {
    return aC_;
}

std::vector<double>& fvMatrix::aE() {
    return aE_;
}

std::vector<double>& fvMatrix::aW() {
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
    int n = psi_.size();

    int Nx = psi_.mesh().Nx();
    int Ny = psi_.mesh().Ny();
    int Nxy = Nx * Ny;

    int maxIter = 1000;
    double tolerance  = 1e-6;

    for (int iter = 0; iter < maxIter; ++iter){
        double maxResidual=0.0;

        for (int i=0; i < n; ++i){
            if(std::abs(aC_[i]) < 1e-12) continue;

            double neighborSum = 0.0;

            if(i + 1 < n) neighborSum += aE_[i] * psi_[i + 1];
            if (i - 1 >= 0)   neighborSum += aW_[i] * psi_[i - 1];
            if (i + Nx < n)   neighborSum += aN_[i] * psi_[i + Nx];
            if (i - Nx >= 0)  neighborSum += aS_[i] * psi_[i - Nx];
            if (i + Nxy < n)  neighborSum += aT_[i] * psi_[i + Nxy];
            if (i - Nxy >= 0) neighborSum += aB_[i] * psi_[i - Nxy];
            
            double x_new = (source_[i] - neighborSum) / aC_[i];

            double residual = std::abs(x_new - psi_[i]);
            if(residual > maxResidual){
                maxResidual = residual;
            }

            psi_[i] = x_new;
        }
        if (maxResidual < tolerance) {
            std::cout << "Gauss-Seidel Converged: " << iter << " steps" << std::endl;
            break;
        }
    }
}