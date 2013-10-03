#include "..\stdafx.h"

#include "ShaderEditor.hpp"
#include "..\Libs\pugixml.hpp"
#include "..\Math\AABB2D.hpp"
#include "..\UI\UINodeBox.hpp"
#include "..\Utility\Utility.hpp"

namespace woodman
{
	ShaderEditor::ShaderEditor(EventSystem* eventsystem )
		: UIController(eventsystem),
		m_vertexCanvas(  new UICanvas( eventsystem, AABB2D( Vector2f(0.0f, 0.0f ), Vector2f(1400.0f, 900.0f) ), (TEXTURES_PATH + "Hexagon_Background.png"), Vector2i(1024, 1024),  Vector2i(1, 15)		) ),
		m_fragmentCanvas(new UICanvas( eventsystem, AABB2D( Vector2f(200.0f, 0.0f ), Vector2f(1600.0f, 900.0f) ), (TEXTURES_PATH + "Frag_background.jpg"), Vector2i(700, 700), Vector2i(1, 15)			) ),
		m_dividerCanvas( new UICanvas( eventsystem, AABB2D( Vector2f(1400.0f, 0.0f ), Vector2f(1600.0f, 900.0f) ), (TEXTURES_PATH + "divider_background.jpg"), Vector2i(200, 900), Vector2i(10, 10)	) ),
		m_vertexToFragmentRatio(1.0f),
		m_vertexToFramentRatioGoal(1.0f),
		m_previewMode(false)
	{
		m_canvases.insert(m_vertexCanvas);
		m_canvases.insert(m_dividerCanvas);
		m_canvases.insert(m_fragmentCanvas);
	}

	//-------------------------------------------------------------------------------------------------------------------

	void ShaderEditor::Initialize(  )
	{
		UIController::Initialize();
		LoadNodeDefinitionsFromFile("ShaderEditor\\ShaderNodes\\Math.xml");
		LoadNodeDefinitionsFromFile("ShaderEditor\\ShaderNodes\\Core.xml");
		LoadNodeDefinitionsFromFile("ShaderEditor\\ShaderNodes\\Color.xml");
		p_eventSystem->RegisterObjectForEvent(this, &ShaderEditor::catchAddNode, "AddNode");
		p_eventSystem->RegisterObjectForEvent(this, &ShaderEditor::catchCompile, "Compile");
		p_eventSystem->RegisterObjectForEvent(this, &ShaderEditor::catchPreview, "Preview");
		p_eventSystem->RegisterObjectForEvent(this, &ShaderEditor::catchKeyDown, "KeyDown");


		//create Menu
		m_mouse->MainMenu = std::shared_ptr<MouseMenu>(new MouseMenu());
		m_mouse->MainMenu->name = "Main Menu";

		std::shared_ptr<MouseMenu> addNodeMenu(new MouseMenu());
		addNodeMenu->name = "Add Node";

		m_mouse->MainMenu->subMenus.push_back(addNodeMenu);
		
		for(auto it = m_nodeCategories.begin(); it != m_nodeCategories.end(); ++it)
		{
			std::shared_ptr<MouseMenu> catMenu(new MouseMenu());
			catMenu->name = it->second->name;

			for(auto it2 = it->second->nodes.begin(); it2 != it->second->nodes.end(); ++it2)
			{
				std::shared_ptr<MouseMenu> nodeMenu(new MouseMenu());
				nodeMenu->name = it2->second->name;
				nodeMenu->EventToFire = "AddNode";
				nodeMenu->Parameters.insertNamedData(HashedString("Category"), it2->second->category);
				nodeMenu->Parameters.insertNamedData(HashedString("Name"), it2->second->name);
				catMenu->subMenus.push_back(nodeMenu);
			}

			m_mouse->MainMenu->subMenus[0]->subMenus.push_back(catMenu);
		}

		std::shared_ptr<MouseMenu> PreviewMenu(new MouseMenu());
		PreviewMenu->name = "Preview";
		PreviewMenu->EventToFire = "Preview";
		m_mouse->MainMenu->subMenus.push_back(PreviewMenu);

		std::shared_ptr<MouseMenu> CompileMenu(new MouseMenu());
		CompileMenu->name = "Compile";
		CompileMenu->EventToFire = "Compile";
		m_mouse->MainMenu->subMenus.push_back(CompileMenu);


		


		m_previewWidget = std::shared_ptr<ModelPreviewWidget>(new ModelPreviewWidget(m_dividerCanvas, nullptr, "Previewer", HashedString("Previewer01") ) );

		m_previewWidget->loadModelFromFile(ASSETS + "Models\\jax.xml");
		
		UIWidget::RegisterUIWidget(m_previewWidget);
		m_dividerCanvas->RegisterUIWidget(m_previewWidget);
	}


