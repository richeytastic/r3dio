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
#include <fstream>
#include <sstream>
using r3d::Vec3f;
using r3dio::LatexWriter;
using Colour = r3d::Colour;
using Cam = r3d::CameraParams;
using r3dio::Box;
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
      //<< "  LIGHTS=AmbientLight\n"
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
    Pimpl( bool doDelete) : _doDelete(doDelete),
        _workdir( BFS::temp_directory_path() / BFS::unique_path()),
        _texfile( _workdir / "scene.tex")
    {}

    ~Pimpl()
    {
        if ( !_doDelete)
            return;
        boost::system::error_code ec;
        BFS::remove_all( _workdir, ec);
        if ( ec)
        {
            std::cerr << "[WARNING] r3dio::LatexWriter: Unable to remove working directory: "
                << ec.message() << std::endl;
        }   // end if
    }   // end dtor

    bool open( float wmm, float hmm)
    {
        if ( !BFS::create_directories( _workdir))
        {
            std::cerr << "[ERROR] r3dio::LatexWriter: Unable to create working directory!" << std::endl;
            return false;
        }   // end if

        bool success = false;
        try
        {
            _fout.open( _texfile.string(), std::ios::out);
            _fout << "\\documentclass{article}\n"
                << "\\listfiles\n" // So log shows packages used (useful for debugging)
                << "\\usepackage[textwidth=" << wmm << "mm,textheight=" << hmm << "mm,"
                    << "paperwidth=" << wmm << "mm,paperheight=" << hmm << "mm]{geometry}\n"
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
                << "\\setlength{\\parindent}{0pt}\n";
            success = true;
        }   // end try
        catch (...)
        {
            std::cerr << "[ERROR] r3dio::LatexWriter: Unable to open/write file stream!" << std::endl;
            success = false;
        }   // end catch
        return success;
    }   // end ctor

    void defineColour( const Colour &col, const std::string &nm)
    {
        _fout << "\\definecolor{" << sanit(nm) << "}{RGB}{"
            << col.ired() << "," << col.igreen() << "," << col.iblue() << "}\n";
    }   // end defineColour

    bool copyInFile( const std::string &fpath, const std::string &fname)
    {
        if ( !BFS::is_regular_file(fpath))
        {
            std::cerr << "[ERROR] r3dio::LatexWriter::copyInFile '"
                        << fpath << "' not a regular file!" << std::endl;
            return false;
        }   // end if
        const BFS::path tofile = _workdir / fname;
        if ( BFS::exists(tofile))
        {
            std::cerr << "[ERROR] r3dio::LatexWriter::copyInFile '"
                        << tofile << "' already exists!" << std::endl;
            return false;
        }   // end if

        bool success = true;
        try
        {
            BFS::copy_file( fpath, tofile, BFS::copy_option::fail_if_exists);
        }   // end try
        catch ( ...)
        {
            std::cerr << "[ERROR] r3dio::LatexWriter::copyInFile "
                        << "Failed copy to '" << tofile << "'" << std::endl;
            success = false;
        }   // end catch
        return success;
    }   // end copyInFile


    std::string workingDirectory() const { return _workdir.string();}


    void addRaw( const std::string &tex) { _fout << tex;}


    void beginDocument()
    {
        _fout << "\\begin{document}\n"
            << "\\pagenumbering{gobble}\n"
            << "\\thispagestyle{fancy}\n";
    }   // end beginDocument

    void endDocument() { _fout << "\\end{document}\n";}

    void close()
    {
        if ( _fout.is_open())
            _fout.close();
    }   // end close

    bool makePDF( std::string &outfile)
    {
        bool success = r3dio::PDFGenerator( false)( _texfile.string());
        if ( success)
            outfile = _texfile.replace_extension("pdf").string();
        else
        {
            std::cerr << "[ERROR] r3dio::LatexWriter: Failed to generate PDF from '" << _texfile << "'" << std::endl;
            _doDelete = false;
        }   // end else
        return success;
    }   // end makePDF


    void addText( const Box &box, const std::string &txt, bool centre)
    {
        _startBlock( box);
        if ( centre)
            _fout << "\\centering\n";
        _fout << sanit(txt);
        _endBlock();
    }   // end addText

    void addRaw( const Box &box, const std::string &tex, bool centre)
    {
        _startBlock( box);
        if ( centre)
            _fout << "\\centering\n";
        _fout << tex;
        _endBlock();
    }   // end addRaw

    void startTIKZ( const Box &box)
    {
        _startBlock( box);
        _fout << "\\begin{tikzpicture}[x=1mm,y=1mm]\n";  // Scale always mm
    }   // end startTIKZ

    void endTIKZ()
    {
        _fout << "\\end{tikzpicture}\n";
        _endBlock();
    }   // end endTIKZ

    // Use within TIKZ block.
    void fillRectangle( const Box &box, const std::string &definedColour)
    {
        const std::string dcol = sanit(definedColour);
        assert( !dcol.empty());
        _fout << "\\filldraw[" << dcol << "," << dcol << "]";
        _writeRectangleDims( box);
    }   // end fillRectangle

    // Use within TIKZ block.
    void drawRectangle( const Box &box, const std::string &definedColour)
    {
        _fout << "\\draw";
        if (!definedColour.empty())
            _fout << "[" << sanit(definedColour) << "]";
        _writeRectangleDims( box);
    }   // end drawRectangle

    void addImage( const Box &box, const std::string &imgpath, const std::string &caption)
    {
        _startBlock(box);
        _fout << "\\begin{figure}\n";
        _fout << "\\includegraphics[width=" << box[2] << "mm,height=" << box[3] << "mm]{" << BFS::path(imgpath) << "}";
        if ( !caption.empty())
            _fout << "\\caption*{" << sanit(caption) << "}\n";
        _fout << "\\end{figure}\n";
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
        _fout << "\\begin{figure}\n";
        _fout << "\\includemedia[\n"
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
            _fout << "\\includegraphics[width=" << w << "mm,height=" << h << "mm]{" << bgimg << "}";
        _fout << "}{" << u3d << "}\n";
        if ( !caption.empty())
            _fout << "\\caption*{" << sanit(caption) << "}\n";
        _fout << "\\end{figure}\n";
        _endBlock();
    }   // end addMesh

