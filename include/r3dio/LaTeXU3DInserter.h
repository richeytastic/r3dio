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

#ifndef R3DIO_LATEX_U3D_INSERTER_H
#define R3DIO_LATEX_U3D_INSERTER_H

/**
 * See PDFGenerator for details of use.
 */

#include "r3dio_Export.h"
#include <r3d/CameraParams.h>
#include <r3d/Mesh.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace r3dio {

class LaTeXU3DInserter;
r3dio_EXPORT std::ostream& operator<<( std::ostream&, const LaTeXU3DInserter&);

class r3dio_EXPORT LaTeXU3DInserter
{
public:
    typedef std::shared_ptr<LaTeXU3DInserter> Ptr;

    // New figure inserter. Returns null if unable to convert given model to U3D.
    // Don't try to delete the returned object - will be deleted automatically
    // when the PDFGenerator's destructor runs.
    static Ptr create( const r3d::Mesh&,
                       const std::string& u3ddir,           // The directory in which the temporary U3D model is exported
                       float figWidthMM, float figHeightMM, // Width and height of figure in mm
                       const r3d::CameraParams& cam,        // ONLY pos-focus distance and FoV used
                       const std::string& figCaption="",    // Caption ignored if empty
                       const std::string& figLabel="",      // Label not written if empty
                       bool activate3DContentOnOpen=true,   // Activate 3D model on open (true) or click (false)
                       bool removeOnDestroy=true);          // Remove generated U3D files on destruction

    // Set the object to export into the figure, either as the filepath of an already existing U3D model 
    // (in which case the file must have the extension ".u3d" or false is returned), or as a r3d::Mesh
    // instance whereupon conversion will be conducted and the U3D model saved in a temporary file location.
    // If conversion to U3D fails, false is returned. Note that in the second case, the temporary file
    // (the produced U3D file) is deleted upon destruction of this object, so any PDF generation using the
    // same LaTeX file must be conducted while this object is still alive.
    // NB r3dio::U3DExporter::IDTFConverter must be a valid path to the IDTFConverter
    // program in order for the second version to work (see r3dio::U3DExporter for details).
    bool setModel( const std::string& u3dfilename);
    bool setModel( const r3d::Mesh&, const std::string&);

private:
    float _fw;
    float _fh;
    r3d::CameraParams _cam;
    std::string _figCap;
    std::string _figLab;
    bool _actOnOpen;
    bool _remgen;
    std::string _u3dfile;
    std::vector<std::string> _delfiles;

    LaTeXU3DInserter( float figWidthMM, float figHeightMM, const r3d::CameraParams& cam,
                      const std::string& figCaption="", const std::string& figLabel="",
                      bool activate3DContentOnOpen=true, bool removeOnDestroy=true);
    virtual ~LaTeXU3DInserter();

    LaTeXU3DInserter( const LaTeXU3DInserter&) = delete;
    void operator=( const LaTeXU3DInserter&) = delete;
    friend std::ostream& r3dio::operator<<( std::ostream&, const LaTeXU3DInserter&);
};  // end class

}   // end namespace

#endif