	//-------------------------------------------------------------------------------------------------------------------

	void ShaderEditor::update(const Vector2f& MouseScreenPosition )
	{
		if(!m_previewMode)
			UIController::update(MouseScreenPosition);
		else
		{
			m_mouse->prevScreenPosition = m_mouse->screenPosition;
			m_mouse->screenPosition = MouseScreenPosition;

			m_mouse->hoveringWidget = getUIWidgetatPoint(MouseScreenPosition);

			if(m_mouse->isPressed)
			{
				if(m_mouse->selectedWidget != nullptr)
				{
					m_mouse->selectedWidget->MouseDrag(m_mouse);
				}
			}
		}

		setCanvasSpace();
	}

	//-------------------------------------------------------------------------------------------------------------------

	

	//-------------------------------------------------------------------------------------------------------------------

	void ShaderEditor::setCanvasSpace()
	{
		float speed = 2.5;

		//move canvases if necessary
		if( m_vertexToFragmentRatio != m_vertexToFramentRatioGoal)
		{
			if( m_vertexToFragmentRatio < m_vertexToFramentRatioGoal)
			{
				m_vertexToFragmentRatio += .016f * speed;
			}
			else
			{
				m_vertexToFragmentRatio -= .016f * speed;
			}

			m_vertexToFragmentRatio = clamp(m_vertexToFragmentRatio, 0.0f, 1.0f);
		}
		float xDiff = ((1 - m_vertexToFragmentRatio) * -1400.0f);
		m_vertexCanvas->setScreenSpace(AABB2D(Vector2f(0.0f + xDiff , 0.0f), Vector2f(1400.0f + xDiff, 900.0f) ) );
		m_dividerCanvas->setScreenSpace(AABB2D(Vector2f(1400.0f + xDiff , 0.0f), Vector2f(1600.0f + xDiff, 900.0f) ) );
		m_fragmentCanvas->setScreenSpace(AABB2D(Vector2f(1600.0f + xDiff , 0.0f), Vector2f(3000.0f + xDiff, 900.0f) ) );



	}

	//-------------------------------------------------------------------------------------------------------------------

	void ShaderEditor::render()
	{
		if(m_previewMode)
		{
			m_previewWidget->render(m_mouse);

			if(m_mouse->menuOpen)
			{
				m_mouse->render();
			}
		}
		else
		{
			UIController::render();
		}
	}


	//-------------------------------------------------------------------------------------------------------------------


