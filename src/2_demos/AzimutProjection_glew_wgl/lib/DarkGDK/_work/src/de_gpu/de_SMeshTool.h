#pragma once
#include "de_SMesh.h"

namespace de {

// ===========================================================================
struct SMeshBufferTool
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.SMeshBufferTool")

   static void
   transformPoints( std::vector< glm::vec3 > & points, glm::dmat4 const & modelMat );
   static void
   rotatePoints( std::vector< glm::vec3 > & points, float a, float b, float c );
   static void
   rotateTexCoords90_onlyQuads( SMeshBuffer & mesh );
   static SMeshBuffer
   tesselate( SMeshBuffer const & src );
   static void
   fuse( SMeshBuffer & dst, SMeshBuffer const & src, glm::vec3 const & offset = glm::vec3() );
   static void
   fuse( SMeshBuffer & dst, std::vector< SMeshBuffer > const & meshes );
   static uint32_t
   countVertices( std::vector< SMeshBuffer > const & liste );
   static uint32_t
   countIndices( std::vector< SMeshBuffer > const & liste );
   static std::vector< Triangle3f >
   createCollisionTriangles( SMeshBuffer const & b );
   static void
   computeBoundingBox( SMeshBuffer & o );
   static Box3f
   computeBoundingBox( std::vector< S3DVertex > const & vertices );
   static void
   computeNormals( SMeshBuffer & o );
   static void
   computeNormals( std::vector< S3DVertex > & vertices, std::vector< uint32_t > const & indices );
   static void
   bumpVertices( SMeshBuffer & o, Image* bumpMap, float bumpScale = 1.0f );

   static void
   flipX( SMeshBuffer & o );
   static void
   flipY( SMeshBuffer & o );
   static void
   flipZ( SMeshBuffer & o );
   static void
   flipXY( SMeshBuffer & o );
   static void
   flipYZ( SMeshBuffer & o );
   static void
   flipXZ( SMeshBuffer & o );
   static void
   flipNormals( SMeshBuffer & o );
   static void
   flipVertexPosYZ( SMeshBuffer & o );
   static void
   flipWinding( SMeshBuffer & o );

   static void
   translateVertices( SMeshBuffer & o, glm::vec3 const & offset );
   static void
   translateVertices( SMeshBuffer & o, glm::vec3 const & offset, uint32_t vStart );
   static void
   translateVertices( SMeshBuffer & o, float x, float y, float z );
   static void
   transformVertices( SMeshBuffer & o, glm::dmat4 const & modelMat = glm::dmat4(1.0) );

   static void
   randomRadialDisplace( SMeshBuffer & o, float h );

   static void
   rotateVertices( SMeshBuffer & o, float a, float b, float c );
   static void
   scaleVertices( SMeshBuffer & o, float scale = 1.01f );
   static void
   scaleVertices( SMeshBuffer & o, float sx, float sy, float sz );

   static void
   translateNormals( SMeshBuffer & o, glm::vec3 const & offset );
   static void
   translateNormals( SMeshBuffer & o, float nx, float ny, float nz );
   static void
   setNormals( SMeshBuffer & o, glm::vec3 const & normal );
   static void
   setNormals( SMeshBuffer & o, float nx, float ny, float nz );
   static void
   setNormalZ( SMeshBuffer & o, float nz );

