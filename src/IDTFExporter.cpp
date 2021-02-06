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

#include <IDTFExporter.h>
#include <TGAImage.h>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <boost/filesystem/operations.hpp>
using r3dio::IDTFExporter;
using r3d::Mesh;
using r3d::Vec3f;
using r3d::Colour;
using r3d::Vec2f;
using std::unordered_map;


// public
IDTFExporter::IDTFExporter( bool delOnDtor, bool m9, const Colour &ems)
    : r3dio::MeshExporter(), _delOnDtor(delOnDtor), _media9(m9), _ems(ems)
{
    addSupported( "idtf", "Intermediate Data Text Format");
}   // end ctor


// public
IDTFExporter::~IDTFExporter() { _reset();}


// Remove saved files.
void IDTFExporter::_reset()
{
    if ( _delOnDtor)
    {
        using namespace boost::filesystem;
        path ffile( _idtffile);
        if ( exists( ffile) && is_regular_file( ffile))
            remove( ffile);

        for ( const std::string& tgafile : _tgafiles)
        {
            path ifile( tgafile);
            if ( exists( ifile) && is_regular_file(ifile))
                remove( ifile);
        }   // end for
    }   // end _delOnDtor

    _idtffile = "";
    _tgafiles.clear();
}   // end _reset


namespace {

struct TB {
    TB(int ntabs=0) : n(ntabs) {}
    int n;
};  // end struct

struct NL {
    NL(int nNewLines=1) : n(std::max(1,nNewLines)) {} // Always at least one new line
    int n;
};  // end struct

std::ostream& operator<<( std::ostream& os, const TB& t)
{
    for ( int i = 0; i < t.n; ++i)
        os << "\t";
    return os;
}   // end operator<<

std::ostream& operator<<( std::ostream& os, const NL& nl)
{
    for ( int i = 0; i < nl.n; ++i)
        os << std::endl;
    return os;
}   // end operator<<


void nodeGroup( std::ostream& os)
{
    TB t(1), tt(2), ttt(3), tttt(4);
    NL n(1);
    os << "NODE \"GROUP\" {" << n;
    os << t << "NODE_NAME \"ModelGroup\"" << n;
    os << t << "PARENT_LIST {" << n;
    os << tt << "PARENT_COUNT 1" << n;
    os << tt << "PARENT 0 {" << n;
    os << ttt << "PARENT_NAME \"<NULL>\"" << n;
    os << ttt << "PARENT_TM {" << n;
    os << tttt << "1.0 0.0 0.0 0.0" << n;
    os << tttt << "0.0 1.0 0.0 0.0" << n;
    os << tttt << "0.0 0.0 1.0 0.0" << n;
    os << tttt << "0.0 0.0 0.0 1.0" << n;
    os << ttt << "}" << n;  // end PARENT_TM
    os << tt << "}" << n;  // end PARENT 0
    os << t << "}" << n;  // end PARENT_LIST
    os << "}" << n << n; // end NODE "GROUP"
}   // end nodeGroup


void nodeModel( std::ostream& os)
{
    TB t(1), tt(2), ttt(3), tttt(4);
    NL n(1);
    os << "NODE \"MODEL\" {" << n;
    os << t << "NODE_NAME \"Mesh0\"" << n;
    os << t << "PARENT_LIST {" << n;
    os << tt << "PARENT_COUNT 1" << n;
    os << tt << "PARENT 0 {" << n;
    os << ttt << "PARENT_NAME \"ModelGroup\"" << n;
    os << ttt << "PARENT_TM {" << n;
    os << tttt << "1.0 0.0 0.0 0.0" << n;
    os << tttt << "0.0 1.0 0.0 0.0" << n;
    os << tttt << "0.0 0.0 1.0 0.0" << n;
    os << tttt << "0.0 0.0 0.0 1.0" << n;
    os << ttt << "}" << n;  // end PARENT_TM
    os << tt << "}" << n;  // end PARENT 0
    os << t << "}" << n;  // end PARENT_LIST
    os << t << "RESOURCE_NAME \"Mesh0\"" << n;
    os << "}" << n << n; // end NODE "MODEL"
}   // end nodeModel

/*
void nodeLight( std::ostream& os, int lightID, const Vec3f& pos=Vec3f(0,0,0))
{
    TB t(1), tt(2), ttt(3), tttt(4);
    NL n(1);
    os << "NODE \"LIGHT\" {" << n;
    os << t << "NODE_NAME \"Light" << lightID << "\"" << n;
    os << t << "PARENT_LIST {" << n;
    os << tt << "PARENT_COUNT 1" << n;
    os << tt << "PARENT 0 {" << n;
    os << ttt << "PARENT_NAME \"<NULL>\"" << n;
    os << ttt << "PARENT_TM {" << n;
    os << tttt << "1.0 0.0 0.0 " << std::fixed << std::setprecision(6) << pos[0] << n;
    os << tttt << "0.0 1.0 0.0 " << std::fixed << std::setprecision(6) << pos[1] << n;
    os << tttt << "0.0 0.0 1.0 " << std::fixed << std::setprecision(6) << pos[2] << n;
    os << tttt << "0.0 0.0 0.0 1.0" << n;
    os << ttt << "}" << n;  // end PARENT_TM
    os << tt << "}" << n;  // end PARENT 0
    os << t << "}" << n;  // end PARENT_LIST
    os << t << "RESOURCE_NAME \"Light" << lightID << "\"" << n;
    os << "}" << n << n; // end NODE "LIGHT"
}   // end nodeLight


void resourceLight( std::ostream& os, int lightID)
{
    TB t(1), tt(2), ttt(3), tttt(4);
    NL n(1);
    os << "RESOURCE_LIST \"LIGHT\" {" << n;
    os << t << "RESOURCE_COUNT 1" << n;
    os << t << "RESOURCE 0 {" << n;
    os << tt << "RESOURCE_NAME \"Light" << lightID << "\"" << n;
    os << tt << "LIGHT_TYPE \"AMBIENT\"" << n;
    os << tt << "LIGHT_COLOR 1.0 1.0 1.0" << n;
    os << tt << "LIGHT_ATTENUATION 1.0 0.0 0.0" << n;
    os << tt << "LIGHT_INTENSITY 1.0" << n;
    os << t << "}" << n;
    os << "}" << n << n;
}   // end resourceLight
*/

void resourceListShader( std::ostream& os, bool hasTX)
{
    TB t(1), tt(2), ttt(3), tttt(4);
    NL n(1);
    os << "RESOURCE_LIST \"SHADER\" {" << n;
    os << t << "RESOURCE_COUNT 1" << n;
    os << t << "RESOURCE 0 {" << n;
    os << tt << "RESOURCE_NAME \"Shader0\"" << n;
    os << tt << "SHADER_MATERIAL_NAME \"Material0\"" << n;  // All shaders reference same material
    os << tt << "SHADER_ACTIVE_TEXTURE_COUNT " << ( hasTX ? 1 : 0) << n;
    if ( hasTX)
    {
        os << tt << "SHADER_TEXTURE_LAYER_LIST {" << n;
        os << ttt << "TEXTURE_LAYER 0 {" << n;
        os << tttt << "TEXTURE_NAME \"Texture0\"" << n;
        os << ttt << "}" << n;  // end TEXTURE_LAYER 0
        os << tt << "}" << n;  // end SHADER_TEXTURE_LAYER_LIST
    }   // end if
    os << t << "}" << n;  // end RESOURCE
    os << "}" << n << n; // end RESOURCE_LIST "SHADER"
}   // end resourceListShader


void modifierShading( std::ostream& os)
{
    TB t(1), tt(2), ttt(3), tttt(4), ttttt(5);
    NL n(1);
    os << "MODIFIER \"SHADING\" {" << n;
    os << t << "MODIFIER_NAME \"Mesh0\"" << n;
    os << t << "PARAMETERS {" << n;
    os << tt << "SHADER_LIST_COUNT 1" << n;
    os << tt << "SHADING_GROUP {" << n;
    os << ttt << "SHADER_LIST 0 {" << n;
    os << tttt << "SHADER_COUNT 1" << n;
    os << tttt << "SHADER_NAME_LIST {" << n;
    os << ttttt << "SHADER 0 NAME: \"Shader0\"" << n;
    os << tttt << "}" << n; // end SHADER_NAME_LIST
    os << ttt << "}" << n; // end SHADER_LIST
    os << tt << "}" << n; // end SHADING_GROUP
    os << t << "}" << n; // end PARAMETERS
    os << "}" << n; // end MODIFIER "SHADING"
}   // end modifierShading


void resourceListMaterial( std::ostream& os, const Colour &ems)
{
    TB t(1), tt(2);
    NL n(1);
	os << "RESOURCE_LIST \"MATERIAL\" {" << n;
    os << t << "RESOURCE_COUNT 1" << n;
    os << t << "RESOURCE 0 {" << n;
	os << tt << "RESOURCE_NAME \"Material0\"" << n;
	//os << tt << "MATERIAL_AMBIENT 1.0 1.0 1.0 1.0" << n;
	os << tt << "MATERIAL_AMBIENT 0.0 0.0 0.0 0.0" << n;
	//os << tt << "MATERIAL_DIFFUSE 1.0 1.0 1.0 1.0" << n;
	//os << tt << "MATERIAL_DIFFUSE 0.0 0.0 0.0 0.0" << n;    // Completely flat colour
	os << tt << "MATERIAL_DIFFUSE 0.4 0.4 0.4 0.4" << n;
	os << tt << "MATERIAL_SPECULAR 0.0 0.0 0.0 0.0" << n;
    os << tt << "MATERIAL_EMISSIVE " << ems[0] << " " << ems[1] << " " << ems[2] << " 1.0" << n;
	os << tt << "MATERIAL_REFLECTIVITY 0.0" << n;
	os << tt << "MATERIAL_OPACITY 1.0" << n;
	os << t << "}" << n; // end RESOURCE 0
	os << "}" << n << n; // end RESOURCE_LIST "MATERIAL"
}   // end resourceListMaterial


void resourceListTexture( std::ostream& os, const std::string& tgafname)
{
    TB t(1), tt(2);
    NL n(1);
    os << "RESOURCE_LIST \"TEXTURE\" {" << n;
	os << t << "RESOURCE_COUNT 1" << n;
    os << t << "RESOURCE 0 {" << n;
    os << tt << "RESOURCE_NAME \"Texture0\"" << n;
    os << tt << "TEXTURE_PATH \"" << tgafname << "\"" << n;
    os << t << "}" << n;    // end RESOURCE
	os << "}" << n << n; // end RESOURCE_LIST "TEXTURE"
}   // end resourceListTexture


struct ModelResource
{
    ModelResource( const Mesh &mesh, bool media9) : _mesh(mesh), _media9(media9)
    {
        const int matID = mesh.hasMaterials() ? *mesh.materialIds().begin() : -1;
        // Get repeatable sequence of face IDs and the unique set of texture coords for the material
        const IntSet* fids;
        if ( matID < 0)
            fids = &mesh.faces();
        else
            fids = &mesh.materialFaceIds(matID);

        if ( fids->empty())
            std::cerr << "[ERROR] r3dio::ModelResource: no facets found for material " << matID << std::endl;

        _fidv.resize( fids->size());
        int k = 0;
        int vid;
        for ( int fid : *fids)
        {
            _fidv[k++] = fid;
            if ( mesh.faceMaterialId(fid) >= 0)
            {
                const int* uvids = mesh.faceUVs(fid);
                for ( int i = 0; i < 3; ++i)
                {
                    // Only want to store unique UV offsets.
                    const int key = uvids[i];
                    if ( _uvmap.count(key) == 0)
                    {
                        _uvmap[key] = (int)_uvlist.size();  // Map the array index
                        _uvlist.push_back( &mesh.uv( matID, key));
                    }   // end if
                }   // end for
            }   // end if

            const int* vidxs = mesh.fvidxs(fid);
            for ( int i = 0; i < 3; ++i)
            {
                vid = vidxs[i];
                if ( _vmap.count(vid) == 0)
                {
                    _vmap[vid] = (int)_vidv.size();  // For mapping to index of this node's list from a Face.vindices array.
                    _vidv.push_back(vid);
                }   // end if
            }   // end for
        }   // end for
    }   // end ctor

