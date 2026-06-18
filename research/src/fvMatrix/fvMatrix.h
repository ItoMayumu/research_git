#ifndef FVMATRIX_H
#define FVMATRIX_H

#include <vector>
#include "../fields/volScalarField.h"

class fvMatrix {
private:
    volScalarField& psi_;

    std::vector<double> aC_; 
    std::vector<double> aE_; 
    std::vector<double> aW_; 
    std::vector<double> aN_; 
    std::vector<double> aS_; 
    std::vector<double> aT_; 
    std::vector<double> aB_; 

    std::vector<double> source_;


public:
    fvMatrix(volScalarField& psi);

    std::vector<double>& aC();
    std::vector<double>& aE();
    std::vector<double>& aW();
    std::vector<double>& aN();
    std::vector<double>& aS();
    std::vector<double>& aT();
    std::vector<double>& aB();
    std::vector<double>& source();

    void solve();


};

#endif // FVMATRIX_H