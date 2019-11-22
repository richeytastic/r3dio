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

#include <AssetExporter.h>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <cassert>
using r3dio::AssetExporter;
using r3d::Vec3f;
using r3d::Vec2f;


namespace {

using Path = boost::filesystem::path;
using byte = unsigned char;


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

/*
std::string saveImage( aiMaterial* mat,
                       const cv::Mat img,
                       const Path& imgroot,
                       int aitt,                    // aiTextureType_SPECULAR, aiTextureType_AMBIENT, aiTextureType_DIFFUSE
                       std::string fext = ".png")   // "_specular.png", "_ambient.png", "_diffuse.png"
{
    Path imgpath( imgroot.string() + fext);
    const aiString tfile( imgpath.filename().string());
    mat->AddProperty( &tfile, AI_MATKEY_TEXTURE( aitt, 0));
    if ( !boost::filesystem::exists(imgpath))   // Save if not already present
    {
        if ( !cv::imwrite( imgpath.string(), img))
            return "Cannot save texture to " + imgpath.string();
    }   // end if
    return "";
}   // end sameImage
*/


std::string saveMaterialTexture( aiMaterial* mat, const r3d::Mesh& mesh, int matId, const std::string& fname)
{
    const Path filepath(fname);
    const Path fstem = filepath.stem();
    const Path ppath = filepath.parent_path();

    float ka = 0;
    float kd = 0;
    float ks = 0;
    mat->AddProperty<float>( &ka, 1, AI_MATKEY_COLOR_AMBIENT);
    mat->AddProperty<float>( &kd, 1, AI_MATKEY_COLOR_DIFFUSE);
    mat->AddProperty<float>( &ks, 1, AI_MATKEY_COLOR_SPECULAR);

    std::ostringstream oss;
    oss << fstem.string() << "_" << matId;
    const aiString matName( oss.str());
    mat->AddProperty( &matName, AI_MATKEY_NAME);  // newmtl

    const cv::Mat& txmap = mesh.texture(matId);

    const Path imgroot = ppath / fstem;
    std::string err;

    if ( !txmap.empty())
    {
        Path imgpath( imgroot.string() + ".png");
        const aiString tfile( imgpath.filename().string());
        mat->AddProperty( &tfile, AI_MATKEY_TEXTURE( aiTextureType_AMBIENT, 0));
        mat->AddProperty( &tfile, AI_MATKEY_TEXTURE( aiTextureType_SPECULAR, 0));
        mat->AddProperty( &tfile, AI_MATKEY_TEXTURE( aiTextureType_DIFFUSE, 0));
        if ( !boost::filesystem::exists(imgpath))   // Save if not already present
        {
            if ( !cv::imwrite( imgpath.string(), txmap))
                err = "Cannot save texture to " + imgpath.string();
        }   // end if
    }   // end if
    
    return err;
}   // end saveMaterialTexture


// Set the mesh points, texture coords, and face (polygon) info.
void setMaterial( aiMesh* mesh, const r3d::Mesh& model, int matId, IntSet& remfids)
{
    const IntSet& fids = model.materialFaceIds( matId);

    const size_t nFaces = fids.size();
    mesh->mNumFaces = nFaces;
    mesh->mFaces = new aiFace[mesh->mNumFaces];
    mesh->mNumVertices = 3 * nFaces;
    mesh->mVertices = new aiVector3D[mesh->mNumVertices];

    mesh->mNumUVComponents[0] = mesh->mNumVertices;
    mesh->mTextureCoords[0] = new aiVector3D[mesh->mNumUVComponents[0]];

    unsigned int j = 0; // AssImp vertex ID
    unsigned int i = 0; // AssImp face ID
    for ( int fid : fids)
    {
        remfids.erase(fid);
        const int* uvids = model.faceUVs(fid);
        const int* vtxs = model.fvidxs(fid);

        aiFace& face = mesh->mFaces[i++];
        face.mNumIndices = 3;
        face.mIndices = new unsigned int[face.mNumIndices];

        for ( int k = 0; k < 3; ++k)    // 3 vertices of triangle
        {
            const Vec3f& v = model.vtx(vtxs[k]);
            aiVector3D& vertex = mesh->mVertices[j];
            vertex[0] = v[0];   // x
            vertex[1] = v[1];   // y
            vertex[2] = v[2];   // z

            const Vec2f& uv = model.uv( matId, uvids[k]);
            aiVector3D& texture = mesh->mTextureCoords[0][j];
            texture[0] = uv[0]; // u
            texture[1] = uv[1]; // v
            texture[2] = 0;     // not used

            face.mIndices[k] = j++;
        }   // end for
    }   // end foreach
}   // end setMaterial


void setNonMaterialMesh( aiMesh* mesh, const r3d::Mesh& model, const IntSet& remfids)
{
    const size_t nFaces = remfids.size();
    mesh->mNumFaces = nFaces;
    mesh->mFaces = new aiFace[mesh->mNumFaces];
    mesh->mNumVertices = 3 * nFaces;
    mesh->mVertices = new aiVector3D[mesh->mNumVertices];

    mesh->mNumUVComponents[0] = 0;
    mesh->mTextureCoords[0] = new aiVector3D[mesh->mNumUVComponents[0]];

    unsigned int j = 0; // AssImp vertex ID
    unsigned int i = 0; // AssImp face ID
    for ( int fid : remfids)
    {
        assert( model.faceMaterialId( fid) == -1);  // Should be true since face not associated with Material.
        assert( model.faceUVs( fid) == nullptr);       // Should return NULL since not associated with a Material.

        const int* vtxs = model.fvidxs(fid);

        aiFace& face = mesh->mFaces[i++];
        face.mNumIndices = 3;
        face.mIndices = new unsigned int[face.mNumIndices];

        for ( int k = 0; k < 3; ++k)
        {
            const Vec3f& v = model.vtx(vtxs[k]);
            aiVector3D& vertex = mesh->mVertices[j];
            vertex[0] = v[0];   // x
            vertex[1] = v[1];   // y
            vertex[2] = v[2];   // z

            face.mIndices[k] = j++;
        }   // end for
    }   // end foreach
}   // end setNonMaterialMesh


struct AiMesh
{
    explicit AiMesh() : _mat(new aiMaterial), _mesh(new aiMesh) {}
    aiMaterial* _mat;
    aiMesh* _mesh;
};  // end struct


// Since the Assimp library in the version used here doesn't do model export well,
// we have to guess the internals of the aiScene object and trust that we're not
// doubly allocating memory here (which could result in leaks). Testing deleting
// the aiScene causes the internals set here to also be deleted, so aiScene
// destruction only requires the delete operator on the aiScene object itself.
aiScene* createSceneFromMeshes( std::vector<AiMesh>& meshes)
{
    aiScene* scene = new aiScene;
    scene->mRootNode = new aiNode;

    int nmats = (int)meshes.size();
    scene->mRootNode->mMeshes = new unsigned int[nmats];
    scene->mRootNode->mNumMeshes = nmats;

    scene->mNumMeshes = nmats;
    scene->mNumMaterials = nmats;
    scene->mMeshes = new aiMesh*[nmats];
    scene->mMaterials = new aiMaterial*[nmats];

    for ( int i = 0; i < nmats; ++i)
    {
        scene->mRootNode->mMeshes[i] = i;
        meshes[i]._mesh->mMaterialIndex = i;
        scene->mMaterials[i] = meshes[i]._mat;
        scene->mMeshes[i] = meshes[i]._mesh;
    }   // end for

    return scene;
}   // end createSceneFromMeshes


}   // end namespace


