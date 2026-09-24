#!/bin/bash
# Self-consistent update loop: every DT seconds, measure the volume-averaged
# in-bed void fraction "a" (alpha.air over cellZone porosity1), recompute the
# Darcy-Forchheimer (d,f) coefficients from it (same Ergun formulas as
# ../generateCases.sh / ../../../calculation/darcyMethod.cpp), write them into
# fvOptions, then advance the solver by DT more seconds. Repeats NSTEPS times.
set -o pipefail

source /usr/lib/openfoam/openfoam2412/etc/bashrc

set -u

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

EPSILON=0.3731
DP=0.005
DT=0.5
NSTEPS=${1:-5}

HISTORY="history.csv"
if [ ! -f "$HISTORY" ]; then
    echo "step,time,a,D_L,F_L,D_G,F_G" > "$HISTORY"
fi

for i in $(seq 1 "$NSTEPS"); do
    current_time=$(foamListTimes -latestTime)
    echo "=== step $i: current time = $current_time ==="

    postProcess -func volAvgAlpha -latestTime > "log.postProcess_${i}" 2>&1
    a=$(grep "volAverage(porosity1) of alpha.air =" "log.postProcess_${i}" | tail -1 | awk '{print $NF}')
    if [ -z "$a" ]; then
        echo "Failed to read alpha average at step $i" >&2
        exit 1
    fi

    read -r D_L F_L D_G F_G <<< "$(awk -v eps="$EPSILON" -v dp="$DP" -v a="$a" 'BEGIN{
        eps3 = eps^3
        D_L = 150*(1-eps)^2 / ((1-a)^2 * eps3 * dp^2)
        F_L = 3.5*(1-eps)   / ((1-a)   * eps3 * dp)
        D_G = 150*(1-eps)^2 / (a^2     * eps3 * dp^2)
        F_G = 3.5*(1-eps)   / (a       * eps3 * dp)
        printf "%.6g %.6g %.6g %.6g", D_L, F_L, D_G, F_G
    }')"

    foamDictionary -entry "porosity_air.explicitPorositySourceCoeffs.d" -set "($D_G $D_G $D_G)" system/fvOptions >/dev/null
    foamDictionary -entry "porosity_air.explicitPorositySourceCoeffs.f" -set "($F_G $F_G $F_G)" system/fvOptions >/dev/null
    foamDictionary -entry "porosity_water.explicitPorositySourceCoeffs.d" -set "($D_L $D_L $D_L)" system/fvOptions >/dev/null
    foamDictionary -entry "porosity_water.explicitPorositySourceCoeffs.f" -set "($F_L $F_L $F_L)" system/fvOptions >/dev/null

    echo "$i,$current_time,$a,$D_L,$F_L,$D_G,$F_G" >> "$HISTORY"
    echo "    a=$a  D_L=$D_L F_L=$F_L D_G=$D_G F_G=$F_G"

    next_end=$(awk -v t="$current_time" -v dt="$DT" 'BEGIN{printf "%.10g", t+dt}')
    foamDictionary -entry endTime -set "$next_end" system/controlDict >/dev/null

    myTwoPhaseEulerFoam > "log.step_${i}" 2>&1
    solver_status=$?

    if ! grep -q "^End$" "log.step_${i}"; then
        echo "Step $i did not reach 'End' in log (solver_status=$solver_status). Aborting loop." >&2
        exit 1
    fi
    # solver_status may be non-zero due to a known post-run cleanup crash
    # (corrupted double-linked list) even though the run completed correctly
    # (log reaches "End" and the new time directory is written).
done

echo "=== Loop finished. History: ==="
cat "$HISTORY"
