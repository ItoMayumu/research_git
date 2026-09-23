#include "ErgunPorositySource.H"
#include "fvMesh.H"
#include "fvMatrices.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{
    defineTypeNameAndDebug(ErgunPorositySource, 0);
    addToRunTimeSelectionTable(option, ErgunPorositySource, dictionary);
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::fv::ErgunPorositySource::ErgunPorositySource
(
    const word& name,
    const word& modelType,
    const dictionary& dict,
    const fvMesh& mesh
)
:
    fv::cellSetOption(name, modelType, dict, mesh),
    epsilon_(0),
    dp_(0),
    alphaMin_(1e-3),
    muName_("thermo:mu")
{
    read(dict);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::fv::ErgunPorositySource::addSup
(
    const volScalarField& alpha,
    const volScalarField& rho,
    fvMatrix<vector>& eqn,
    const label fieldi
)
{
    const volVectorField& U = eqn.psi();

    const word muName(IOobject::groupName(muName_, U.group()));
    const volScalarField& mu = mesh_.lookupObject<volScalarField>(muName);

    scalarField& Udiag = eqn.diag();
    const scalarField& V = mesh_.V();

    // Ergun coefficients, with the local phase fraction already
    // substituted for the (1-eps) exponents that a superficial-velocity
    // formulation (jL, jG) would carry - see calculation/darcyMethod.cpp
    const scalar coeffD = 150.0*sqr(1.0 - epsilon_)/(pow3(epsilon_)*sqr(dp_));
    const scalar coeffF = 1.75*(1.0 - epsilon_)/(pow3(epsilon_)*dp_);

    scalar CdMin = GREAT;
    scalar CdMax = -GREAT;

    for (const label celli : cells_)
    {
        const scalar a =
            max(min(alpha[celli], 1.0 - alphaMin_), alphaMin_);

        const scalar D = coeffD/sqr(a);
        const scalar F = coeffF/a;

        const scalar Cd = mu[celli]*D + rho[celli]*mag(U[celli])*F;

        Udiag[celli] += V[celli]*Cd;

        CdMin = min(CdMin, Cd);
        CdMax = max(CdMax, Cd);
    }

    Info<< "ErgunPorositySource[" << name_ << "] U=" << U.name()
        << " cells=" << cells_.size()
        << " Cd(min,max)=(" << CdMin << ", " << CdMax << ")"
        << endl;
}


bool Foam::fv::ErgunPorositySource::read(const dictionary& dict)
{
    if (fv::cellSetOption::read(dict))
    {
        coeffs_.readEntry("epsilon", epsilon_);
        coeffs_.readEntry("dp", dp_);
        coeffs_.readIfPresent("alphaMin", alphaMin_);
        muName_ = coeffs_.getOrDefault<word>("mu", "thermo:mu");

        if (!coeffs_.readIfPresent("UNames", fieldNames_))
        {
            fieldNames_.resize(1);
            fieldNames_.first() = coeffs_.getOrDefault<word>("U", "U");
        }

        fv::option::resetApplied();

        return true;
    }

    return false;
}


// ************************************************************************* //
