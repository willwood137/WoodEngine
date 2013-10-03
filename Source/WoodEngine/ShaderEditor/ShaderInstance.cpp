#include "../stdafx.h"

#include "ShaderInstance.hpp"

#include <fstream>

namespace woodman
{
	void ShaderInstance::render()
	{
		//render background

	}

	std::shared_ptr<NodeInstance> ShaderInstance::CreateNewNodeInstance(std::shared_ptr<ShaderNode> NodeDefinition, SHADER_TYPE shaderType, const Vector2f& position)
	{
	
		int i = 0;
		HashedString tempID(NodeDefinition->name + "_00");

		
		bool hasUniqueID = false;
		do
		{
			std::stringstream ss;
			ss << i;
			tempID = HashedString(NodeDefinition->name + "_" + ss.str());

			if(m_nodeInstances.find(tempID) == m_nodeInstances.end())
			{
				hasUniqueID = true;
			}
			i++;
		}while(!hasUniqueID);


		std::shared_ptr<NodeInstance> newNode( new NodeInstance(NodeDefinition, position, shaderType, tempID) );

		m_nodeInstances.insert(std::make_pair(tempID, newNode) );

		return newNode;
	}

	void ShaderInstance::Compile()
	{
		std::string vertexCode, fragmentCode;
		std::vector<std::string> functions;
		std::set<HashedString> compiledDefinitions;

		//go through and find each node that has a openGL variable or a varying
		std::set<std::shared_ptr<NodeLinkInstance> > GLLinks;
		for(auto it = m_nodeInstances.begin(); it != m_nodeInstances.end(); ++it)
		{
			//check if we need to add the Definition
			std::shared_ptr<ShaderNode> nodeDef = it->second->getDefinitionNode();

			HashedString nodeDefName(nodeDef->name);



			if(compiledDefinitions.find(nodeDefName) == compiledDefinitions.end() )
			{
				compiledDefinitions.insert(nodeDefName);

				for(auto funcIt = nodeDef->functions.begin(); funcIt != nodeDef->functions.end(); ++funcIt)
				{
					functions.push_back( (*funcIt).functionCode );

				}
			}



			std::map< HashedString, std::shared_ptr< NodeLinkInstance > >* links = it->second->getUINodeLinkInstances();

			for(auto LinkIt = links->begin(); LinkIt != links->end(); ++LinkIt)
			{
				std::stringstream ss;
				
				ss << clamp<unsigned int>(LinkIt->second->typeSize, 1, 4);
				std::string sizeString(ss.str());

				switch( LinkIt->second->pType)
				{
				case PROPERTYTYPE_SAMPLER2D:
					LinkIt->second->varInfo.variablePrefix = "sampler2D";
					break;
				case PROPERTYTYPE_MATRIX:
					LinkIt->second->varInfo.variablePrefix = "mat" + sizeString;
					break;
				default:
				case PROPERTYTYPE_VECTOR:
					LinkIt->second->varInfo.variablePrefix = "vec" + sizeString;
					break;
				}

				//setup each links linkName
				if(!LinkIt->second->parentLink->attributeName.empty() )
				{
					LinkIt->second->varInfo.VariableName = HashedString("a_" + LinkIt->second->parentLink->attributeName);
					m_attributes.insert( std::make_pair(LinkIt->second->varInfo.VariableName, LinkIt->second->varInfo) );
				}
				else if(!LinkIt->second->parentLink->OpenGLName.empty() )
				{
					LinkIt->second->varInfo.VariableName = HashedString(LinkIt->second->parentLink->OpenGLName);

					//also use this as a starting point
					GLLinks.insert(LinkIt->second);
				}
				else if(!LinkIt->second->parentLink->outName.empty() )
				{
					LinkIt->second->varInfo.VariableName = HashedString(LinkIt->second->parentLink->outName);
					m_outputs.insert(std::make_pair(LinkIt->second->varInfo.VariableName, LinkIt->second->varInfo) );

					//also use this as a starting point
					GLLinks.insert(LinkIt->second);
				}
				else if(!LinkIt->second->parentLink->varyingName.empty() )
				{
					LinkIt->second->varInfo.VariableName = HashedString("v_" + LinkIt->second->parentLink->varyingName);
					m_varying.insert( std::make_pair(LinkIt->second->varInfo.VariableName, LinkIt->second->varInfo) );

					//also use this as a starting point
					if(!LinkIt->second->exitNode)
						GLLinks.insert(LinkIt->second);
				}
				else if(!LinkIt->second->parentLink->uniformName.empty() )
				{
					LinkIt->second->varInfo.VariableName = HashedString("u_" + LinkIt->second->parentLink->uniformName);
					m_uniforms.insert(std::make_pair(LinkIt->second->varInfo.VariableName, LinkIt->second->varInfo) );
				}
				else
				{
					LinkIt->second->varInfo.VariableName = LinkIt->second->m_uniqueID.m_string;
				}
				
					


			}
		}
		//now compile it
		m_compileCounter++;
		for(auto it = GLLinks.begin(); it != GLLinks.end(); it++)
		{
			if( (*it)->parentNodeInstance->getShaderType() == SHADER_TYPE_VERTEX)
			{
				(*it)->parentNodeInstance->CompileLink((*it), vertexCode, m_compileCounter);
			}
			else if( (*it)->parentNodeInstance->getShaderType() == SHADER_TYPE_FRAGMENT)
			{
				(*it)->parentNodeInstance->CompileLink((*it), fragmentCode, m_compileCounter);
			}
		}

		//now put it all together

		//write vertex file / header
		std::ofstream vertexFile;
		vertexFile.open ("ShaderTest.vert");
		vertexFile << "#version 400\n\n";

		std::string typePrefix;

		for(auto it = m_uniforms.begin(); it != m_uniforms.end(); ++it)
		{
			
			vertexFile << "uniform " << it->second.variablePrefix << " " << it->second.VariableName.m_string <<";\n";
		}

		vertexFile << "\n";

		for(auto it = m_attributes.begin(); it != m_attributes.end(); ++it)
		{
			vertexFile << "in " << it->second.variablePrefix << " " << it->second.VariableName.m_string <<";\n";
		}

		vertexFile << "\n";

		for(auto it = m_varying.begin(); it != m_varying.end(); ++it)
		{
			vertexFile << "out " << it->second.variablePrefix << " " << it->second.VariableName.m_string <<";\n";
		}

		for(auto funcIt = functions.begin(); funcIt != functions.end(); ++funcIt)
		{
			vertexFile << "\n" << *funcIt;
		}

		vertexFile << "\nvoid main(void)\n{\n";
		vertexFile << vertexCode << "\n}";


		//write fragment file / header
		std::ofstream fragFile;
		fragFile.open ("ShaderTest.frag");
		fragFile << "#version 400\n\n";

		for(auto it = m_uniforms.begin(); it != m_uniforms.end(); ++it)
		{
			fragFile << "uniform " << it->second.variablePrefix << " " << it->second.VariableName.m_string <<";\n";
		}

		fragFile << "\n";

		for(auto it = m_varying.begin(); it != m_varying.end(); ++it)
		{
			fragFile << "in " << it->second.variablePrefix << " " << it->second.VariableName.m_string <<";\n";
		}

		fragFile << "\n";

		for(auto it = m_outputs.begin(); it != m_outputs.end(); ++it)
		{
			fragFile << "out " << it->second.variablePrefix << " " << it->second.VariableName.m_string <<";\n";
		}

		fragFile << "\n";

		for(auto funcIt = functions.begin(); funcIt != functions.end(); ++funcIt)
		{
			fragFile << "\n" << *funcIt;
		}

		fragFile << "\nvoid main(void)\n{\n";
		fragFile << fragmentCode << "\n}";
	}


// 	void ShaderInstance::CompileLink(std::shared_ptr<NodeLinkInstance> link, std::string& compilation)
// 	{
// 		//we need to resolve this before we can compile this
// 
// 		if( link->exitNode )
// 		{
// 			if( !link->parentLink->ShaderCode.empty() )
// 			{
// 				//this has shader code put it in!
// 				std::stringstream ss, shaderCodeParse(link->parentLink->ShaderCode);
// 
// 				std::string tempString, insertString;
// 
// 				while(shaderCodeParse.good())
// 				{
// 
// 				shaderCodeParse >> tempString;
// 
// 					if(tempString[0] == '$')
// 					{
// 						//this is a variable, get its value
// 						std::shared_ptr<NodeLinkInstance> tempLinkInstance = link->parentNodeInstance->getLinkInstanceByName(tempString.substr(1));
// 
// 						if(tempLinkInstance != nullptr)
// 						{
// 							if(tempLinkInstance->partnerLinkInstance != nullptr)
// 							{
// 								CompileLink(tempLinkInstance, compilation);
// 							}
// 
// 							insertString += tempLinkInstance->m_linkName + " ";
// 						}
// 					}
// 					else
// 					{
// 						insertString += tempString + " ";
// 					}
// 				}
// 
// 				std::string typePrefix;
// 
// 				if(link->pType == PROPERTYTYPE_VECTOR)
// 					typePrefix = "vec";
// 				else if(link->pType == PROPERTYTYPE_SAMPLER2D)
// 					typePrefix = "sampler2D";
// 				else
// 					typePrefix = "mat";
// 
// 				if(link->pType != PROPERTYTYPE_SAMPLER2D)
// 				{
// 					std::stringstream ss;
// 					ss << link->typeSize;
// 					typePrefix.append(ss.str());
// 				}
// 				
// 				ss <<typePrefix << " " << link->m_uniqueID.m_string << " = " << insertString << ";\n";
// 				compilation += ss.str();
// 			}
// 		}
// 		else
// 		{
// 			//we are an Input Node
// 			if(link->partnerLinkInstance != nullptr)
// 			{
// 				CompileLink(link->partnerLinkInstance, compilation);
// 				link->m_linkName = link->partnerLinkInstance->m_linkName;
// 			}
// 
// 			if(link->parentLink->OpenGLName.compare("") != 0)
// 			{
// 				//so we have a openGL variable here USE IT
// 				compilation += link->parentLink->OpenGLName + " = " + link->partnerLinkInstance->m_linkName + ";\n";
// 			}
// 			else if(!link->parentLink->varyingName.empty())
// 			{
// 				//so we have a openGL variable here USE IT
// 				compilation += "v_" + link->parentLink->varyingName + " = " + link->partnerLinkInstance->m_linkName + ";\n";
// 			}
// 			else if(!link->parentLink->outName.empty())
// 			{
// 				//so we have a openGL variable here USE IT
// 				compilation += link->parentLink->outName + " = " + link->partnerLinkInstance->m_linkName + ";\n";
// 			}
// 		}
// 	}

	