private:
    void _writeRectangleDims( const Box &box)
    {
        _fout << "(" << box[0] << "," << box[1] << ")"  // Top left corner
             << "rectangle(" << (box[0] + box[2]) << "," << (box[1] + box[3]) << ");\n";    // Bottom right corner
    }   // end _writeRectangleDims

    void _startBlock( const Box &box)
    {
        _fout << "\\begin{textblock*}{" << box[2] << "mm}(" << box[0] << "mm," << box[1] << "mm)\n";
    }   // end _startBlock

    void _endBlock() { _fout << "\\end{textblock*}\n";}

    bool _doDelete;
    BFS::path _workdir;
    BFS::path _texfile;
    std::ofstream _fout;
};  // end struct


LatexWriter::LatexWriter( bool doDelete) : _pimpl(new Pimpl( doDelete)) {}
LatexWriter::~LatexWriter() { delete _pimpl;}

void LatexWriter::defineColour( const Colour &col, const std::string &nm)
{ _pimpl->defineColour(col,nm);}

bool LatexWriter::copyInFile( const std::string &fpath, const std::string &fname)
{ return _pimpl->copyInFile(fpath, fname);}

std::string LatexWriter::workingDirectory() const { return _pimpl->workingDirectory();}

bool LatexWriter::makePDF( std::string &pdffile) { return _pimpl->makePDF( pdffile);}

void LatexWriter::beginDocument() { _pimpl->beginDocument();}
void LatexWriter::endDocument() { _pimpl->endDocument();}

bool LatexWriter::open( float w, float h) { return _pimpl->open(w,h);}
void LatexWriter::close() { _pimpl->close();}

void LatexWriter::addText( const Box &box, const std::string &txt, bool cntr)
{ _pimpl->addText( box, txt, cntr);}

void LatexWriter::addRaw( const Box &box, const std::string &tx, bool cntr) { _pimpl->addRaw( box, tx, cntr);}

void LatexWriter::addRaw( const std::string &tx) { _pimpl->addRaw(tx);}
LatexWriter& LatexWriter::operator<<( const std::string &tx) { addRaw(tx); return *this;}

void LatexWriter::startTIKZ( const Box &box) { _pimpl->startTIKZ(box);}
void LatexWriter::endTIKZ(){ _pimpl->endTIKZ();}

void LatexWriter::fillRectangle( const Box &box, const std::string &definedColour)
{ _pimpl->fillRectangle( box, definedColour);}

void LatexWriter::drawRectangle( const Box &box, const std::string &definedColour)
{ _pimpl->drawRectangle( box, definedColour);}

void LatexWriter::addImage( const Box &box, const std::string &imgpath, const std::string &caption)
{ _pimpl->addImage( box, imgpath, caption);}

void LatexWriter::addMesh( const Box &box, const std::string &u3d, const Cam &cam, const std::string &bgimg, const std::string &caption)
{
    _pimpl->addMesh( box, u3d, cam, bgimg, caption);
}   // end addMesh
