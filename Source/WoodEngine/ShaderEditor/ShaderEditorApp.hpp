#pragma once

#include "ShaderEditor.hpp"

#include "..\Engine\OpenGLApp.hpp"


namespace woodman
{
	class ShaderEditorApp : public OpenGLApp
	{
	public:

		ShaderEditorApp( std::shared_ptr<HINSTANCE> hInstance, int nCmdShow, Vector2i size );
		~ShaderEditorApp();



	private:
		std::unique_ptr<ShaderEditor> m_shaderEditor;


		virtual void updateFrame();
		virtual void render();
	};
}