	void ShaderInstance::linkSlots(const variableInfo& linkAInfo, const variableInfo& linkBInfo  )
	{
// 		std::shared_ptr<NodeLinkInstance> linkA, linkB;
// 
// 		//first find slot A and B
// 		for(auto it = m_nodeInstances.begin(); it != m_nodeInstances.end(); ++it)
// 		{
// 			std::shared_ptr<NodeLinkInstance> tempLink;
// 			tempLink = (it->second)->getLinkByID(linkAInfo.name);
// 			if(tempLink != nullptr)
// 			{
// 				linkA = tempLink;
// 				linkA->pType = linkAInfo.pType;
// 				linkA->typeSize = clamp<unsigned int>(linkAInfo.typeSize, 1, 4);
// 			}
// 
// 			tempLink = (it->second)->getLinkByID(linkBInfo.name);
// 			if(tempLink != nullptr)
// 			{
//  				linkB = tempLink;
//  				linkB->pType = linkBInfo.pType;
//  				linkB->typeSize = clamp<unsigned int>(linkBInfo.typeSize, 1, 4);
// 			}
// 
// 			if(linkA != nullptr && linkB != nullptr)
// 				break;
// 		}
// 
// 		linkA->partnerLinkInstance = linkB;
// 		linkB->partnerLinkInstance = linkA;
	}


