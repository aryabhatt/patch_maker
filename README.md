# TIFF Viewer - Multi-Page Float32 TIFF Viewer

A command-line application for viewing multi-page TIFF files with float32 data. The application automatically rescales float32 data to unsigned int (0-255) for display.

## Features

- Loads multi-page TIFF files with float32 data
- Automatic rescaling from float32 to uint8 for display
- Per-page dynamic range adjustment (min/max normalization)
- Keyboard and mouse navigation
- Lightweight Qt-based GUI

## Building

```bash
cmake -B build -S .
cmake --build build
```

## Usage

```bash
./build/tiffview <tiff_file>
```

### Example

```bash
./build/tiffview /path/to/your/data.tif
```

## Keyboard Controls

| Key | Action |
|-----|--------|
| Up Arrow / Down Arrow | Navigate one page forward/backward |
| Page Up / Page Down | Jump 5 pages forward/backward |
| Home | Go to first page |
| End | Go to last page |
| Mouse Wheel | Navigate pages (hold Ctrl to jump 5 pages) |

## Data Handling

- **Input**: Multi-page TIFF files with 32-bit floating-point data
- **Rescaling**: Each page is independently rescaled using min/max normalization:
  - Find minimum and maximum values in the current page
  - Map [min, max] → [0, 255] for display
  - Handles constant-value images gracefully

## Requirements

- Qt6 (Core, Gui, Widgets)
- libtiff
- C++20 compiler

## Implementation Details

The viewer uses the existing `tomocam::Array` and TIFF I/O infrastructure:
- Reads float32 TIFF data using `tomocam::tiff::read<float>()`
- Converts to grayscale images with per-page normalization
- Displays using Qt's QGraphicsView for efficient rendering

## Notes

- The rescaling is performed per-page, so each page is normalized independently
- This ensures optimal contrast for each individual page
- The original float32 data is preserved in memory (rescaling only affects display)
