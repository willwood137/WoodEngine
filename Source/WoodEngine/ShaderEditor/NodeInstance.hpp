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
		NodeLinkInstance(HashedString uniqueID) : m_uniqueID(uniqueID), parentLink(nullptr), parentNodeInstance(nullptr), partnerLinkInstance(nullptr) {}

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

		DataType m_dType;

		bool exitNode;
		unsigned int lastCompile;
	};

	struct DataFieldInstance
	{
		DataFieldInstance(HashedString id) : m_uniqueID(id) {}
		std::string m_name;
		HashedString m_uniqueID;
		float m_value;
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
		std::map< HashedString, std::unique_ptr<DataFieldInstance> >* getDataFields();
		NodeLinkInstance* getLinkByID(HashedString uniqueID);
		NodeLinkInstance* getLinkInstanceByName(const std::string& linkName);
		DataFieldInstance* getDataFieldInstanceByName(const std::string& fieldName);

		HashedString getUniqueID()
		{
			return m_uniqueID;
		}
		void CompileLink(NodeLinkInstance* link, std::string& compilation, unsigned int compileID);

		virtual void setPosition(const Vector2f& position)
		{
			m_position = position;
		}


		virtual void setDataField(HashedString fieldName, float value);

	private:

		ShaderNode* m_referenceNode;

		// collection of the Link instances
		std::map< HashedString, std::unique_ptr< NodeLinkInstance > > m_linkInstances;
		std::map< HashedString, std::unique_ptr<DataFieldInstance> > m_dataFields;
		
		HashedString					m_uniqueID;
		SHADER_TYPE						m_shaderType;
		Vector2f						m_position;
		Vector2f						m_size;
	};
}