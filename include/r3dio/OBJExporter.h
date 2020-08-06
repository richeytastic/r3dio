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
 * Export to Wavefront OBJ format.
 */

#ifndef R3DIO_OBJ_EXPORTER_H
#define R3DIO_OBJ_EXPORTER_H

#include "MeshExporter.h"

namespace r3dio {

class r3dio_EXPORT OBJExporter : public MeshExporter
{
public:
    /**
     * If saveTextureAsPNG is false, textures are saved in JPEG format.
     */
    explicit OBJExporter( bool saveTextureAsPNG=false);

protected:
    bool doSave( const r3d::Mesh&, const std::string& filename) override;

private:
    const bool _asPNG;
};  // end class

}   // end namespace

#endif
