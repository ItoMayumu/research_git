#ifndef FVMESH_H
#define FVMESH_H

class fvMesh {
private:
   int Nx_, Ny_, Nz_;
   double Lx_, Ly_, Lz_;
   double dx_, dy_, dz_;

public:
    fvMesh(int nx, int ny, int nz, double lx, double ly, double lz);

    int Nx() const;
    int Ny() const;
    int Nz() const;
    double dx() const;
    double dy() const;
    double dz() const;

    int size() const;

    double V() const;

    int index(int x, int y, int z) const;
};

#endif // FVMESH_H