	void ShaderEditor::LoadNodeDefinitionsFromFile( const std::string &filePath)
	{
		pugi::xml_document* doc = new pugi::xml_document();
		pugi::xml_parse_result result = doc->load_file( (filePath).c_str() );
		pugi::xml_node topNode = doc->first_child();

		if(std::string(topNode.name()).compare(std::string("ShaderEditor") ) == 0 )
		{
			pugi::xml_node DefinitionNode = topNode.first_child();
			while(DefinitionNode)
			{
				std::shared_ptr<ShaderNode> currentDefinition = std::make_shared<ShaderNode>(ShaderNode());
				std::shared_ptr<NodeCategory> currentCategory;

				//is this a definition?
				if(std::string(DefinitionNode.name()).compare(std::string("NodeDefinition") ) == 0 )
				{
					currentDefinition->name = DefinitionNode.attribute( "name" ).as_string();
					currentDefinition->category = DefinitionNode.attribute( "category" ).as_string();

					//lets check if this category exists
					auto it = m_nodeCategories.find(HashedString(currentDefinition->category) );
					if( it == m_nodeCategories.end() )
					{
						currentCategory = std::shared_ptr<NodeCategory>(new NodeCategory() );
						currentCategory->name = currentDefinition->category;
						m_nodeCategories.insert(std::make_pair(HashedString(currentDefinition->category), currentCategory) );
					}
					else
					{
						currentCategory = ( it->second );
					}

					//get all the data about it
					pugi::xml_node dataNode = DefinitionNode.first_child();
					while(dataNode)
					{
						if(std::string(dataNode.name()).compare(std::string("Function") ) == 0 )
						{
							FunctionDefinition def;

							def.functionCode = dataNode.first_child().value();

							def.dType = std::shared_ptr<DataType>(new DataType());

							std::string sType = dataNode.attribute("returnType").as_string();

							if(sType.compare("vector") == 0)
							{
								def.dType->type = PROPERTYTYPE_VECTOR;
							}
							else if( sType.compare("matrix") == 0)
							{
								def.dType->type = PROPERTYTYPE_MATRIX;
							}
							else if( sType.compare("sampler2D") == 0)
							{
								def.dType->type = PROPERTYTYPE_SAMPLER2D;
							}

							def.dType->maxSize = dataNode.attribute("returnSize").as_uint();
							def.dType->minSize = def.dType->maxSize;

							currentDefinition->functions.push_back(def);

						}
						else if(std::string(dataNode.name()).compare(std::string("Input") ) == 0 || std::string(dataNode.name()).compare(std::string("Output") ) == 0)
						{
							std::shared_ptr<NodeLink> newLink(new NodeLink);

							
							newLink->name = dataNode.attribute("name").as_string();
							//newLink->type = PROPERTYTYPE_FLOAT;
						
							pugi::xml_node codeNode = dataNode.first_child();
							while(codeNode)
							{
								if(std::string(codeNode.name()).compare(std::string("dataType")) == 0)
								{
									std::shared_ptr<DataType> dType(new DataType() );
									
									std::string sType = codeNode.attribute("type").as_string();

									if(sType.compare("vector") == 0)
									{
										dType->type = PROPERTYTYPE_VECTOR;
									}
									else if( sType.compare("matrix") == 0)
									{
										dType->type = PROPERTYTYPE_MATRIX;
									}
									else if( sType.compare("sampler2D") == 0)
									{
										dType->type = PROPERTYTYPE_SAMPLER2D;
									}

									//get sizes
									pugi::xml_attribute attrib = codeNode.first_attribute();

									while(attrib)
									{

										if(std::string(attrib.name()).compare("minSize") == 0)
										{
											dType->minSize = attrib.as_uint();
										}
										else if(std::string(attrib.name()).compare("maxSize") == 0)
										{
											dType->maxSize = attrib.as_uint();
										}
										else if(std::string(attrib.name()).compare("size") == 0)
										{
											dType->minSize = attrib.as_uint();
											dType->maxSize = dType->minSize;
										}
										else if(std::string(attrib.name()).compare("smartSize") == 0)
										{
											std::string linkName = attrib.as_string();
											std::shared_ptr<NodeLink> refLink = currentDefinition->getLinkByName(linkName.substr(1));
											if(refLink != nullptr)
												dType->smartSizeReferences.insert(refLink->typeData);
										}
									
										attrib = attrib.next_attribute();
									}

									newLink->typeData = dType;
								}

								else if( std::string(codeNode.name()).compare(std::string("Attribute")) == 0)
								{
									newLink->attributeName = codeNode.attribute("name").as_string();
								}
								else if( std::string(codeNode.name()).compare(std::string("Uniform")) == 0)
								{
									newLink->uniformName = codeNode.attribute("name").as_string();
								}
								else if( std::string(codeNode.name()).compare(std::string("GLVariable")) == 0)
								{
									newLink->OpenGLName = codeNode.attribute("name").as_string();
								}
								else if( std::string(codeNode.name()).compare(std::string("Out")) == 0)
								{
									newLink->outName = codeNode.attribute("name").as_string();
								}
								else if( std::string(codeNode.name()).compare(std::string("Varying")) == 0)
								{
									newLink->varyingName = codeNode.attribute("name").as_string();
								}
								else if( std::string(codeNode.name()).compare(std::string("Shader_Code")) == 0)
								{
									newLink->ShaderCode = codeNode.first_child().value();
								}

								codeNode = codeNode.next_sibling();
							}

							if(std::string(dataNode.name()).compare(std::string("Input") ) == 0)
								currentDefinition->inLinks.push_back(newLink);
							else
								currentDefinition->outLinks.push_back(newLink);
						}
						

						// go to next sibling
						dataNode = dataNode.next_sibling();
					}

				}

				currentCategory->nodes.insert(std::make_pair(HashedString(currentDefinition->name), currentDefinition));

				// go to next sibling
				DefinitionNode = DefinitionNode.next_sibling();
			}

			delete doc;
		}
	}


	//-------------------------------------------------------------------------------------------------------------------