   static void
   scaleTexture( SMeshBuffer & o, float u, float v );
   static void
   transformTexCoords( SMeshBuffer & o, Recti const & r_atlas, int atlas_w, int atlas_h );
   static void
   colorVertices( SMeshBuffer & o, uint32_t color );
   static void
   centerVertices( SMeshBuffer & o );
/*
   static SMeshBuffer
   createLines();

   static SMeshBuffer
   createTriangles();
*/
   static void
   addVertex( SMeshBuffer & o, S3DVertex const & vertex );
   static void
   addVertex( SMeshBuffer & o, glm::vec3 const & pos, glm::vec3 const & nrm, uint32_t color, glm::vec2 const & tex );
   static void
   addVertex( SMeshBuffer & o, glm::vec3 const & pos, uint32_t color );
   static void
   addLine( SMeshBuffer & o, S3DVertex const & a, S3DVertex const & b );
   static void
   addTriangle( SMeshBuffer & o, S3DVertex const & a, S3DVertex const & b, S3DVertex const & c );
   static void
   addLine( SMeshBuffer & o, glm::vec3 const & a, glm::vec3 const & b, uint32_t color = 0xFFFFFFFF );
   static void
   addLine( SMeshBuffer & o, glm::vec3 const & a, glm::vec3 const & b, uint32_t colorA, uint32_t colorB );
   static void
   addLine( SMeshBuffer & o, float x1, float y1, float x2, float y2, float z = 0.0f, uint32_t color = 0xFFFFFFFF );
   static void
   addLine( SMeshBuffer & o, float x1, float y1, float x2, float y2, float z, uint32_t colorA, uint32_t colorB );
   static void
   addLineTriangle( SMeshBuffer & o, glm::vec3 const & a, glm::vec3 const & b, glm::vec3 const & c, uint32_t color = 0xFFFFFFFF );
   static void
   addLineTriangle( SMeshBuffer & o, glm::vec3 const & a, glm::vec3 const & b, glm::vec3 const & c, uint32_t colorA, uint32_t colorB, uint32_t colorC );
   static void
   addLineQuad( SMeshBuffer & o, glm::vec3 const & a, glm::vec3 const & b, glm::vec3 const & c, glm::vec3 const & d, uint32_t color = 0xFFFFFFFF );
   static void
   addLineQuad( SMeshBuffer & o, glm::vec3 const & a, glm::vec3 const & b, glm::vec3 const & c, glm::vec3 const & d,
                uint32_t colorA, uint32_t colorB, uint32_t colorC = 0xFFFFFFFF, uint32_t colorD = 0xFFFFFFFF );
   static void
   addLineBox( SMeshBuffer & o, Box3f const & box, uint32_t color );

   // =======================================================================
   // DebugGeometry:
   // =======================================================================
   static void
   addLineNormals( SMeshBuffer & o, SMeshBuffer const & src, float n_len, uint32_t color );
   static void
   addWireframe( SMeshBuffer & o, SMeshBuffer const & src, float outScale, uint32_t color );
   static SMeshBuffer
   createWireframe( SMeshBuffer const & src, float scale = 1.0f, uint32_t color = 0 );
   static SMeshBuffer
   createScaled( SMeshBuffer const & src, float scale = 1.01f );
   static SMeshBuffer
   createTranslated( SMeshBuffer const & src, glm::vec3 pos );
   static SMeshBuffer
   createLineBox( Box3f const & box, uint32_t color );
   static SMeshBuffer
   createLineNormals( SMeshBuffer const & src, float normalLen, uint32_t color );
};

// ===========================================================================
struct SMeshTool
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.SMeshTool")
   static void
   transformVertices( SMesh & o, glm::dmat4 const & modelMat = glm::dmat4(1.0) );

   static void
   flipNormals( SMeshBuffer & o );
   static void
   flipWinding( SMeshBuffer & o );

   static void
   flipX(SMesh & o);
   static void
   flipY(SMesh & o);
   static void
   flipZ(SMesh & o);
   static void
   flipXY(SMesh & o);
   static void
   flipYZ(SMesh & o);
   static void
   flipXZ(SMesh & o);
   static void
   flipNormals( SMesh & o );
   static void
   flipWinding( SMesh & o );
   static void
   colorVertices( SMesh & o, uint32_t color );
   static void
   translateVertices( SMesh & o, glm::vec3 const & offset );
   static void
   translateVertices( SMesh & o, float x, float y, float z );
   static void
   rotateVertices( SMesh & o, float a, float b, float c );
   static void
   rotateVertices( SMesh & o, glm::vec3 const & degrees );
   static void
   scaleVertices( SMesh & o, glm::vec3 const & scale );
   static void
   scaleVertices( SMesh & o, float x, float y, float z );
   static void
   centerVertices( SMesh & o );
   static void
   recalculateNormals( SMesh & o );
   static void
   fitVertices( SMesh & o, float maxSize = 1000.0f );
   static void
   removeDoubleVertices( SMesh & mesh );
   static void
   removeDoubleMaterials( SMesh & mesh );
   static void
   removeEmptyMeshBuffer( SMesh & mesh );
};

} // end namespace de.
