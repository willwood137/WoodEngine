#ifndef UIWIDGET_HPP
#define UIWIDGET_HPP

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
	class UIController;
	struct UIStyle;
	struct UIMouse;

	struct UIWidgetParentInfo
	{
		UIWidgetParentInfo( const Vector2f& _Position, float _Layer) : Position(_Position), Layer(_Layer) {}
		Vector2f Position;
		float Layer;
	};


	class UIWidget
	{
	public:

		virtual ~UIWidget();
		virtual void Initialize();
		virtual void update( std::shared_ptr<UIMouse> currentMouse);
		virtual void render( std::shared_ptr<UIMouse> currentMouse );
		virtual void move(	const Vector2f& amountToMove );
		virtual void MouseClick( std::shared_ptr<UIMouse> currentMouse );
		virtual void MouseRClick( std::shared_ptr<UIMouse> currentMouse );
		virtual void MouseRelease( std::shared_ptr<UIMouse> currentMouse);
		virtual void MouseDrag( std::shared_ptr<UIMouse> currentMouse);
		virtual bool isPointInBounds(const Vector2f& point);
		virtual std::weak_ptr<UIWidget> getTopWidgetColliding( const Vector2f& PointCanvasSpace );


		void addChild( std::shared_ptr<UIWidget> child);
		void removeChild( std::shared_ptr<UIWidget> child);


		AABB2D calcFullCollisionBox();
		void calcAbsoluteData(const UIWidgetParentInfo& info);

		//////////////////////////////////////////////////////////////////////////
		//	Getters and setters
		//////////////////////////////////////////////////////////////////////////
		void setRelativeCoordinates( const Vector2f& RelativeCoordinates );
		Vector2f getRelativeCoordinates() const;

		void setCollisionSize( const Vector2f& size);
		Vector2f getCollisionSize() const;

		void setCollisionOffset( const Vector2f& offset);
		Vector2f getCollisionOffset() const;

		void setStyle( std::shared_ptr<UIStyle> style);
		std::shared_ptr<UIStyle> getStyle() const;

		void setLockedToParent( bool lockToParent );
		bool getLockedToParent() const;


		HashedString getUniqueID();

		void setLockKeyboard( bool lock );
		bool getLockKeyboard();

		std::weak_ptr<UICanvas> getParentCanvas();
		std::weak_ptr<UIWidget> getParentWidget();
		UIController* getParentController() const;

		float getAbsoluteLayer() const;
		Vector2f getAbsoluteCoordinates() const;
		AABB2D getAbsoluteCollisionBox() const;

	protected:

		UIWidget( UIController* parentController,
			std::weak_ptr<UICanvas> ParentCanvas,
			std::weak_ptr<UIWidget> parentWidget,
			HashedString uniqueID,
			float RelativeLayer,
			const Vector2f& relativeCoordinates,
			const Vector2f& collisionSize );

		
		

		std::set< std::shared_ptr<UIWidget> > m_children;  
		std::weak_ptr<UIWidget> m_parentWidget;
		std::weak_ptr<UICanvas> m_parentCanvas;
		UIController* m_parentController;

		std::shared_ptr<UIStyle> m_style;

		bool m_blockKeyboard;			//do we want to prevent other things from using the keyboard when we are selected
		bool m_blockedToParent;			//is it locked to its parent
		
	private:

		
		HashedString m_uniqueID;
		Vector2f m_relativeCoordinates;
		Vector2f m_canvasCollisionBoxSize;
		Vector2f m_canvasCollisionBoxOffset;
		float m_relativeLayer;
		float m_absoluteLayer;
		Vector2f m_absoluteCoordinates;
		AABB2D m_absoluteCanvasCollisionBox;
		AABB2D m_fullCanvasCollisionBox;				//This is the collision Box that encompasses all the children
		std::weak_ptr<UIWidget> m_selfPtr;


	};

	
}

#endif