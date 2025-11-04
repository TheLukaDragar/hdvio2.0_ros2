# RPG VFR Pinhole Dataset

This directory should contain the VFR (Visual Feature Recognition) pinhole camera dataset.

## Required Structure

The tests expect this directory to contain:
- Camera calibration file (calib.yaml)
- Image sequences
- Ground truth data

## How to obtain the data

The actual test data is not included in the repository.
You can either:
1. Use your own image sequences matching the expected format
2. Download sample datasets from the SVO Pro project if available
3. Comment out the tests that require this data

For now, empty placeholder files are created to allow the package to build.
