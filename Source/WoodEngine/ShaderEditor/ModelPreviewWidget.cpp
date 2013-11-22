#include "../stdafx.h"

#include "ModelPreviewWidget.hpp"
#include "..\\UI\\UIController.hpp"
#include "..\\UI\\UICanvas.hpp"

namespace woodman
{
	

	ModelPreviewWidget::ModelPreviewWidget( UIController* parentController, 
		std::weak_ptr<UICanvas> ParentCanvas, 
		std::weak_ptr<UIWidget> parentWidget, 
		HashedString uniqueID, 
		float RelativeLayer, 
		const Vector2f& relativeCoordinates, 
		const Vector2f& collisionSize )
		: UIWidget(parentController, ParentCanvas, parentWidget, uniqueID, RelativeLayer, relativeCoordinates, collisionSize),
		m_camera(new Camera(Vector3f(0.0f, 50.0f, -200.0f) ) ),
		m_clock(HashedString("PreviewClock"), .2, Clock::MasterClock())
	{

	}

	std::weak_ptr<ModelPreviewWidget> ModelPreviewWidget::CreateModelPreviewWidget( UIController* _ParentController, std::weak_ptr<UICanvas> _ParentCanvas, std::weak_ptr<UIWidget> _ParentWidget, const HashedString& _ID, float _RelativeLayer, const Vector2f& _RelativeCoordinates, const Vector2f& _CollisionSize )
	{
		assert(_ParentController != nullptr);
		std::shared_ptr<ModelPreviewWidget> newBox(new ModelPreviewWidget( _ParentController, _ParentCanvas, _ParentWidget, _ID, _RelativeLayer, _RelativeCoordinates, _CollisionSize) );
		_ParentController->RegisterUIWidget(std::dynamic_pointer_cast<UIWidget>(newBox) );
		newBox->Initialize();
		return std::weak_ptr<ModelPreviewWidget>(newBox);
	}

	void ModelPreviewWidget::loadBackgroundTexture( const std::string& texturePath )
	{
		m_backgroundTexture = Texture::CreateOrGetTexture(texturePath);
	}

	void ModelPreviewWidget::loadModelFromFile(const std::string& modelFilePath)
	{
		m_showTextureShader = Shader::CreateOrGetShader(ASSETS + "SHADERS\\UI\\PreviewWindow");
		m_model = std::shared_ptr<Model>(new Model( modelFilePath ) );
		m_model->loadData();
	}

	void ModelPreviewWidget::updateShader( const std::string& shaderFilePath )
	{

		//remove the old shader!
		if(m_previewShader != nullptr)
		{
			Shader::DestroyShader(m_previewShader);	
		}

		m_previewShader = Shader::CreateOrGetShader(shaderFilePath);
	}

	void drawOrigin(Camera* camera);

