#ifndef FVM_H
#define FVM_H

#include "../fvMatrix/fvMatrix.h"
#include "../fields/volScalarField.h"

namespace fvm {
    fvMatrix laplacian(double k, volScalarField& psi);
}

#endif