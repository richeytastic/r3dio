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
 * Export to PLY format.
 */

#ifndef R3DIO_PLY_EXPORTER_H
#define R3DIO_PLY_EXPORTER_H

#include "MeshExporter.h"

namespace r3dio {

class r3dio_EXPORT PLYExporter : public MeshExporter
{
public:
    PLYExporter();

protected:
    bool doSave( const r3d::Mesh&, const std::string& filename) override;
};  // end class

}   // end namespace

#endif
