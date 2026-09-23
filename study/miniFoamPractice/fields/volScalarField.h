#ifndef VOLSCALARFIELD_H
#define VOLSCALARFIELD_H


#include "../fvMesh/fvMesh.h"
#include <vector>
#include <string>

class volScalarField{
    private:
        std::string name_;
        const fvMesh& mesh_;
        std::vector<double> internalField_;

        public:
            volScalarField(std::string name, const fvMesh& mesh, double initialValue = 0.0);

            std::string name() const;
            const fvMesh& mesh() const;
            int size() const;

            double &operator[](int i);
            const double &operator[](int i) const;

            volScalarField& operator=(double value);
};


#endif