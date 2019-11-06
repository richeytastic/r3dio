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
 * Common interface to r3d::Mesh importers.
 */

#ifndef R3DIO_MESH_IMPORTER_H
#define R3DIO_MESH_IMPORTER_H

#include "IOFormats.h"
#include <r3d/Mesh.h>

namespace r3dio {

class r3dio_EXPORT MeshImporter : public IOFormats
{
public:
    MeshImporter();
    virtual ~MeshImporter(){}

    // On error, null object returned. The filename extension must be supported.
    r3d::Mesh::Ptr load( const std::string& filename);

protected:
    virtual r3d::Mesh::Ptr doLoad( const std::string& filename) = 0;
};  // end class

}   // end namespace

#endif
