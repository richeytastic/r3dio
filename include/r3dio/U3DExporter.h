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
 * Export r3d::Mesh objects to U3D format via creation
 * of IDTF files (see r3dio::IDTFExporter).
 *
 * The IDTFConverter that can convert .idtf files to .u3d files must be
 * available on the PATH. IDTFConverter can be found at
 * https://www2.iaas.msu.ru/tmp/u3d/ (thanks to Michail Vidiassov).
 *
 * Richard Palmer
 * August 2017
 */

#ifndef r3dio_U3D_EXPORTER_H
#define r3dio_U3D_EXPORTER_H

#include "MeshExporter.h"

namespace r3dio {

class r3dio_EXPORT U3DExporter : public MeshExporter
{
public:
    // Defines name of the IDTFConverter program which must be on the path.
    // Defaults to "IDTFConverter" ("IDTFConverter.exe" on Windows).
    static std::string IDTFConverter;   

    // Returns true iff IDTFConverter is on the PATH.
    static bool isAvailable();

    // U3D conversion produces an IDTF file and a tga texture.
    // Normally, both are destroyed immediately after saving the
    // U3D model. Set delOnDestroy to false to retain these files.
    // Setting media9 true will transform coordinates as (a,b,c) --> (a,-c,b).
    U3DExporter( bool delOnDestroy=true, bool media9=false);

protected:
    virtual bool doSave( const r3d::Mesh&, const std::string& filename);

private:
    const bool _delOnDestroy;
    const bool _media9;
};  // end class

}   // end namespace

#endif
