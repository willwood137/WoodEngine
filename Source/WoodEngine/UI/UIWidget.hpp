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


	struct ParentInfo
	{
		float Layer;
		Vector2f Coordinates;
		AABB2D CanvasBounds;
	};

	class UICanvas;
	
	class UIWidget
	{
	public:
		UIWidget(UICanvas* ParentCanvas,
			UIWidget* parentWidget,
			const std::string& name,
			float RelativeLayer,
			const Vector2f& relativeCoordinates );

		virtual ~UIWidget();

		virtual void Initialize();
		
		virtual void update( UIMouse* currentMouse, const ParentInfo& parent_info);
		virtual void updateData(UIMouse* currentMouse, const ParentInfo& parent_info );
		virtual void updateAbsoluteData(UIMouse* currentMouse, const ParentInfo& parent_info);

		virtual void render( UIMouse* currentMouse, const AABB2D& CanvasBounds ) const;
		
		virtual void MouseEvent( UIMouse* currentMouse, MOUSE_EVENT_TYPE mouse_event, const ParentInfo& parent_info );

		virtual UIWidget* getTopWidgetColliding(const Vector2f& PointCanvasSpace, const ParentInfo& parent_info, float &HighestLayer);

		virtual void move(	const Vector2f& amountToMove );

		virtual void addChild( UIWidget* child);
		virtual void removeChild( UIWidget* child);


		
		/*
		* Getters and setters
		*/

		//Absolute Coordinates Info
		Vector2f getAbsoluteCoordinates() const;
		AABB2D getAbsoluteCollisionBox() const;
		float getAbsoluteLayer() const;


		//Relative Coordinates Info
		void setRelativeCoordinates(const Vector2f& relative_coordinates );
		Vector2f getRelativeCoordinates() const;

		void setRelativeLayer(float relative_layer);
		float getRelativeLayer() const;

		// CollisionBoxInfo
		void setSize(const Vector2f& size );
		Vector2f getSize() const;
		AABB2D getRelativeCollisionBox() const;

		// Style
		void setStyle( std::shared_ptr<UIStyle> style );
		std::shared_ptr<UIStyle> getStyle() const;


		//Name
		void setName(const std::string& name);
		std::string getName() const;

		//parent info
		UICanvas* getParentCanvas() const;
		UIWidget* getParentWidget() const;

		// end Getters/Setters

	protected:


		// Absolute Coordinates Info
		// Absolute info of a widget should be calculated and not set by other objects
		void setAbsoluteCoordinates( const Vector2f& coordinates );
		void setAbsoluteLayer(float layer);
		AABB2D calcFullCollisionBox();
		
	private:

		std::string m_name;

		Vector2f m_relativeCoordinates;
		float m_relativeLayer;

		Vector2f m_absoluteCoordinates;
		float m_absoluteLayer;

		//This is just the widget itself, not including any children
		Vector2f m_size;
		AABB2D m_absoluteCollisionBox;
		
		//This is the collision Box that encompasses all the children
		AABB2D m_fullCanvasCollisionBox;

		//Which Canvas This is On
		UICanvas* m_parentCanvas;

		// Who is my parent, null means it is the canvas
		UIWidget* m_parentWidget;

		//any children UIWidgets
		std::set<std::unique_ptr<UIWidget> > m_children;

		std::shared_ptr<UIStyle> m_style;
	};
}

#endif