	void ModelPreviewWidget::render( std::shared_ptr<UIMouse> currentMouse )
	{
		
		if(isKeyDown(VK_OEM_PLUS))
		{
			m_clock.setTimeScale( m_clock.getTimeScale() + .01);
		}

		if(isKeyDown(VK_OEM_MINUS))
		{
			m_clock.setTimeScale( m_clock.getTimeScale() - .01);
		}

		glPushAttrib(GL_VIEWPORT_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferName);
		
		glViewport(0,0,m_previewTextureSize.x,m_previewTextureSize.y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawOrigin(m_camera.get());

		if(m_previewShader != nullptr && m_previewShader->IsLoaded() )
		{

			

			//first we get the MVP Matrix
			Matrix4f projectionMatrix, mvpMatrix, modelView;

			GLint viewport[4];
			GLdouble projection[16];

			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();
			gluPerspective( 45., 1.8, .1, 1000. );

			glGetDoublev( GL_PROJECTION_MATRIX, projection );
			glGetIntegerv( GL_VIEWPORT, viewport );

			for(size_t i = 0; i < 16; ++i)
			{
				projectionMatrix[i] = static_cast<float>(projection[i]);
			}
			Matrix4f modelMatrix, viewMatrix;

			ConstructScaleMatrix(modelMatrix, m_model->m_scale);


			m_camera->update(.016f);
			viewMatrix = m_camera->getMatrix();
			modelView = modelMatrix * viewMatrix;
			mvpMatrix = modelView * projectionMatrix;

 			glEnable(GL_DEPTH_TEST);
 			glCullFace(GL_CW);
			glFrontFace(GL_CCW);

			std::vector<BatchData>* batches(m_model->getBatchData());

			m_previewShader->load();

			m_previewShader->SetUniformMatrix(HASHED_STRING_u_viewMatrix, viewMatrix, 1, false );
			m_previewShader->SetUniformMatrix(HASHED_STRING_u_modelMatrix, modelMatrix, 1 );
			m_previewShader->SetUniformMatrix(HASHED_STRING_u_MVP, mvpMatrix, 1 );
			m_previewShader->SetUniformMatrix(HASHED_STRING_u_projection, projectionMatrix, 1 );
			m_previewShader->SetUniformMatrix(HASHED_STRING_u_modelView, modelView, 1 );

			float time = static_cast<float>(m_clock.getTimeSeconds());
			m_previewShader->SetUniformFloat(HASHED_STRING_u_time, time , 1 );

			Vector3f lightPos( cos(time) * 100, 100.0f, sin(time) * 100 );
			m_previewShader->SetUniformVector3(HASHED_STRING_u_light, lightPos, 1 );

			Texture::ApplyTexture(m_model->getMaterialTexture("Diffuse"));

			for(auto batchIt = batches->begin(); batchIt != batches->end(); ++batchIt)
			{

				glBindBuffer(GL_ARRAY_BUFFER, batchIt->idVBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batchIt->idIBO);

				m_previewShader->setUpAttrbituesFromModel(m_model);

				glDrawElements(GL_TRIANGLES, batchIt->numIndices, GL_UNSIGNED_INT, 0);

			}


			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);

		}
		glPopAttrib();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//now render the texture

		Vector2f LinkBoxMinScreen, LinkBoxMaxScreen, LinkBoxMin(getAbsoluteCoordinates()), LinkBoxMax(LinkBoxMin + getCollisionSize() );

		m_parentCanvas.lock()->mapPointToScreenSpace(LinkBoxMin, LinkBoxMinScreen);
		m_parentCanvas.lock()->mapPointToScreenSpace(LinkBoxMax, LinkBoxMaxScreen);

		Vector2f LinkBoxSizeScreen = LinkBoxMaxScreen - LinkBoxMinScreen;

		AABB2D screenSpaceBounds = m_parentCanvas.lock()->getScreenSpace();

		m_showTextureShader->load();
		glBindBuffer(GL_ARRAY_BUFFER, Shader::QuadBufferID);
		glDisable(GL_CULL_FACE);
		glUniform2f(m_showTextureShader->getUniformID(HASHED_STRING_u_position), LinkBoxMinScreen.x, LinkBoxMinScreen.y );
		glUniform2f(m_showTextureShader->getUniformID(HASHED_STRING_u_size), LinkBoxSizeScreen.x, LinkBoxSizeScreen.y);
		glUniform2f(m_showTextureShader->getUniformID(HASHED_STRING_u_screenMin), screenSpaceBounds.m_vMin.x, screenSpaceBounds.m_vMin.y  );
		glUniform2f(m_showTextureShader->getUniformID(HASHED_STRING_u_screenMax), screenSpaceBounds.m_vMax.x, screenSpaceBounds.m_vMax.y);
		glUniform2f(m_showTextureShader->getUniformID(HASHED_STRING_u_inverseScreenResolution), 1.0f / static_cast<float>(ScreenSize.x), 1.0f / static_cast<float>(ScreenSize.y) );
		glUniform1i(m_showTextureShader->getUniformID(HASHED_STRING_u_diffuse), 0 );
		glEnable( GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_renderedTextureID);

		m_showTextureShader->setAttribute(HASHED_STRING_in_position, 2, GL_FLOAT, sizeof(Vector2f), 0);

		glDrawArrays( GL_QUADS, 0, 4);

		m_showTextureShader->disableAttribute(HASHED_STRING_in_position);

		std::stringstream ss, ss2;
		ss << m_clock.getTimeSeconds();
		ss2 << "x" << m_clock.getTimeScale();
		Vector2f screenPos;
		m_parentCanvas.lock()->mapPointToScreenSpace(getAbsoluteCoordinates(), screenPos);

		Font::DrawTextToScreen(ss.str(), m_style->TitleTextFont, RGBA(1.0f, 0.0f, 0.0f, .8f), screenPos + Vector2f(10.0, 30.0f), 20.0f, ALIGNMENT_LEFT  );
		Font::DrawTextToScreen(ss2.str(), m_style->TitleTextFont, RGBA(1.0f, 0.0f, 0.0f, .8f), screenPos + Vector2f(10.0, 10.0f), 20.0f, ALIGNMENT_LEFT  );


	}

	void ModelPreviewWidget::MouseDrag( std::shared_ptr<UIMouse> currentMouse )
	{

	}

	bool ModelPreviewWidget::SetUpRenderTarget(Vector2i textureResolution)
	{
		// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
		m_previewTextureSize = textureResolution;
		m_framebufferName =0;
		GLuint depthrenderbuffer;
		glGenFramebuffers(1, &m_framebufferName);
		glGenRenderbuffers(1, &depthrenderbuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferName);

		// The texture we're going to render to
		glGenTextures(1, &m_renderedTextureID);

		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, m_renderedTextureID);

		// Give an empty image to OpenGL ( the last "0" )
		glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, textureResolution.x, textureResolution.y, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

		// Poor filtering. Needed !
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// The depth buffer
		glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, textureResolution.x, textureResolution.y);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_renderedTextureID, 0);

		// Set the list of draw buffers.
		GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	}

	

}