#pragma once
#include <string>

namespace woodman
{
	struct HashedString
	{
		std::string m_string;
		unsigned int m_hash;

		HashedString(const std::string&	StringToHash ) : m_string(StringToHash), m_hash(0)
		{
			for each (char c in m_string)
			{
				m_hash *= 31;
				m_hash += tolower(c);
			}
		}

		bool operator<( const HashedString& rhs) const
		{
			if(m_hash != rhs.m_hash)
				return m_hash < rhs.m_hash;
			else
				return _stricmp(m_string.c_str(), rhs.m_string.c_str() ) < 0;
		}
	};

	//Hashed String Definitions
	//
	
	// event Names
	const HashedString HASHED_STRING_Console("Console");
	const HashedString HASHED_STRING_Scale("Scale");
	const HashedString HASHED_STRING_Name("Name");



	//uniforms
	const HashedString HASHED_STRING_u_backgroundColor("u_backgroundColor");
	const HashedString HASHED_STRING_u_lineColor("u_lineColor");
	const HashedString HASHED_STRING_u_boneTMs("u_boneTMs");
	const HashedString HASHED_STRING_u_BottomRight("u_BottomRight");
	const HashedString HASHED_STRING_u_cameraPosition("u_cameraPosition");
	const HashedString HASHED_STRING_u_color("u_color");
	const HashedString HASHED_STRING_u_cursorPosition("u_cursorPosition");
	const HashedString HASHED_STRING_u_debugFloat("u_debugFloat");
	const HashedString HASHED_STRING_u_debugInt("u_debugInt");
	const HashedString HASHED_STRING_u_diffuse("u_diffuse");
	const HashedString HASHED_STRING_u_emissiveMap("u_emissiveMap");
	const HashedString HASHED_STRING_u_hasEmissiveMap("u_hasEmissiveMap");
	const HashedString HASHED_STRING_u_hasNormalMap("u_hasNormalMap");
	const HashedString HASHED_STRING_u_hasSpecularMap("u_hasSpecularMap");
	const HashedString HASHED_STRING_u_height("u_height");
	const HashedString HASHED_STRING_u_instanced("u_instanced");
	const HashedString HASHED_STRING_u_instanceID("u_instanceID");
	const HashedString HASHED_STRING_u_intersection("u_intersection");
	const HashedString HASHED_STRING_u_isSelected("u_isSelected");
	const HashedString HASHED_STRING_u_light1("u_light1");
	const HashedString HASHED_STRING_u_maxGrid("u_maxGrid");
	const HashedString HASHED_STRING_u_middleLine("u_middleLine");
	const HashedString HASHED_STRING_u_minGrid("u_minGrid");
	const HashedString HASHED_STRING_u_modelTransform("u_modelTransform");
	const HashedString HASHED_STRING_u_MV("u_MV");
	const HashedString HASHED_STRING_u_MVP("u_MVP");
	const HashedString HASHED_STRING_u_normalMap("u_normalMap");
	const HashedString HASHED_STRING_u_offset("u_offset");
	const HashedString HASHED_STRING_u_position("u_position");
	const HashedString HASHED_STRING_u_ProjectionMatrix("u_ProjectionMatrix");
	const HashedString HASHED_STRING_u_rotate("u_rotate");
	const HashedString HASHED_STRING_u_size("u_size");
	const HashedString HASHED_STRING_u_specularMap("u_specularMap");
	const HashedString HASHED_STRING_u_time("u_time");
	const HashedString HASHED_STRING_u_TopLeft("u_TopLeft");
	const HashedString HASHED_STRING_u_viewProjectionMatrix("u_viewProjectionMatrix");
	const HashedString HASHED_STRING_u_windowPosition("u_windowPosition");
	const HashedString HASHED_STRING_u_worldTransform("u_worldTransform");
	const HashedString HASHED_STRING_u_cornerSize("u_cornerSize");
	const HashedString HASHED_STRING_u_borderFilter("u_borderFilter");
	const HashedString HASHED_STRING_u_filter("u_filter");
	const HashedString HASHED_STRING_u_borderSize("u_borderSize");
	const HashedString HASHED_STRING_u_inverseScreenResolution("u_inverseScreenResolution");
	const HashedString HASHED_STRING_u_backgroundTexture("u_backgroundTexture");
	const HashedString HASHED_STRING_u_backgroundResolution("u_backgroundResolution");
	const HashedString HASHED_STRING_u_canvasZoom("u_canvasZoom");
	const HashedString HASHED_STRING_u_canvasCenter("u_canvasCenter");
	const HashedString HASHED_STRING_u_mousePos("u_mousePos");
	const HashedString HASHED_STRING_u_canvasMin("u_canvasMin");
	const HashedString HASHED_STRING_u_canvasMax("u_canvasMax");
	const HashedString HASHED_STRING_u_screenMin("u_screenMin");
	const HashedString HASHED_STRING_u_screenMax("u_screenMax");
	const HashedString HASHED_STRING_u_lineSegmentsStrip("u_lineSegmentsStrip");
	const HashedString HASHED_STRING_u_lineLengths("u_lineLengths");
	const HashedString HASHED_STRING_u_fullLength("u_fullLength");
	const HashedString HASHED_STRING_u_startSize("u_startSize");
	const HashedString HASHED_STRING_u_endSize("u_endSize");
	const HashedString HASHED_STRING_u_numLines("u_numLines");
	const HashedString HASHED_STRING_u_lineSize("u_lineSize");
	const HashedString HASHED_STRING_u_lineWidth("u_lineWidth");
	const HashedString HASHED_STRING_u_boxHeight("u_boxHeight");
	

	//attributes
	const HashedString HASHED_STRING_a_biTangent("a_biTangent");
	const HashedString HASHED_STRING_a_boneIDs("a_boneIDs");
	const HashedString HASHED_STRING_a_boneWeights("a_boneWeights");
	const HashedString HASHED_STRING_a_color("a_color");
	const HashedString HASHED_STRING_a_instanceID("a_instanceID");
	const HashedString HASHED_STRING_a_position("a_position");
	const HashedString HASHED_STRING_a_tangent("a_tangent");
	const HashedString HASHED_STRING_a_texCoords("a_texCoords");

	const HashedString HASHED_STRING_in_position("in_position");

	//textures

	//Fonts
	const HashedString HASHED_STRING_MainFont_EN("MainFont_EN");
	const HashedString HASHED_STRING_tahoma("tahoma");

	//Events 

	//Console commands
	const HashedString HASHED_STRING_help("help");


}