	std::shared_ptr<ShaderNode> ShaderEditor::getNode( HashedString name )
	{
		for( auto it = m_nodeCategories.begin(); it != m_nodeCategories.end(); ++it)
		{
			auto it2 = it->second->nodes.find(name);
			if(it2 != it->second->nodes.end())
			{
				return (it2->second);
			}
		}

		return nullptr;
	}


	//-------------------------------------------------------------------------------------------------------------------

	void ShaderEditor::CreateUINodeBoxFromNodeInstance( std::shared_ptr<NodeInstance> node )
	{
		std::shared_ptr<UICanvas> canvasToPutOn;

		switch( node->getShaderType() )
		{
		case SHADER_TYPE_VERTEX:
		default:
			canvasToPutOn = m_vertexCanvas;
			break;
		case SHADER_TYPE_FRAGMENT:
			canvasToPutOn = m_fragmentCanvas;
			break;
		}
		
		

		std::shared_ptr<UIWidget> NodeBox(new UINodeBox(canvasToPutOn, nullptr, node->getName(), node->getUniqueID(), node->getPosition() ) );

		//get all the nodes
		std::map< HashedString, std::shared_ptr< NodeLinkInstance > >* links = node->getUINodeLinkInstances();


		std::shared_ptr<UIStyle> style = UIStyle::DefaultUIStyle;
		NodeBox->setStyle( style );


		float TitleWidth = Font::getLengthOfString( node->getName(), Font::CreateOrGetFont(HASHED_STRING_tahoma), style->TitleSize) + style->NodeBoxBorderLength * 2.0f + style->NodeBoxCornerSize;
		unsigned int numInSlots = 0;
		unsigned int numOutSlots = 0;
		float outLongest = 0.0f;
		float inLongest = 0.0f;
		
		//Calculate the size of the NodeBox
		for(auto it = links->begin(); it != links->end(); ++it)
		{
			Vector2f Offset(0.0f, 0.0f);

			//add in Title/title buffer
			Offset.y += style->TitleSize + style->TitleBuffer;

			float textlength = Font::getLengthOfString((it->second)->parentLink->name, Font::CreateOrGetFont(HASHED_STRING_tahoma), style->subTitleSize);
			if(!(it->second)->exitNode)
			{
				//this is an entrance node, so its on the left side
				numInSlots++;
				if(textlength > inLongest)
					inLongest = textlength;
			}
			else
			{
				if(textlength > outLongest)
					outLongest = textlength;
				numOutSlots++;
			}
		}
		float totalSubLength = outLongest + inLongest + style->NodeBoxBorderLength * 2.2f + 10.0f;
		
		Vector2f boxSize;
		boxSize.x = max(totalSubLength, TitleWidth);

		//get height
		boxSize.y = style->NodeBoxBorderLength * 2 + style->NodeBoxCornerSize + style->TitleSize + style->TitleBuffer + static_cast<float>(max( numInSlots, numOutSlots)) * (style->subTitleSize + style->subTitleBuffer);
		NodeBox->setCollisionSize(boxSize);

			//titleMax length

		numInSlots = 0;
		numOutSlots = 0;
		//Calculate the size of the NodeBox
		for(auto it = links->begin(); it != links->end(); ++it)
		{
			Vector2f Offset(0.0f, 0.0f);
			Offset.y += boxSize.y;
			Offset.y -= (style->NodeBoxBorderLength + style->TitleBuffer + style->TitleSize);
			if(!(it->second)->exitNode)
			{
				//this is an entrance node, so its on the left side

				//add in TL corner

				Offset.y -= style->NodeBoxCornerSize;

				//add subtile size for each slot
				Offset.y -= static_cast<float>(numInSlots) * ( style->subTitleSize + style->subTitleBuffer);
				numInSlots++;
			}
			else
			{
				Offset.x += boxSize.x;
				//this is an exit node
				Offset.y -= static_cast<float>(numOutSlots) * ( style->subTitleSize + style->subTitleBuffer);
				numOutSlots++;
			}

			
			std::shared_ptr<UIWidget> newSlot( new UINodeSlot(canvasToPutOn, NodeBox, (it->second)->parentLink->name, (it->second)->m_uniqueID, (node->getPosition() + Offset), (it->second)->exitNode, (it->second)->parentLink->typeData, it->second ) );
			newSlot->setStyle(UIStyle::DefaultUIStyle);
			newSlot->setCollisionSize(Vector2f(style->subTitleSize * 1.2f, style->subTitleSize*1.2f) );
			newSlot->setCollisionOffset(Vector2f(style->subTitleSize * -.6f, style->subTitleSize * -.6f) );
			newSlot->setLockedToParent(true);
			newSlot->setParentOffset(Offset);
			newSlot->Initialize();
			UIWidget::RegisterUIWidget(newSlot);
			NodeBox->addChild(newSlot);
		}
		

		
		NodeBox->calcFullCollisionBox();
		NodeBox->Initialize();
		UIWidget::RegisterUIWidget(NodeBox);
		canvasToPutOn->RegisterUIWidget(NodeBox);

	}