// public
AssetExporter::AssetExporter() : r3dio::MeshExporter()
{
    std::unordered_set<std::string> disallowed;
    disallowed.insert("3d");
    disallowed.insert("assbin");  // Leave as import only
    disallowed.insert("assxml");
    disallowed.insert("dae"); // Not allowed - no exporter available
    disallowed.insert("pk3"); // Not available
    disallowed.insert("xml"); // Too generic
    disallowed.insert("cob");
    disallowed.insert("scn");
    disallowed.insert("mesh.xml");  // Too generic
    disallowed.insert("stp");   // Importer doesn't work
    disallowed.insert("glb");   // Doesn't work correctly!
    disallowed.insert("gltf");  // Doesn't work correctly!
    disallowed.insert("x");     // Doesn't work for large files

    std::unordered_set<std::string> descSet;  // Don't add same descriptions more than once.
    Assimp::Exporter exporter;
    const size_t n = exporter.GetExportFormatCount();
    for ( size_t i = 0; i < n; ++i)
    {
        const aiExportFormatDesc* efd = exporter.GetExportFormatDescription(i);
        const std::string ext = efd->fileExtension;
        const std::string desc = efd->description;

        if ( ext.empty() || desc.empty() || descSet.count(desc) || disallowed.count(ext))
            continue;

        descSet.insert(desc);
        _available[ext] = desc;
    }   // end for
}   // end ctor


// public
bool AssetExporter::enableFormat( const std::string& ext)
{
    if ( _available.count(ext) == 0)
        return false;

    const std::string testfname = "tonythetiger." + ext;
    if ( isSupported( testfname))
        return true;

    return addSupported( ext, _available.at(ext));
}   // end enableFormat


// protected
bool AssetExporter::doSave( const r3d::Mesh& mesh, const std::string& fname)
{
    IntSet remfids = mesh.faces();   // Copy out. Used to track parsing of all polygons.
    std::vector<AiMesh> meshes;

    // Set a mesh for each material (having texture coordinates associated with polygons).
    const IntSet& matIds = mesh.materialIds();
    for ( int matId : matIds)
    {
        meshes.resize( meshes.size()+1);
        AiMesh &aim = meshes.back();
        setMaterial( aim._mesh, mesh, matId, remfids);
        std::string txSaveErr = saveMaterialTexture( aim._mat, mesh, matId, fname);
        if (!txSaveErr.empty())
        {
            setErr( "AssetExporter::write( " + fname + "): " + txSaveErr);
            return false;
        }   // end if
        //std::cout << "Saved textures for material " << matId << std::endl;
    }   // end foreach

    // Polygons not attached to a material need to be included in the scene as a mesh without texture coordinates.
    if ( !remfids.empty())
    {
        //std::cout << "Creating non-material mesh" << std::endl;
        meshes.resize( meshes.size()+1);
        AiMesh &aim = meshes.back();
        setNonMaterialMesh( aim._mesh, mesh, remfids);
    }   // end if

    //std::cout << "Creating scene from " << meshes.size() << " meshes" << std::endl;
    aiScene* scene = createSceneFromMeshes( meshes);

    bool savedOkay = false;
    std::string fext = getExtension(fname);
    //std::cout << "Saving scene using Assimp::Exporter to " << fname << " with " << fext << " format" << std::endl;
    Assimp::Exporter exporter;
    if ( exporter.Export( scene, fext, fname) == AI_SUCCESS)
        savedOkay = true;
    else
        setErr( "AssetExporter::write( " + fname + "): " + "Cannot save model! Assimp::Exporter error: " + exporter.GetErrorString());

    //std::cout << "Deleting scene, savedokay = " << std::boolalpha << savedOkay << std::endl;
    delete scene;
    return savedOkay;
}   // end doSave