    void writeMesh( std::ostream& os) const
    {
        _writeHeader(os);
        _writeShadingDescriptionList(os);
        _writeFacePositionList(os);
        _writeFaceNormalList(os);
        _writeFaceShadingList(os);
        const bool hasTX = _mesh.numMats() > 0;
        if ( hasTX)
            _writeFaceTextureCoordList(os);
        _writePositionList(os);
        _writeNormalList(os);
        if ( hasTX)
            _writeTextureCoordList(os);
    }   // end writeMesh

private:
    const Mesh &_mesh;
    const bool _media9;
    std::vector<int> _fidv;          // Predictable seq. of face IDs
    std::vector<int> _vidv;          // Predictable seq. of vertex IDs
    unordered_map<int,int> _vmap;    // Mesh vertexID --> MODEL_POSITION_LIST index
    unordered_map<int, int> _uvmap;  // Mesh uvID --> _uvlist index
    std::vector<const Vec2f*> _uvlist;  // List of texture UVs to output in MODEL_TEXTURE_COORD_LIST


    void _writeHeader( std::ostream& os) const
    {
        TB ttt(3);
        NL n(1);
        os << ttt << "FACE_COUNT " << _fidv.size() << n;
        os << ttt << "MODEL_POSITION_COUNT " << _vidv.size() << n;
        os << ttt << "MODEL_NORMAL_COUNT " << (_fidv.size() * 3) << n;
        os << ttt << "MODEL_DIFFUSE_COLOR_COUNT 0" << n;
        os << ttt << "MODEL_SPECULAR_COLOR_COUNT 0" << n;
        os << ttt << "MODEL_TEXTURE_COORD_COUNT " << _uvmap.size() << n;
        os << ttt << "MODEL_BONE_COUNT 0" << n; // No skeleton
        os << ttt << "MODEL_SHADING_COUNT 1" << n;
    }   // end _writeHeader


