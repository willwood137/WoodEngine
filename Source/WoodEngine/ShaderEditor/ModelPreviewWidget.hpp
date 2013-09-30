#pragma once

#include "..\UI\UIWidget.hpp"

#include "..\Engine\Camera.hpp"
#include "..\Engine\Model.hpp"
//#include "..\Engine\MetaShader.hpp"
#include "..\Engine\Shader.hpp"


namespace woodman
{
	class ModelPreviewWidget : public UIWidget
	{
	public:
		ModelPreviewWidget( std::shared_ptr<UICanvas> ParentCanvas,
			std::shared_ptr<UIWidget> parentWidget,
			const std::string& name,
			HashedString uniqueID );


		virtual void render( std::shared_ptr<UIMouse> currentMouse);

		virtual void MouseDrag( std::shared_ptr<UIMouse> currentMouse);

		void loadBackgroundTexture( const std::string& texturePath );
		void loadModelFromFile(const std::string& modelFilePath);
		void updateShader( const std::string& shaderFilePath );


		bool m_focus;

	private:
		std::shared_ptr<Model> m_model;
		std::shared_ptr<Texture> m_backgroundTexture;
		std::shared_ptr<Shader> m_previewShader;
		std::shared_ptr<Camera> m_camera;

	};
}