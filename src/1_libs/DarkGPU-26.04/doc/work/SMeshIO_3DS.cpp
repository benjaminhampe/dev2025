#include "SMeshIO_3DS.hpp"
#include "VideoDriver.hpp"

namespace de {
namespace gpu {
namespace {

enum e3DSChunk
{
   // Primary chunk
    C3DS_MAIN3DS = 0x4D4D,

   // Main Chunks
    C3DS_EDIT3DS = 0x3D3D,
    C3DS_KEYF3DS = 0xB000,
    C3DS_VERSION = 0x0002,
    C3DS_MESHVERSION = 0x3D3E,

   // sub chunks of C3DS_EDIT3DS
    C3DS_EDIT_MATERIAL = 0xAFFF,
    C3DS_EDIT_OBJECT   = 0x4000,

   // sub chunks of C3DS_EDIT_MATERIAL
    C3DS_MATNAME       = 0xA000,
    C3DS_MATAMBIENT    = 0xA010,
    C3DS_MATDIFFUSE    = 0xA020,
    C3DS_MATSPECULAR   = 0xA030,
    C3DS_MATSHININESS  = 0xA040,
    C3DS_MATSHIN2PCT   = 0xA041,
    C3DS_TRANSPARENCY  = 0xA050,
    C3DS_TRANSPARENCY_FALLOFF  = 0xA052,
    C3DS_REFL_BLUR     = 0xA053,
    C3DS_TWO_SIDE      = 0xA081,
    C3DS_WIRE          = 0xA085,
    C3DS_SHADING       = 0xA100,
    C3DS_MATTEXMAP     = 0xA200,
    C3DS_MATSPECMAP    = 0xA204,
    C3DS_MATOPACMAP    = 0xA210,
    C3DS_MATREFLMAP    = 0xA220,
    C3DS_MATBUMPMAP    = 0xA230,
    C3DS_MATMAPFILE    = 0xA300,
    C3DS_MAT_TEXTILING = 0xA351,
    C3DS_MAT_USCALE    = 0xA354,
    C3DS_MAT_VSCALE    = 0xA356,
    C3DS_MAT_UOFFSET   = 0xA358,
    C3DS_MAT_VOFFSET   = 0xA35A,

   // subs of C3DS_EDIT_OBJECT
    C3DS_OBJTRIMESH    = 0x4100,

   // subs of C3DS_OBJTRIMESH
    C3DS_TRIVERT       = 0x4110,
    C3DS_POINTFLAGARRAY= 0x4111,
    C3DS_TRIFACE       = 0x4120,
    C3DS_TRIFACEMAT    = 0x4130,
    C3DS_TRIUV         = 0x4140,
    C3DS_TRISMOOTH     = 0x4150,
    C3DS_TRIMATRIX     = 0x4160,
    C3DS_MESHCOLOR     = 0x4165,
    C3DS_DIRECT_LIGHT  = 0x4600,
    C3DS_DL_INNER_RANGE= 0x4659,
    C3DS_DL_OUTER_RANGE= 0x465A,
    C3DS_DL_MULTIPLIER = 0x465B,
    C3DS_CAMERA        = 0x4700,
    C3DS_CAM_SEE_CONE  = 0x4710,
    C3DS_CAM_RANGES    = 0x4720,

   // subs of C3DS_KEYF3DS
    C3DS_KF_HDR        = 0xB00A,
    C3DS_AMBIENT_TAG   = 0xB001,
    C3DS_OBJECT_TAG    = 0xB002,
    C3DS_CAMERA_TAG    = 0xB003,
    C3DS_TARGET_TAG    = 0xB004,
    C3DS_LIGHTNODE_TAG = 0xB005,
    C3DS_KF_SEG        = 0xB008,
    C3DS_KF_CURTIME    = 0xB009,
    C3DS_KF_NODE_HDR   = 0xB010,
    C3DS_PIVOTPOINT    = 0xB013,
    C3DS_BOUNDBOX      = 0xB014,
    C3DS_MORPH_SMOOTH  = 0xB015,
    C3DS_POS_TRACK_TAG = 0xB020,
    C3DS_ROT_TRACK_TAG = 0xB021,
    C3DS_SCL_TRACK_TAG = 0xB022,
    C3DS_NODE_ID       = 0xB030,

   // Viewport definitions
    C3DS_VIEWPORT_LAYOUT = 0x7001,
    C3DS_VIEWPORT_DATA   = 0x7011,
    C3DS_VIEWPORT_DATA_3 = 0x7012,
    C3DS_VIEWPORT_SIZE   = 0x7020,

   // different color chunk types
    C3DS_COL_RGB    = 0x0010,
    C3DS_COL_TRU    = 0x0011,
    C3DS_COL_LIN_24 = 0x0012,
    C3DS_COL_LIN_F  = 0x0013,

   // percentage chunk types
    C3DS_PERCENTAGE_I = 0x0030,
    C3DS_PERCENTAGE_F = 0x0031,

    C3DS_CHUNK_MAX		= 0xFFFF
};


// byte-align structures
#pragma pack( push )
#pragma pack( 1 )

struct ChunkHeader
{
   uint16_t id;
   int32_t length;
};


struct ChunkData
{
   ChunkData() : read(0) {}

   ChunkHeader header;
   int32_t read;
};

// Default alignment
#pragma pack( pop )

struct SCurrentMaterial
{
   de::gpu::Material Material;
   std::string Name;
   std::string Filename[5];
   float Strength[5];

   void clear()
   {
      Material = de::gpu::Material();
      Name = "";
      Filename[0]="";
      Filename[1]="";
      Filename[2]="";
      Filename[3]="";
      Filename[4]="";
      Strength[0]=0.f;
      Strength[1]=0.f;
      Strength[2]=0.f;
      Strength[3]=0.f;
      Strength[4]=0.f;
   }
};

struct SMaterialGroup
{
   SMaterialGroup() : faceCount(0), faces(0) {}
   SMaterialGroup( SMaterialGroup const & o ) { *this = o; }
   ~SMaterialGroup() { clear(); }

   void clear()
   {
      delete [] faces;
      faces = 0;
      faceCount = 0;
   }

