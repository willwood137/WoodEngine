#pragma once

#include <string>
#include <vector>

#include "..\Math\Vector2.hpp"
#include "..\Math\AttachedAABB2D.hpp"
#include "..\Math\ContainedAABB2D.hpp"
#include "..\Engine\EventSystem.hpp"
#include "..\Engine\Shader.hpp"
#include "UIWidget.hpp"
#include "pugixml.hpp"

namespace shadow
{
	struct WindowVertexData
	{
		Vector2f pos;
		Vector2f uv;
		WindowVertexData( const Vector2f& p, const Vector2f& tc ) : pos(p), uv(tc) {}
		WindowVertexData() {}
	};

	struct WindowRenderData
	{
		unsigned int startVBO;
		unsigned int numVerts;
		ContainedAABB2D relativeWindowBox;
		std::shared_ptr<Texture> texture;
		bool flip;
	};

	class UIWindow : public EventRecipient<UIWindow>
	{
	friend class UI;
	public:
		UIWindow(const std::string& name, EventSystem* eventsystem);
		virtual ~UIWindow() {}
		virtual UIWindow* parseWindow(pugi::xml_node& node);
		virtual void initialize( const AABB2D& parentBox );
		virtual float render( float z);
		virtual void update(const Vector2f& cursorPosition);
		bool checkMouseClick(const Vector2f& cursorPosition);
		void catchShow(NamedPropertyContainer& param);
		void catchHide(NamedPropertyContainer& param);


		void setParent(UIWindow* parent);
		void setName(const std::string& name);

	protected:
		void CreateRenderDataForElement(const std::string& textureFile, 
			const ContainedAABB2D& relativeWindowPosition,
			const AABB2D& positionBox,
			const AABB2D& textureBox,
			bool flip,
			std::vector<WindowVertexData>& verts );


		bool m_hidden;
		bool m_drawBorderOnEdges;

		UIWindow* m_pParentWindow;
		std::string m_name;
		
		std::vector<UIWindow*> m_childrenWindows;
		std::string m_borderBarTextureFile;
		std::string m_borderCornerTextureFile;
		std::string m_backgroundTextureFile;

		AttachedAABB2D m_collisionBox;
		float m_borderLength;
		std::vector<WindowRenderData> m_renderData;
		unsigned int m_idVBO;
		Shader m_shaderBorder;
		Vector2f m_cursorPosition;

		std::vector<UIWidget*> m_widgets;
	};

}