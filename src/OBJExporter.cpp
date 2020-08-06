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

#include <OBJExporter.h>
#include <boost/filesystem/operations.hpp>
#include <fstream>
using r3dio::OBJExporter;
using r3d::Mesh;
using r3d::Vec3f;
using r3d::Vec2f;


OBJExporter::OBJExporter( bool asPNG) : _asPNG(asPNG)
{
    addSupported( "obj", "Wavefront OBJ");
}   // end ctor

namespace {

std::string getMaterialName( const std::string& fname, int midx)
{
    const std::string fstem = boost::filesystem::path(fname).filename().stem().string();
    std::ostringstream oss;
    oss << fstem << "_" << midx;
    return oss.str();
}   // end getMaterialName


// Write out the .mtl file - returning any error string.
std::string writeMaterialFile( const Mesh &mesh, const std::string& fname, bool asPNG)
{
    const boost::filesystem::path ppath = boost::filesystem::path(fname).parent_path();
    std::string err;
    std::ofstream ofs;
    try
    {
        ofs.open( fname.c_str(), std::ios::out);
        ofs << "# Wavefront OBJ material file produced by r3dio (https://github.com/richeytastic/r3dio)" << std::endl;
        ofs << std::endl;

        int pmid = 0;   // Will be set to the 'pseudo' material ID in the event nfaces < total mesh faces.
        int nfaces = 0;
        const IntSet& mids = mesh.materialIds();

        const std::string IMG_EXT = asPNG ? ".png" : ".jpg";
        for ( int mid : mids)
        {
            nfaces += int(mesh.materialFaceIds(mid).size());
            const std::string matname = getMaterialName( fname, mid);
            ofs << "newmtl " << matname << std::endl;
            ofs << "illum 1" << std::endl;
            const cv::Mat tx = mesh.texture(mid);
            if ( !tx.empty())
            {
                std::ostringstream oss;
                oss << matname << IMG_EXT;
                ofs << "map_Kd " << oss.str() << std::endl;
                const std::string imgfile = (ppath / oss.str()).string();
                cv::imwrite( imgfile, tx);
            }   // end if

            ofs << std::endl;
            pmid = mid+1;
        }   // end for

        // Do we need an extra 'pseudo' material?
        assert( nfaces <= int(mesh.numFaces()));
        if ( nfaces < int(mesh.numFaces()))
        {
            ofs << "newmtl " << getMaterialName( fname, pmid) << std::endl;
            ofs << "illum 1" << std::endl;
        }   // end if

        ofs.close();
    }   // end try
    catch ( const std::exception &e)
    {
        err = e.what();
    }   // end catch

    return err;
}   // end writeMaterialFile


using IIMap = std::unordered_map<int,int>;

void writeVertices( std::ostream& os, const Mesh &mesh, IIMap& vvmap)
{
    const IntSet& vidSet = mesh.vtxIds();
    // Ensure always sorted into ascending order for write consistency
    std::vector<int> vids( vidSet.begin(), vidSet.end());
    std::sort( vids.begin(), vids.end());

    int i = 0;
    for ( int vid : vids)
    {
        vvmap[vid] = ++i;   // Preincrement because vertex indices start at one for OBJ
        const Vec3f& v = mesh.vtx(vid);
        os << "v\t" << v[0] << " " << v[1] << " " << v[2] << std::endl;
    }   // end for
}   // end writeVertices


void writeMaterialUVs( std::ostream& os, const Mesh &mesh, int midx, IIMap& uvmap)
{
    const IntSet& uvidSet = mesh.uvs( midx);
    // Ensure always sorted into ascending order for write consistency
    std::vector<int> uvids( uvidSet.begin(), uvidSet.end());
    std::sort( uvids.begin(), uvids.end());

    int i = 0;
    for ( int uvid : uvids)
    {
        uvmap[uvid] = ++i;  // Pre-increment (.obj lists start at 1)
        const Vec2f& uv = mesh.uv(midx, uvid);
        os << "vt\t" << uv[0] << " " << uv[1] << " " << 0.0 << std::endl;
    }   // end for
    os << std::endl;
}   // end writeMaterialUVs


void writeMaterialFaces( std::ostream& os, const Mesh &mesh, int midx, const IIMap& vvmap, const IIMap& uvmap, IntSet& rfids)
{
    // Vertex indices are +1 because .obj vertex list starts at 1.
    const IntSet& mfidSet = mesh.materialFaceIds( midx);
    std::vector<int> mfids( mfidSet.begin(), mfidSet.end());
    std::sort( mfids.begin(), mfids.end());

    for ( int fid : mfids)
    {
        rfids.erase(fid);
        const int* vidxs = mesh.fvidxs(fid);
        const int* fuvs = mesh.faceUVs(fid);
        os << "f\t" << vvmap.at(vidxs[0]) << "/" << uvmap.at(fuvs[0]) << " "
                    << vvmap.at(vidxs[1]) << "/" << uvmap.at(fuvs[1]) << " "
                    << vvmap.at(vidxs[2]) << "/" << uvmap.at(fuvs[2]) << std::endl;
    }   // end for
}   // end writeMaterialFaces

}   // end namespace


