#pragma once

#include "ShaderNode.hpp"
#include "../Math/Vector2.hpp"
#include "../Engine/HashedString.hpp"
#include "../Utility/Utility.hpp"


#include "../Ui/UINodeLink.hpp"
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

	struct NodeLinkInstance : public UINodeLinkCallBackBase
	{
		NodeLinkInstance(HashedString uniqueID) : m_uniqueID(uniqueID) {}

		//CallBackFunctions
		virtual void CallBackLinkToNodeSlot(UINodeLinkCallBackBase* partner); 

		HashedString m_uniqueID;
		std::string m_linkName;
		
		// needs to know which link it is referencing
		NodeLink* parentLink;

		// the parent node of this link
		NodeInstance* parentNodeInstance;
		
		// what this is connecting to
		NodeLinkInstance* partnerLinkInstance;

		variableInfo varInfo;

		PropertyType pType;
		unsigned int typeSize;

		bool exitNode;
		unsigned int lastCompile;
	};

	class NodeInstance : public UINodeBoxCallBackRecipient
	{

	public:
		NodeInstance( ShaderNode* NodeReference, const Vector2f& startPosition, SHADER_TYPE shaderType, HashedString uniqueID );
		
		void render();
		void update();

		SHADER_TYPE getShaderType() { return m_shaderType;}
		ShaderNode* getDefinitionNode()
		{
			return m_referenceNode;
		}


		// Getters
		Vector2f getPosition() { return m_position;} 
		std::string& getName() { return m_referenceNode->name;} 
		std::map< HashedString, std::unique_ptr< NodeLinkInstance > >* getUINodeLinkInstances( )
		{
			return &m_linkInstances;
		}
		NodeLinkInstance* getLinkByID(HashedString uniqueID);
		NodeLinkInstance* getLinkInstanceByName(const std::string& linkName);

		HashedString getUniqueID()
		{
			return m_uniqueID;
		}
		void CompileLink(NodeLinkInstance* link, std::string& compilation, unsigned int compileID);

		virtual void setPosition(const Vector2f& position)
		{
			m_position = position;
		}

	private:

		ShaderNode* m_referenceNode;

		// collection of the Link instances
		std::map< HashedString, std::unique_ptr< NodeLinkInstance > > m_linkInstances;
		
		HashedString					m_uniqueID;
		SHADER_TYPE						m_shaderType;
		Vector2f						m_position;
		Vector2f						m_size;
	};
}