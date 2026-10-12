#include "readGravitationalAcceleration.H"
#include "readhRef.H"

Info<< "Creating twoPhaseSystem\n" << endl;

twoPhaseSystem fluid(mesh, g);

phaseModel& phase1 = fluid.phase1();
phaseModel& phase2 = fluid.phase2();

volScalarField& alpha1 = phase1;
volVectorField& U1 = phase1.U();
volVectorField& U2 = phase2.U();

volScalarField& p = phase1.thermo().p();

dimensionedScalar pMin
(
    "pMin",
    dimPressure,
    fluid
);

// Interphase drag is multiplied by dragMask. In the cellZones listed in
// phaseProperties "dragFreeCellZones" (e.g. the packed bed, to compare with
// drag-free 1D models such as Lipinski) the drag is switched off, but only
// where both phases are really present (min(alpha1, alpha2) above
// "dragFreeAlphaMin", ramped up to 2*dragFreeAlphaMin). In nearly
// single-phase cells the drag must stay on: every term of the momentum
// equation of the vanishing phase is scaled by its alpha, and without drag
// its velocity is no longer determined (it ran away to O(100 m/s)).
// dragMask is updated in updateDragMask.H
const scalar dragFreeAlphaMin
(
    fluid.getOrDefault<scalar>("dragFreeAlphaMin", 0.05)
);

volScalarField dragFreeZone
(
    IOobject
    (
        "dragFreeZone",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::NO_WRITE
    ),
    mesh,
    dimensionedScalar("zero", dimless, 0),
    fvPatchFieldBase::zeroGradientType()
);

volScalarField dragMask
(
    IOobject
    (
        "dragMask",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::NO_WRITE
    ),
    mesh,
    dimensionedScalar("one", dimless, 1),
    fvPatchFieldBase::zeroGradientType()
);

{
    const wordRes dragFreeZones
    (
        fluid.getOrDefault<wordRes>("dragFreeCellZones", wordRes())
    );

    if (!dragFreeZones.empty())
    {
        const labelList zoneIDs(mesh.cellZones().indices(dragFreeZones));

        if (zoneIDs.empty())
        {
            FatalErrorInFunction
                << "dragFreeCellZones " << dragFreeZones
                << " match no cellZone. Available: "
                << mesh.cellZones().names() << exit(FatalError);
        }

        label nCells = 0;
        for (const label zonei : zoneIDs)
        {
            for (const label celli : mesh.cellZones()[zonei])
            {
                dragFreeZone[celli] = 1;
                ++nCells;
            }
        }
        dragFreeZone.correctBoundaryConditions();

        Info<< "Interphase drag switched off in cellZones " << dragFreeZones
            << " (" << returnReduce(nCells, sumOp<label>()) << " cells)"
            << " where min(alpha1, alpha2) > " << dragFreeAlphaMin << nl
            << endl;
    }
}

#include "gh.H"

Info<< "Reading field p_rgh\n" << endl;
volScalarField p_rgh
(
    IOobject
    (
        "p_rgh",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

volVectorField U
(
    IOobject
    (
        "U",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    fluid.U()
);

label pRefCell = 0;
scalar pRefValue = 0.0;
setRefCell
(
    p,
    p_rgh,
    pimple.dict(),
    pRefCell,
    pRefValue
);
mesh.setFluxRequired(p_rgh.name());
mesh.setFluxRequired(alpha1.name());

Info<< "Creating field dpdt\n" << endl;
volScalarField dpdt
(
    IOobject
    (
        "dpdt",
        runTime.timeName(),
        mesh
    ),
    mesh,
    dimensionedScalar(p.dimensions()/dimTime, Zero)
);


Info<< "Creating field kinetic energy K\n" << endl;
volScalarField K1(IOobject::groupName("K", phase1.name()), 0.5*magSqr(U1));
volScalarField K2(IOobject::groupName("K", phase2.name()), 0.5*magSqr(U2));

#include "createMRF.H"
#include "createFvOptions.H"