    void _writeShadingDescriptionList( std::ostream& os) const
    {
        const bool hasTX = _mesh.numMats() > 0;
        TB ttt(3), tttt(4), ttttt(5), tttttt(6);
        NL n(1);
        os << ttt << "MODEL_SHADING_DESCRIPTION_LIST {" << n;
        os << tttt << "SHADING_DESCRIPTION 0 {" << n;
        os << ttttt << "TEXTURE_LAYER_COUNT " << (hasTX ? 1 : 0) << n;    // No multi-texturing!
        if ( hasTX)
        {
            os << ttttt << "TEXTURE_COORD_DIMENSION_LIST {" << n;
            os << tttttt << "TEXTURE_LAYER 0 DIMENSION: 2" << n;    // 2D texture map
            os << ttttt << "}" << n; // end TEXTURE_COORD_DIMENSION_LIST
        }   // end if
        os << ttttt << "SHADER_ID 0" << n;
        os << tttt << "}" << n; // end SHADING_DESCRIPTION
        os << ttt << "}" << n;  // end MODEL_SHADING_DESCRIPTION_LIST
    }   // end _writeShadingDescriptionList


    // For each face, record the vertex IDs it's composed of - these must be the
    // index of the vertices as given in MODEL_POSITION_LIST, so map using vmap.
    // Collect all face indices into a repeatable list for subsequent nodes (texture)
    void _writeFacePositionList( std::ostream& os) const
    {
        os << TB(3) << "MESH_FACE_POSITION_LIST {" << NL(1);
        TB ttt(3), tttt(4);
        NL n(1);
        for ( int fid : _fidv)
        {
            const int* vidxs = _mesh.fvidxs( fid);
            os << tttt << _vmap.at(vidxs[0]) << " " << _vmap.at(vidxs[1]) << " " << _vmap.at(vidxs[2]) << n;
        }   // end for
        os << ttt << "}" << n;
    }   // end _writeFacePositionList


