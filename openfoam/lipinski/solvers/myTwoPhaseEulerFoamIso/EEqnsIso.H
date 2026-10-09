// Isothermal variant of EEqns.H: the phase energy equations are not solved,
// so T (and he) stay at their initial values. Only the thermo update is kept,
// because rho(p, T) of the compressible phases still has to follow p.
{
    thermo1.correct();
    thermo2.correct();
}
