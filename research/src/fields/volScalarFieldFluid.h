#ifndef VOLVECTORFIELD_H
#define VOLVECTORFIELD_H

#include <vector>
#include <string>
#include "../fvMesh/fvMesh.h"

struct Vector3{
    double u, v, w;
};

class volVectorField{
private:
    std::string name_;
    const fvMesh& mesh_;
    std::vector<Vector3> internalField_;

public:
    volVectorField(std::string name, const fvMesh& mesh);

    Vector3& operator[](int i);
    const Vector3& operator[](int i) const;

    volVectorField& operator=(const Vector3& value);

    void writeVTK(const std::string& filename) const;
};



#endif
