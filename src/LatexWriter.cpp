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

#include <LatexWriter.h>
#include <PDFGenerator.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <unordered_map>
#include <fstream>
#include <sstream>
using r3d::Vec3f;
using r3dio::LatexWriter;
using Colour = r3d::Colour;
using Cam = r3d::CameraParams;
using r3dio::Box;
using r3dio::Point;
namespace BFS = boost::filesystem;

bool LatexWriter::testGeneratePDF()
{
    static const std::string TESTTEX = R"(
\documentclass[a4paper]{article}
\listfiles
\usepackage[paperwidth=210mm,paperheight=297mm]{geometry}
\usepackage[absolute]{textpos}
\usepackage{graphicx}
\usepackage{float}
\usepackage{verbatim}
\usepackage{xcolor}
\usepackage[justification=centering]{caption}
\usepackage{tikz}
\usepackage{media9}
\usepackage{amsmath}
\usepackage[parfill]{parskip}
\usepackage[colorlinks=true,urlcolor=blue]{hyperref}
\DeclareGraphicsExtensions{.png,.jpg,.pdf,.eps}
\setlength{\TPHorizModule}{1mm}
\setlength{\TPVertModule}{\TPHorizModule}
\setlength{\parindent}{0pt}
\begin{document}
\pagenumbering{gobble}
This is just a test.
\end{document}
)";
    const BFS::path workdir = BFS::temp_directory_path() / BFS::unique_path();
    const BFS::path texpath = workdir / "test.tex";
    if ( !BFS::create_directories( workdir))
    {
        std::cerr << "[ERROR] r3dio::LatexWriter: Unable to create temporary directory!" << std::endl;
        return false;
    }   // end if
    std::ofstream ofs;
    try
    {
        ofs.open( texpath.string(), std::ios::out);
        ofs << TESTTEX;
        ofs.close();
    }   // end try
    catch ( ...)
    {
        std::cerr << "[ERROR] r3dio::LatexWriter: Unable to open/write temporary file!" << std::endl;
        return false;
    }   // end catch

    const bool success = r3dio::PDFGenerator( false)( texpath.string());
    BFS::remove_all( workdir);
    return success;
}   // end testGeneratePDF

namespace {

void _writeVWSView( std::ostream &f, const std::string &vtitle, const Vec3f &c2c, float roo, const std::string &astr)
{
    f << "VIEW=" << vtitle << "\n"
      << "  C2C=" << c2c[0] << " " << c2c[1] << " " << c2c[2] << "\n"
      << "  ROO=" << roo << "\n"
      << "  " << astr << "\n"
      << "  LIGHTS=None\n"
      << "END\n";
}   // end _writeVWSView


// Pass in the filepath of the .vws file, the camera and width and height of the viewport.
bool _writeVWS( const std::string &vwsfile, float w, float h, const Cam &cam)
{
    bool success = false;
    std::ofstream ofs;
    try
    {
        ofs.open( vwsfile, std::ios::out);

        std::ostringstream oss;
        if ( cam.isParallel())
        {
            float ps = 0.5f / cam.parallelScale();  //Height viewport in world-coordinate units
            if (w < h)
                ps *= h/w;
            oss << "ORTHO=" << ps;
        }   // end if
        else
        {
            float aac = cam.fov();  // Vertical FoV
            if (w < h)  // Media9 requires FoV in the narrowest aspect of the viewport
            {
                static const float D2R = float(EIGEN_PI/180);
                aac = 2*atanf( w/h * tanf(aac/2 * D2R)) / D2R;
            }   // end if
            oss << "AAC=" << aac;
        }   // end else

        const float roo = cam.distance();
        const std::string astr = oss.str();
        _writeVWSView( ofs, "Front", Vec3f( 0,-1, 0), roo, astr);
        _writeVWSView( ofs, "Right", Vec3f(-1, 0, 0), roo, astr);
        _writeVWSView( ofs, "Left",  Vec3f( 1, 0, 0), roo, astr);
        ofs.close();
        success = true;
    }   // end try
    catch ( ...)
    {
        std::cerr << "[ERROR] r3dio::LatexWriter: Unable to open/write views file!" << std::endl;
        success = false;
    }   // end catch
    return success;
}   // end _writeVWS


bool _writeHideAxesFile( const std::string &axsfile)
{
    std::ofstream ofs;
    bool success = false;
    try
    {
        ofs.open( axsfile, std::ios::out);
        ofs << "scene.showOrientationAxes = false;";
        ofs.close();
        success = true;
    }   // end try
    catch ( ...)
    {
        std::cerr << "[ERROR] r3dio::LatexWriter: Unable to write hide axes JS!" << std::endl;
        success = false;
    }   // end catch
    return success;
}   // end _writeHideAxesFile


}   // end namespace


