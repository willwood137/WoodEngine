#pragma once

#include "../Math/Vector3.hpp"
#include "../Math/Vector2.hpp"
#include "../Math/Matrix4.hpp"

#include "../Engine/Texture.hpp"
#include "../Engine/Shader.hpp"

#include <memory>
#include <vector>

namespace woodman
{
	struct MaxVertex
	{
		Vector3f vPosition;
		float uv[2];
		Vector3f normal;
		Vector3f tangent;
		Vector3f bitangent;
		float weights[4];
		float bones[4];

		bool operator<( const MaxVertex& Rhs ) const 
		{
			return memcmp(this,&Rhs,sizeof(MaxVertex)) < 0;
		}
	};

	enum eLumps
	{
		kMaterials = 0,
		kBatches,
		kNodes,
		kAnimationData,
		kCameraData,
		kMaxLumps          // A constant to store the number of lumps
	};

	struct MaxLump
	{
		int Offset;
		int Length;
	};

	struct MaxHeader
	{
		char Magic[4];
		float version;
		MaxLump Lumps[kMaxLumps];
	};

	struct MaxTriangleBatch
	{
		std::shared_ptr<Material> m_material;

		unsigned int numIndices;
		unsigned int numVertices;
		std::vector<Vertex> m_verts;
		std::vector<unsigned int> m_indicies;
		unsigned int idVBO;
		unsigned int idIBO;

		~TriangleBatch();
		void Unload();

	};


	struct BatchData
	{
		unsigned int numVerts;
		unsigned int numIndicies;
	};

	struct Header
	{
		char Magic[4];
		float version;
		Lump Lumps[kMaxLumps];
	};

	struct Material
	{
		std::string m_name;
		std::shared_ptr<Texture> m_diffuseTexture;
		std::shared_ptr<Texture> m_normalMap;
		std::shared_ptr<Texture> m_emissiveMap;
		std::shared_ptr<Texture> m_specularMap;

		~Material();
	};

	struct TriangleBatch
	{
		
	};

	struct MaxModelNodeInfo
	{
		int parent;
		std::vector<unsigned int> childrenIDs;
	};


	class MaxModel
	{
	public:

		class MaxModelNode
		{
		public:
			void AddChildNode(std::shared_ptr<MaxModelNode> node);
			void Render( std::shared_ptr<Shader> renderShader, int curFrame );
			Matrix4f getWorldTM( int frame );
			void Unload();

			std::string m_name;
			Matrix4f m_inverseRestMatrix;
			MaxModelNode* m_parent;
			std::vector< std::shared_ptr<TriangleBatch> > m_batches;
			std::vector< std::shared_ptr<MaxModelNode> > m_children;
			std::vector<Matrix4f> m_localTMs;
		};

		void ReadDataFromMaxFile( const char* filepath );
		void Render( Matrix4& TransformIn, int CameraNumber, std::shared_ptr<Material> materialOveride );
		void Init( shaderInfo Shader );
		void Unload();
		void Update( double delta);
		int getNumCameras() {return m_cameras.size();}

	private:

		std::vector<std::shared_ptr<ModelNode> > m_topNodes;
		std::vector<std::shared_ptr<ModelNode> > m_bones;
		std::vector<std::shared_ptr<ModelNode> > m_cameras;

		AnimationData m_AnimData;
		int m_curFrame;
		int m_totalFrames;
		double m_curtime;

		shaderInfo m_shaderInfoSkinned;

		std::vector<float> m_boneMatriciesToShader;
		std::shared_ptr<Texture> m_defaultTexture;
	};

	struct Vertex
	{
		std::vector<float> data;
	};

	struct VertexAttribute
	{
		std::string Name;
		int NumValues;

		bool operator<( const VertexAttribute& other ) const
		{
			return (Name.compare(other.Name) < 0);
		}
		bool operator==( const VertexAttribute& other ) const
		{
			return (Name.compare(other.Name) == 0);
		}
	};

}