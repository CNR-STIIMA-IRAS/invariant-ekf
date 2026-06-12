# Changelog

All notable changes to this project are documented in this file.

The format is based on Keep a Changelog.

## [Unreleased]

## [2026-06-12]

### Added
- Added [CMakePresets.json](CMakePresets.json) with `default-release`, `default-debug`, and `core-only-release` configure/build presets.
- Added CTest integration for speed binaries (`inekf_propagation_speed`, `inekf_correction_speed`) when `BUILD_TESTING=ON` and speed targets are enabled.
- Added this changelog file for future release tracking.

### Changed
- Modernized build system to CMake 3.16+ with target-based dependency management and package exports.
- Made Boost optional for non-core binaries: examples and speed tests are disabled when Boost is unavailable or explicitly turned off.
- Updated package config usage to exported target consumption (`find_package(inekf REQUIRED)` and `target_link_libraries(... inekf::inekf)`).
- Updated C/C++ file headers to BSD-3-Clause style with additional attribution:
  - Copyright (c) 2026, Nicola Pedrocchi, CNR.