std::string LatexWriter::sanit( const std::string &s)
{
    std::string ns = boost::algorithm::replace_all_copy( s, "\\", "\\textbackslash");
    using boost::algorithm::replace_all;
    replace_all( ns, "#", "\\#");
    replace_all( ns, "$", "\\$");
    replace_all( ns, "%", "\\%");
    replace_all( ns, "&", "\\&");
    replace_all( ns, "^", "\\textasciicircum");
    replace_all( ns, "_", "\\_");
    replace_all( ns, "{", "\\{");
    replace_all( ns, "}", "\\}");
    replace_all( ns, "~", "\\~{}");
    replace_all( ns, "|", "$\\mid$");
    return ns;
}   // end sanit


struct LatexWriter::Pimpl
{
    Pimpl( float wmm, float hmm, bool doDelete)
        : _wmm(wmm), _hmm(hmm), _doDelete(doDelete),
          _workdir( BFS::temp_directory_path() / BFS::unique_path())
    {
        if ( !BFS::create_directories( _workdir))
            std::cerr << "[ERROR] r3dio::LatexWriter: Unable to create working directory!" << std::endl;
        // Need to draw extent of page area first to make relative page measurement drawing work
        drawRectangle( Box(0,0,wmm,hmm), r3d::Colour::white());
    }   // end ctor

    ~Pimpl()
    {
        if ( !_doDelete)
            return;
        boost::system::error_code ec;
        BFS::remove_all( _workdir, ec);
        if ( ec)
            std::cerr << "[WARNING] r3dio::LatexWriter: Unable to remove working directory: " << ec.message() << std::endl;
    }   // end dtor


    bool copyInFile( const std::string &fpath, const std::string &fname) const
    {
        if ( !BFS::is_regular_file(fpath))
        {
            std::cerr << "[ERROR] r3dio::LatexWriter::copyInFile '" << fpath << "' not a regular file!" << std::endl;
            return false;
        }   // end if
        const BFS::path tofile = _workdir / fname;
        if ( BFS::exists(tofile))
        {
            std::cerr << "[ERROR] r3dio::LatexWriter::copyInFile '" << tofile << "' already exists!" << std::endl;
            return false;
        }   // end if

        bool success = true;
        try
        {
            BFS::copy_file( fpath, tofile, BFS::copy_option::fail_if_exists);
        }   // end try
        catch ( ...)
        {
            std::cerr << "[ERROR] r3dio::LatexWriter::copyInFile " << "Failed copy to '" << tofile << "'" << std::endl;
            success = false;
        }   // end catch
        return success;
    }   // end copyInFile


