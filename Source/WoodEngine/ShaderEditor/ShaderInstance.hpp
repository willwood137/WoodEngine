#pragma once

#include "NodeInstance.hpp"
#include "ShaderNode.hpp"

#include "../Engine/HashedString.hpp"
#include "../Utility/Utility.hpp"

#include <map>
#include <set>

namespace woodman
{
	

	class ShaderInstance
	{
	public:
		void render();
		std::shared_ptr<NodeInstance> CreateNewNodeInstance( std::shared_ptr<ShaderNode> NodeDefinition, SHADER_TYPE shaderType, const Vector2f& position );
		void Compile();
		//void CompileLink(std::shared_ptr<NodeLinkInstance> link, std::string& compilation);
		void linkSlots( const variableInfo& linkAInfo, const variableInfo& linkBInfo );

		void LoadShaderInstance( const std::string& fileName );
		void SaveShaderInstance( const std::string& fileName );
	
	private:
		std::string m_shaderName;

		// Node Instances
		std::map< HashedString, std::shared_ptr< NodeInstance > > m_nodeInstances;:/
		std::string m_shaderName;

		unsigned int m_compileCounter;
		
		
		std::map<HashedString, variableInfo> m_attributes;
		std::map<HashedString, variableInfo> m_uniforms;
		std::map<HashedString, variableInfo> m_varying;
		std::map<HashedString, variableInfo> m_outputs;
		
	};
}