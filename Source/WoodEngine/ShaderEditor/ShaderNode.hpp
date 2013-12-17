#pragma once

#include "../Engine/HashedString.hpp"

#include "../Utility/Utility.hpp"

#include <vector>
#include <list>
#include <memory>
#include <string>
#include <array>
#include <map>


namespace woodman
{
	struct FunctionDefinition
	{
		std::string functionCode;
		DataType returnType;
	};

	struct NodeLink
	{
		std::string name;
		DataType typeData;
		std::string ShaderCode;
		std::string attributeName;
		std::string uniformName;
		std::string varyingName;
		std::string OpenGLName;
		std::string outName;

	};

	struct DataField
	{
		std::string name;
	};

	enum LINK_SLOT_TYPE;

	struct NodeLinkSlot
	{
		LINK_SLOT_TYPE type;
		std::vector<std::string> linkNames;
	};

	struct ShaderNode
	{
		NodeLink* getLinkByName(const std::string& name)
		{
			for(auto it = inLinks.begin(); it != inLinks.end(); ++it)
			{
				if( (*it)->name.compare(name) == 0)
				{
					return (*it).get();
				}
			}
			return nullptr;
		}

		std::string name;
		std::string category;
		//std::list< std::unique_ptr<PropertyList> > propertyLists;
		std::vector< DataField > dataFields;
		std::vector<std::unique_ptr<NodeLink> > inLinks;
		std::vector<std::unique_ptr<NodeLink> > outLinks;
		std::vector<NodeLinkSlot> linkSlots;
		std::vector<FunctionDefinition> functions;
		bool canBeVert;
		bool canBeFrag;
	};

	struct NodeCategory
	{
		std::string name;
		std::map<HashedString, std::unique_ptr<ShaderNode> > nodes;
	};
}