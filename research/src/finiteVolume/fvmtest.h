#ifndef FVMTEST_H
#define FVMTEST_H

#include "../fvMatrix/fvMatrix.h"
#include "../fields/volScalarField.h"

namespace fvmtest {
    fvMatrix laplacian(double k, volScalarField& psi);
    fvMatrix div(double u, volScalarField& psi);
}

#endif