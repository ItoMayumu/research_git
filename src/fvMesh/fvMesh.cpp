#include "fvMesh.h"

fvMesh::fvMesh(int nx, int ny, int nz, double lx, double ly, double lz)
    : Nx_(nx), Ny_(ny), Nz_(nz), Lx_(lx), Ly_(ly), Lz_(lz) {
    dx_ = Lx_ / (Nx_ - 1);
    dy_ = Ly_ / (Ny_ - 1);
    dz_ = Lz_ / (Nz_ - 1);
}

int fvMesh::Nx() const { return Nx_; }
int fvMesh::Ny() const { return Ny_; }
int fvMesh::Nz() const { return Nz_; }
double fvMesh::dx() const { return dx_; }
double fvMesh::dy() const { return dy_; }
double fvMesh::dz() const { return dz_; }   

int fvMesh::size() const {
    return Nx_ * Ny_ * Nz_;
}

double fvMesh::V() const {
    return dx_ * dy_ * dz_;
}

int fvMesh::index(int i, int j, int k) const {
    return i + j * Nx_ + k * (Nx_ * Ny_);
}