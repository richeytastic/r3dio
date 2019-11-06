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

#include <IOFormats.h>
#include <cassert>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
using r3dio::IOFormats;


namespace {
std::string getExtension( const std::string& fname)
{
    std::string fname2 = fname;
    boost::algorithm::trim(fname2);
    boost::filesystem::path p( fname2);
    if ( !p.has_extension())    // No extension
        return "";

    std::string ext = p.extension().string();
    assert( ext[0] == '.');
    if ( ext == ".")    // Empty extension (ends with dot)
        return "";

    ext = ext.substr(1);
    boost::algorithm::to_lower(ext);  // Don't want preceeding period & set to lower case
    return ext;
}   // end getExtension

}   // end namespace


const std::string& IOFormats::getDescription( const std::string& ext) const
{
    const std::string lext = boost::algorithm::to_lower_copy(ext);
    assert( _exts2desc.count(lext) > 0);
    return _exts2desc.at(lext);
}   // end getDescription


bool IOFormats::isSupported( const std::string& fname) const
{
    if ( fname.empty())
        return false;

    // Check if the filename extension is supported.
    const std::string ext = getExtension(fname);
    if ( ext.empty())
    {
        std::cerr << fname << " is missing an extension!" << std::endl;
        return false;
    }   // end if
    return _exts2desc.count(ext) > 0;
}   // end isSupported


// protected
void IOFormats::setErr( const std::string& errMsg)
{
    _err = errMsg;
}   // end setErr


// protected
bool IOFormats::addSupported( const std::string& ext, const std::string& desc)
{
    std::string lext = boost::algorithm::to_lower_copy(ext);
    boost::algorithm::trim( lext);  // Don't want any white space around the extension
    while ( lext[0] == '.')
        lext = lext.substr(1); // Trim leading '.'
    while ( lext[lext.size()-1] == '.')
        lext = lext.substr(0, lext.size()-1); // Trim trailing '.'

    if ( lext.empty())
    {
        std::cerr << "[ERROR] IOFormats::addSupported: Can't add empty filename extensions!" << std::endl;
        return false;
    }   // end if

    if ( _exts2desc.count(lext) > 0)    // It's an error to try to add the same file extension more than once
    {
        std::cerr << "[ERROR] IOFormats::addSupported: Tried to add the same file extension more than once!" << std::endl;
        return false;
    }   // end if

    _exts.push_back(lext);
    _exts2desc[lext] = boost::algorithm::trim_copy(desc);
    return true;
}   // end addSupported
