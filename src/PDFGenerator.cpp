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

#include <PDFGenerator.h>
#include <U3DExporter.h>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/process.hpp>
#include <boost/process/start_dir.hpp>
#ifdef _WIN32
#include <boost/process/windows.hpp>
#endif
#include <cassert>
#include <fstream>
#include <iomanip>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
using r3dio::PDFGenerator;
using r3dio::U3DExporter;
using r3d::Mesh;
namespace bp = boost::process;
namespace bfs = boost::filesystem;

std::string PDFGenerator::pdflatex( "pdflatex"); // public static


// public
bool PDFGenerator::isAvailable()
{
    if ( bfs::exists( pdflatex))
        return true;
    const bfs::path genpath = bp::search_path(pdflatex);
    return !genpath.empty();
}   // end isAvailable


// public
PDFGenerator::PDFGenerator( bool remGen) : _remGen(remGen)
{
    if ( pdflatex.empty())
        pdflatex = "pdflatex";
}   // end ctor


namespace {
bool runcmd( const std::string &cmd, const std::string &ppath)
{
    bp::ipstream out;
#ifdef _WIN32
    bp::child c( cmd, bp::std_out > out, bp::windows::hide, bp::start_dir=ppath);
#else
    bp::child c( cmd, bp::std_out > out, bp::start_dir=ppath);
#endif
    c.wait();
    return c.exit_code() == 0;
}   // end runcmd
}   // end namespace


// public
bool PDFGenerator::operator()( const std::string& texfile, bool remtexfile)
{
    if ( !isAvailable())
    {
        std::cerr << "[WARNING] r3dio::PDFGenerator: pdflatex not available! PDF generation disabled." << std::endl;
        return false;
    }   // end if

    bfs::path tpath = texfile;
    bool success = false;
    std::ostringstream errMsg;

    // Get the parent path of the texfile to run pdflatex in.
    const std::string ppath = tpath.parent_path().string();
    const std::string cmd = "\"" + pdflatex + "\" --shell-escape -interaction batchmode -output-directory \"" + ppath + "\" \"" + texfile + "\"";
    try
    {
        success = runcmd( cmd, ppath);
        if ( success)   // Run twice because of the annoyances of pdflatex not being able to work out numbers of pages...
            success = runcmd( cmd, ppath);
    }   // end try
    catch ( const std::exception& e)
    {
        errMsg << "[EXCEPTION!] r3dio::PDFGenerator:" << e.what();
        success = false;
    }   // end ctch

    if ( !success)
        std::cerr << "[FAILED] r3dio::PDFGenerator: " << cmd << std::endl;
    if ( !errMsg.str().empty())
        std::cerr << errMsg.str() << std::endl;

    bool remGen = _remGen;
    // Don't remove pdflatex generated files on failure if this is a debug build
#ifndef NDEBUG
    remtexfile = false; // Ensure the .tex file is never removed in debug mode
    if ( !success && _remGen)
        remGen = false;
#endif

    if ( remGen)
    {
        bfs::remove( tpath.replace_extension("aux"));
        bfs::remove( tpath.replace_extension("log"));
        bfs::remove( tpath.replace_extension("out"));
    }   // end if

    // Remove the input .tex file?
    if ( success && remtexfile)
        bfs::remove(texfile);

    return success;
}   // end operator()

