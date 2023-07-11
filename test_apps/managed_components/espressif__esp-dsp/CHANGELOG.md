# Esp-dsp Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased] 

### Added
- Target tests

### Fixed
- Add Bi-Quad for esp32s3 into common CMakeLists.txt 
### Changed
- Update documentation build


### Removed

## [1.4.0] 2023-05-29

### Added
- Complex signal generator dsps_cplx_gen()
- FIR f32 filter optimized for esp32s3 
- Memcpy and Memset optimized for esp32s3

### Fixed
- Fix in tests to pass
- Minimum coeffcient length for fird_s16
- Include malloc.h into dsps_fft4r_fc32_ansi.c
- Fix for calculation length for dsps_corr_f32_axxx

### Changed

### Removed

## [1.3.0] 2023-03-10

### Added

- Fixed point FIR filter with decimation
- Update tag to 1.2.1 for component manager
- Extend dsp_power_of_two() to 32-bit value 

### Fixed

- add various links to idf_component.yml, exclude unnecessary files
- cmake: update component CMakeLists.txt file syntax to IDF v4.x+
- docs: move instructions for contributors into CONTRIBUTING.md
- docs: update README.md to use IDF component manager for installation
- drop IDF v4.0 and v4.1 support, add a CI build with IDF release/v5.0
- examples: remove GNU Make instructions from README files
- examples: allow examples to be installed from the component manager
- Fix for fft_cplx2reC_f32 function
- Wno-format-fix related errors
- Wrong path for extra component directory

### Changed

### Removed

## [1.2.0] 2022-09-22

