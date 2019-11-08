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

#include <PLYExporter.h>
#include <fstream>
#include <cassert>
using r3dio::PLYExporter;
using r3d::Mesh;


PLYExporter::PLYExporter() : r3dio::MeshExporter()
{
    addSupported( "ply", "Polygon File Format");
}   // end ctor


// protected
bool PLYExporter::doSave( const Mesh& m, const std::string& fname)
{
    std::string err;
    std::ofstream ofs;
    try
    {
        const size_t nv = m.numVtxs();
        const size_t np = m.numFaces();

        ofs.open( fname.c_str(), std::ios::out);
        ofs << "ply" << std::endl;
        ofs << "format ascii 1.0" << std::endl;
        ofs << "comment Polygon File Format file produced by r3dio (https://github.com/richeytastic/r3dio)" << std::endl;
        ofs << "element vertex " << nv << std::endl;
        ofs << "property float x" << std::endl;
        ofs << "property float y" << std::endl;
        ofs << "property float z" << std::endl;
        ofs << "element face " << np << std::endl;
        ofs << "property list uchar int vertex_index" << std::endl;
        ofs << "end_header" << std::endl;

        const IntSet& vids = m.vtxIds();
        std::unordered_map<int,int> vvmap;
        int i = 0;
        for ( int vid : vids)
        {
            vvmap[vid] = i++;   // Post-increment for PLY
            ofs << m.vtx(vid)[0] << " " << m.vtx(vid)[1] << " " << m.vtx(vid)[2] << std::endl;
        }   // end for

        const IntSet& fids = m.faces();
        for ( int fid : fids)
        {
            const int* f = m.fvidxs(fid);
            ofs << "3 " << vvmap.at(f[0]) << " " << vvmap.at(f[1]) << " " << vvmap.at(f[2]) << std::endl;
        }   // end for

        ofs.close();
    }   // end try
    catch ( const std::exception &e)
    {
        err = e.what();
    }   // end catch

    if ( !err.empty())
        setErr( "Unable to write PLY file! : " + err);

    return err.empty();
}   // end doSave
