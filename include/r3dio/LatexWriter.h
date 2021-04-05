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

#ifndef R3DIO_LATEX_WRITER_H
#define R3DIO_LATEX_WRITER_H

#include "r3dio_Export.h"
#include <r3d/CameraParams.h>
#include <r3d/Colour.h>
#include <r3d/Mesh.h>
#include <string>

namespace r3dio {

using Box = r3d::Vec4f;
using Point = r3d::Vec2f;

class r3dio_EXPORT LatexWriter
{
public:
    // Test generation of PDF returning true on success.
    static bool testGeneratePDF();

    // Returns the same string with characters that may be interpretted as Latex syntax
    // replaced with the corresponding command strings so that the returned string
    // can be safely embedded within a Latex .tex file.
    static std::string sanit( const std::string&);

    // Open page for writing with given with and height in millimetres.
    // Set removeWorkingDir to false to retain the working directory and
    // its file contents after this object is destroyed.
    LatexWriter( float wmm, float hmm, bool removeWorkingDir=true);

    ~LatexWriter();

    // Generate PDF from whatever content has been set so far and set
    // return the location of the generated PDF or an empty string on failure.
    std::string makePDF() const;

    // Returns the working directory for this writer.
    std::string workingDirectory() const;

    // Add raw tex to the header (useful for adding other packages).
    void addHeader( const std::string&);
    // Synonymous for addHeader.
    LatexWriter &operator<<( const std::string&);

    // Add latex commands at the given position in the main document body.
    void addRaw( const Box&, const std::string&, bool centre=false);

    // Add text (sanitised for Latex commands) at the given position in the main document body.
    void addText( const Box&, const std::string&, bool centre=false);

    // Copy in the given filepath to the working directory with name fname.
    bool copyInFile( const std::string &filepath, const std::string &fname);

    // Draw outline and filled rectangles in the main document body.
    void fillRectangle( const Box&, const r3d::Colour&);
    void drawRectangle( const Box&, const r3d::Colour&);

    // Draw a line between p and q in the given colour.
    void drawLine( const Point &p, const Point &q, const r3d::Colour&);

    // Add an image at the given position with optional caption.
    void addImage( const Box&, const std::string &imgPath, const std::string &caption="");

    // Add a U3D mesh at the given position with optional background image and caption.
    void addMesh( const Box&, const std::string &u3dPath,
                              const r3d::CameraParams&,
                              const std::string &bgImgPath="",
                              const std::string &caption="");

private:
    LatexWriter( const LatexWriter&) = delete;
    void operator=( const LatexWriter&) = delete;
    struct Pimpl;
    Pimpl *_pimpl;
};  // end class

}   // end namespace

#endif
