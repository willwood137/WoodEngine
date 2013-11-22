#pragma once

#include "..\Math\Vector3.hpp"
#include "..\Math\Vector4.hpp"
#include "..\Math\Matrix4.hpp"
#include <vector>
#include <array>
#include <string>
#include "Texture.hpp"
#include "Camera.hpp"


namespace woodman
{
	struct BatchData
	{
		unsigned int idVBO;
		unsigned int idIBO;
		unsigned int numVerts;
		unsigned int numIndices;
	};
	
	struct VertexAttribute
	{
		unsigned int numValues;
		unsigned int offsetOf;
		std::string Name;
	};

	struct Material
	{
		std::string name;
		std::shared_ptr<Texture> Texture;
	};

	class Model
	{
	public:
		
		Model(const std::string& _ModelHeaderPath );

		bool loadData();
		VertexAttribute* getVertexAttribute(const std::string& AttributeName);
		
		std::shared_ptr<Texture> getMaterialTexture( const std::string& MaterialName );

		unsigned int getVertexSize()
		{
			return m_vertexSize;
		}

		std::vector<BatchData>* getBatchData()
		{
			return &m_batchDatas;
		}

		float m_scale;
	private:
		std::string m_modelHeaderPath;
		std::string m_modelDataPath;
		std::string m_name;

		unsigned int m_vertexSize;
		std::vector<BatchData> m_batchDatas;
		std::vector<VertexAttribute> m_attributes;
		std::set<std::shared_ptr<Material>> m_materials; 




	};
}