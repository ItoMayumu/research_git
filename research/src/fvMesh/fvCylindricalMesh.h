#ifndef FVCYLINDRICALMESH_H
#define FVCYLINDRICALMESH_H

class fvCylindricalMesh {
private:
    int Nr_, Ntheta_, Nx_;
    double R_, Theta_, Lx_;
    double dr_, dtheta_, dx_;

public:
    fvCylindricalMesh(int nr, int ntheta, int nx, double R, double theta, double lx);

    int Nr() const {return Nr_;}
    int Ntheta() const {return Ntheta_; }
    int Nx() const {return Nx_; }

    double dr() const {return dr_; }
    double dtheta() const {return dtheta_; }
    double dx() const {return dx_; }

    int size() const {return Nr_* Ntheta_ * Nx_; }

    int index(int ir, int itheta, int ix) const;

    double V(int ir) const;

};

#endif
