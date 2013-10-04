#pragma once

#include "ShaderNode.hpp"
#include "../Math/Vector2.hpp"
#include "../Engine/HashedString.hpp"
#include "../Utility/Utility.hpp"


#include "../Ui/UINodeSlot.hpp"
#include "../Ui/UINodeBox.hpp"

#include <memory>
#include <vector>
#include <map>

namespace woodman
{
	class NodeInstance;

	enum SHADER_TYPE
	{
		SHADER_TYPE_VERTEX,
		SHADER_TYPE_FRAGMENT,
		SHADER_TYPE_SIZE
	};

	struct variableInfo
	{
		variableInfo() : VariableName("") {}
		variableInfo(HashedString _name) : VariableName(_name) {}

		HashedString VariableName;
		std::string variablePrefix;
	};

	struct NodeLinkInstance : public NodeSlotCallBackInfoBase
	{
		NodeLinkInstance(HashedString uniqueID) : m_uniqueID(uniqueID) {}

		//CallBackFunctions
		virtual void CallBackLinkToNodeSlot(std::shared_ptr<NodeSlotCallBackInfoBase> partner); 

		HashedString m_uniqueID;
		std::string m_linkName;
		
		// needs to know which link it is referencing
		std::shared_ptr<NodeLink> parentLink;

		// the parent node of this link
		NodeInstance* parentNodeInstance;
		
		// what this is connecting to
		std::shared_ptr<NodeLinkInstance> partnerLinkInstance;

		variableInfo varInfo;

		PropertyType pType;
		unsigned int typeSize;

		bool exitNode;
		unsigned int lastCompile;
	};

	class NodeInstance : public UINodeBoxCallBackRecipient
	{

	public:
		NodeInstance( std::shared_ptr<ShaderNode > NodeReference, const Vector2f& startPosition, SHADER_TYPE shaderType, HashedString uniqueID );
		
		void render();
		void update();
		SHADER_TYPE getShaderType() { return m_shaderType;}
		std::shared_ptr<ShaderNode> getDefinitionNode()
		{
			return m_referenceNode;
		}


		// Getters
		Vector2f getPosition() { return m_position;} 
		std::string& getName() { return m_referenceNode->name;} 
		std::map< HashedString, std::shared_ptr< NodeLinkInstance > >* getUINodeLinkInstances( )
		{
			return &m_linkInstances;
		}
		std::shared_ptr<NodeLinkInstance> getLinkByID(HashedString uniqueID);
		std::shared_ptr<NodeLinkInstance> getLinkInstanceByName(const std::string& linkName);

		HashedString getUniqueID()
		{
			return m_uniqueID;
		}
		void CompileLink(std::shared_ptr<NodeLinkInstance> link, std::string& compilation, unsigned int compileID);

		virtual void setPosition(const Vector2f& position)
		{
			m_position = position;
		}

	private:

		std::shared_ptr< ShaderNode > m_referenceNode;

		// collection of the Link instances 
// 		std::vector< std::shared_ptr< NodeLinkInstance > > m_inLinkInstances;
// 		std::vector< std::shared_ptr< NodeLinkInstance > > m_outLinkInstances;

		std::map< HashedString, std::shared_ptr< NodeLinkInstance > > m_linkInstances;
		
		HashedString m_uniqueID;


		SHADER_TYPE						m_shaderType;
		Vector2f						m_position;
		Vector2f						m_size;
	};
}