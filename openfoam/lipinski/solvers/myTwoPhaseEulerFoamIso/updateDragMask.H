// Drag is off (dragMask = 0) only inside the dragFree cellZones and only where
// both phases are present; see createFields.H
dragMask =
    scalar(1)
  - dragFreeZone
   *min
    (
        max((min(alpha1, alpha2) - dragFreeAlphaMin)/dragFreeAlphaMin, scalar(0)),
        scalar(1)
    );
