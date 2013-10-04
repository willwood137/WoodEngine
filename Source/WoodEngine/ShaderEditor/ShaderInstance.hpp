#pragma once

#include "NodeInstance.hpp"
#include "ShaderNode.hpp"

#include "../Engine/HashedString.hpp"
#include "../Utility/Utility.hpp"

#include <map>
#include <set>

namespace woodman
{
	struct LinkInfo
	{
		std::string Name;
		std::string PartnerNodeInstanceName;
		std::string PartnerLinkName;
	};

	struct NodeInstanceData
	{
		std::string defCategory;
		std::string defName;
		std::string Name;
		SHADER_TYPE shaderType;
		Vector2f Position;

		std::vector<std::shared_ptr<LinkInfo> > LinkInfos;
	};

	class ShaderInstance
	{
	public:
		void render();
		NodeInstance* CreateNewNodeInstance( ShaderNode* NodeDefinition, SHADER_TYPE shaderType, const Vector2f& position );
		NodeInstance* CreateNewNodeInstance( ShaderNode* NodeDefinition, SHADER_TYPE shaderType, const Vector2f& position, const std::string& nameOverride );
		void Compile();
		//void CompileLink(std::shared_ptr<NodeLinkInstance> link, std::string& compilation);
		void PairlinkSlots( const variableInfo& linkAInfo, const variableInfo& linkBInfo );

		void LoadShaderInstance( const std::string& fileName, std::vector<std::shared_ptr<NodeInstanceData> >& NodeInstanceInfos );
		void SaveShaderInstance( const std::string& fileName );

		NodeInstance* getNodeInstanceByID( HashedString ID );
	
	private:

		// Node Instances
		std::map< HashedString, std::unique_ptr< NodeInstance > > m_nodeInstances;
		std::string m_shaderName;

		unsigned int m_compileCounter;
		
		
		std::map<HashedString, variableInfo> m_attributes;
		std::map<HashedString, variableInfo> m_uniforms;
		std::map<HashedString, variableInfo> m_varying;
		std::map<HashedString, variableInfo> m_outputs;
		
	};
}