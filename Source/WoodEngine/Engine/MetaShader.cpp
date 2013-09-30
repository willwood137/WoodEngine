
#include "../stdafx.h"
#include "MetaShader.hpp"
#include "..\libs\pugixml.hpp"

namespace woodman
{

	MetaShader::MetaShader(const std::string& shaderName)
		: m_shaderName(shaderName)
	{

	}

	bool MetaShader::LoadShader()
	{
		std::string headerPath = m_shaderName;
		headerPath.append(".xml");

		pugi::xml_document* doc = new pugi::xml_document();
		pugi::xml_parse_result result = doc->load_file( headerPath.c_str() );

		pugi::xml_node ModelNode = doc->first_child().first_child();
		pugi::xml_node ModelChild = ModelNode.first_child();
		while( ModelChild)
		{

			std::string s(ModelChild.name());
			if(_stricmp("Attribute", s.c_str() ) == 0)
			{
				//this is an attribute add it
				Attribute attrib;
				attrib.name = ModelChild.attribute("name").as_string();
				attrib.numValues = ModelChild.attribute("numValues").as_uint();
				m_attributes.emplace_back(attrib);
			}
			else if(_stricmp("Uniform", s.c_str() ) == 0)
			{
				//this is triangle batch data
				Uniform uni;
				//uni.dType = (DataType)(ModelChild.attribute("DataType").as_uint());
				uni.numValues = ModelChild.attribute("numValues").as_uint();
				uni.name = ModelChild.attribute("Name").as_string();

				m_uniforms.emplace_back(uni);
			}
			ModelChild = ModelChild.next_sibling();
		}

		//Load Shader
		bool good;

		std::string vertexShaderSourceText;
		good = getFileText( m_shaderName + ".vert", vertexShaderSourceText );

		if(!good)
		{
			//error!
			return false;	
		}

		std::string fragmentShaderSourceText;
		good = getFileText( m_shaderName + ".frag", fragmentShaderSourceText );

		if(!good)
		{
			//error!
			return false;	
		}



		m_shaderID = glCreateProgram();

		GLuint idVertexShader = glCreateShader( GL_VERTEX_SHADER );

		const char* szVertexShaderSourceText = vertexShaderSourceText.c_str();
		glShaderSource( idVertexShader, 1, &szVertexShaderSourceText, NULL );
		glCompileShader( idVertexShader );

		std::string log;
		OutputDebugStringA( (m_shaderName + " Vertex Shader Log: \n").c_str() );
		getShaderLog( idVertexShader, log );
		OutputDebugStringA( log.c_str() );
		log.clear();

		const char* szFragmentShaderSourceText = fragmentShaderSourceText.c_str();
		GLuint idFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
		glShaderSource( idFragmentShader, 1, &szFragmentShaderSourceText, NULL );
		glCompileShader( idFragmentShader );

		OutputDebugStringA( (m_shaderName + " Fragment Shader Log: \n").c_str() );
		getShaderLog( idFragmentShader, log );
		OutputDebugStringA( log.c_str() );
		log.clear();

		glAttachShader( m_shaderID, idVertexShader );
		glAttachShader( m_shaderID, idFragmentShader );

		glLinkProgram( m_shaderID );

		OutputDebugString( TEXT("Terrain Shader Program Link Log: \n") );
		getProgramLog(m_shaderID, log );
		OutputDebugStringA( log.c_str() );
		log.clear();

		glValidateProgram( m_shaderID );
		OutputDebugString( TEXT("Terrain Shader Program Validation Log: \n") );
		getProgramLog( m_shaderID, log);
		OutputDebugStringA( log.c_str() );
		log.clear();


		//link attributes and uniforms
		for(auto it = m_attributes.begin(); it != m_attributes.end(); ++it)
		{
			it->id = glGetAttribLocation(m_shaderID, it->name.c_str());
		}

		for(auto it = m_uniforms.begin(); it != m_uniforms.end(); ++it)
		{

		}

		return true;
	}

	

	void MetaShader::render( Model* modelToRender )
	{
// 		glUseProgram(m_shaderID);
// 		unsigned int vertexSize = modelToRender->getVertexSize();
// 		for(auto it = m_attributes.begin(); it!= m_attributes.end(); ++it)
// 		{
// 			VertexAttribute* attrib = nullptr;
// 			attrib = modelToRender->getVertexAttribute(it->name);
// 			glVertexAttribPointer(it->id, it->numValues, GL_FLOAT, GL_FALSE,
// 				vertexSize,	reinterpret_cast< void* >(attrib->offsetOf) );
// 			glEnableVertexAttribArray(it->id);
// 		}
// 
// 		// Render Calls
// 
// 		//get Batch Data
// 		std::vector<BatchData>* ModelBatchData = modelToRender->getBatchData();
// 
// 		for(auto it = ModelBatchData->begin(); it != ModelBatchData->end(); ++it)
// 		{
// 			glBindBuffer( GL_ARRAY_BUFFER, it->idVBO );
// 			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, it->idIBO );
// 			glDrawElements( GL_TRIANGLES, it->numIndices, GL_UNSIGNED_INT, 0 ); 
// 		}
	}
}