    std::string makePDF() const
    {
        std::string outfile;
        BFS::path texfile( _workdir / "scene.tex");
        std::ofstream fout;        // File stream
        bool success = false;
        try
        {
            fout.open( texfile.string(), std::ios::out);
            fout << "\\documentclass{article}\n"
                << "\\listfiles\n" // So log shows packages used (useful for debugging)
                << "\\usepackage[textwidth=" << _wmm << "mm,textheight=" << _hmm << "mm,"
                    << "paperwidth=" << _wmm << "mm,paperheight=" << _hmm << "mm]{geometry}\n"
                << "\\usepackage[absolute]{textpos}\n" // Absolute positioning
                << "\\usepackage{graphicx}\n"
                << "\\usepackage{verbatim}\n"
                << "\\usepackage{xcolor}\n"
                << "\\usepackage{float}\n"
                << "\\usepackage[justification=centering]{caption}\n"
                << "\\usepackage{tikz}\n"
                << "\\usepackage{media9}\n"
                << "\\usepackage{amsmath}\n"
                << "\\usepackage[parfill]{parskip}\n"
                << "\\usepackage[colorlinks=true,urlcolor=blue]{hyperref}\n"
                << "\\DeclareGraphicsExtensions{.png,.jpg,.pdf,.eps}\n"
                << "\\setlength{\\TPHorizModule}{1mm}\n"
                << "\\setlength{\\TPVertModule}{\\TPHorizModule}\n"
                << "\\setlength{\\parindent}{0pt}\n"
                << _hout.str() << "\n";
            
            // Write out the defined colours
            for ( const auto &p : _dcols)
            {
                const r3d::Colour &col = p.first;
                fout << "\\definecolor{" << p.second << "}{RGB}{" << col.ired() << "," << col.igreen() << "," << col.iblue() << "}\n";
            }   // end for

            // Add in the main document content
            fout << "\n"
                << "\\begin{document}\n"
                << "\\pagenumbering{gobble}\n"
                << "\\thispagestyle{fancy}\n"
                << _dout.str() << "\n"
                << "\\begin{textblock*}{0mm}(0mm,0mm)\n"
                << "\\begin{tikzpicture}[x=1mm,y=1mm]\n"  // Scale always mm
                << _tout.str()   // Output all drawing commands here (includes outer edge box from ctor)
                << "\\end{tikzpicture}\n"
                << "\\end{textblock*}\n"
                << "\\end{document}\n";

            fout.close();

            success = r3dio::PDFGenerator( false)( texfile.string());
            if ( success)
                outfile = texfile.replace_extension("pdf").string();
            else
                std::cerr << "[ERROR] r3dio::LatexWriter: Failed to generate PDF from '" << texfile << "'" << std::endl;
        }   // end try
        catch (...)
        {
            std::cerr << "[ERROR] r3dio::LatexWriter: Unable to open/write file stream!" << std::endl;
            success = false;
        }   // end catch

        _doDelete &= success;
        return success ? outfile : "";
    }   // end makePDF


    std::string workingDirectory() const { return _workdir.string();}

    void addHeader( const std::string &tex) { _hout << tex;}

    void addRaw( const Box &box, const std::string &tex, bool centre)
    {
        _startBlock( box);
        if ( centre)
            _dout << "\\centering\n";
        _dout << tex;
        _endBlock();
    }   // end addRaw

    void addText( const Box &box, const std::string &txt, bool centre)
    {
        _startBlock( box);
        if ( centre)
            _dout << "\\centering\n";
        _dout << sanit(txt);
        _endBlock();
    }   // end addText

    void fillRectangle( const Box &box, const r3d::Colour &col)
    {
        const std::string &dcol = _getDefinedColourName( col);
        _tout << "\\filldraw[" << dcol << "," << dcol << "]";
        _writeRectangleDims( box);
    }   // end fillRectangle

    void drawRectangle( const Box &box, const r3d::Colour &col)
    {
        _tout << "\\draw[" << _getDefinedColourName(col) << "]";
        _writeRectangleDims( box);
    }   // end drawRectangle

    void drawLine( const Point &p, const Point &q, const r3d::Colour &col)
    {
        _tout << "\\draw[" << _getDefinedColourName(col) << "]";
        _tout << "(" << p[0] << "," << (_hmm - p[1]) << ") -- (" << q[0] << "," << (_hmm - q[1]) << ");\n";
    }   // end drawRectangle

    void addImage( const Box &box, const std::string &imgpath, const std::string &caption)
    {
        _startBlock(box);
        _dout << "\\begin{figure}\n";
        _dout << "\\includegraphics[width=" << box[2] << "mm,height=" << box[3] << "mm]{" << BFS::path(imgpath) << "}";
        if ( !caption.empty())
            _dout << "\\caption*{" << sanit(caption) << "}\n";
        _dout << "\\end{figure}\n";
        _endBlock();
    }   // end addImage

