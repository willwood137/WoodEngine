#pragma once

#include "..\UI\UIWidget.hpp"

#include "..\Engine\Camera.hpp"
#include "..\Engine\Model.hpp"
//#include "..\Engine\MetaShader.hpp"
#include "..\Engine\Shader.hpp"
#include "..\\Engine\\Clock.hpp"


namespace woodman
{
	class ModelPreviewWidget : public UIWidget
	{
	public:
		ModelPreviewWidget( UICanvas* ParentCanvas, UIWidget* parentWidget, const std::string& name, float RelativeLayer, const Vector2f& Coordinates);

		bool SetUpRenderTarget(Vector2i textureResolution);

		virtual void render( UIMouse* currentMouse, float ParentLayer );

		virtual void MouseDrag( UIMouse* currentMouse);

		void loadBackgroundTexture( const std::string& texturePath );
		void loadModelFromFile(const std::string& modelFilePath);
		void updateShader( const std::string& shaderFilePath );



	private:
		std::shared_ptr<Model> m_model;
		std::shared_ptr<Texture> m_backgroundTexture;
		std::shared_ptr<Shader> m_previewShader;
		std::shared_ptr<Shader> m_showTextureShader;
		std::shared_ptr<Camera> m_camera;

		Vector2i m_previewTextureSize;
		GLuint m_renderedTextureID;
		GLuint m_framebufferName;

		Clock m_clock;
	};
}