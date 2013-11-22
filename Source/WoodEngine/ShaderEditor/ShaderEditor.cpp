#include "..\stdafx.h"

#include "ShaderEditor.hpp"
#include "..\UI\UIController.hpp"
#include "..\Libs\pugixml.hpp"
#include "..\Math\AABB2D.hpp"
#include "..\UI\UINodeBox.hpp"
#include "..\UI\UITextEntry.hpp"
#include "..\Utility\Utility.hpp"



namespace woodman
{
	ShaderEditor::ShaderEditor(EventSystem* eventsystem )
		: UIController(eventsystem),
		m_vertexToFragmentRatio(1.0f),
		m_vertexToFramentRatioGoal(1.0f)
	{
		std::shared_ptr<UICanvas> vertexCanvas(  new UICanvas( eventsystem, AABB2D( Vector2f(0.0f, 0.0f ), Vector2f(1200.0f, 900.0f) ), (TEXTURES_PATH + "Hexagon_Background.png"), Vector2i(1024, 1024),  Vector2i(1, 15),	2000.0f	) );
		std::shared_ptr<UICanvas> fragmentCanvas(new UICanvas( eventsystem, AABB2D( Vector2f(400.0f, 0.0f ), Vector2f(1600.0f, 900.0f) ), (TEXTURES_PATH + "Frag_background.jpg"), Vector2i(700, 700), Vector2i(1, 15),	2000.0f	) );
		std::shared_ptr<UICanvas> dividerCanvas( new UICanvas( eventsystem, AABB2D( Vector2f(1200.0f, 0.0f ), Vector2f(1600.0f, 900.0f) ), (TEXTURES_PATH + "divider_background.jpg"), Vector2i(400, 900), Vector2i(10, 10),	1000.0f ) );

		m_canvases.push_back(vertexCanvas );
		m_canvases.push_back(fragmentCanvas );
		m_canvases.push_back(dividerCanvas );
		dividerCanvas->m_moveable = false;

		m_dividerCanvas = dividerCanvas;
		m_vertexCanvas = vertexCanvas;
		m_fragmentCanvas = fragmentCanvas;
	}

	//-------------------------------------------------------------------------------------------------------------------

