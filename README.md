# Tomographic Patch Extractor

A simple Qt-based application for visualizing and extracting image patches from tomographic reconstructions. This tool is designed to assist in creating datasets for machine learning models by enabling easy selection and saving of 256x256 patches from volumetric data.

## Features

- Load image stacks from:
  - **HDF5 files** (currently hardcoded to `/recon` dataset)
  - **TIFF stacks**
- Scroll through slices interactively
- Click to select a pixel center for a (256x256) patch
- Enable/disable patch-saving mode with a toggle switch
- Patches saved as:
- - patch00000.tif
- - patch00001.tif
- - ...
- Designed for fast dataset creation for training ML models

## Installation

### Requirements

- Qt6
- libtiff
- hdf5
- cmake

### Build from source

```bash
cmake --preset release
cmake --build --preset release
```