    void _writeFaceNormalList( std::ostream& os) const
    {
        os << TB(3) << "MESH_FACE_NORMAL_LIST {" << NL(1);
        TB ttt(3), tttt(4);
        NL n(1);
        int i = 0;
        for ( size_t j = 0; j < _fidv.size(); ++j, i += 3)
            os << tttt << i << " " << (i+1) << " " << (i+2) << n;
        os << ttt << "}" << n;
    }   // end _writeFaceNormalList


    // For each face, record the shader ID (as stored in this file)
    void _writeFaceShadingList( std::ostream& os) const
    {
        TB ttt(3), tttt(4);
        NL n(1);
        os << ttt << "MESH_FACE_SHADING_LIST {" << n;
        for ( size_t j = 0; j < _fidv.size(); ++j)
            os << tttt << 0 << n;
        os << ttt << "}" << n;  // end MESH_FACE_SHADING_LIST
    }   // end _writeFaceShadingList


    int _getUVListIndex( int faceId, int uvOrderIndex/*[0,2]*/) const
    {
        assert( _mesh.faceMaterialId(faceId) >= 0);
        const int uvid = _mesh.faceUVs(faceId)[uvOrderIndex];
        return _uvmap.at( uvid);
    }   // end _getUVListIndex


    // Write out texture coordinates if Mesh has materials.
    void _writeFaceTextureCoordList( std::ostream& os) const
    {
        TB ttt(3), tttt(4), ttttt(5);
        NL n(1);
        const int nf = int(_fidv.size());
        os << ttt << "MESH_FACE_TEXTURE_COORD_LIST {" << n;
        for ( int i = 0; i < nf; ++i)
        {
            const int fid = _fidv[i];
            const int uv0 = _getUVListIndex( fid, 0);
            const int uv1 = _getUVListIndex( fid, 1);
            const int uv2 = _getUVListIndex( fid, 2);
            os << tttt << "FACE " << i << " {" << n;
            os << ttttt << "TEXTURE_LAYER 0 TEX_COORD: " << uv0 << " " << uv1 << " " << uv2 << n;
            os << tttt << "}" << n; // end FACE i
        }   // end for
        os << ttt << "}" << n;  // end MESH_FACE_TEXTURE_COORD_LIST
    }   // end _writeFaceTextureCoordList


