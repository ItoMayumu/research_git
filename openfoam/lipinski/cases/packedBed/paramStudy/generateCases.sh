#!/bin/bash
# Generate parametric packedBed cases from conditions.csv.
# Usage: ./generateCases.sh
#
# conditions.csv gives jL, jG in cm/s and a = in-bed void fraction (alpha,
# measured experimentally). Homogeneous-flow inlet mapping
# (chosen for this study, S = uG/uL = 1):
#   alpha.air(INLET) = jG / (jG + jL)         (unit-independent ratio)
#   U.air = U.water = (jG + jL) / 100         [m/s]   (along +Z, matching the pipe axis)
#
# Darcy-Forchheimer porous-zone coefficients (d, f) are computed from the
# case's experimental "a", following the same Ergun-based formulas as
# ../../../calculation/darcyMethod.cpp (epsilon, dp kept fixed to match it):
#   D_L = 150*(1-eps)^2 / ((1-a)^2 * eps^3 * dp^2)   F_L = 3.5*(1-eps) / ((1-a) * eps^3 * dp)
#   D_G = 150*(1-eps)^2 / (a^2     * eps^3 * dp^2)   F_G = 3.5*(1-eps) / (a     * eps^3 * dp)
#
# Mesh is shared across cases (symlinked from ../standard), so blockMesh /
# topoSet do not need to be re-run per case.

#conditions.csvに書かれた内容のケースディレクトリを作成し、#各ケースディレクトリ内のU.air, U.water, alpha.air, fvOptionsを#conditions.csvの内容に応じて書き換えるスクリプト
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEMPLATE="$SCRIPT_DIR/../standard"
CONDITIONS="$SCRIPT_DIR/conditions.csv"

# Must match calculation/darcyMethod.cpp
EPSILON=0.3731
DP=0.005

if [ ! -d "$TEMPLATE/0" ]; then
    echo "Template case not found at $TEMPLATE" >&2
    exit 1
fi

# Normalize conditions.csv into "caseId,jL,jG,a" quadruples:
#   - accepts comma- or tab-separated fields
#   - skips blank lines and the header row
#   - if only 3 fields (jL,jG,a) are given, auto-numbers the case
awk '
{
    n = split($0, f, /[,\t]+/)
    for (i = 1; i <= n; i++) { gsub(/^[ \t\r]+|[ \t\r]+$/, "", f[i]) }
    while (n > 0 && f[n] == "") n--
    if (n == 0) next
    if (f[1] ~ /^[Cc]ase$/) next
    if (n == 3) { row++; print row","f[1]","f[2]","f[3] }
    else if (n >= 4) { print f[1]","f[2]","f[3]","f[4] }
}' "$CONDITIONS" | while IFS=',' read -r caseId jL jG a; do
    [ -z "$caseId" ] && continue

    tag="jL${jL}_jG${jG}"
    tag="${tag//./p}"
    name="case${caseId}_${tag}"
    dest="$SCRIPT_DIR/$name"

    echo "=== $name (jL=$jL, jG=$jG) ==="
    rm -rf "$dest"
    mkdir -p "$dest"

    cp -r "$TEMPLATE/0" "$dest/0"
    cp -r "$TEMPLATE/system" "$dest/system"
    mkdir -p "$dest/constant"
    cp "$TEMPLATE"/constant/g \
       "$TEMPLATE"/constant/phaseProperties \
       "$TEMPLATE"/constant/thermophysicalProperties.air \
       "$TEMPLATE"/constant/thermophysicalProperties.water \
       "$TEMPLATE"/constant/turbulenceProperties.air \
       "$TEMPLATE"/constant/turbulenceProperties.water \
       "$dest/constant/"
    ln -s "$(realpath --relative-to="$dest/constant" "$TEMPLATE/constant/polyMesh")" "$dest/constant/polyMesh"

    # jL, jG are in cm/s in conditions.csv; U must be in m/s for OpenFOAM
    U=$(awk -v jl="$jL" -v jg="$jG" 'BEGIN{printf "%.6g", (jl+jg)/100}')
    alpha=$(awk -v jl="$jL" -v jg="$jG" 'BEGIN{printf "%.6g", jg/(jl+jg)}')

    foamDictionary -entry internalField -set "uniform (0 0 $U)" "$dest/0/U.air" >/dev/null
    foamDictionary -entry "boundaryField.INLET.value" -set "uniform (0 0 $U)" "$dest/0/U.air" >/dev/null
    foamDictionary -entry internalField -set "uniform (0 0 $U)" "$dest/0/U.water" >/dev/null
    foamDictionary -entry "boundaryField.INLET.value" -set "uniform (0 0 $U)" "$dest/0/U.water" >/dev/null
    # OUTLET.value is only a fallback used if flow reverses at that patch;
    # keep it consistent with this case's U instead of leaving the template's stale value.
    foamDictionary -entry "boundaryField.OUTLET.value" -set "uniform (0 0 $U)" "$dest/0/U.air" >/dev/null
    foamDictionary -entry "boundaryField.OUTLET.value" -set "uniform (0 0 $U)" "$dest/0/U.water" >/dev/null
    foamDictionary -entry "boundaryField.INLET.value" -set "uniform $alpha" "$dest/0/alpha.air" >/dev/null

    foamDictionary -entry startFrom -set "startTime" "$dest/system/controlDict" >/dev/null
    foamDictionary -entry startTime -set "0" "$dest/system/controlDict" >/dev/null

    # Darcy-Forchheimer coefficients from this case's experimental in-bed
    # void fraction "a" (same formulas as calculation/darcyMethod.cpp)
    read -r D_L F_L D_G F_G <<< "$(awk -v eps="$EPSILON" -v dp="$DP" -v a="$a" 'BEGIN{
        eps3 = eps^3
        D_L = 150*(1-eps)^2 / ((1-a)^2 * eps3 * dp^2)
        F_L = 3.5*(1-eps)   / ((1-a)   * eps3 * dp)
        D_G = 150*(1-eps)^2 / (a^2     * eps3 * dp^2)
        F_G = 3.5*(1-eps)   / (a       * eps3 * dp)
        printf "%.6g %.6g %.6g %.6g", D_L, F_L, D_G, F_G
    }')"

    foamDictionary -entry "porosity_air.explicitPorositySourceCoeffs.d" -set "($D_G $D_G $D_G)" "$dest/system/fvOptions" >/dev/null
    foamDictionary -entry "porosity_air.explicitPorositySourceCoeffs.f" -set "($F_G $F_G $F_G)" "$dest/system/fvOptions" >/dev/null
    foamDictionary -entry "porosity_water.explicitPorositySourceCoeffs.d" -set "($D_L $D_L $D_L)" "$dest/system/fvOptions" >/dev/null
    foamDictionary -entry "porosity_water.explicitPorositySourceCoeffs.f" -set "($F_L $F_L $F_L)" "$dest/system/fvOptions" >/dev/null

    echo "    -> U=$U m/s, alpha.air(inlet)=$alpha, a(in-bed)=$a"
    echo "    -> air:   d=$D_G  f=$F_G"
    echo "    -> water: d=$D_L  f=$F_L"
done

echo "Done. Run each case with: (cd caseN_... && myTwoPhaseEulerFoam > log 2>&1)"