	//-------------------------------------------------------------------------------------------------------------------

	void ShaderEditor::KeyDown(unsigned int key)
	{	
		if(key == VK_SPACE)
		{
// 			add a node box
// 						std::shared_ptr<ShaderNode> tempDef(getNode(HashedString("DivisionMatical")));
// 			
// 						if(tempDef != nullptr)
// 						{
// 							std::shared_ptr<NodeInstance> newNode(m_shaderInstance.CreateNewNodeInstance(tempDef, SHADER_TYPE_VERTEX) );
// 							CreateUINodeBoxFromNodeInstance(newNode);
// 						}
		}
		else if(key == VK_LEFT)
		{
			m_vertexToFramentRatioGoal = 0.0f;
		}
		else if(key == VK_RIGHT)
		{
			m_vertexToFramentRatioGoal = 1.0f;
		}

		UIController::KeyDown(key);
	}


	void ShaderEditor::catchAddNode(NamedPropertyContainer& parameters)
	{
		std::string category, nodeName;

		parameters.getNamedData(HashedString("category"), category);
		parameters.getNamedData(HashedString("name"), nodeName);


		std::shared_ptr<ShaderNode> tempDef(getNode(HashedString(nodeName) ) );

		Vector2f canvasPos;
		std::shared_ptr<UICanvas> curCanvas;
		SHADER_TYPE shadertype;
		if(m_vertexToFramentRatioGoal == 1.0f )
		{
			shadertype = SHADER_TYPE_VERTEX;
			curCanvas = m_vertexCanvas;
		}
		else
		{
			shadertype = SHADER_TYPE_FRAGMENT;
			curCanvas = m_fragmentCanvas;
		}

		curCanvas->mapPointToCanvasSpace(m_mouse->prevRClickPosition, canvasPos);


		
		std::shared_ptr<NodeInstance> newNode(m_shaderInstance.CreateNewNodeInstance(tempDef, shadertype, canvasPos) );
		CreateUINodeBoxFromNodeInstance(newNode);
	}

	void ShaderEditor::catchCompile(NamedPropertyContainer& parameters)
	{
		//Link up the shader node instances
		SyncUIAndShaderInstance();
		m_shaderInstance.Compile();
	}

	void ShaderEditor::SyncUIAndShaderInstance()
	{
		for(auto it = UIWidget::WidgetDatabase.begin(); it != UIWidget::WidgetDatabase.end(); ++it)
		{
			std::shared_ptr<UINodeSlot> asNodeSlot = std::dynamic_pointer_cast<UINodeSlot>(it->second);

			if(asNodeSlot != nullptr)
			{
				

				//this is a nodeslot, if it has a partner then hook it up with the instance
				if(asNodeSlot->getPartnerSlot() != nullptr)
				{
 					variableInfo infoA, infoB;
// 					infoA.name = asNodeSlot->getUniqueID();
// 					infoA.typeSize = asNodeSlot->getTypeSize();
// 					infoA.pType = asNodeSlot->getDataType()->type;
// 
// 
// 					infoB.name = asNodeSlot->getPartnerSlot()->getUniqueID();
// 					infoB.typeSize = asNodeSlot->getPartnerSlot()->getTypeSize();
// 					infoB.pType = asNodeSlot->getPartnerSlot()->getDataType()->type;

					m_shaderInstance.linkSlots( infoA, infoB );
				}
			}
		}
	}


	void ShaderEditor::catchPreview(NamedPropertyContainer& parameters)
	{
		m_previewMode = !m_previewMode;
		m_previewWidget->m_focus = m_previewMode;

		m_previewWidget->updateShader("ShaderTest");
	}

	void ShaderEditor::catchKeyDown(NamedPropertyContainer& parameters)
	{

		unsigned int key; 
		parameters.getNamedData(HashedString("Key"), key);

		if(key=='P')
		{
			m_previewMode = !m_previewMode;
			m_previewWidget->m_focus = m_previewMode;

			m_previewWidget->updateShader("ShaderTest");
		}



	}
}