   void operator= ( SMaterialGroup const & o )
   {
      MaterialName = o.MaterialName;
      faceCount = o.faceCount;
      faces = new uint16_t[faceCount];
      for (uint16_t i=0; i<faceCount; ++i)
         faces[i] = o.faces[i];
   }

   std::string MaterialName;
   uint16_t faceCount;
   uint16_t* faces;
};

//, VideoDriver* driver )
typedef glm::vec4 SColor;

struct SMeshLoad3DSFile
{
   DE_CREATE_LOGGER("de.gpu.SMeshLoad3DSFile")
   VideoDriver* m_Driver;
   float* Vertices;
   uint16_t* Indices;
   uint32_t* SmoothingGroups;
   std::vector< uint32_t > TempIndices;
   float* TCoords;
   uint16_t CountVertices;
   uint16_t CountFaces; // = CountIndices/4
   uint16_t CountTCoords;
   std::vector< SMaterialGroup > MaterialGroups;
   SCurrentMaterial CurrentMaterial;
   std::vector< SCurrentMaterial > Materials;
   std::vector< std::string > MeshBufferNames;
   glm::mat4 TransformationMatrix;
   SMesh* Mesh;

   SMeshLoad3DSFile()
      : m_Driver( nullptr )
      , Vertices(0)
      , Indices(0)
      , SmoothingGroups(0)
      , TCoords(0)
      , CountVertices(0)
      , CountFaces(0)
      , CountTCoords(0)
      , Mesh(0)
   {

   }

   ~SMeshLoad3DSFile()
   {
      cleanUp();
   }

   bool
   load( SMesh & out, Binary & file, VideoDriver* driver )
   {
      Mesh = &out;
      m_Driver = driver;

      ChunkData data;
      readChunkData(file, data);

      if (data.header.id != C3DS_MAIN3DS )
      {
         return false;
      }
      CurrentMaterial.clear();
      Materials.clear();
      MeshBufferNames.clear();
      cleanUp();

      bool ok = readChunk( file, &data );
      if ( ok )
      {
         Mesh->setName( file.getUri() );
         Mesh->setLighting( 1 );
         SMeshTool::removeEmptyMeshBuffer( *Mesh );
         SMeshTool::removeDoubleMaterials( *Mesh );
         Mesh->recalculateNormals();

         if ( m_Driver->getDebugLevel() > 0 )
         {
            DE_DEBUG( Mesh->toString() )

            auto texList = Mesh->getUniqueTextures();
            DE_DEBUG("TexCount = ", texList.size() )
            for ( size_t i = 0; i < texList.size(); ++i )
            {
               DE_DEBUG("Tex[",i,"] ", texList[ i ]->toString() )
            }

            //DE_DEBUG("BoundingBox = ", Mesh->getBoundingBox().toString() )
            int32_t nLimbs = Mesh->getMeshBufferCount();
            DE_DEBUG("LimbCount = ", nLimbs )
            for ( int32_t i = 0; i < nLimbs; ++i )
            {
               DE_DEBUG("Limb[",i,"] ", Mesh->getMeshBuffer(i).toString() )
            }
         }
      }



      return ok;
   }


   void
   readChunkData( de::Binary & file, ChunkData& data);

   bool
   readChunk(  de::Binary & file, ChunkData* parent);

   bool
   readMaterialChunk( de::Binary & file, ChunkData* parent);

   bool
   readFrameChunk( de::Binary & file, ChunkData* parent);

   bool
   readTrackChunk( de::Binary & file, ChunkData& data, SMeshBuffer* mb, glm::vec3 const & pivot);

   bool
   readObjectChunk( de::Binary & file, ChunkData* parent);

   bool readPercentageChunk( de::Binary & file, ChunkData* chunk, float& percentage);
   bool readColorChunk( de::Binary & file, ChunkData* chunk, SColor& out);


   void readString( de::Binary & file, ChunkData& data, std::string& out);
   void readVertices( de::Binary & file, ChunkData& data);
   void readIndices( de::Binary & file, ChunkData& data);
   void readMaterialGroup( de::Binary & file, ChunkData& data);
   void readTextureCoords( de::Binary & file, ChunkData& data);

