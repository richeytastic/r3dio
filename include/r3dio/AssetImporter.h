/************************************************************************
 * Copyright (C) 2019 Richard Palmer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ************************************************************************/

/**
 * Wraps assimp library to import mesh data (e.g. .obj files)
 */

#ifndef R3DIO_ASSET_IMPORTER_H
#define R3DIO_ASSET_IMPORTER_H

#include "MeshImporter.h"

namespace r3dio {

class r3dio_EXPORT AssetImporter : public MeshImporter
{
public:
    // Set loadTextures true to read in the textures if available.
    // Set whether to fail or not if a mesh is imported containing at least one non-triangular polygon.
    // If setReadFailOnNonTriangles(true) called before read() called, read will return null if any
    // non-triangular polygons are found in the mesh file. Whether or not this function is called
    // prior to importing mesh, warnings about any non-triangular faces found will be printed to stderr.
    // NB AssImp tries to triangulate all meshes on import.
    AssetImporter( bool loadTextures=true, bool failOnNonTriangles=false);
    virtual ~AssetImporter(){}

    // Get the available formats as extension description pairs. These are not
    // enabled by default. Use enableFormat( fmt) below where fmt is the extension
    // (the first item of the available pairs returned here).
    const std::unordered_map<std::string, std::string>& getAvailable() const { return _available;}

    // Returns true if the format is enabled (safe to call multiple times with same parameter).
    bool enableFormat( const std::string& ext);

protected:
    virtual r3d::Mesh::Ptr doLoad( const std::string& filename);

private:
    bool _loadTextures;
    bool _failOnNonTriangles;
    std::unordered_map<std::string, std::string> _available;
};  // end class

}   // end namespace

#endif
