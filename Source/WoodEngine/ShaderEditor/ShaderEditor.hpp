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

	struct NodeCategory
	{
		std::string name;
		std::map<HashedString, std::shared_ptr<ShaderNode> > nodes;
	};

	class ShaderEditor : public UIController
	{
	public:
		ShaderEditor(EventSystem* eventsystem );
		
		virtual void Initialize();
		virtual void render();
		virtual void update(const Vector2f& MouseScreenPosition  );

		void LoadNodeDefinitionsFromFile(const std::string &filePath);
		void addNode();
		void CreateUINodeBoxFromNodeInstance( std::shared_ptr<NodeInstance> node);

		std::shared_ptr<ShaderNode> getNode( HashedString name );

		virtual void KeyDown(unsigned int key);
		void catchAddNode(NamedPropertyContainer& parameters);
		void catchCompile(NamedPropertyContainer& parameters);
		void catchPreview(NamedPropertyContainer& parameters);
		void catchKeyDown(NamedPropertyContainer& parameters);
		void catchSaveFile(NamedPropertyContainer& parameters);

		void SyncUIAndShaderInstance();


	private:

		//private functions
		void setCanvasSpace();

		std::map<HashedString, std::shared_ptr<NodeCategory> > m_nodeCategories;

		float m_vertexToFragmentRatio;
		float m_vertexToFramentRatioGoal;

		// shader Instances?
		ShaderInstance m_shaderInstance;
		std::shared_ptr<UICanvas> m_vertexCanvas;
		std::shared_ptr<UICanvas> m_fragmentCanvas;
		std::shared_ptr<UICanvas> m_dividerCanvas;
		std::shared_ptr<ModelPreviewWidget> m_previewWidget;

		bool m_previewMode;

	};
}