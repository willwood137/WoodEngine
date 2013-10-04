#pragma once

#include "HashedString.hpp"

#include "..\Math\Vector2.hpp"
#include "..\Math\Vector3.hpp"
#include "..\Math\Vector4.hpp"
#include "..\Math\Matrix4.hpp"

#include <map>
#include <memory>
#include <set>

#include "Model.hpp"

namespace woodman
{


	class Shader
	{
	public:

		struct ShaderCompare
		{
			bool operator()( const std::shared_ptr<Shader>& lhs, const std::shared_ptr<Shader>& rhs ) const;
		};

		Shader(const std::string& filepath);

		Shader() {}
		
		bool loadShader( const std::string& filepath );
		bool addAttribute(const HashedString& attributeName);
		bool addUniform(const HashedString& uniformName);

		bool load();

		unsigned int getAttributeID(const HashedString& attributeName) const;
		unsigned int getUniformID(const HashedString& uniformName) const;

		bool setUpAttrbituesFromModel(std::shared_ptr<Model> model);

		bool SetUniformFloat(const HashedString& uniformName, float& values, unsigned int numValues);
		bool SetUniformInt(const HashedString& uniformName, int& values, unsigned int numValues);
		bool SetUniformVector2(const HashedString& uniformName, Vector2f& vec2Values, unsigned int numValues);
		bool SetUniformVector3(const HashedString& uniformName, Vector3f& vec3Values, unsigned int numValues);
		bool SetUniformVector4(const HashedString& uniformName, Vector4f& vec4Values, unsigned int numValues);
		bool SetUniformMatrix(const HashedString& uniformName, Matrix4f& matrices, unsigned int numValues);

		bool setAttribute( const HashedString& attributeName, 
			const unsigned int numValues, 
			const unsigned int glType, 
			const size_t VertexSize, 
			const size_t Offset ) const;

		bool disableAttribute( const HashedString& attributeName ) const;

		template <typename T_ObjType>
		bool setUniform( const HashedString& uniformName, const T_ObjType& data) const;


		bool IsLoaded() const;
		// Returns true if the texture has been successfully loaded through 
		// one of the Load...() functions below 
		// and is ready to be applied for rendering.

		std::string GetFilePath();
		// Returns the path of the file (if any) from which this
		// texture was loaded.
		// Returns an empty string if no file was used in loading this
		// texture or if the texture has not been loaded.

		void renderSimpleQuad();

		static std::shared_ptr<Shader> CreateOrGetShader( 
			const std::string& ShaderPath );

		static void DestroyShader( std::shared_ptr<Shader>& pShader );

		static unsigned int QuadBufferID;

		static void LoadQuadBuffer();

	private:
		std::map<HashedString, unsigned int> m_attributes;
		std::map<HashedString, unsigned int> m_uniforms;
		std::string m_filePath;
		unsigned int m_shaderID;
		bool m_isLoaded;

		static std::set< std::shared_ptr<Shader>, ShaderCompare > Shaders;



	};

	inline bool Shader::ShaderCompare::operator()( const std::shared_ptr<Shader>& lhs, const std::shared_ptr<Shader>& rhs ) const
	{
		return lhs->m_filePath.compare(rhs->m_filePath) < 0;
	}


// 	inline bool Shader::setUniform( const HashedString& uniformName, const float& data) const 
// 	{
// 		glUniform1f( m_uniforms[uniformName], data);
// 	}


}