#ifndef UICANVAS_HPP
#define UICANVAS_HPP

#include "UIStyle.hpp"
#include "UIWidget.hpp"


#include "../Math/Vector2.hpp"
#include "../Math/Vector4.hpp"
#include "../Math/AABB2D.hpp"

#include "../Engine/Shader.hpp"
#include "../Engine/Texture.hpp"
#include "../Engine/Font.hpp"
#include "../Engine/Shader.hpp"
#include "../Engine/EventSystem.hpp"


#include <array>


namespace woodman
{
	class UIWidget;
	
	class UICanvas : public EventRecipient<UICanvas>
	{
	public:

		UICanvas( EventSystem* eventsystem, const AABB2D& canvasScreenSpace, const std::string& backgroundTexturePath, const Vector2i& backgroundResolution, Vector2i zoomBounds, float Layer );

		void update( UIMouse* currentMouse );

		// from (-1, -1) to (1, 1)
		void setScreenSpace( const AABB2D& ScreenSpace );

		void RegisterUIWidget( UIWidget* widget );
		void UnRegisterUIWidget(  UIWidget* widget  );

		bool Initialize();

		void RenderCanvas( UIMouse* currentMouse);

		/* PresentCanvasToScreen
		*	
		*	Renders the current generated texture to the screen in a box topleft to bottomRight
		*	
		*	ScreenTopLeft / ScreenBottomRight - Box to be rendered, this is measured in resolution e.g. (800x600)
		*					so TopLeft being (0, 0) and Bottom Right being (800, 600) would fill up the whole screen.
		*
		*
		*/
		void PresentCanvasToScreen(
				Vector2i ScreenTopLeft,
				Vector2i ScreenBottomRight,
				RGBA color);

		//TODO
		// Allow to be rendered to a Quad in the world

		//void RenderToWorld();
		
		float getZoomScale();
		AABB2D getScreenSpace();
		float getLayer()
		{
			return m_layer;
		}

		UIWidget* getUIWidgetatPoint(const Vector2f& PointScreenSpace);
		UIWidget* getUIWidgetByID(HashedString ID);


		void mapPointToCanvasSpace(Vector2f pointToMap, Vector2f& pointInCanvasSpace);
		void mapPointToScreenSpace(Vector2f pointToMap, Vector2f& pointInScreenSpace);

		//Event Callback Functions
		void catchMouseWheel(NamedPropertyContainer& parameters);


		Vector2f getCurrentMouseCanvasPosition()
		{
			return m_currentMouseCanvasPosition;
		}

		Vector2f getPrevMouseCanvasPosition()
		{
			return m_prevMouseCanvasPosition;
		}

		bool m_moveable;
		
	private:

		
		void calcCanvasSpace();

		//EventSystem* p_eventSystem;

		
		/* zoom level  [1 - 20] */
		int m_zoom;
		Vector2i m_zoomBounds;

		/* This is the position of the canvas that should be rendered at the center of the quad rendered on */
		Vector2f m_center;

		/* this is the part of the screen we are given to render to */
		AABB2D m_screenSpace;

		/* what part of the canvas is fitting in the screen space given */
		AABB2D m_canvasSpace;

		/* the ID of the texture were rendering to */
		GLuint m_renderTarget;

		GLuint m_frameBuffer;

		Vector2i m_textureResolution;
		

		Vector2i m_backgroundResolution;
		std::string m_backgroundTexturePath;


		Vector2f m_currentMouseCanvasPosition;
		Vector2f m_prevMouseCanvasPosition;

		//UIWidgets
		std::set< std::unique_ptr<UIWidget> > m_UIWidgets;

		float m_layer;

		std::shared_ptr<Font> m_textFont;
		std::shared_ptr<Texture> m_backgroundTexture;
		std::shared_ptr<Shader> m_backgroundShader;
	};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	struct CanvasCoordinates
	{
		CanvasCoordinates(UICanvas* Owner, const Vector2f& canvasCoordinates)
			: OwningCanvas(Owner),
			coordinatesCanvasSpace(canvasCoordinates)
		{

		}

		Vector2f getScreenCoordinates()
		{
			Vector2f screenSpace;
			OwningCanvas->mapPointToScreenSpace(coordinatesCanvasSpace, screenSpace);
			return screenSpace;
		}

		void ChangeCanvas(UICanvas* newOwner)
		{
			OwningCanvas = newOwner;
		}

		UICanvas* getOwningCanvas()
		{
			return OwningCanvas;
		}

		Vector2f coordinatesCanvasSpace;

	private:
		UICanvas* OwningCanvas;
	};

}


#endif