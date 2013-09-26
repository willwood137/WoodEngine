#include "../stdafx.h"

#include "NodeInstance.hpp"
#include "../Engine/Font.hpp"
#include "../UI/UIStyle.hpp"


namespace woodman
{
	void NodeInstance::render()
	{

	}

	NodeInstance::NodeInstance( std::shared_ptr<ShaderNode > NodeReference, const Vector2f& startPosition, SHADER_TYPE shaderType, HashedString uniqueID )
		: m_referenceNode(NodeReference),
			m_position(startPosition),
			m_uniqueID(uniqueID),
			m_shaderType(shaderType)
	{

		//create in Links
		int i = 0;
		for(auto it = NodeReference->inLinks.begin(); it != NodeReference->inLinks.end(); ++it)
		{
			HashedString LinkuniqueID( uniqueID.m_string + "_" + (*it)->name );
			std::shared_ptr<NodeLinkInstance> newNodeLink(new NodeLinkInstance(LinkuniqueID));
			newNodeLink->parentLink = (*it);
			newNodeLink->parentNodeInstance = this;
			newNodeLink->exitNode = false;
			m_linkInstances.insert( std::make_pair(LinkuniqueID, newNodeLink) );
		}

		//create out links
		for( auto it = NodeReference->outLinks.begin(); it != NodeReference->outLinks.end(); ++it)
		{
			HashedString LinkuniqueID( uniqueID.m_string + "_" + (*it)->name );

			std::shared_ptr<NodeLinkInstance> newNodeLink(new NodeLinkInstance(LinkuniqueID));
			newNodeLink->parentLink = (*it);
			newNodeLink->parentNodeInstance = this;
			newNodeLink->exitNode = true;
			m_linkInstances.insert(std::make_pair(LinkuniqueID, newNodeLink) );
		}

		// get title Size
		
		
	}

	

	void NodeInstance::update()
	{
	}

 	

	std::shared_ptr<NodeLinkInstance> NodeInstance::getLinkByID(HashedString uniqueID)
	{
		auto it = m_linkInstances.find(uniqueID);

		if(it != m_linkInstances.end())
		{
			return it->second;
		}
		else 
			return nullptr;
	}

	void NodeInstance::CompileLink(std::shared_ptr<NodeLinkInstance> link, std::string& compilation)
	{
		compilation += link->parentLink->ShaderCode;
	}


	std::shared_ptr<NodeLinkInstance> NodeInstance::getLinkInstanceByName(const std::string& linkName)
	{
		for(auto it = m_linkInstances.begin(); it != m_linkInstances.end(); ++it)
		{
			if( (it->second)->parentLink->name.compare(linkName) == 0 )
			{
				return it->second;
			}
		}

		return nullptr;
	}
}