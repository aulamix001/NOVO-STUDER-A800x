#pragma once
#include <cmath>

namespace a800 {
// Supplied user specification; not yet checked against the original manuals.
inline constexpr double kOperatingFluxNwbM = 320.0;
inline constexpr double kNominalPeakFluxNwbM = 640.0;
inline constexpr double kCalibrationOutputDbu = -3.0;
inline constexpr double kDefaultAlignmentDbfs = -18.0;
inline constexpr double kVarispeedRangeSemitones = 7.5;

// TODO-MANUAL: provisional ranges, NOT hardware specifications.
inline constexpr double kTrimRangeDb = 3.0;
// TODO-MANUAL: conservative placeholder pending bias adjustment range.
inline constexpr double kBiasOffsetRangeDb = 1.0;
// TODO-MANUAL: conservative placeholder pending CH. ADJUST range.
inline constexpr double kFineBiasRangeDb = 0.5;
// TODO-MANUAL: software alignment range; not a hardware control.
inline constexpr double kAlignmentRangeDbfs[2] = {-24.0, -12.0};
// TODO-MANUAL: VU ballistics need verification against the specified standard.
inline constexpr double kVuIntegrationSeconds = 0.300;
// TODO-MANUAL: noise and modulation remain disabled until measured/calibrated.
inline constexpr double kTapeNoiseAmplitude = 0.0;
inline constexpr double kTransportModulationDepth = 0.0;

inline double fluxFromVu(double vu) noexcept {
    return kOperatingFluxNwbM * std::pow(10.0, vu / 20.0);
}
inline double calibrateVu(double jumperDbu) noexcept {
    return kCalibrationOutputDbu - jumperDbu;
}
inline double nominalSpeedIps(int machineIndex, bool fast) noexcept {
    constexpr double slow[3] = {3.75, 7.5, 15.0};
    return (machineIndex >= 0 && machineIndex < 3)
        ? slow[machineIndex] * (fast ? 2.0 : 1.0) : 0.0;
}
} // namespace a800
