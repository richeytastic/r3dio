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
 * Common interface to Mesh exporters.
 */

#ifndef R3DIO_MESH_EXPORTER_H
#define R3DIO_MESH_EXPORTER_H

#include "IOFormats.h"
#include <r3d/Mesh.h>

namespace r3dio {

class r3dio_EXPORT MeshExporter : public IOFormats
{
public:
    MeshExporter();
    virtual ~MeshExporter(){}

    // Returns true on success. The filename extension must be supported.
    bool save( const r3d::Mesh&, const std::string& filename);

protected:
    virtual bool doSave( const r3d::Mesh&, const std::string& filename) = 0;
};  // end class

}   // end namespace

#endif
