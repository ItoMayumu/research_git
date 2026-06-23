#include "fvCylindricalMesh.h"
#include <cmath>

fvCylindricalMesh::fvCylindricalMesh(int nr, int ntheta, int nx, double R, double theta, double lx)
    : Nr_(nr), Ntheta_(ntheta), Nx_(nx), R_(R), Theta_(theta), Lx_(lx){
        dr_ = R_ / Nr_;
        dtheta_ = Theta_ / Ntheta_;
        dx_ = Lx_ / (Nx_ - 1);
    }


int fvCylindricalMesh::index(int ir, int itheta, int ix) const{
    return ir + itheta * Nr_ * ix * (Nr_ * Ntheta_);
}

double fvCylindricalMesh::V(int ir) const {
    double r_in = ir * dr_;
    double r_out = (ir + 1) * dr_;

    double area = 0.5 * (r_out * r_out - r_in * r_in) * dtheta_;
    return area * dx_;
}