    // Output mesh positions (mapping the vertex ID to the position of the vertex in this list)
    void _writePositionList( std::ostream& os) const
    {
        TB ttt(3), tttt(4);
        NL n(1);
        os << ttt << "MODEL_POSITION_LIST {" << n;
        //os << std::fixed << std::setprecision(6);

        if ( _media9)
        {
            for ( int vid : _vidv)
            {
                const Vec3f& v = _mesh.vtx(vid);
                os << tttt << v[0] << " " << -v[2] << " " << v[1] << n;
            }   // end for
        }   // end if
        else
        {
            for ( int vid : _vidv)
            {
                const Vec3f& v = _mesh.vtx(vid);
                os << tttt << v[0] << " " << v[1] << " " << v[2] << n;
            }   // end for
        }   // end else

        os << ttt << "}" << n;  // end MODEL_POSITION_LIST
    }   // end _writePositionList


    // vertex normals not used
    void _writeNormalList( std::ostream& os) const
    {
        static const Vec3f NRM(0,0,0);
        TB ttt(3), tttt(4);
        NL n(1);
        os << ttt << "MODEL_NORMAL_LIST {" << n;
        //os << std::fixed << std::setprecision(6);
        for ( size_t j = 0; j < _fidv.size(); ++j)
        {
            os << tttt << NRM[0] << " " << NRM[1] << " " << NRM[2] << n;
            os << tttt << NRM[0] << " " << NRM[1] << " " << NRM[2] << n;
            os << tttt << NRM[0] << " " << NRM[1] << " " << NRM[2] << n;
        }   // end for
        os << ttt << "}" << n;  // end MODEL_NORMAL_LIST
    }   // end _writeNormalList


