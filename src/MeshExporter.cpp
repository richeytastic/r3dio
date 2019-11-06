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

#include <MeshExporter.h>
using r3dio::MeshExporter;

MeshExporter::MeshExporter() : r3dio::IOFormats() { }   // end ctor


bool MeshExporter::save( const r3d::Mesh& mesh, const std::string& fname)
{
    if ( fname.empty())
    {
        setErr( "Empty filename passed to RModelIO::MeshExporter::save!");
        return false;
    }   // end if

    setErr(""); // Clear error
    if ( !isSupported( fname))
    {
        setErr( fname + " has an unsupported file extension for exporting!");
        return false;
    }   // end if

    return doSave( mesh, fname);    // virtual
}   // end save
