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
		UIWidget(std::shared_ptr<UICanvas> ParentCanvas,
			std::shared_ptr<UIWidget> parentWidget,
			const std::string& name,
			HashedString uniqueID);

		UIWidget(std::shared_ptr<UICanvas> ParentCanvas,
			std::shared_ptr<UIWidget> parentWidget,
			const std::string& name,
			HashedString uniqueID, 
			const Vector2f& canvasCoordinates);

		virtual ~UIWidget();

		virtual void Initialize();
		virtual void update( std::shared_ptr<UIMouse> currentMouse);
		virtual void render( std::shared_ptr<UIMouse> currentMouse);
		virtual void move(	const Vector2f& amountToMove );
		virtual void MouseClick( std::shared_ptr<UIMouse> currentMouse );
		virtual void MouseRClick( std::shared_ptr<UIMouse> currentMouse );
		virtual void MouseRelease( std::shared_ptr<UIMouse> currentMouse);
		virtual void MouseDrag( std::shared_ptr<UIMouse> currentMouse);
		virtual bool isPointInBounds(const Vector2f& point);
		virtual void getTopWidgetColliding(const Vector2f& PointCanvasSpace, std::shared_ptr<UIWidget>& TopWidget);


		void addChild(std::shared_ptr<UIWidget> child);
		void removeChild(std::shared_ptr<UIWidget> child);
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


		//Static Methods
		static unsigned int RegisterUIWidget(std::shared_ptr<UIWidget> Widget);
		static void UnRegisterUIWidget(std::shared_ptr<UIWidget> Widget);
		static std::map<unsigned int, std::shared_ptr<UIWidget> > WidgetDatabase;

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
		std::set<std::shared_ptr<UIWidget> > m_children;

		// to do with the parent
		std::shared_ptr<UIWidget> m_parentWidget;
		Vector2f				  m_parentCoordinatesOffset;

		std::shared_ptr<UICanvas> m_parentCanvas;

		std::shared_ptr<UIStyle> m_style;

		bool m_lockedToParent;

		float m_layer;

		unsigned int m_id;
		

	};

	
}

#endif