   void composeObject( de::Binary & file,std::string const & name);
   void loadMaterials( de::Binary & file);
   void cleanUp();

};

void
SMeshLoad3DSFile::readChunkData( de::Binary & file, ChunkData& data )
{
   file.read( &data.header, sizeof(ChunkHeader) );
#ifdef __BIG_ENDIAN__
   data.header.id = os::Byteswap::byteswap(data.header.id);
   data.header.length = os::Byteswap::byteswap(data.header.length);
#endif
   data.read += sizeof(ChunkHeader);
}

bool
SMeshLoad3DSFile::readChunk( de::Binary & file, ChunkData* parent )
{
   while( parent->read < parent->header.length )
   {
      ChunkData data;
      readChunkData(file, data);

      switch(data.header.id)
      {
      case C3DS_VERSION:
         {
            uint16_t version;
            file.read( &version, sizeof(uint16_t) );
#ifdef __BIG_ENDIAN__
            version = os::Byteswap::byteswap(version);
#endif
            file.seek(data.header.length - data.read - 2, SEEK_CUR);
            data.read += data.header.length - data.read;
            if (version != 0x03)
            {
               DE_ERROR("3ds file version is other than 3.")
            }
         }
         break;
      case C3DS_EDIT_MATERIAL:
         readMaterialChunk(file, &data);
         break;
      case C3DS_KEYF3DS:
         readFrameChunk(file, &data);
         break;
      case C3DS_EDIT3DS:
         break;
      case C3DS_MESHVERSION:
      case 0x01:
         {
            uint32_t version;
            file.read(&version, sizeof(uint32_t));
#ifdef __BIG_ENDIAN__
            version = os::Byteswap::byteswap(version);
#endif
            data.read += sizeof(uint32_t);
         }
         break;
      case C3DS_EDIT_OBJECT:
         {
            std::string name;
            readString(file, data, name);
            readObjectChunk(file, &data);
            composeObject(file, name);
         }
         break;

      default:
         // ignore chunk
         file.seek(data.header.length - data.read, SEEK_CUR);
         data.read += data.header.length - data.read;
      }

      parent->read += data.read;
   }

   return true;
}

bool
SMeshLoad3DSFile::readPercentageChunk( de::Binary & file, ChunkData* chunk, float& percentage )
{
   ChunkData data;
   readChunkData(file, data);
   short intpercentage;
   float fpercentage;

   switch(data.header.id)
   {
   case C3DS_PERCENTAGE_I:
   {
      // read short
      file.read( &intpercentage, 2);
#ifdef __BIG_ENDIAN__
      intpercentage = os::Byteswap::byteswap(intpercentage);
#endif
      percentage = intpercentage/100.0f;
      data.read += 2;
   }
   break;
   case C3DS_PERCENTAGE_F:
   {
      // read float
      file.read( &fpercentage, sizeof(float) );
      data.read += sizeof(float);
#ifdef __BIG_ENDIAN__
      percentage = os::Byteswap::byteswap(fpercentage);
#else
      percentage = (float)fpercentage;
#endif
   }
   break;
   default:
   {
      // unknown percentage chunk
      DE_WARN("Unknown percentage chunk in 3Ds file.")
      file.seek( data.header.length - data.read, SEEK_CUR );
      data.read += data.header.length - data.read;
   }
   }

   chunk->read += data.read;

   return true;
}

bool
SMeshLoad3DSFile::readColorChunk( de::Binary & file, ChunkData* chunk, SColor & out)
{
   ChunkData data; readChunkData(file, data);
   uint8_t c[3];
   float cf[3];

   switch(data.header.id)
   {
      case C3DS_COL_TRU:
      case C3DS_COL_LIN_24:
      {
         // read 8 bit data
         file.read(c, sizeof(c));
         //out = RGBA( c[0], c[1], c[2] );
         out.r = c[0]/255.0f;
         out.g = c[1]/255.0f;
         out.b = c[2]/255.0f;
         out.a = 1.0f;
         data.read += sizeof(c);
      }
      break;
      case C3DS_COL_RGB:
      case C3DS_COL_LIN_F:
      {
         // read float data
         file.read(cf, sizeof(cf));
   #ifdef __BIG_ENDIAN__
         cf[0] = os::Byteswap::byteswap(cf[0]);
         cf[1] = os::Byteswap::byteswap(cf[1]);
         cf[2] = os::Byteswap::byteswap(cf[2]);
   #endif
//         out = RGBA( (int32_t)(cf[0]*255.0f),
//                     (int32_t)(cf[1]*255.0f),
//                     (int32_t)(cf[2]*255.0f) );
         out.r = cf[0];
         out.g = cf[1];
         out.b = cf[2];
         out.a = 1.0f;
         data.read += sizeof(cf);
      }
      break;
      default:
      {
         // unknown color chunk size
         DE_WARN("Unknown size of color chunk in 3Ds file.");
         file.seek(data.header.length - data.read, SEEK_CUR);
         data.read += data.header.length - data.read;
      }
   }

   chunk->read += data.read;
   return true;
}

bool
SMeshLoad3DSFile::readMaterialChunk( de::Binary & file, ChunkData* parent)
{
   uint16_t matSection=0;
   float tx = 0.0f;
   float ty = 0.0f;
   float sx = 1.0f;
   float sy = 1.0f;
   float tx2 = 0.0f;
   float ty2 = 0.0f;
   float sx2 = 1.0f;
   float sy2 = 1.0f;
   while( parent->read < parent->header.length )
   {
      ChunkData data;
      readChunkData(file, data);

      switch(data.header.id)
      {
      case C3DS_MATNAME:
         {
            char* c = new char[data.header.length - data.read];
            file.read(c, data.header.length - data.read);

            if (strlen(c))
               CurrentMaterial.Name = c;

            data.read += data.header.length - data.read;
            delete [] c;
         }
         break;
      case C3DS_MATAMBIENT:
         readColorChunk(file, &data, CurrentMaterial.Material.Ka);
         break;
      case C3DS_MATDIFFUSE:
         readColorChunk(file, &data, CurrentMaterial.Material.Kd);
         break;
      case C3DS_MATSPECULAR:
         readColorChunk(file, &data, CurrentMaterial.Material.Ks);
         break;
      case C3DS_MATSHININESS:
         readPercentageChunk(file, &data, CurrentMaterial.Material.Shininess);
         CurrentMaterial.Material.Shininess = (1.f-CurrentMaterial.Material.Shininess)*128.f;
         break;
      case C3DS_TRANSPARENCY:
         {
            float percentage;
            readPercentageChunk(file, &data, percentage);
            if (percentage>0.0f)
            {
               CurrentMaterial.Material.MaterialType = 1; // video::EMT_TRANSPARENT_VERTEX_ALPHA;
               //CurrentMaterial.Material.state.blend = Blend::alphaBlend();
               CurrentMaterial.Material.Opacity = std::clamp( 1.0f - percentage, 0.0f, 1.0f );
               //CurrentMaterial.Material.TransparentColor = percentage;
            }
            else
            {
               CurrentMaterial.Material.MaterialType = 0; // video::EMT_SOLID;
               //CurrentMaterial.Material.state.blend = Blend::disabled();
            }
         }
         break;
      case C3DS_WIRE:
         CurrentMaterial.Material.Wireframe = true;
         break;
      case C3DS_TWO_SIDE:
         CurrentMaterial.Material.state.culling = Culling::disabled();
         break;
      case C3DS_SHADING:
         {
            int16_t flags;
            file.read(&flags, 2);
#ifdef __BIG_ENDIAN__
            flags = os::Byteswap::byteswap(flags);
#endif
            switch (flags)
            {
               case 0:
                  CurrentMaterial.Material.Wireframe = true;
                  break;
               case 1:
                  CurrentMaterial.Material.Wireframe = false;
                  CurrentMaterial.Material.GouraudShading = false;
                  break;
               case 2:
                  CurrentMaterial.Material.Wireframe = false;
                  CurrentMaterial.Material.GouraudShading = true;
                  break;
               default:
                  // phong and metal missing
                  break;
            }
            data.read += data.header.length - data.read;
         }
         break;
      case C3DS_MATTEXMAP:
      case C3DS_MATSPECMAP:
      case C3DS_MATOPACMAP:
      case C3DS_MATREFLMAP:
      case C3DS_MATBUMPMAP:
         {
            matSection=data.header.id;
            // Should contain a percentage chunk, but does
            // not always have it
            int16_t testval;
            const long pos = file.tell();
            file.read(&testval, 2);
#ifdef __BIG_ENDIAN__
            testval = os::Byteswap::byteswap(testval);
#endif
            file.seek( pos );
            if ((testval == C3DS_PERCENTAGE_I) || (testval == C3DS_PERCENTAGE_F))
            switch (matSection)
            {
            case C3DS_MATTEXMAP:
               readPercentageChunk(file, &data, CurrentMaterial.Strength[0]);
               //DE_DEBUG("C3DS_MATTEXMAP Strength[0] = ",CurrentMaterial.Strength[0])
            break;
            case C3DS_MATSPECMAP:
               readPercentageChunk(file, &data, CurrentMaterial.Strength[1]);
               //DE_DEBUG("C3DS_MATSPECMAP Strength[1] = ",CurrentMaterial.Strength[1])
            break;
            case C3DS_MATOPACMAP:
               readPercentageChunk(file, &data, CurrentMaterial.Strength[2]);
               //DE_DEBUG("C3DS_MATOPACMAP Strength[2] = ",CurrentMaterial.Strength[2])
            break;
            case C3DS_MATBUMPMAP:
               readPercentageChunk(file, &data, CurrentMaterial.Strength[4]);
               //DE_DEBUG("C3DS_MATBUMPMAP Strength[4] = ",CurrentMaterial.Strength[4])
            break;
            }
         }
         break;
      case C3DS_MATMAPFILE:
         {
            // read texture file name
            char* c = new char[data.header.length - data.read];
            file.read(c, data.header.length - data.read);
            switch (matSection)
            {
            case C3DS_MATTEXMAP:
               CurrentMaterial.Filename[0] = c;
               //DE_DEBUG("C3DS_MATTEXMAP Filename[0] = ",c)
            break;
            case C3DS_MATSPECMAP:
               CurrentMaterial.Filename[1] = c;
               //DE_DEBUG("C3DS_MATSPECMAP Filename[1] = ",c)
            break;
            case C3DS_MATOPACMAP:
               CurrentMaterial.Filename[2] = c;
               //DE_DEBUG("C3DS_MATOPACMAP Filename[2] = ",c)
            break;
            case C3DS_MATREFLMAP:
               CurrentMaterial.Filename[3] = c;
               //DE_DEBUG("C3DS_MATREFLMAP Filename[3] = ",c)
            break;
            case C3DS_MATBUMPMAP:
               CurrentMaterial.Filename[4] = c;
               //DE_DEBUG("C3DS_MATBUMPMAP Filename[4] = ",c)
            break;
            }

            data.read += data.header.length - data.read;
            delete [] c;
         }
         break;
      case C3DS_MAT_TEXTILING:
         {
            int16_t flags;
            file.read(&flags, 2);
#ifdef __BIG_ENDIAN__
            flags = os::Byteswap::byteswap(flags);
#endif
            data.read += 2;
         }
         break;
      case C3DS_MAT_USCALE:
      case C3DS_MAT_VSCALE:
      case C3DS_MAT_UOFFSET:
      case C3DS_MAT_VOFFSET:
         {
            float value;
            file.read(&value, 4);
#ifdef __BIG_ENDIAN__
            value = os::Byteswap::byteswap(value);
#endif
            if (matSection != C3DS_MATTEXMAP)
            {
                    if (data.header.id == C3DS_MAT_USCALE) { sx2 = value; }
               else if (data.header.id == C3DS_MAT_VSCALE) { sy2 = value; }
               else if (data.header.id == C3DS_MAT_UOFFSET){ tx2 = value; }
               else if (data.header.id == C3DS_MAT_VOFFSET){ ty2 = value; }
               else {}
            }
            else
            {
                    if (data.header.id == C3DS_MAT_USCALE) { sx = value; }
               else if (data.header.id == C3DS_MAT_VSCALE) { sy = value; }
               else if (data.header.id == C3DS_MAT_UOFFSET){ tx = value; }
               else if (data.header.id == C3DS_MAT_VOFFSET){ ty = value; }
               else {}
            }

            data.read += 4;
         }
         break;
      default:
         // ignore chunk
         file.seek(data.header.length - data.read, SEEK_CUR);
         data.read += data.header.length - data.read;
      }

      parent->read += data.read;
   }

   CurrentMaterial.Material.getTexture( 0 ).setMatrix( tx, ty, sx, sy );
   CurrentMaterial.Material.getTexture( 1 ).setMatrix( tx2, ty2, sx2, sy2 );
   Materials.push_back(CurrentMaterial);
   CurrentMaterial.clear();

   return true;
}



bool
SMeshLoad3DSFile::readTrackChunk( de::Binary & file, ChunkData& data,
               SMeshBuffer* mb, glm::vec3 const & pivot )
{
   uint16_t flags;
   uint32_t flags2;
   // Track flags
   file.read(&flags, 2);
#ifdef __BIG_ENDIAN__
   flags = os::Byteswap::byteswap(flags);
#endif
   file.read(&flags2, 4);
#ifdef __BIG_ENDIAN__
   flags2 = os::Byteswap::byteswap(flags2);
#endif
   file.read(&flags2, 4);
#ifdef __BIG_ENDIAN__
   flags2 = os::Byteswap::byteswap(flags2);
#endif
   // Num keys
   file.read(&flags2, 4);
#ifdef __BIG_ENDIAN__
   flags2 = os::Byteswap::byteswap(flags2);
#endif
   file.read(&flags2, 4);
#ifdef __BIG_ENDIAN__
   flags2 = os::Byteswap::byteswap(flags2);
#endif
   // TCB flags
   file.read(&flags, 2);
#ifdef __BIG_ENDIAN__
   flags = os::Byteswap::byteswap(flags);
#endif
   data.read += 20;

   float angle=0.0f;
   if (data.header.id== C3DS_ROT_TRACK_TAG)
   {
      // Angle
      file.read(&angle, sizeof(float));
#ifdef __BIG_ENDIAN__
      angle = os::Byteswap::byteswap(angle);
#endif
      data.read += sizeof(float);
   }
   glm::vec3 vec;
   file.read(&vec.x, sizeof(float));
   file.read(&vec.y, sizeof(float));
   file.read(&vec.z, sizeof(float));
#ifdef __BIG_ENDIAN__
   vec.x = os::Byteswap::byteswap(vec.x);
   vec.y = os::Byteswap::byteswap(vec.y);
   vec.z = os::Byteswap::byteswap(vec.z);
#endif
   data.read += 12;
   vec-=pivot;

   // apply transformation to mesh buffer
   if (false)//mb)
   {
      S3DVertex *vertices=(S3DVertex*)mb->getVertices();
      if (data.header.id==C3DS_POS_TRACK_TAG)
      {
         for (uint32_t i=0; i < mb->getVertexCount(); ++i )
         {
            vertices[i].pos += vec;
         }
      }
      else if (data.header.id==C3DS_ROT_TRACK_TAG)
      {
         //TODO
      }
      else if (data.header.id==C3DS_SCL_TRACK_TAG)
      {
         //TODO
      }
   }
   // skip further frames
   file.seek(data.header.length - data.read, SEEK_CUR);
   data.read += data.header.length - data.read;
   return true;
}


bool SMeshLoad3DSFile::readFrameChunk( de::Binary & file, ChunkData* parent)
{
   ChunkData data;
   readChunkData(file, data);   //KF_HDR is always at the beginning
   if ( data.header.id != C3DS_KF_HDR )
   {
      return false;
   }
#ifdef _IRR_DEBUG_3DS_LOADER_
   os::Printer::log("Load keyframe header.", ELL_DEBUG);
#endif
   uint16_t version;
   file.read(&version, 2);
#ifdef __BIG_ENDIAN__
   version = os::Byteswap::byteswap(version);
#endif
   std::string name;
   readString(file, data, name);
   uint32_t flags;
   file.read(&flags, 4);
#ifdef __BIG_ENDIAN__
   flags = os::Byteswap::byteswap(flags);
#endif

   data.read += 4;
   parent->read += data.read;
   data.read=0;

   SMeshBuffer* mb=0;
   glm::vec3 pivot,bboxCenter;
   while(parent->read < parent->header.length)
   {
      readChunkData(file, data);

      switch(data.header.id)
      {
      case C3DS_OBJECT_TAG:
         {
#ifdef _IRR_DEBUG_3DS_LOADER_
   os::Printer::log("Load object tag.", ELL_DEBUG);
#endif
            mb=0;
            pivot = glm::vec3(0.0f, 0.0f, 0.0f);
         }
         break;
      case C3DS_KF_SEG:
         {
#ifdef _IRR_DEBUG_3DS_LOADER_
   os::Printer::log("Load keyframe segment.", ELL_DEBUG);
#endif
            uint32_t flags;
            file.read(&flags, 4);
#ifdef __BIG_ENDIAN__
            flags = os::Byteswap::byteswap(flags);
#endif
            file.read(&flags, 4);
#ifdef __BIG_ENDIAN__
            flags = os::Byteswap::byteswap(flags);
#endif
            data.read += 8;
         }
         break;
      case C3DS_KF_NODE_HDR:
         {
#ifdef _IRR_DEBUG_3DS_LOADER_
   os::Printer::log("Load keyframe node header.", ELL_DEBUG);
#endif
            int16_t flags;
            char* c = new char[data.header.length - data.read-6];
            file.read(c, data.header.length - data.read-6);

            // search mesh buffer to apply these transformations to
            for (uint32_t i=0; i<MeshBufferNames.size(); ++i)
            {
               if (MeshBufferNames[i]==c)
               {
                  mb = &Mesh->getMeshBuffer(i);
                  break;
               }
            }

            file.read(&flags, 2);
#ifdef __BIG_ENDIAN__
            flags = os::Byteswap::byteswap(flags);
#endif
            file.read(&flags, 2);
#ifdef __BIG_ENDIAN__
            flags = os::Byteswap::byteswap(flags);
#endif
            file.read(&flags, 2);
#ifdef __BIG_ENDIAN__
            flags = os::Byteswap::byteswap(flags);
#endif
            data.read += data.header.length - data.read;
            delete [] c;
         }
         break;
      case C3DS_KF_CURTIME:
         {
#ifdef _IRR_DEBUG_3DS_LOADER_
   os::Printer::log("Load keyframe current time.", ELL_DEBUG);
#endif
            uint32_t flags;
            file.read(&flags, 4);
#ifdef __BIG_ENDIAN__
            flags = os::Byteswap::byteswap(flags);
#endif
            data.read += 4;
         }
         break;
      case C3DS_NODE_ID:
         {
#ifdef _IRR_DEBUG_3DS_LOADER_
   os::Printer::log("Load node ID.", ELL_DEBUG);
#endif
            uint16_t flags;
            file.read(&flags, 2);
#ifdef __BIG_ENDIAN__
            flags = os::Byteswap::byteswap(flags);
#endif
            data.read += 2;
         }
         break;
      case C3DS_PIVOTPOINT:
         {
#ifdef _IRR_DEBUG_3DS_LOADER_
   os::Printer::log("Load pivot point.", ELL_DEBUG);
#endif
            file.read(&pivot.x, sizeof(float));
            file.read(&pivot.y, sizeof(float));
            file.read(&pivot.z, sizeof(float));
#ifdef __BIG_ENDIAN__
            pivot.x = os::Byteswap::byteswap(pivot.x);
            pivot.y = os::Byteswap::byteswap(pivot.y);
            pivot.z = os::Byteswap::byteswap(pivot.z);
#endif
            data.read += 12;
         }
         break;
      case C3DS_BOUNDBOX:
         {
#ifdef _IRR_DEBUG_3DS_LOADER_
   os::Printer::log("Load bounding box.", ELL_DEBUG);
#endif
            Box3f bbox;
            // abuse bboxCenter as temporary variable
            file.read(&bboxCenter.x, sizeof(float));
            file.read(&bboxCenter.y, sizeof(float));
            file.read(&bboxCenter.z, sizeof(float));
#ifdef __BIG_ENDIAN__
            bboxCenter.x = os::Byteswap::byteswap(bboxCenter.x);
            bboxCenter.y = os::Byteswap::byteswap(bboxCenter.y);
            bboxCenter.z = os::Byteswap::byteswap(bboxCenter.z);
#endif
            bbox.reset(bboxCenter);
            file.read(&bboxCenter.x, sizeof(float));
            file.read(&bboxCenter.y, sizeof(float));
            file.read(&bboxCenter.z, sizeof(float));
#ifdef __BIG_ENDIAN__
            bboxCenter.x = os::Byteswap::byteswap(bboxCenter.x);
            bboxCenter.y = os::Byteswap::byteswap(bboxCenter.y);
            bboxCenter.z = os::Byteswap::byteswap(bboxCenter.z);
#endif
            bbox.addInternalPoint(bboxCenter);
            bboxCenter = bbox.getCenter();
            data.read += 24;
         }
         break;
      case C3DS_MORPH_SMOOTH:
         {
#ifdef _IRR_DEBUG_3DS_LOADER_
   os::Printer::log("Load morph smooth.", ELL_DEBUG);
#endif
            float flag;
            file.read(&flag, 4);
#ifdef __BIG_ENDIAN__
            flag = os::Byteswap::byteswap(flag);
#endif
            data.read += 4;
         }
         break;
      case C3DS_POS_TRACK_TAG:
      case C3DS_ROT_TRACK_TAG:
      case C3DS_SCL_TRACK_TAG:
         readTrackChunk(file, data, mb, bboxCenter-pivot);
         break;
      default:
         // ignore chunk
         file.seek(data.header.length - data.read, SEEK_CUR);
         data.read += data.header.length - data.read;
      }

      parent->read += data.read;
      data.read=0;
   }

   return true;
}

bool
SMeshLoad3DSFile::readObjectChunk( de::Binary & file, ChunkData* parent)
{
   while(parent->read < parent->header.length)
   {
      ChunkData data;
      readChunkData(file, data);

      switch(data.header.id)
      {
      case C3DS_OBJTRIMESH:
         readObjectChunk(file, &data);
         break;

      case C3DS_TRIVERT:
         readVertices(file, data);
         break;

      case C3DS_POINTFLAGARRAY:
         {
            uint16_t numVertex, flags;
            file.read(&numVertex, sizeof(uint16_t));
#ifdef __BIG_ENDIAN__
            numVertex= os::Byteswap::byteswap(numVertex);
#endif
            for (uint16_t i=0; i<numVertex; ++i)
            {
               file.read(&flags, sizeof(uint16_t));
#ifdef __BIG_ENDIAN__
               flags = os::Byteswap::byteswap(flags);
#endif
            }
            data.read += (numVertex+1)*sizeof(uint16_t);
         }
         break;

      case C3DS_TRIFACE:
         readIndices(file, data);
         readObjectChunk(file, &data); // read smooth and material groups
         break;

      case C3DS_TRIFACEMAT:
         readMaterialGroup(file, data);
         break;

      case C3DS_TRIUV: // getting texture coordinates
         readTextureCoords(file, data);
         break;

      case C3DS_TRIMATRIX:
         {
            //DE_WARN( "Poss crashing here")
            float mat[4][3];
            file.read( &mat, 12*sizeof(float) );
            TransformationMatrix = glm::mat4( 1.0f );
            for (int i=0; i<4; ++i)
            {
               for (int j=0; j<3; ++j)
               {
#ifdef __BIG_ENDIAN__
                  TransformationMatrix(i,j)=os::Byteswap::byteswap(mat[i][j]);
#else
                  TransformationMatrix[i][j] = mat[i][j];
#endif
               }
            }
            data.read += 12*sizeof(float);
         }
         break;
      case C3DS_MESHCOLOR:
         {
            uint8_t flag;
            file.read( &flag, sizeof(uint8_t) );
            ++data.read;
         }
         break;
      case C3DS_TRISMOOTH: // TODO
         {
            SmoothingGroups = new uint32_t[CountFaces];
            file.read(SmoothingGroups, CountFaces*sizeof(uint32_t));
#ifdef __BIG_ENDIAN__
            for (uint16_t i=0; i<CountFaces; ++i)
               SmoothingGroups[i] = os::Byteswap::byteswap(SmoothingGroups[i]);
#endif
            data.read += CountFaces*sizeof(uint32_t);
         }
         break;

      default:
         // ignore chunk
         file.seek(data.header.length - data.read, SEEK_CUR);
         data.read += data.header.length - data.read;
      }

      parent->read += data.read;
   }

   return true;
}


void SMeshLoad3DSFile::composeObject( de::Binary & file, std::string const & name)
{
   loadMaterials(file);

   if (Mesh->getMeshBufferCount() != int32_t(Materials.size()))
   {
   }


   if (MaterialGroups.empty())
   {
      // no material group, so add all
      SMaterialGroup group;
      group.faceCount = CountFaces;
      group.faces = new uint16_t[group.faceCount];
      for (uint16_t i=0; i<group.faceCount; ++i)
         group.faces[i] = i;
      MaterialGroups.push_back(group);

      // if we've got no material, add one without a texture
      if (Materials.empty())
      {
         SCurrentMaterial m;
         Materials.push_back(m);
         Mesh->addMeshBuffer( SMeshBuffer() );
         SMeshBuffer* mb = &Mesh->getMeshBuffer( Mesh->getMeshBufferCount() - 1 );
         mb->getMaterial() = Materials[0].Material;
         // add an empty mesh buffer name
         MeshBufferNames.push_back("");
      }
   }

   for (uint32_t i=0; i<MaterialGroups.size(); ++i)
   {
      SMeshBuffer* mb = 0;
      Material* mat=0;
      uint32_t mbPos;

      // find mesh buffer for this group
      for (mbPos=0; mbPos<Materials.size(); ++mbPos)
      {
         if (MaterialGroups[i].MaterialName == Materials[mbPos].Name)
         {
            mb = &Mesh->getMeshBuffer(mbPos);
            mat = &Materials[mbPos].Material;
            MeshBufferNames[mbPos]=name;
            break;
         }
      }

      if (mb != 0)
      {
         // add geometry to the buffer.

         S3DVertex vtx;
         //glm::vec3 vec;
         vtx.color = RGBA( mat->Kd );
         if (mat->MaterialType == 1) // video::EMT_TRANSPARENT_VERTEX_ALPHA
         {
            auto r = RGBA_R( vtx.color );
            auto g = RGBA_G( vtx.color );
            auto b = RGBA_B( vtx.color );
            auto a = std::clamp( (int)(255.0f*mat->Opacity), 0, 255 );
            vtx.color = RGBA( r,g,b,a );
         }
         vtx.normal = glm::vec3(0,0,0);

         for (int32_t f=0; f<MaterialGroups[i].faceCount; ++f)
         {
            uint32_t vtxCount = mb->Vertices.size();

            for (int32_t v=0; v<3; ++v)
            {
               int32_t idx = Indices[MaterialGroups[i].faces[f]*4 +v];

               if (CountVertices > idx)
               {
                  vtx.pos.x = Vertices[idx*3 + 0];
                  vtx.pos.z = Vertices[idx*3 + 1];
                  vtx.pos.y = Vertices[idx*3 + 2];
//						TransformationMatrix.transformVect(vtx.Pos);
               }

               if (CountTCoords > idx)
               {
                  vtx.tex.x = TCoords[idx*2 + 0];
                  vtx.tex.y = 1.0f -TCoords[idx*2 + 1];
               }

               mb->Vertices.push_back( vtx );
            }

            // compute normal
            auto const & A = mb->Vertices[vtxCount].pos;
            auto const & B = mb->Vertices[vtxCount+2].pos;
            auto const & C = mb->Vertices[vtxCount+1].pos;
            auto n = Math::getNormal3D( A,B,C);
            mb->Vertices[vtxCount].normal = n;
            mb->Vertices[vtxCount+1].normal = n;
            mb->Vertices[vtxCount+2].normal = n;

            // add indices
            mb->Indices.push_back(vtxCount);
            mb->Indices.push_back(vtxCount+2);
            mb->Indices.push_back(vtxCount+1);
         }
      }
      else
      {
         // DE_WARN("Found no matching material for Group in 3ds file.")
      }
   }

   cleanUp();
}


void SMeshLoad3DSFile::loadMaterials( de::Binary & file )
{
   // create a mesh buffer for every material
   std::string modelUri = file.getUri();

   if (Materials.empty())
   {
      DE_DEBUG("No materials found in 3ds file.")
   }

   MeshBufferNames.reserve( Materials.size() );

   for (uint32_t i=0; i<Materials.size(); ++i)
   {
      MeshBufferNames.push_back("");
      Mesh->addMeshBuffer( SMeshBuffer() );
      SMeshBuffer* m = &Mesh->getMeshBuffer( Mesh->getMeshBufferCount() - 1 );

      auto & matData = Materials[ i ];
      m->getMaterial() = matData.Material;

      if ( matData.Filename[0].size() )
      {
         //DE_DEBUG("Found tex0 ", matData.Filename[0] )
         std::string uri = dbGetFileDir(modelUri) + "/" + matData.Filename[0];
         //DE_DEBUG("TexUri = ", uri )
         if ( !dbExistFile(uri) )
         {
            dbStrLowerCase( uri );
            //DE_DEBUG("TexUri = ", uri )
            if ( !dbExistFile(uri) )
            {
               uri = "";
               //DE_DEBUG("TexUri = No chance" )
            }
         }

         if ( uri.size() )
         {
            TexRef ref = m_Driver->getTexture( uri );
            if (!ref.m_tex)
            {
               //DE_WARN("Could not load material tex0 in 3ds file ", uri )
            }
            else
            {
               m->getMaterial().setTexture(0, ref );
            }
         }
      }

      if (matData.Filename[1].size())
      {
         //DE_DEBUG("Found tex1 ", matData.Filename[1] )
         std::string uri = dbGetFileDir(modelUri) + "/" + matData.Filename[1];
         //DE_DEBUG("Tex1Uri = ", uri )
         if ( !dbExistFile(uri) )
         {
            dbStrLowerCase( uri );
            //DE_DEBUG("Tex1Uri = ", uri )
            if ( !dbExistFile(uri) )
            {
               uri = "";
               //DE_DEBUG("Tex1Uri = No chance" )
            }
         }

         if ( uri.size() )
         {
            TexRef ref = m_Driver->getTexture( uri );
            if (!ref.m_tex)
            {
               //DE_WARN("Could not load material tex1 in 3ds file ", uri )
            }
            else
            {
               m->getMaterial().setTexture(1, ref );
            }
         }
      }

      if (matData.Filename[2].size())
      {
         //DE_DEBUG("Found tex2 ", matData.Filename[2] )
         std::string uri = dbGetFileDir(modelUri) + "/" + matData.Filename[2];
         //DE_DEBUG("Tex2Uri = ", uri )

         if ( !dbExistFile(uri) )
         {
            dbStrLowerCase( uri );
            //DE_DEBUG("Tex2Uri = ", uri )
            if ( !dbExistFile(uri) )
            {
               uri = "";
               //DE_DEBUG("Tex2Uri = No chance" )
            }
         }

         if ( uri.size() )
         {
            TexRef ref = m_Driver->getTexture( uri );
            if (!ref.m_tex)
            {
               //DE_WARN("Could not load material tex2 in 3ds file ", uri )
            }
            else
            {
               //m->getMaterial().setTexture(1, tex );
               m->getMaterial().setTexture(0, ref);
               m->getMaterial().MaterialType = 2; // video::EMT_TRANSPARENT_ADD_COLOR;

            }
         }
      }

      if (matData.Filename[3].size())
      {
         //DE_DEBUG("Found tex3 ", matData.Filename[3] )
         std::string uri = dbGetFileDir(modelUri) + "/" + matData.Filename[3];
         //DE_DEBUG("Tex3Uri = ", uri )

         if ( !dbExistFile(uri) )
         {
            dbStrLowerCase( uri );
            //DE_DEBUG("Tex3Uri = ", uri )
            if ( !dbExistFile(uri) )
            {
               uri = "";
               //DE_DEBUG("Tex3Uri = No chance" )
            }
         }

         if ( uri.size() )
         {
            TexRef ref = m_Driver->getTexture( uri );
            if (!ref.m_tex)
            {
               //DE_WARN("Could not load material tex3 in 3ds file ", uri )
            }
            else
            {
               //DE_WARN("Dont use tex2 ", uri )
               //m->getMaterial().setTexture(1, m->getMaterial().getTexture(0));
               m->getMaterial().setTexture(0, ref);
               //m->getMaterial().MaterialType = 3; // video::EMT_REFLECTION_2_LAYER;
            }
         }
      }

      if (matData.Filename[4].size())
      {
         //DE_DEBUG("Found tex4 ", matData.Filename[4] )
         std::string uri = dbGetFileDir(modelUri) + "/" + matData.Filename[4];
         //DE_DEBUG("Tex4Uri = ", uri )

         if ( !dbExistFile(uri) )
         {
            dbStrLowerCase( uri );
            //DE_DEBUG("Tex4Uri = ", uri )
            if ( !dbExistFile(uri) )
            {
               uri = "";
               //DE_DEBUG("Tex4Uri = No chance" )
            }
         }

         if ( uri.size() )
         {
            TexRef ref = m_Driver->getTexture( uri );
            if (!ref.m_tex)
            {
               //DE_WARN("Could not load material EMT_PARALLAX_MAP_SOLID in 3ds file ", uri )
            }
            else
            {
               //DE_WARN("EMT_PARALLAX_MAP_SOLID ", uri )
               m->getMaterial().setTexture( 1, ref );
               // m_Driver->makeNormalMapTexture(texture, Materials[i].Strength[4]*10.f);
               m->getMaterial().MaterialType = 2; // video::EMT_PARALLAX_MAP_SOLID;
               //m->getMaterial().MaterialTypeParam =.035f;
            }
         }
      }
   }
}


void SMeshLoad3DSFile::cleanUp()
{
   delete [] Vertices; Vertices = 0;
   delete [] Indices; Indices = 0;
   delete [] SmoothingGroups; SmoothingGroups = 0;
   delete [] TCoords; TCoords = 0;
   CountVertices = 0;
   CountFaces = 0;
   CountTCoords = 0;
   MaterialGroups.clear();
}

void SMeshLoad3DSFile::readTextureCoords( de::Binary & file, ChunkData& data)
{
   file.read(&CountTCoords, sizeof(CountTCoords));
#ifdef __BIG_ENDIAN__
   CountTCoords = os::Byteswap::byteswap(CountTCoords);
#endif
   data.read += sizeof(CountTCoords);

   int32_t tcoordsBufferByteSize = CountTCoords * sizeof(float) * 2;

   if (data.header.length - data.read != tcoordsBufferByteSize)
   {
      DE_WARN("Invalid size of tcoords found in 3ds file.")
      return;
   }

   TCoords = new float[CountTCoords * 3];
   file.read(TCoords, tcoordsBufferByteSize);
#ifdef __BIG_ENDIAN__
   for (int i=0;i<CountTCoords*2;i++) TCoords[i] = os::Byteswap::byteswap(TCoords[i]);
#endif
   data.read += tcoordsBufferByteSize;
}


void SMeshLoad3DSFile::readMaterialGroup( de::Binary & file, ChunkData& data)
{
   SMaterialGroup group;
   readString(file, data, group.MaterialName);

   file.read(&group.faceCount, sizeof(group.faceCount));
#ifdef __BIG_ENDIAN__
   group.faceCount = os::Byteswap::byteswap(group.faceCount);
#endif
   data.read += sizeof(group.faceCount);

   // read faces
   group.faces = new uint16_t[group.faceCount];
   file.read(group.faces, sizeof(uint16_t) * group.faceCount);
#ifdef __BIG_ENDIAN__
   for (uint32_t i=0;i<group.faceCount;++i)
      group.faces[i] = os::Byteswap::byteswap(group.faces[i]);
#endif
   data.read += sizeof(uint16_t) * group.faceCount;

   MaterialGroups.push_back(group);
}


void SMeshLoad3DSFile::readIndices( de::Binary & file, ChunkData & data )
{
   file.read(&CountFaces, sizeof(CountFaces));
#ifdef __BIG_ENDIAN__
   CountFaces = os::Byteswap::byteswap(CountFaces);
#endif
   data.read += sizeof(CountFaces);

   int32_t indexBufferByteSize = CountFaces * sizeof(uint16_t) * 4;

   // Indices are uint16_ts.
   // After every 3 Indices in the array, there follows an edge flag.
   Indices = new uint16_t[CountFaces * 4];
   file.read(Indices, indexBufferByteSize);
#ifdef __BIG_ENDIAN__
   for (int i=0;i<CountFaces*4;++i)
      Indices[i] = os::Byteswap::byteswap(Indices[i]);
#endif
   data.read += indexBufferByteSize;
}


void SMeshLoad3DSFile::readVertices( de::Binary & file, ChunkData& data)
{
   file.read(&CountVertices, sizeof(CountVertices));
#ifdef __BIG_ENDIAN__
   CountVertices = os::Byteswap::byteswap(CountVertices);
#endif
   data.read += sizeof(CountVertices);

   int32_t const vertexBufferByteSize = CountVertices * sizeof(float) * 3;

   if (data.header.length - data.read != vertexBufferByteSize)
   {
      DE_ERROR("Invalid size of vertices found in 3ds file", int(CountVertices) )
      return;
   }

   Vertices = new float[CountVertices * 3];
   file.read(Vertices, vertexBufferByteSize);
#ifdef __BIG_ENDIAN__
   for (int i=0;i<CountVertices*3;i++)
      Vertices[i] = os::Byteswap::byteswap(Vertices[i]);
#endif
   data.read += vertexBufferByteSize;
}




void SMeshLoad3DSFile::readString( de::Binary & file, ChunkData& data, std::string& out)
{
   char c = 1;
   out = "";

   while (c)
   {
      file.read(&c, sizeof(char));
      if (c)
         out += c;
   }
   data.read+=out.size()+1;
}

} // end namespace

bool
SMeshLoad3DS::load( SMesh & mesh, de::Binary & file, VideoDriver* driver )
{
   SMeshLoad3DSFile f3ds;
   return f3ds.load( mesh, file, driver );
}


} // end namespace scene
} // end namespace irr