	void ShaderEditor::Initialize(  )
	{
		UIController::Initialize();
		LoadNodeDefinitionsFromFile("ShaderEditor\\ShaderNodes\\Math.xml");
		LoadNodeDefinitionsFromFile("ShaderEditor\\ShaderNodes\\Core.xml");
		LoadNodeDefinitionsFromFile("ShaderEditor\\ShaderNodes\\Color.xml");
		LoadNodeDefinitionsFromFile("ShaderEditor\\ShaderNodes\\Utility.xml");
		LoadNodeDefinitionsFromFile("ShaderEditor\\ShaderNodes\\Constant.xml");
		LoadNodeDefinitionsFromFile("ShaderEditor\\ShaderNodes\\Vector.xml");

		//load default shader work file
		std::vector<std::shared_ptr<NodeInstanceData> > nodeInstanceData;

		m_shaderInstance.LoadShaderInstance(ASSETS + "ShaderWorkfiles\\" + "default.xml", nodeInstanceData);

		// create Nodes
		for(auto it = nodeInstanceData.begin(); it != nodeInstanceData.end(); ++it)
		{
			CreateNodeInstanceFromData( (*it).get() );
		}
		// Pair up nodeLinks
		for(auto it = nodeInstanceData.begin(); it != nodeInstanceData.end(); ++it)
		{
			for(auto linkIt = (*it)->LinkInfos.begin(); linkIt != (*it)->LinkInfos.end(); ++linkIt)
			{
				PairLinksFromData( (*linkIt).get(), HashedString( (*it)->Name) );
			}
		}

		p_eventSystem->RegisterObjectForEvent(this, &ShaderEditor::catchAddNode, "AddNode");
		p_eventSystem->RegisterObjectForEvent(this, &ShaderEditor::catchCompile, "Compile");
		p_eventSystem->RegisterObjectForEvent(this, &ShaderEditor::catchPreview, "Preview");
		p_eventSystem->RegisterObjectForEvent(this, &ShaderEditor::catchKeyDown, "KeyDown");
		p_eventSystem->RegisterObjectForEvent(this, &ShaderEditor::catchSaveFile, "SaveFile");
		p_eventSystem->RegisterObjectForEvent(this, &ShaderEditor::catchLoadFile, "LoadFile");


		//create Menu
 		m_mouse->MainMenu = std::unique_ptr<MouseMenu>(new MouseMenu());
 		m_mouse->MainMenu->name = "Main Menu";

 		std::unique_ptr<MouseMenu> addNodeMenu(new MouseMenu());
 		addNodeMenu->name = "Add Node";
 
 		m_mouse->MainMenu->subMenus.emplace_back( std::move(addNodeMenu) );
 		
 		for(auto it = m_nodeCategories.begin(); it != m_nodeCategories.end(); ++it)
 		{
 			std::unique_ptr<MouseMenu> catMenu(new MouseMenu());
 			catMenu->name = it->second->name;
 
 			for(auto it2 = it->second->nodes.begin(); it2 != it->second->nodes.end(); ++it2)
 			{
 				std::unique_ptr<MouseMenu> nodeMenu(new MouseMenu());
 				nodeMenu->name = it2->second->name;
 				nodeMenu->EventToFire = "AddNode";
 				nodeMenu->Parameters.insertNamedData(HashedString("Category"), it2->second->category);
 				nodeMenu->Parameters.insertNamedData(HashedString("Name"), it2->second->name);
 				catMenu->subMenus.emplace_back(std::move(nodeMenu) );
 			}
 
 			m_mouse->MainMenu->subMenus[0]->subMenus.emplace_back(std::move(catMenu) );
 		}
 
 		std::unique_ptr<MouseMenu> PreviewMenu(new MouseMenu());
 		PreviewMenu->name = "Preview";
 		PreviewMenu->EventToFire = "Preview";
 		m_mouse->MainMenu->subMenus.emplace_back(std::move(PreviewMenu));
 
 		std::unique_ptr<MouseMenu> CompileMenu(new MouseMenu());
 		CompileMenu->name = "Compile";
 		CompileMenu->EventToFire = "Compile";
 		m_mouse->MainMenu->subMenus.emplace_back(std::move(CompileMenu));
 
 		std::unique_ptr<MouseMenu> SaveMenu(new MouseMenu());
 		SaveMenu->name = "Save";
 		SaveMenu->EventToFire = "SaveFile";
 		m_mouse->MainMenu->subMenus.emplace_back(std::move(SaveMenu) );

		std::unique_ptr<MouseMenu> LoadMenu(new MouseMenu());
		LoadMenu->name = "Load";
		LoadMenu->EventToFire = "LoadFile";
		m_mouse->MainMenu->subMenus.emplace_back(std::move(LoadMenu) );
 
 		m_previewWidget = ModelPreviewWidget::CreateModelPreviewWidget(dynamic_cast<UIController*>(this), m_dividerCanvas, std::weak_ptr<UIWidget>(), HashedString("Previewer01"), 100, Vector2f(-200, 450.0-512.0), Vector2f(400.0, 512.0) );
 		m_previewWidget.lock()->SetUpRenderTarget(Vector2i(400, 512));
  		m_previewWidget.lock()->loadModelFromFile(ASSETS + "Models\\jax.xml");
		
		m_dividerCanvas.lock()->RegisterUIWidget(std::dynamic_pointer_cast<UIWidget>(m_previewWidget.lock()));
	}


	//-------------------------------------------------------------------------------------------------------------------

	void ShaderEditor::CreateNodeInstanceFromData(NodeInstanceData* data)
	{
		
		ShaderNode* tempDef(getDefinitionNode(HashedString(data->defName) ) );

		UICanvas* curCanvas(nullptr);


		NodeInstance* newNode = m_shaderInstance.CreateNewNodeInstance(tempDef, data->shaderType, data->Position, data->Name);
		CreateUINodeBoxFromNodeInstance(newNode);
	}

	//-------------------------------------------------------------------------------------------------------------------
	
