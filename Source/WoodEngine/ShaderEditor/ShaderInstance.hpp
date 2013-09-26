#pragma once

#include "NodeInstance.hpp"
#include "ShaderNode.hpp"

#include "../Engine/HashedString.hpp"

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
		void CompileLink(std::shared_ptr<NodeLinkInstance> link, std::string& compilation);
		void linkSlots( HashedString linkAID, HashedString linkBID );
	
	private:
		// Node Instances
		std::map< HashedString, std::shared_ptr< NodeInstance > > m_nodeInstances;
		std::string m_shaderName;
		
		std::set<HashedString> m_attributes;
		std::set<HashedString> m_uniforms;
		std::set<HashedString> m_varying;
		std::set<HashedString> m_outputs;
		
	};
}