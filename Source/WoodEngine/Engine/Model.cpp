#include "..\stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "..\libs\pugixml.hpp"
#include "Model.hpp"
#include "..\Utility\Utility.hpp"


namespace woodman
{
	Model::Model(const std::string& _ModelHeaderPath )
		: m_modelHeaderPath(_ModelHeaderPath),
		m_vertexSize(0)
	{
		m_modelDataPath = m_modelHeaderPath.substr(0, m_modelHeaderPath.size() - 4);
		m_modelDataPath.append(".bmf");
	}

	bool Model::loadData()
	{

		//get the Binary Data

		std::ifstream in(m_modelDataPath, std::ios::binary);
		if (in.is_open())
		{
			//get header data
			pugi::xml_document* doc = new pugi::xml_document();
			pugi::xml_parse_result result = doc->load_file( m_modelHeaderPath.c_str() );
			
			pugi::xml_node ModelNode = doc->first_child().first_child();
			m_name = ModelNode.attribute("name").as_string();
			pugi::xml_node ModelChild = ModelNode.first_child();
			m_vertexSize = ModelNode.attribute("VertexSize").as_uint();
			m_scale = ModelNode.attribute("scale").as_float();
			while( ModelChild)
			{

				std::string s(ModelChild.name());
				if(_stricmp("Attribute", s.c_str() ) == 0)
				{
					//this is an attribute add it
					VertexAttribute attrib;
					attrib.Name = ModelChild.attribute("name").as_string();
					attrib.numValues = ModelChild.attribute("numValues").as_uint();
					attrib.offsetOf = ModelChild.attribute("offsetOf").as_uint();
					m_attributes.emplace_back(attrib);
				}
				else if(_stricmp("Material", s.c_str() ) == 0)
				{
					//this is an attribute add it
					std::shared_ptr<Material> mat(new Material());
					mat->name = ModelChild.attribute("name").as_string();
				
					mat->Texture = Texture::CreateOrGetTexture(TEXTURES_PATH + ModelChild.attribute("fileName").as_string() );
					m_materials.insert(mat);
				}
				else if(_stricmp("Batch", s.c_str() ) == 0)
				{
					//this is triangle batch data
					BatchData bData;
					bData.numVerts = ModelChild.attribute("numVerts").as_uint();
					bData.numIndices = ModelChild.attribute("numIndices").as_uint();
					unsigned int offset = ModelChild.attribute("OffsetStart").as_uint();

					in.seekg(offset, std::ios::beg);
					
					std::vector<char> vertexData;
					vertexData.resize(m_vertexSize * bData.numVerts);
					in.read((char*)vertexData.data(), m_vertexSize * bData.numVerts);

					//load buffer object
					bData.idVBO = loadVertexBufferObject(vertexData.data(), m_vertexSize * bData.numVerts);

					std::vector<unsigned int> indices;
					indices.resize(bData.numIndices);
					in.read((char*)indices.data(), bData.numIndices * sizeof(unsigned int) );

					bData.idIBO = loadIndexBufferObject(indices.data(), bData.numIndices * sizeof(unsigned int));
					
					m_batchDatas.emplace_back(bData);
				}
				ModelChild = ModelChild.next_sibling();
			}
			return true;
		}

		return false;
	}

	VertexAttribute* Model::getVertexAttribute(const std::string& AttributeName)
	{
		for(auto it = m_attributes.begin(); it != m_attributes.end(); it++)
		{
			if(_stricmp(AttributeName.c_str(), it->Name.c_str()) == 0)
			{
				return &(*it);
			}
		}
		return nullptr;
	}

	std::shared_ptr<Texture> Model::getMaterialTexture( const std::string& MaterialName )
	{
		for(auto it = m_materials.begin(); it != m_materials.end(); it++)
		{
			if(_stricmp(MaterialName.c_str(), (*it)->name.c_str()) == 0)
			{
				return (*it)->Texture;
			}
		}

		return nullptr;
	}

}
