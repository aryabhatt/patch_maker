#include <filesystem>

#include "array.h"
#include "hdf5/reader.h"
#include "tiff/tiffio.h"

#ifndef LOADER__H
#define LOADER__H

namespace fs = std::filesystem;

namespace tomocam {
    Array<float> loader(const std::string &filename) {
        // check for file extension (h5 or tif)
        if (fs::path(filename).extension() == ".h5") {
            auto reader = h5::Reader(filename.c_str());
            return reader.read2<float>("recon");
        } else if (fs::path(filename).extension() == ".tif" ||
                   fs::path(filename).extension() == ".tiff") {
            return tiff::read<float>(filename);
        } else {
            throw std::runtime_error("Unsupported file format: " +
                                     fs::path(filename).extension().string());
        }
    }
} // namespace tomocam

#endif // LOADER__H