// protected
bool OBJExporter::doSave( const Mesh& mesh, const std::string& fname)
{
    std::string err = "";

    // Only need to write out the material file if have materials
    std::string matfile = "";
    if ( mesh.numMats() > 0)
    {
        matfile = boost::filesystem::path(fname).replace_extension("mtl").string();
        err = writeMaterialFile( mesh, matfile, _asPNG);
        if ( !err.empty())
        {
            setErr( "Unable to write OBJ .mtl file! " + err);
            return false;
        }   // end if
    }   // end if

    std::ofstream ofs;
    try
    {
        ofs.open( fname.c_str(), std::ios::out);
        ofs << "# Wavefront OBJ file produced by r3dio (https://github.com/richeytastic/r3dio)" << std::endl;
        ofs << std::endl;

        if ( !matfile.empty())
        {
            ofs << "mtllib " << boost::filesystem::path(matfile).filename().string() << std::endl;
            ofs << std::endl;
        }   // end if

        ofs << "# Mesh has " << mesh.numVtxs() << " vertices" << std::endl;

        IIMap vvmap;
        writeVertices( ofs, mesh, vvmap);

        ofs << std::endl;

        IntSet remfids; // Will hold face IDs not associated with a material
        const IntSet& fids = mesh.faces();
        for ( int fid : fids)
            remfids.insert(fid);

        int pmid = 0;   // Pseudo material ID if required.
        const IntSet& mids = mesh.materialIds();
        for ( int mid : mids)
        {
            const std::string mname = getMaterialName( fname, mid);
            ofs << "# " << mesh.uvs(mid).size() << " UV coordinates on material '" << mname << "'" << std::endl;
            IIMap uvmap;
            writeMaterialUVs( ofs, mesh, mid, uvmap);
            ofs << std::endl;
            ofs << "# Mesh '" << mname << "' with " << mesh.materialFaceIds(mid).size() << " faces" << std::endl;
            ofs << "usemtl " << mname << std::endl;
            writeMaterialFaces( ofs, mesh, mid, vvmap, uvmap, remfids);
            pmid = mid+1;
        }   // end for

        ofs << std::endl;
        // Not all faces accounted for in materials, so write out the remainder without texture coordinates.
        if ( !remfids.empty())
        {
            const std::string mname = getMaterialName( fname, pmid);
            ofs << "# Mesh '" << mname << "' with " << remfids.size() << " faces" << std::endl;
            ofs << "usemtl " << mname << std::endl;
            std::vector<int> rfids( remfids.begin(), remfids.end());
            std::sort( rfids.begin(), rfids.end()); // Sort into ascending order for file output consistency
            for ( int fid : rfids)
            {
                const int* vidxs = mesh.fvidxs(fid);
                ofs << "f\t" << vvmap.at(vidxs[0]) << " " << vvmap.at(vidxs[1]) << " " << vvmap.at(vidxs[2]) << std::endl;
            }   // end for
        }   // end if

        ofs << std::endl;
        ofs.close();
    }   // end try
    catch ( const std::exception &e)
    {
        err = e.what();
    }   // end catch

    bool success = true;
    if ( !err.empty())
    {
        setErr( "Unable to write OBJ file! : " + err);
        success = false;
    }   // end if
    return success;
}   // end doSave

