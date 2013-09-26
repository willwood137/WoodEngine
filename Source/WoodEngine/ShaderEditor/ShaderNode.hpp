#pragma once

#include <vector>
#include <list>
#include <memory>
#include <string>
#include <array>

namespace woodman
{

	enum PropertyType
	{
		PROPERTYTYPE_FLOAT,
		PROPERTYTYPE_VECTOR,
		PROPERTYTYPE_MATRIX,
		PROPERTYTYPE_STRING,
		PROPERTYTYPE_LIST,
		PROPERTYTYPE_SIZE,
	};


	struct PropertyList
	{
		PropertyType type;
		std::string name;		
		std::list< std::shared_ptr<PropertyList> > children;
	};

	struct NodeLink
	{
		std::string name;
		PropertyType type;
		std::string ShaderCode;
		std::string attributeName;
		std::string uniformName;
		std::string varyingName;
		std::string OpenGLName;
		std::string outName;
	};

	 struct ShaderNode
	 {
		std::string name;
		std::string category;
		std::list< std::shared_ptr<PropertyList> > propertyLists;
		std::vector<std::shared_ptr<NodeLink> > inLinks;
		std::vector<std::shared_ptr<NodeLink> > outLinks;
		bool canBeVert;
		bool canBeFrag;
	 };
}