	void ShaderInstance::SaveShaderInstance( const std::string& fileName )
	{
		pugi::xml_document doc;

		doc.append_child().set_name("Model_Header");

		pugi::xml_node shaderHeader = doc.child("Shader_WorkFile");
		shaderHeader.append_attribute("name").set_value(m_shaderName.c_str());



// 		unsigned int offset = 0;
// 		size_t vertTotalSize = 0;
// 		for(auto it = m_vertexAttributes.begin(); it != m_vertexAttributes.end(); ++it)
// 		{
// 			pugi::xml_node attributeNode = modelNode.append_child();
// 			attributeNode.set_name("Attribute");
// 			attributeNode.append_attribute("name").set_value(it->Name.c_str());
// 			attributeNode.append_attribute("numValues").set_value(it->NumValues);
// 			attributeNode.append_attribute("offsetOf").set_value(offset);
// 			offset += sizeof(float) * it->NumValues;
// 			vertTotalSize += sizeof(float) * it->NumValues;
// 		}
// 
// 		offset = 0;
// 		for(auto it = BatchDatas.begin(); it != BatchDatas.end(); ++it)
// 		{
// 			pugi::xml_node batchNode = modelNode.append_child();
// 			batchNode.set_name("Batch");
// 			batchNode.append_attribute("OffsetStart").set_value(offset);
// 			batchNode.append_attribute("numVerts").set_value(it->numVerts );
// 			batchNode.append_attribute("numIndices").set_value(it->numIndicies );
// 			offset += it->numIndicies * sizeof(unsigned int) + it->numVerts * vertTotalSize;
// 		}
// 
// 		modelNode.append_attribute("VertexSize").set_value(vertTotalSize);
// 
// 		doc.save_file(headerPath.c_str(), "", pugi::format_default);
// 
// 		return true;

		for(auto nodeIt = m_nodeInstances.begin(); nodeIt != m_nodeInstances.end(); ++nodeIt)
		{

			pugi::xml_node xmlNode = shaderHeader.append_child();
			xmlNode.set_name("Node");
			xmlNode.append_attribute("Name").set_value(nodeIt->second->getName().c_str());
			
			//save data bits

			//save links

			std::map< HashedString, std::shared_ptr< NodeLinkInstance > >* nodeLinks = nodeIt->second->getUINodeLinkInstances();

			for(auto linkIt = nodeLinks->begin(); linkIt != nodeLinks->end(); ++linkIt)
			{

			}

		}
	}
}