	void ShaderEditor::PairLinksFromData(LinkInfo* info, HashedString NodeID)
	{

		std::weak_ptr<UINodeLink> inLink =  std::dynamic_pointer_cast<UINodeLink>(getUIWidgetByID( HashedString( info->Name ) ).lock() );

// 		NodeInstance* inCallBackNode = m_shaderInstance.getNodeInstanceByID(NodeID);
// 		NodeLinkInstance* inCallBack = inCallBackNode->getLinkByID(info->Name);
// 
// 		inLink->set

		std::weak_ptr<UINodeLink> outLink =  std::dynamic_pointer_cast<UINodeLink>(getUIWidgetByID( HashedString( info->PartnerLinkName ) ).lock() );

		UINodeLink::pair(outLink.lock(), inLink.lock());

	}

	//-------------------------------------------------------------------------------------------------------------------

	void ShaderEditor::update(const Vector2f& MouseScreenPosition )
	{
		UIController::update(MouseScreenPosition);
		

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
		float xDiff = ((1 - m_vertexToFragmentRatio) * -1200.0f);
		m_vertexCanvas.lock()->setScreenSpace(AABB2D(Vector2f(0.0f + xDiff , 0.0f), Vector2f(1200.0f + xDiff, 900.0f) ) );
		m_dividerCanvas.lock()->setScreenSpace(AABB2D(Vector2f(1200.0f + xDiff , 0.0f), Vector2f(1600.0f + xDiff, 900.0f) ) );
		m_fragmentCanvas.lock()->setScreenSpace(AABB2D(Vector2f(1600.0f + xDiff , 0.0f), Vector2f(3000.0f + xDiff, 900.0f) ) );



	}

	//-------------------------------------------------------------------------------------------------------------------

