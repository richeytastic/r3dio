/************************************************************************
 * Copyright (C) 2020 Richard Palmer
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

#ifndef R3DIO_IO_HELPERS_H
#define R3DIO_IO_HELPERS_H

#include "r3dio_Export.h"
#include <r3d/Mesh.h>

namespace r3dio {

// Load a triangulated mesh from 3DS, 3MF, DAE, OBJ, OFF, PLY, STL, or X3D file formats.
r3dio_EXPORT r3d::Mesh::Ptr loadMesh( const std::string &fname);

// Save a triangulated mesh with format determined from the given filename's extension.
// The extension should be one of the available formats listed n the below specific saveAs...
// functions. If any other extension is used, the generic AssetExporter tries to save in
// the given format, but if a suitable exporter isn't found, false is returned.
r3dio_EXPORT bool saveMesh( const r3d::Mesh&, const std::string &filename);

/*** SPECIFIC SAVE FORMATS FOLLOW ***/

// Save mesh in PLY format; file extension set/replaced as "ply".
r3dio_EXPORT bool saveAsPLY( const r3d::Mesh&, const std::string &filename);

// Save mesh in OBJ format; file extension set/replaced as "obj".
r3dio_EXPORT bool saveAsOBJ( const r3d::Mesh&, const std::string &filename, bool asPNG=false);

// Save mesh in STL format; file extension set/replaced as "stl".
r3dio_EXPORT bool saveAsSTL( const r3d::Mesh&, const std::string &filename);

// Save mesh in 3DS format; file extension set/replaced as "3ds".
r3dio_EXPORT bool saveAs3DS( const r3d::Mesh&, const std::string &filename);

// Save mesh in U3D format; file extension set/replaced as "u3d".
r3dio_EXPORT bool saveAsU3D( const r3d::Mesh&, const std::string &filename);

}   // end namespace

#endif
