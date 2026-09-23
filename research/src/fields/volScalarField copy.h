#ifndef VOLSCALARFIELD_H
#define VOLSCALARFIELD_H

#include <vector>
#include <string>
#include "../fvMesh/fvMeshWrite.h"

class volScalarField{
    private:
        std::string name_;
        const fvMeshWrite& mesh_;
        std::vector<double> internalField_;

        public:
            volScalarField(std::string name, const fvMeshWrite& mesh, double initialValue = 0.0);

            std::string name() const;
            const fvMeshWrite& mesh() const;
            int size() const;

            double& operator[](int i);
            const double& operator[](int i) const;

            volScalarField& operator=(double value);

            void writeVTK(const std::string& filename) const;


};
#endif