    void addMesh( const Box &box, const std::string &u3d, const Cam &cam, const std::string &bgimg, const std::string &caption)
    {
        const BFS::path vwsfile = BFS::unique_path( "%%%%-%%%%-%%%%-%%%%.vws");
        const BFS::path axsfile = BFS::unique_path( "%%%%-%%%%-%%%%-%%%%.js");
        const float w = box[2];
        const float h = box[3];
        _writeVWS( (_workdir / vwsfile).string(), w, h, cam);
        _writeHideAxesFile( (_workdir / axsfile).string());

        _startBlock(box);
        _dout << "\\begin{figure}\n";
        _dout << "\\includemedia[\n"
            //<< "\tlabel=mesh,\n"
            << "\twidth=" << w << "mm,\n"
            << "\theight=" << h << "mm,\n"
            << "\tadd3Djscript=" << axsfile.string() << ",\n"  // Hide orientation axes
            << "\tadd3Djscript=3Dspintool.js,\n"    // Let scene rotate about z-axis
            << "\tactivate=pageopen,\n"
            << "\tplaybutton=none,\n"
            << "\ttransparent=false,\n"
            << "\t3Dbg=1 1 1,\n"    // Can't be used if transparent set true
            << "\t3Dmenu,\n"
            << "\t3Dviews=" << vwsfile.string() << ",\n"
            << "]{";
        if (!bgimg.empty()) // Background image?
            _dout << "\\includegraphics[width=" << w << "mm,height=" << h << "mm]{" << bgimg << "}";
        _dout << "}{" << u3d << "}\n";
        if ( !caption.empty())
            _dout << "\\caption*{" << sanit(caption) << "}\n";
        _dout << "\\end{figure}\n";
        _endBlock();
    }   // end addMesh

private:
    void _writeRectangleDims( const Box &box)
    {
        _tout << "(" << box[0] << "," << (_hmm - box[1]) << ")"  // One corner
             << "rectangle(" << (box[0] + box[2]) << "," << (_hmm - box[1] - box[3]) << ");\n";    // Opposite corner
    }   // end _writeRectangleDims

    void _startBlock( const Box &box)
    {
        _dout << "\\begin{textblock*}{" << box[2] << "mm}(" << box[0] << "mm," << box[1] << "mm)\n";
    }   // end _startBlock

    void _endBlock() { _dout << "\\end{textblock*}\n";}

    const std::string &_getDefinedColourName( const r3d::Colour &col)
    {
        if ( _dcols.count(col) == 0)
        {
            std::ostringstream oss;
            oss << "rgb-" << col.ired() << "-" << col.igreen() << "-" << col.iblue();
            _dcols[col] = oss.str();
        }   // end if
        return _dcols.at(col);
    }   // end _getDefinedColourName

    float _wmm, _hmm;
    mutable bool _doDelete;
    BFS::path _workdir;
    std::unordered_map<r3d::Colour, std::string, r3d::HashColour> _dcols;    // Defined colours go at end of header
    std::ostringstream _hout;   // Header tex
    std::ostringstream _dout;   // Document tex
    std::ostringstream _tout;   // TIKZ content
};  // end struct


/********************** INTERFACE FOLLOWS *************************/

LatexWriter::LatexWriter( float w, float h, bool doDelete) : _pimpl(new Pimpl( w, h, doDelete)) {}

LatexWriter::~LatexWriter() { delete _pimpl;}

bool LatexWriter::copyInFile( const std::string &fpath, const std::string &fname)
{
    return _pimpl->copyInFile(fpath, fname);
}   // end copyInFile

std::string LatexWriter::workingDirectory() const { return _pimpl->workingDirectory();}

std::string LatexWriter::makePDF() const { return _pimpl->makePDF();}

void LatexWriter::addHeader( const std::string &tx) { _pimpl->addHeader(tx);}
LatexWriter& LatexWriter::operator<<( const std::string &tx) { addHeader(tx); return *this;}
void LatexWriter::addRaw( const Box &box, const std::string &tx, bool cntr) { _pimpl->addRaw( box, tx, cntr);}
void LatexWriter::addText( const Box &box, const std::string &txt, bool cntr) { _pimpl->addText( box, txt, cntr);}
void LatexWriter::fillRectangle( const Box &box, const r3d::Colour &col) { _pimpl->fillRectangle( box, col);}
void LatexWriter::drawRectangle( const Box &box, const r3d::Colour &col) { _pimpl->drawRectangle( box, col);}
void LatexWriter::drawLine( const Point &p, const Point &q, const r3d::Colour &col) { _pimpl->drawLine( p, q, col);}

void LatexWriter::addImage( const Box &box, const std::string &imgpath, const std::string &caption)
{
    _pimpl->addImage( box, imgpath, caption);
}   // end addImage

void LatexWriter::addMesh( const Box &box, const std::string &u3d, const Cam &cam, const std::string &bgimg, const std::string &caption)
{
    _pimpl->addMesh( box, u3d, cam, bgimg, caption);
}   // end addMesh
