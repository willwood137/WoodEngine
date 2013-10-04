#ifndef UIWIDGET_HPP
#define UIWIDGET_HPP

#include "UIStyle.hpp"
#include "UIMouse.hpp"

#include "pugixml.hpp"
#include "..\Math\AABB2D.hpp"
#include "..\Math\Vector2.hpp"
#include "..\Engine\Shader.hpp"

#include <string>
#include <memory>
#include <set>

namespace woodman
{
	enum MOUSE_INFO_RETURN_STATUS
	{
		MOUSE_INFO_RETURN_STATUS_OVERLAPPING_ME_ONLY,
		MOUSE_INFO_RETURN_STATUS_OVERLAPPING_CHILD,
		MOUSE_INFO_RETURN_STATUS_NOT_OVERLAPPING,
		MOUSE_INFO_RETURN_STATUS_SIZE,
	};

	class UICanvas;
	
	class UIWidget
	{
	public:
		UIWidget(UICanvas* ParentCanvas,
			UIWidget* parentWidget,
			const std::string& name,
			HashedString uniqueID);

		UIWidget(UICanvas* ParentCanvas,
			UIWidget* parentWidget,
			const std::string& name,
			HashedString uniqueID, 
			const Vector2f& canvasCoordinates);

		virtual ~UIWidget();

		virtual void Initialize();
		virtual void update( UIMouse* currentMouse);
		virtual void render( UIMouse* currentMouse);
		virtual void move(	const Vector2f& amountToMove );
		virtual void MouseClick( UIMouse* currentMouse );
		virtual void MouseRClick( UIMouse* currentMouse );
		virtual void MouseRelease( UIMouse* currentMouse);
		virtual void MouseDrag( UIMouse* currentMouse);
		virtual bool isPointInBounds(const Vector2f& point);
		virtual void getTopWidgetColliding(const Vector2f& PointCanvasSpace, UIWidget*& TopWidget);


		void addChild( UIWidget* child);
		void removeChild( UIWidget* child);
		AABB2D calcFullCollisionBox();

		
		//Getters and setters

		void setParentOffset( const Vector2f& offset)
		{
			m_parentCoordinatesOffset = offset;
		}
		void setCollisionSize( const Vector2f& size)
		{
			m_canvasCollisionBoxSize = size;
		}
		void setCollisionOffset( const Vector2f& offset)
		{
			m_canvasCollisionBoxOffset = offset;
		}

		void setStyle( std::shared_ptr<UIStyle> style)
		{
			m_style = style;
		}

		void setLockedToParent( bool lockToParent )
		{
			m_lockedToParent = lockToParent;
		}

		bool getLockedToParent()
		{
			return m_lockedToParent;
		}

		Vector2f getCanvasCoordinates() const
		{
			return m_coordinates;
		}

		void setCanvasCoordinates(const Vector2f& coords) 
		{
			m_coordinates = coords;
		}

		Vector2f getCollisionSize() const
		{
			return m_canvasCollisionBoxSize;
		}

		std::string getName() const
		{
			return m_name;
		}

		std::shared_ptr<UIStyle> getStyle()
		{
			return m_style;
		}

		HashedString getUniqueID()
		{
			return m_uniqueID;
		}

		float getLayer()
		{
			return m_layer;
		}

		UICanvas* getParentCanvas();
		UIWidget* getUIWidgetByID(HashedString ID);

	protected:
		std::string m_name;

		//this is how we get back to our data
		HashedString m_uniqueID;

		Vector2f m_coordinates;

		//This is just the widget itself, not includeing any children
		Vector2f m_canvasCollisionBoxOffset;
		Vector2f m_canvasCollisionBoxSize;
		AABB2D m_canvasCollisionBox;
		
		//This is the collision Box that encompases all the children
		AABB2D m_fullCanvasCollisionBox;

		//any children UIWidgets
		std::set<std::unique_ptr<UIWidget> > m_children;

		// to do with the parent
		UIWidget* m_parentWidget;
		Vector2f  m_parentCoordinatesOffset;

		UICanvas* m_parentCanvas;

		std::shared_ptr<UIStyle> m_style;

		bool m_lockedToParent;

		float m_layer;

		unsigned int m_id;
		

	};

	
}

#endif