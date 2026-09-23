/*---------------------------------------------------------------------------*\
Class
    Foam::fv::ErgunPorositySource

Description
    Packed-bed momentum sink for a two-fluid Euler-Euler phase equation,
    following the same Ergun-based Darcy-Forchheimer formulas used in
    calculation/darcyMethod.cpp, but with the resistance coefficients
    D(a), F(a) evaluated per cell from the phase fraction actually being
    solved for by the CFD (the "alpha" passed in to addSup), instead of a
    single case-wide constant taken from an external correlation:

        D(a) = 150*(1-eps)^2 / (a^2 * eps^3 * dp^2)
        F(a) = 1.75*(1-eps)  / (a   * eps^3 * dp)

    where "a" is the local value of the phase fraction field that this
    fvOption instance is attached to (alpha.air for porosity_air, alpha.water
    for porosity_water), clipped to [alphaMin, 1 - alphaMin] to avoid the
    1/a^2 singularity where a cell is (near-)fully occupied by the other
    phase.

    Momentum sink added to the cellZone:
        Cd = mu*D(a) + rho*mag(U)*F(a)
        (implicit, isotropic: applied equally to all velocity components)

Usage
    \verbatim
    porosity_air
    {
        type            ErgunPorositySource;
        active          true;

        ErgunPorositySourceCoeffs
        {
            selectionMode   cellZone;
            cellZone        porosity1;
            U               U.air;

            epsilon         0.3731;
            dp              0.005;
            alphaMin        1e-3;
        }
    }
    \endverbatim

SourceFiles
    ErgunPorositySource.C

\*---------------------------------------------------------------------------*/

#ifndef ErgunPorositySource_H
#define ErgunPorositySource_H

#include "cellSetOption.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{

/*---------------------------------------------------------------------------*\
                   Class ErgunPorositySource Declaration
\*---------------------------------------------------------------------------*/

class ErgunPorositySource
:
    public fv::cellSetOption
{
protected:

    // Protected Data

        //- Bed void fraction (epsilon)
        scalar epsilon_;

        //- Particle diameter [m]
        scalar dp_;

        //- Clipping bound for the local phase fraction, [alphaMin, 1-alphaMin]
        scalar alphaMin_;

        //- Name of the dynamic viscosity field (defaults to "thermo:mu")
        word muName_;


public:

    //- Runtime type information
    TypeName("ErgunPorositySource");


    // Constructors

        //- Construct from components
        ErgunPorositySource
        (
            const word& name,
            const word& modelType,
            const dictionary& dict,
            const fvMesh& mesh
        );

        //- No copy construct
        ErgunPorositySource(const ErgunPorositySource&) = delete;

        //- No copy assignment
        void operator=(const ErgunPorositySource&) = delete;


    //- Destructor
    virtual ~ErgunPorositySource() = default;


    // Member Functions

        //- Add implicit contribution to phase momentum equation
        virtual void addSup
        (
            const volScalarField& alpha,
            const volScalarField& rho,
            fvMatrix<vector>& eqn,
            const label fieldi
        );

        //- Read dictionary
        virtual bool read(const dictionary& dict);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fv
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
