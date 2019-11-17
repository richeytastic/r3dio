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
 * Export model data using the AssImp library.
 */

#ifndef R3DIO_ASSET_EXPORTER_H
#define R3DIO_ASSET_EXPORTER_H

#include "MeshExporter.h"

namespace r3dio {

class r3dio_EXPORT AssetExporter : public MeshExporter
{
public:
    AssetExporter();
    virtual ~AssetExporter(){}

    // Get the available formats as extension description pairs. These are not
    // enabled by default. Use enableFormat( fmt) below where fmt is the extension
    // (the first item of the available pairs returned here).
    const std::unordered_map<std::string, std::string>& getAvailable() const { return _available;}

    // Returns true if the format is enabled.
    bool enableFormat( const std::string& ext);

protected:
    virtual bool doSave( const r3d::Mesh&, const std::string& filename);

private:
    std::unordered_map<std::string, std::string> _available;
};  // end class

}   // end namespace

#endif
