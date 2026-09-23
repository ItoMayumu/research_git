#include "fvMeshWrite.h"

fvMeshWrite::fvMeshWrite(int nx, int ny, int nz, double lx, double ly, double lz):
    Nx_(nx), Ny_(ny), Nz_(nz), Lx_(lx), Ly_(ly), Lz_(lz) {

    dx_ = Lx_ / (Nx_ - 1);
    dy_ = Ly_ / (Ny_ - 1);
    dz_ = Lz_ / (Nz_ - 1); }

int fvMeshWrite::Nx() const{ return Nx_; }
int fvMeshWrite::Ny() const{ return Ny_; }
int fvMeshWrite::Nz() const{ return Nz_; }
double fvMeshWrite::dx() const{ return dx_; }
double fvMeshWrite::dy() const{ return dy_; }
double fvMeshWrite::dz() const{ return dz_; }

int fvMeshWrite::size() const {
    return Nx_ * Ny_ * Nz_;
}

double fvMeshWrite::V() const {
    return dx_ * dy_ * dz_;
}

int fvMeshWrite::index(int i, int j, int k) const {
    return i + j * Nx_ + k * (Nx_ * Ny_);
}