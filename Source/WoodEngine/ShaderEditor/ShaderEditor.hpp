#pragma once

#include "ShaderNode.hpp"
#include "ShaderInstance.hpp"
#include "NodeInstance.hpp"
#include "ModelPreviewWidget.hpp"

#include "..\engine\HashedString.hpp"

#include "..\UI\UIController.hpp"

#include <map>
#include <memory>



namespace woodman
{
	class ShaderEditor : public UIController
	{
	public:
		ShaderEditor(EventSystem* eventsystem );
		
		virtual void Initialize();
		virtual void render();
		virtual void update(const Vector2f& MouseScreenPosition  );

		void LoadNodeDefinitionsFromFile(const std::string &filePath);
		void addNode();
		void CreateUINodeBoxFromNodeInstance( NodeInstance* node);
		void CreateNodeInstanceFromData(NodeInstanceData* data);
		void PairLinksFromData(LinkInfo* info, HashedString NodeID);

		ShaderNode* getDefinitionNode( HashedString name );

		virtual void KeyDown(unsigned int key);
		void DeleteNodeBox( UINodeBox* NodeBoxToDelete );
		void catchAddNode(NamedPropertyContainer& parameters);
		void catchCompile(NamedPropertyContainer& parameters);
		void catchPreview(NamedPropertyContainer& parameters);
		void catchKeyDown(NamedPropertyContainer& parameters);
		void catchSaveFile(NamedPropertyContainer& parameters);
		void catchLoadFile(NamedPropertyContainer& parameters);

		void SyncUIAndShaderInstance();


	private:

		//private functions
		void setCanvasSpace();

		std::map<HashedString, std::unique_ptr<NodeCategory> > m_nodeCategories;

		float m_vertexToFragmentRatio;
		float m_vertexToFramentRatioGoal;

		// shader Instances?
		ShaderInstance m_shaderInstance;
		std::weak_ptr<UICanvas> m_vertexCanvas;
		std::weak_ptr<UICanvas> m_fragmentCanvas;
		std::weak_ptr<UICanvas> m_dividerCanvas;
		std::weak_ptr<ModelPreviewWidget> m_previewWidget;


	};
}