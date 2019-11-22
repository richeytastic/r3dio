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

#include <IOHelpers.h>
#include <AssetImporter.h>
#include <AssetExporter.h>
#include <PLYExporter.h>
#include <OBJExporter.h>
#include <U3DExporter.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

r3d::Mesh::Ptr r3dio::loadMesh( const std::string &fname)
{
    r3d::Mesh::Ptr model;
    if ( !fname.empty())
    {
        AssetImporter aimp(true, true);
        aimp.enableFormat("3ds");
        aimp.enableFormat("3mf");
        aimp.enableFormat("dae");
        aimp.enableFormat("obj");
        aimp.enableFormat("off");
        aimp.enableFormat("ply");
        aimp.enableFormat("stl");
        aimp.enableFormat("x3d");
        model = aimp.load( fname);
    }   // end if
    return model;
}   // end loadMesh


bool r3dio::saveMesh( const r3d::Mesh &mesh, const std::string &fn)
{
    if ( !boost::filesystem::path(fn).has_extension())  // Check for filename extension
        return false;
    const boost::filesystem::path fpath( fn);
    const std::string ext = boost::algorithm::to_lower_copy( fpath.extension().string());
    if ( ext == "ply")
        return saveAsPLY( mesh, fn);
    else if ( ext == "obj")
        return saveAsOBJ( mesh, fn);
    else if ( ext == "u3d")
        return saveAsU3D( mesh, fn);
    else if ( ext == "stl")
        return saveAsSTL( mesh, fn);
    else if ( ext == "3ds")
        return saveAs3DS( mesh, fn);
    else
    {
        AssetExporter aexp; // Otherwise try to save in some other kind of format...
        if ( aexp.enableFormat( ext))
            return aexp.save( mesh, fn);
    }   // end else

    return false;   // Nothing worked!
}   // end saveMesh


bool r3dio::saveAsPLY( const r3d::Mesh &mesh, const std::string &fn)
{
    const std::string fname = boost::filesystem::path(fn).replace_extension("ply").string();
    return PLYExporter().save( mesh, fname);
}   // end saveAsPLY


bool r3dio::saveAsOBJ( const r3d::Mesh &mesh, const std::string &fn)
{
    const std::string fname = boost::filesystem::path(fn).replace_extension("obj").string();
    return OBJExporter().save( mesh, fname);
}   // end saveAsOBJ


bool r3dio::saveAsU3D( const r3d::Mesh &mesh, const std::string &fn)
{
    const std::string fname = boost::filesystem::path(fn).replace_extension("u3d").string();
    return U3DExporter().save( mesh, fname);
}   // end saveAsU3D


bool r3dio::saveAsSTL( const r3d::Mesh &mesh, const std::string &fn)
{
    const std::string fname = boost::filesystem::path(fn).replace_extension("stl").string();
    AssetExporter aexp;
    aexp.enableFormat("stl");
    return aexp.save( mesh, fname);
}   // end saveAsSTL


bool r3dio::saveAs3DS( const r3d::Mesh &mesh, const std::string &fn)
{
    if ( mesh.numFaces() > 65536)
    {
        std::cerr << "[WARNING] r3dio::saveAs3DS: Mesh contains more than 65536 faces (limit for 3DS format)." << std::endl;
        return false;
    }   // end if
    const std::string fname = boost::filesystem::path(fn).replace_extension("3ds").string();
    AssetExporter aexp;
    aexp.enableFormat("3ds");
    return aexp.save( mesh, fname);
}   // end saveAs3DS