    void _writeTextureCoordList( std::ostream& os) const
    {
        TB ttt(3), tttt(4);
        NL n(1);
        os << ttt << "MODEL_TEXTURE_COORD_LIST {" << n;
        //os << std::fixed << std::setprecision(6);
        for ( const Vec2f* uv : _uvlist)
            os << tttt << std::fixed << (*uv)[0] << " " << (*uv)[1] << " " << 0.0 << " " << 0.0 << n;
        os << ttt << "}" << n;  // end MODEL_TEXTURE_COORD_LIST
    }   // end _writeTextureCoordList
};  // end struct


// Write the mesh data in IDTF format. Only vertex, face, and texture mapping info are stored.
std::string _writeFile( const Mesh &mesh, bool media9, const Colour &ems,
                const std::string& filename, const std::string &tgafname)
{
    const int nTX = tgafname.empty() ? 0 : 1;
    std::string errMsg;
    std::ofstream ofs;
    try
    {
        ofs.open( filename.c_str(), std::ios::out);

        TB t(1), tt(2);
        NL n(1);
        const std::string meshName("Model");

        // File header
        ofs << "FILE_FORMAT \"IDTF\"" << n;
        ofs << "FORMAT_VERSION 100" << n << n;

        nodeGroup( ofs);
        nodeModel( ofs);

        //nodeLight( ofs, 1);
        //resourceLight( ofs, 1);

        ofs << "RESOURCE_LIST \"MODEL\" {" << n;
        ofs << t << "RESOURCE_COUNT 1" << n;
        ofs << t << "RESOURCE 0 {" << n;
        ofs << tt << "RESOURCE_NAME \"Mesh0\"" << n;
        ofs << tt << "MODEL_TYPE \"MESH\"" << n;
        ofs << tt << "MESH {" << n;
        const ModelResource modelResource( mesh, media9);
        modelResource.writeMesh( ofs);
        ofs << tt << "}" << n;    // end MESH
        ofs << t << "}" << n;    // end RESOURCE
        ofs << "}" << n << n;

        resourceListShader( ofs, nTX > 0);
        resourceListMaterial( ofs, ems);
        if ( !tgafname.empty())
            resourceListTexture( ofs, tgafname);
        modifierShading( ofs);

        ofs.close();
    }   // end try
    catch ( const std::exception &e)
    {
        errMsg = e.what();
    }   // end catch

    return errMsg;
}   // end _writeFile

}   // end namespace



// protected
bool IDTFExporter::doSave( const Mesh& inMesh, const std::string& filename)
{
    _reset();
    // Set the texture map filename (if present) and save out as TGA adjacent to mesh.
    using Path = boost::filesystem::path;
    const Path mpath( filename);
    Path tpath = mpath.parent_path();  // Directory mesh is being saved in
    tpath /= mpath.stem();             // Use stem of save filename as basis for texture filename

    const Mesh* mesh = nullptr;
    Mesh::Ptr nMesh;
    if ( inMesh.numMats() <= 1)
        mesh = &inMesh;
    else
    {
        std::cerr << "[INFO] r3dio::IDTFExporter::doSave: Multi-materials merged for export" << std::endl;
        nMesh = inMesh.deepCopy();
        nMesh->mergeMaterials();
        mesh = nMesh.get();
    }   // end else

    std::string tgafname;
    if ( mesh->hasMaterials())
    {
        // Texture needs to be in TGA format for IDTF intermediate format.
        cv::Mat tx = mesh->texture( *mesh->materialIds().begin());
        if ( tx.empty())
        {
            std::ostringstream eoss;
            eoss << "[ERROR] r3dio::IDTFExporter::doSave: Material has no texture!";
            setErr(eoss.str());
            return false;
        }   // end else

        std::ostringstream oss;
        oss << tpath.string() << "_M0.tga";
        tgafname = oss.str();
        _tgafiles.push_back( tgafname);    // Record to delete on destruction
        if ( !saveTGA( tx, tgafname))
            return false;
    }   // end if

    _idtffile = filename;
    const std::string errMsg = _writeFile( *mesh, _media9, _ems, filename, tgafname);
    if ( !errMsg.empty())
        setErr( "Unable to write IDTF text file: " + errMsg);
    return errMsg.empty();
}   // end doSave