	void ShaderEditor::render()
	{

		if(m_vertexToFragmentRatio != 0.0f)
			m_vertexCanvas.lock()->RenderCanvas(m_mouse);
		if(m_vertexToFragmentRatio != 1.0f)
			m_fragmentCanvas.lock()->RenderCanvas(m_mouse);
		m_dividerCanvas.lock()->RenderCanvas(m_mouse);

		if(m_mouse->menuOpen)
		{
			m_mouse->render();
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
				std::unique_ptr<ShaderNode> currentDefinition(new ShaderNode());
				NodeCategory* currentCategory = new NodeCategory();

				//is this a definition?
				if(std::string(DefinitionNode.name()).compare(std::string("NodeDefinition") ) == 0 )
				{
					currentDefinition->name = DefinitionNode.attribute( "name" ).as_string();
					currentDefinition->category = DefinitionNode.attribute( "category" ).as_string();

					//lets check if this category exists
					auto it = m_nodeCategories.find(HashedString(currentDefinition->category) );
					if( it == m_nodeCategories.end() )
					{
						currentCategory->name = currentDefinition->category;
						m_nodeCategories.insert(std::make_pair(HashedString(currentDefinition->category), std::unique_ptr<NodeCategory>(currentCategory) ) );
					}
					else
					{
						currentCategory = ( it->second.get() );
					}

					//get all the data about it
					pugi::xml_node dataNode = DefinitionNode.first_child();
					while(dataNode)
					{
						if(std::string(dataNode.name()).compare(std::string("Function") ) == 0 )
						{
							FunctionDefinition def;

							def.functionCode = dataNode.first_child().value();

							std::string sType = dataNode.attribute("returnType").as_string();

							if(sType.compare("vector") == 0)
							{
								def.returnType.type = PROPERTYTYPE_FLOAT;
							}
							else if( sType.compare("float") == 0)
							{
								def.returnType.type = PROPERTYTYPE_FLOAT;
							}
							else if( sType.compare("matrix") == 0)
							{
								def.returnType.type = PROPERTYTYPE_MATRIX;
							}
							else if( sType.compare("sampler2D") == 0)
							{
								def.returnType.type = PROPERTYTYPE_SAMPLER2D;
							}

							def.returnType.maxSize = dataNode.attribute("returnSize").as_uint();
							def.returnType.minSize = def.returnType.maxSize;

							currentDefinition->functions.push_back(def);

						}
						if(std::string(dataNode.name()).compare(std::string("DataField") ) == 0 )
						{
							DataField field;
							field.name = dataNode.attribute("name").as_string();
							currentDefinition->dataFields.push_back(field);
						}
						else if(std::string(dataNode.name()).compare(std::string("Input") ) == 0 || std::string(dataNode.name()).compare(std::string("Output") ) == 0)
						{
							std::unique_ptr<NodeLink> newLink(new NodeLink() );

							
							newLink->name = dataNode.attribute("name").as_string();
							//newLink->type = PROPERTYTYPE_FLOAT;
						
							pugi::xml_node codeNode = dataNode.first_child();
							while(codeNode)
							{
								if(std::string(codeNode.name()).compare(std::string("dataType")) == 0)
								{
									DataType dType;
									
									std::string sType = codeNode.attribute("type").as_string();

									if(sType.compare("vector") == 0)
									{
										dType.type = PROPERTYTYPE_FLOAT;
									}
									else if( sType.compare("float") == 0)
									{
										dType.type = PROPERTYTYPE_FLOAT;
									}
									else if( sType.compare("matrix") == 0)
									{
										dType.type = PROPERTYTYPE_MATRIX;
									}
									else if( sType.compare("sampler2D") == 0)
									{
										dType.type = PROPERTYTYPE_SAMPLER2D;
									}

									//get sizes
									pugi::xml_attribute attrib = codeNode.first_attribute();

									bool sizeRangeFound = false;
									while(attrib)
									{

										if(std::string(attrib.name()).compare("minSize") == 0)
										{
											sizeRangeFound = true;
											dType.minSize = attrib.as_uint();
										}
										else if(std::string(attrib.name()).compare("maxSize") == 0)
										{
											sizeRangeFound = true;
											dType.maxSize = attrib.as_uint();
										}
										else if(std::string(attrib.name()).compare("size") == 0)
										{
											dType.currentSize = attrib.as_uint();	
										}
										else if(std::string(attrib.name()).compare("smartSize") == 0)
										{
											std::string linkName = attrib.as_string();
											//std::shared_ptr<NodeLink> refLink = currentDefinition->getLinkByName(linkName.substr(1));
										}
									
										attrib = attrib.next_attribute();
									}

									if(!sizeRangeFound)
									{
										dType.minSize = dType.currentSize;
										dType.maxSize = dType.currentSize;
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
								currentDefinition->inLinks.emplace_back(std::move(newLink) );
							else
								currentDefinition->outLinks.emplace_back(std::move(newLink) );
						}
						

						// go to next sibling
						dataNode = dataNode.next_sibling();
					}

				}

				currentCategory->nodes.insert(std::make_pair(HashedString(currentDefinition->name), std::move(currentDefinition) ));

				// go to next sibling
				DefinitionNode = DefinitionNode.next_sibling();
			}

			delete doc;
		}
	}


	//-------------------------------------------------------------------------------------------------------------------

	ShaderNode* ShaderEditor::getDefinitionNode( HashedString name )
	{
		for( auto it = m_nodeCategories.begin(); it != m_nodeCategories.end(); ++it)
		{
			auto it2 = it->second->nodes.find(name);
			if(it2 != it->second->nodes.end())
			{
				return (it2->second.get());
			}
		}

		return nullptr;
	}


	//-------------------------------------------------------------------------------------------------------------------

	void ShaderEditor::CreateUINodeBoxFromNodeInstance( NodeInstance* node )
	{
		std::weak_ptr<UICanvas> canvasToPutOn;

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
		
		

		std::shared_ptr<UINodeBox> NodeBox( UINodeBox::CreateUINodeBox( node->getName(), this, canvasToPutOn, std::weak_ptr<UIWidget>(), node->getUniqueID(), 100, node->getPosition(), Vector2f(20.0f, 20.0f) ).lock() );
 		NodeBox->setCallBackRecipient(node);
 
 		//get all the nodes
 		std::map< HashedString, std::unique_ptr< NodeLinkInstance > >* links = node->getUINodeLinkInstances();
 		//Calculate the size of the NodeBox
 		for(auto it = links->begin(); it != links->end(); ++it)
 		{
 			std::shared_ptr<UINodeLink> newSlot = UINodeLink::CreateUINodeLink( (it->second)->parentLink->name, (it->second)->exitNode, it->second.get(), this, canvasToPutOn, std::dynamic_pointer_cast<UIWidget>(NodeBox), (it->second)->m_uniqueID, 10.0f, Vector2f(0.0f, 0.0f), Vector2f(26.0f, 26.0f)).lock();
 			newSlot->setLockedToParent(true);
			newSlot->setDataType(&(it->second)->m_dType);
 			NodeBox->addLink(newSlot);
 		}
 
 		//add the data fields
 		std::map< HashedString, std::unique_ptr<DataFieldInstance> >* fields = node->getDataFields();
 		unsigned int n = 1;
 		for( auto it = fields->begin(); it != fields->end(); ++it )
 		{
 			std::shared_ptr<UITextEntry> text = UITextEntry::CreateUITextEntry((it->second)->m_name, p_eventSystem, this, m_dividerCanvas, std::weak_ptr<UIWidget>(), (it->second)->m_uniqueID, 20.0f, Vector2f(-200, 450.0 - 500 - 50 * n), Vector2f(180.0f, 30.0f) ).lock();
 			NodeBox->addDataField(text);
 			text->setCollisionOffset(Vector2f(40.0f, -6.0f));
			text->parentNodeBox = NodeBox.get();
 			m_dividerCanvas.lock()->RegisterUIWidget(text);
 			n++;
 		}
 		canvasToPutOn.lock()->RegisterUIWidget(NodeBox);

	}

	void ShaderEditor::SyncUIAndShaderInstance()
	{

	}


	//-------------------------------------------------------------------------------------------------------------------
	//--------Call Back Functions---------------------Call Back Functions-----------------Call Back Functions------------
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


		ShaderNode* tempDef(getDefinitionNode(HashedString(nodeName) ) );

		Vector2f canvasPos;
		std::weak_ptr<UICanvas> curCanvas;
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

		curCanvas.lock()->mapPointToCanvasSpace(m_mouse->prevRClickPosition, canvasPos);
	
		NodeInstance* newNode = m_shaderInstance.CreateNewNodeInstance(tempDef, shadertype, canvasPos);
		CreateUINodeBoxFromNodeInstance(newNode);
	}

	void ShaderEditor::catchCompile(NamedPropertyContainer& parameters)
	{
		//Link up the shader node instances
		SyncUIAndShaderInstance();
		m_shaderInstance.Compile();
	}

	void ShaderEditor::catchPreview(NamedPropertyContainer& parameters)
	{
		m_previewWidget.lock()->updateShader("ShaderTest");
	}

	void ShaderEditor::catchKeyDown(NamedPropertyContainer& parameters)
	{
		unsigned int key; 
		parameters.getNamedData(HashedString("Key"), key);

		if(key=='P')
		{
			m_previewWidget.lock()->updateShader("ShaderTest");
		}
	}

	void ShaderEditor::catchSaveFile(NamedPropertyContainer& parameters)
	{
		m_shaderInstance.SaveShaderInstance(ASSETS + "ShaderWorkfiles\\" + "temp.xml");
	}

	void ShaderEditor::catchLoadFile(NamedPropertyContainer& parameters)
	{
		std::vector<std::shared_ptr<NodeInstanceData> > nodeInstanceData;

		m_shaderInstance.LoadShaderInstance(ASSETS + "ShaderWorkfiles\\" + "temp.xml", nodeInstanceData);

		// create Nodes
		for(auto it = nodeInstanceData.begin(); it != nodeInstanceData.end(); ++it)
		{
			CreateNodeInstanceFromData( (*it).get() );
		}
		// Pair up nodeLinks
		for(auto it = nodeInstanceData.begin(); it != nodeInstanceData.end(); ++it)
		{
			for(auto linkIt = (*it)->LinkInfos.begin(); linkIt != (*it)->LinkInfos.end(); ++linkIt)
			{
				PairLinksFromData( (*linkIt).get(), HashedString( (*it)->Name) );
			}
		}

	}
}