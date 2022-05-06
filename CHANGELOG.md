# Changelog

All notable changes to this project will be documented in this file

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/).
This project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.4.0][] ~ 2022-05-06

### Changed

- Use C++17, Minimum CMake 3.10
- Update google test
- added negative return codes for stack buffer pop and circular buffer pop to indicate insufficient size in memory storage.

### Fixed

- some threading issues with the WorkQueue and simpleQueue

### Added

- DualStringMappedVector and StringMappedVector which are specializations of DualMappedVector and MappedVector with the first search term as a std::string and using stable references.
- added a header for suppressing warnings
- added several CI tests and precommit.ci

### Removed

- removed optionalDefinition as we are now using std::optional
