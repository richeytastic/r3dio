/************************************************************************
 * Copyright (C) 2021 Richard Palmer
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

#include <U3DExporter.h>
#include <IDTFExporter.h>
#include <cassert>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <boost/filesystem/operations.hpp>
#ifdef _WIN32
#include <boost/process/windows.hpp>    // For hiding console window
#endif
#include <boost/process.hpp>    // Requires at least boost 1.64+
using r3dio::IDTFExporter;
using r3dio::U3DExporter;
using r3d::Mesh;
using r3d::Colour;
namespace bp = boost::process;


std::string U3DExporter::IDTFConverter( "IDTFConverter"); // public static


// public static
bool U3DExporter::isAvailable()
{
    if ( boost::filesystem::exists( IDTFConverter))
        return true;
    const boost::filesystem::path exepath = bp::search_path( IDTFConverter);
    return !exepath.empty();
}   // end isAvailable


// public
U3DExporter::U3DExporter( bool delOnDestroy, bool m9, const Colour &ems)
    : r3dio::MeshExporter(), _delOnDestroy(delOnDestroy), _media9(m9), _ems(ems)
{
    if ( IDTFConverter.empty())
        IDTFConverter = "IDTFConverter";

    if ( isAvailable())
        addSupported( "u3d", "Universal 3D");
    else
        std::cerr << "[WARNING] r3dio::U3DExporter: U3D export disabled; IDTFConverter not found on PATH!" << std::endl;
}   // end ctor


namespace {

bool convertIDTF2U3D( const std::string& idtffile, const std::string& u3dfile)
{
    // -debuglevel 0    No debug dump
    // -pq 1000         Position quality MAX
    // -tcq 1000        Texture coordinates quality MAX
    // -gq 1000         Geometry quality MAX
    // -tq 100          Texture quality MAX
    // -en 1            Enable normals exclusion 
    // -eo 65535        Export everything
    static const std::string PARAMS = " -debuglevel 0 -pq 1000 -tcq 1000 -gq 1000 -tq 100 -en 1 -eo 65535 ";
    std::ostringstream cmd;
    cmd << "\"" << U3DExporter::IDTFConverter << "\"" << PARAMS
        << "-input \"" << idtffile << "\" -output \"" << u3dfile << "\"";
#ifdef _WIN32
    const std::string pexe = "cmd /Q /S /C \"" + cmd.str() + " > nul\"";
#else
    const std::string pexe = cmd.str() + " > /dev/null";
#endif
    //std::cerr << "[INFO] r3dio::U3DExporter executing: " << pexe << std::endl;
    bool success = false;
    try
    {
        success = std::system( pexe.c_str()) == 0;
    }   // end try
    catch ( const std::exception& e)
    {
        std::cerr << "Failed on read of byte from forked process " << std::endl;
        std::cerr << e.what() << std::endl;
        success = false;
    }   // end catch

    return success;
}   // end convertIDTF2U3D
}   // end namespace


// protected
bool U3DExporter::doSave( const Mesh& mesh, const std::string& filename)
{
    static const std::string istr = "[INFO] r3dio::U3DExporter::doSave: ";
    static const std::string wstr = "[WARNING] r3dio::U3DExporter::doSave: ";
    bool savedOkay = true;

    // First save to intermediate IDTF format.
    IDTFExporter idtfExporter( _delOnDestroy, _media9, _ems);
    const std::string idtffile = boost::filesystem::path(filename).replace_extension("idtf").string();
    if ( !idtfExporter.save( mesh, idtffile))
    {   
        setErr( idtfExporter.err());
        savedOkay = false;
    }   // end if
    else if ( !convertIDTF2U3D( idtffile, filename))
    {
        setErr("Unable to convert from IDTF format to U3D format!");
        savedOkay = false;
    }   // end if

    if ( !savedOkay)
        std::cerr << wstr << err() << std::endl;
#ifndef NDEBUG
    else
        std::cerr << istr << "Converted IDTF to U3D" << std::endl;
#endif

    return savedOkay;
}   // end doSave
