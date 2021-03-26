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

#ifndef R3DIO_PDF_GENERATOR_H
#define R3DIO_PDF_GENERATOR_H

#ifdef _WIN32
#pragma warning( disable : 4251)
#endif

#include "r3dio_Export.h"
#include <iostream>
#include <string>


namespace r3dio {

class r3dio_EXPORT PDFGenerator
{
public:
    // Define the name of the pdflatex program which must be on the path.
    // Defaults to "pdflatex" ("pdflatex.exe" on Windows).
    static std::string pdflatex;

    // Returns true iff pdflatex is on the PATH.
    static bool isAvailable();

    // Set remGen to true to remove files generated by pdflatex whether it
    // succeeds or fails, but never if pdflatex fails within a debug build.
    explicit PDFGenerator( bool remGen=true);
    virtual ~PDFGenerator(){}

    // Run pdflatex against texfile - returns false if pdflatex fails.
    // On returning true, file created is texfile with extension replaced with .pdf.
    // Set removeTexfileOnSuccess to delete texfile on success (unless debug build is
    // active in which case the input texfile is never deleted).
    bool operator()( const std::string& texfile, bool removeTexfileOnSuccess=false);

private:
    const bool _remGen;
};  // end class

}   // end namespace

#endif
