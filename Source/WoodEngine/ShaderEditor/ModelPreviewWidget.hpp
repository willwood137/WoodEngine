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
		
		static std::weak_ptr<ModelPreviewWidget> CreateModelPreviewWidget(UIController* _ParentController,
			std::weak_ptr<UICanvas> _ParentCanvas,
			std::weak_ptr<UIWidget> _ParentWidget,
			const HashedString& _ID,
			float _RelativeLayer,
			const Vector2f& _RelativeCoordinates,
			const Vector2f& _CollisionSize );

		bool SetUpRenderTarget(Vector2i textureResolution);

		virtual void render( std::shared_ptr<UIMouse> currentMouse );

		virtual void MouseDrag( std::shared_ptr<UIMouse> currentMouse);

		void loadBackgroundTexture( const std::string& texturePath );
		void loadModelFromFile(const std::string& modelFilePath);
		void updateShader( const std::string& shaderFilePath );



	private:

		ModelPreviewWidget( UIController* parentController,
			std::weak_ptr<UICanvas> ParentCanvas,
			std::weak_ptr<UIWidget> parentWidget,
			HashedString uniqueID,
			float RelativeLayer,
			const Vector2f& relativeCoordinates,
			const Vector2f& collisionSize );

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