#include "..\stdafx.h"

#include "Shader.hpp"
#include "..\Utility\Utility.hpp"\

#include <sstream>


namespace woodman
{


	std::set<std::shared_ptr<Shader>, Shader::ShaderCompare > Shader::Shaders;
	unsigned int Shader::QuadBufferID;

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	Shader::Shader( const std::string& filePath ) : m_filePath(filePath)
	{
		loadShader(m_filePath);
	}

	bool Shader::loadShader( const std::string& filepath )
	{
		m_filePath = filepath;
		std::string shaderName = m_filePath.substr(m_filePath.find_last_of("\\") + 1);
		bool good;

		std::string vertexShaderSourceText;
		good = getFileText( m_filePath + ".vert", vertexShaderSourceText );

		if(!good)
		{
			//error!
			return false;	
		}

		std::string fragmentShaderSourceText;
		good = getFileText( m_filePath + ".frag", fragmentShaderSourceText );

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
		OutputDebugStringA( (shaderName + " Vertex Shader Log: \n").c_str() );
		getShaderLog( idVertexShader, log );
		OutputDebugStringA( log.c_str() );
		log.clear();

		const char* szFragmentShaderSourceText = fragmentShaderSourceText.c_str();
		GLuint idFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
		glShaderSource( idFragmentShader, 1, &szFragmentShaderSourceText, NULL );
		glCompileShader( idFragmentShader );

		OutputDebugStringA( (shaderName + " Fragment Shader Log: \n").c_str() );
		getShaderLog( idFragmentShader, log );
		OutputDebugStringA( log.c_str() );
		log.clear();

		glAttachShader( m_shaderID, idVertexShader );
		glAttachShader( m_shaderID, idFragmentShader );

		glLinkProgram( m_shaderID );

		OutputDebugStringA( (shaderName + "Program Link Log: \n").c_str() );
		getProgramLog(m_shaderID, log );
		OutputDebugStringA( log.c_str() );
		log.clear();

		glValidateProgram( m_shaderID );
		OutputDebugStringA( ( shaderName + "Program Validation Log: \n").c_str() );
		getProgramLog( m_shaderID, log);
		OutputDebugStringA( log.c_str() );
		log.clear();



		//get all uniforms from the vertex shader
		
		std::stringstream vertexSS(vertexShaderSourceText);

		while(vertexSS)
		{
			std::string sub;
			vertexSS >> sub;
			if(sub.compare("uniform") == 0)
			{
				vertexSS >> sub;
				vertexSS >> sub;
				unsigned int last = min( sub.find(";"), sub.find("[") );
				addUniform(sub.substr(0, last) );
			}
			else if(sub.compare("attribute") == 0 || sub.compare("in") == 0)
			{

				vertexSS >> sub;
				vertexSS >> sub;
				addAttribute(sub.substr(0, sub.find(";") ) );
			}
		}

		std::stringstream fragSS(fragmentShaderSourceText);

		while(fragSS)
		{
			std::string sub;
			fragSS >> sub;
			if(sub.compare("uniform") == 0)
			{
				fragSS >> sub;
				fragSS >> sub;
				unsigned int last = min( sub.find(";"), sub.find("[") );
				addUniform(sub.substr(0, last) );
			}
		}

		return true;
	}
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	bool Shader::addAttribute(const HashedString& attributeName)
	{
		unsigned int temp = glGetAttribLocation(m_shaderID, attributeName.m_string.c_str());
		if( temp == -1)
		{
			//error report
			// 
			// TODO
			return false;
		}

		m_attributes[attributeName] = temp;
		return true;
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	bool Shader::addUniform( const HashedString& uniformName)
	{
		unsigned int temp = glGetUniformLocation(m_shaderID, uniformName.m_string.c_str());
		if( temp == -1)
		{
			//error report
			//
			// TODO
			return false;
		}
		m_uniforms[uniformName] = temp;
		return true;
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	unsigned int Shader::getAttributeID(const HashedString& attributeName) const
	{
		//return m_attributes[attributeName];
		return m_attributes.find(attributeName)->second;
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	unsigned int Shader::getUniformID(const HashedString& uniformName) const
	{
		//return m_uniforms[uniformName];
		return m_uniforms.find(uniformName)->second;
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	bool Shader::setAttribute( const HashedString& attributeName, 
		const unsigned int numValues, 
		const unsigned int glType, 
		const size_t VertexSize, 
		const size_t Offset ) const
	{
		auto it = m_attributes.find(attributeName);
		if( it != m_attributes.end())
		{
			unsigned int attribID = it->second;
			glVertexAttribPointer(attribID, numValues, glType, GL_FALSE,
				VertexSize,	reinterpret_cast< void* >(Offset) );
			glEnableVertexAttribArray(attribID);
			return true;
		}
		else
		{
			//report error
			return false;
		}
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	bool Shader::disableAttribute( const HashedString& attributeName ) const
	{
		//glDisableVertexAttribArray(m_attributes[attributeName]);
		glDisableVertexAttribArray(m_attributes.find(attributeName)->second);
		return true;
	}

	bool Shader::load()
	{
		
		glUseProgram(m_shaderID);
		
		return true;
	}

	std::string Shader::GetFilePath()
	{
		// Returns the path of the file (if any) from which this
		// texture was loaded.
		// Returns an empty string if no file was used in loading this
		// texture or if the texture has not been loaded.

		if( m_isLoaded )
			return m_filePath;

		return "";
	}

	void Shader::LoadQuadBuffer()
	{
		std::vector<Vector2f> quadBuffer;

		quadBuffer.push_back(Vector2f(0.0, 0.0));
		quadBuffer.push_back(Vector2f(0.0, 1.0));
		quadBuffer.push_back(Vector2f(1.0, 1.0));
		quadBuffer.push_back(Vector2f(1.0, 0.0));

		QuadBufferID = LoadBufferObject(GL_ARRAY_BUFFER, quadBuffer.data(), sizeof(Vector2f) * quadBuffer.size());
	}

	bool Shader::IsLoaded() const
	{
		// Returns true if the texture has been successfully loaded through 
		// one of the Load...() functions below 
		// and is ready to be applied for rendering.

		return m_isLoaded;
	}

	std::shared_ptr<Shader> Shader::CreateOrGetShader( 
		const std::string& ShaderPath )
	{
		REQUIRES( !ShaderPath.empty() );
		REQUIRES( strlen( ShaderPath.c_str() ) > 0 );

		bool found = false;
		std::shared_ptr<Shader> result = 0;

		std::set<std::shared_ptr<Shader>, Shader::ShaderCompare >::iterator ShadersIterator;
		ShadersIterator = Shaders.begin();

		while(ShadersIterator != Shaders.end() )
		{
			if( (*ShadersIterator)->GetFilePath().compare(ShaderPath) == 0 ) 
			{
				found = true;
				result = (*ShadersIterator);
				break;
			}
			++ShadersIterator;
		}
		if(!found)
		{	
			result = std::make_shared<Shader>(Shader( ShaderPath ));
			Shaders.insert(result);
		}



		PROMISES( result );
		PROMISES( result->IsLoaded() );
		PROMISES( result->GetFilePath().compare( ShaderPath ) == 0 );

		if(!result || !result->IsLoaded() || !( result->GetFilePath() == ShaderPath ) )
		{
			// TODO
			// Throw exception
		}


		return result;
	}

	void Shader::DestroyShader( std::shared_ptr<Shader>& pShader )
	{
		if(pShader != 0)
		{
			if(pShader.use_count() <= 2 )
			{
				glDeleteProgram(pShader->m_shaderID);
				Shaders.erase(pShader);
				pShader = 0;

			}
			else
			{
				// Shader is still being used by another object
				pShader = 0;

			}
		}
		PROMISES( pShader == 0 );
	}

	void Shader::renderSimpleQuad()
	{
		setAttribute(HASHED_STRING_in_position, 2, GL_FLOAT, sizeof(Vector2f), 0);

		glDrawArrays( GL_QUADS, 0, 4);

		disableAttribute(HASHED_STRING_in_position);
	}

	bool Shader::SetUniformFloat(const HashedString& uniformName, float& values, unsigned int numValues)
	{
		if(m_uniforms.find(uniformName) != m_uniforms.end() )
		{
			glUniform1fv(m_uniforms[uniformName], numValues, reinterpret_cast<GLfloat*>(&values) );
			return true;
		}

		return false;
	}


	bool Shader::SetUniformInt(const HashedString& uniformName, int& values, unsigned int numValues)
	{
		if(m_uniforms.find(uniformName) != m_uniforms.end() )
		{
			glUniform1iv(m_uniforms[uniformName], numValues, reinterpret_cast<GLint*>( &values) );
			return true;
		}

		return false;
	}

	bool Shader::SetUniformVector2(const HashedString& uniformName, Vector2f& vec2Values, unsigned int numValues)
	{
		if(m_uniforms.find(uniformName) != m_uniforms.end() )
		{
			glUniform2fv(m_uniforms[uniformName], numValues, reinterpret_cast<GLfloat*>( &vec2Values ) );
			return true;
		}

		return false;
	}

	bool Shader::SetUniformVector3(const HashedString& uniformName, Vector3f& vec3Values, unsigned int numValues)
	{
		if(m_uniforms.find(uniformName) != m_uniforms.end() )
		{
			glUniform3fv(m_uniforms[uniformName], numValues, reinterpret_cast<GLfloat*>( &vec3Values ) );
			return true;
		}

		return false;
	}

	bool Shader::SetUniformVector4(const HashedString& uniformName, Vector4f& vec4Values, unsigned int numValues)
	{
		if(m_uniforms.find(uniformName) != m_uniforms.end() )
		{
			glUniform4fv(m_uniforms[uniformName], numValues, reinterpret_cast<GLfloat*>( &vec4Values ) );
			return true;
		}

		return false;
	}

	bool Shader::SetUniformMatrix(const HashedString& uniformName, Matrix4f& matrices, unsigned int numValues)
	{
		if(m_uniforms.find(uniformName) != m_uniforms.end() )
		{
			glUniformMatrix4fv(m_uniforms[uniformName], numValues, false, reinterpret_cast<GLfloat*>( &matrices ) );
			return true;
		}

		return false;
	}


	bool Shader::setUpAttrbituesFromModel(std::shared_ptr<Model> model)
	{

		bool good = true;
		for(auto attributeIt = m_attributes.begin(); attributeIt != m_attributes.end(); ++attributeIt)
		{
			std::string attributeName = attributeIt->first.m_string.substr(2);
			VertexAttribute* modelAttribute = model->getVertexAttribute(attributeName);

			if(modelAttribute == nullptr)
			{
				//this model file does not have the given attribute
				good = false;
			}
			else
			{


				glVertexAttribPointer(attributeIt->second, modelAttribute->numValues, GL_FLOAT, GL_FALSE,
					model->getVertexSize(),	reinterpret_cast< void* >(modelAttribute->offsetOf) );
				glEnableVertexAttribArray(attributeIt->second);
			}
		}

		return good;
	}

}