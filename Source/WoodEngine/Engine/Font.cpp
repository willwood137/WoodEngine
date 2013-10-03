#pragma once

#include "../stdafx.h"

#include "Font.hpp"
#include "..\libs\pugixml.hpp"
#include "..\Math\Matrix4.hpp"
#include "OpenGLApp.hpp"

#include <string>
#include <set>
#include <sstream>
#include <vector>
#include <fstream>



namespace woodman
{

	const std::string TEXT_SHADER_SOURCE_FILE_PATH = ASSETS + "Shaders\\Text";
	const std::string SDF_FONT_SHADER_SOURCE_FILE_PATH = ASSETS + "Shaders\\SDFFont";

	struct sdf_glyph
	{
		int ID;
		int width, height;
		int x, y;
		float xoff, yoff;
		float xadv;
	};

	Shader Font::s_Shader, Font::s_SDFShader;
	std::map<HashedString, std::shared_ptr<Font> > Font::Fonts;
	std::string Font::s_FontDirectory(ASSETS + "Fonts\\");

	Font::Font( const HashedString& fontName ) : m_name( fontName ) 
	{
		LoadFromFile( fontName );
	}

	Font::~Font()
	{

	}

	bool Font::IsLoaded() const
	{
		// TODO
		//
		return true;
	}

	std::string& Font::GetDirectory()
	{
		return s_FontDirectory;
	}

	bool Font::_LoadFromSDFFile( const HashedString& fontName )
	{
		m_isSDFFont = true;

		std::ifstream myFile(s_FontDirectory + fontName.m_string + ".sfd", std::ios::binary);

		if(myFile.good())
		{
			int numGlyphs;
			float spacing = .1f;
			int TextureSize;
			int scale_factor;
			float scale_factorF, TextureSizeF;

			myFile.read((char*)&numGlyphs, sizeof(int));
			myFile.read((char*)&TextureSize, sizeof(int));
			myFile.read((char*)&scale_factor, sizeof(int));

			scale_factorF = static_cast<float>(scale_factor);
			TextureSizeF = static_cast<float>(TextureSize);


			scale_factorF = 100.0f;
			std::vector<sdf_glyph> SDFglyphs;

			SDFglyphs.resize(numGlyphs);

			myFile.read( (char*)SDFglyphs.data(), sizeof(sdf_glyph) * (numGlyphs) );
			myFile.close();

			m_characterInfos.resize(255-31);
			std::vector<GlyphVert> verts;
			verts.resize((255-31) * 4);

			m_cellPixelHeight = 72;

			unsigned int n = 0;
			for(auto it = SDFglyphs.begin(); it != SDFglyphs.end(); ++it, ++n)
			{
				FontCharacterInfo glyph;

				glyph.ucsIndex = it->ID;

				glyph.textureMins.x = it->x / TextureSizeF;
				glyph.textureMins.y = it->y / TextureSizeF;
				glyph.textureMaxs.x = glyph.textureMins.x + it->width / TextureSizeF;
				glyph.textureMaxs.y = glyph.textureMins.y + it->height / TextureSizeF;

				glyph.ttfA = it->xoff / scale_factorF;
				glyph.ttfB = it->xadv / scale_factorF;
				glyph.ttfC = spacing;

				glyph.yOffset =it->yoff / scale_factorF;
				//glyph.yOffset = 0.0f;

				if(glyph.ucsIndex > 48 && glyph.ucsIndex <= 57)
				{
					glyph.ttfA = m_characterInfos[48-32].ttfA;
					glyph.ttfB = m_characterInfos[48-32].ttfB;
					glyph.ttfC = m_characterInfos[48-32].ttfC;
					glyph.yOffset = m_characterInfos[48-32].yOffset;
				}

				m_characterInfos[glyph.ucsIndex - 32] = glyph;

				float height = it->height / scale_factorF;
				verts[ (glyph.ucsIndex - 32) * 4 ]     = (GlyphVert(Vector2f(0.0f, 0.0f), Vector2f( glyph.textureMins.x, glyph.textureMins.y) ) );
				verts[ (glyph.ucsIndex - 32) * 4 + 1] = (GlyphVert(Vector2f(0.0f, -height), Vector2f( glyph.textureMins.x, glyph.textureMaxs.y) ) );
				verts[ (glyph.ucsIndex - 32) * 4 + 2] = (GlyphVert(Vector2f(glyph.ttfB, -height), Vector2f( glyph.textureMaxs.x, glyph.textureMaxs.y) ) );
				verts[ (glyph.ucsIndex - 32) * 4 + 3] = (GlyphVert(Vector2f(glyph.ttfB, 0.0f), Vector2f( glyph.textureMaxs.x, glyph.textureMins.y) ) );

// 				verts[ (glyph.ucsIndex - 32) * 4 ]     = (GlyphVert(Vector2f(0.0f,-height), Vector2f( glyph.textureMins.x, glyph.textureMins.y) ) );
// 				verts[ (glyph.ucsIndex - 32) * 4 + 1] = (GlyphVert(Vector2f(0.0f, 0.0f), Vector2f( glyph.textureMins.x, glyph.textureMaxs.y) ) );
// 				verts[ (glyph.ucsIndex - 32) * 4 + 2] = (GlyphVert(Vector2f(glyph.ttfB, -height), Vector2f( glyph.textureMaxs.x, glyph.textureMins.y) ) );
// 				verts[ (glyph.ucsIndex - 32) * 4 + 3] = (GlyphVert(Vector2f(glyph.ttfB, 0.0f), Vector2f( glyph.textureMaxs.x, glyph.textureMaxs.y) ) );




			}


			m_idVBO = LoadBufferObject(GL_ARRAY_BUFFER, verts.data(), sizeof(GlyphVert) * verts.size());

			Texture::TextureFormat tf;

			tf.MagFilter = GL_LINEAR;
			tf.MinFilter = GL_LINEAR_MIPMAP_LINEAR;
			tf.MipMapTexture = true;

			m_fontTexture = Texture::CreateOrGetTexture(s_FontDirectory + fontName.m_string + ".png", tf);


			return true;
		}


		return false;
	}

	bool Font::LoadFromFile( const HashedString& fontName )
	{
		pugi::xml_document* doc = new pugi::xml_document();
		pugi::xml_parse_result result = doc->load_file( (s_FontDirectory + fontName.m_string + ".xml").c_str() );

		if(result.status == pugi::status_file_not_found)
		{
			return _LoadFromSDFFile( fontName );
		}
		else
		{
			m_isSDFFont = false;
			pugi::xml_node topNode = doc->first_child();

			m_cellPixelHeight = topNode.child("FontInfo").attribute("cellPixelHeight").as_int();


			m_characterInfos.resize(255-31);
			std::vector<GlyphVert> verts;
			verts.resize((255-31) * 4);
			if(std::string(topNode.name()).compare(std::string("FontDefinition") ) == 0 )
			{
				pugi::xml_node glyphNode = topNode.first_child();
				while(glyphNode)
				{
					std::string s(glyphNode.name());
					if( std::string(glyphNode.name()).compare(std::string("Glyph") ) == 0 )
					{
						FontCharacterInfo glyph;
						pugi::xml_attribute topAttributes = glyphNode.first_attribute();
						while(topAttributes)
						{
							if(std::string(topAttributes.name()).compare("ucsIndex") == 0)
							{
								glyph.ucsIndex = topAttributes.as_int();

								if(m_characterInfos.size() < (size_t)(glyph.ucsIndex - 31))
									m_characterInfos.resize(glyph.ucsIndex - 31);
							}
							else if(std::string(topAttributes.name()).compare("texCoordMins") == 0)
							{
								std::stringstream ss;
								std::string s = topAttributes.value();
								int firstComma = s.find(',');
								ss << s.substr(1, firstComma - 1);
								float x;
								ss >> x;
								ss.clear();
								ss << s.substr(firstComma + 1, s.length() - firstComma - 2 );
								float y;
								ss >> y;
								glyph.textureMins = Vector2f(x, y);
							}
							else if(std::string(topAttributes.name()).compare("texCoordMaxs") == 0)
							{
								std::stringstream ss;
								std::string s = topAttributes.value();
								int firstComma = s.find(',');
								ss << s.substr(1, firstComma - 1);
								float x;
								ss >> x;

								ss.clear();
								ss << s.substr(firstComma + 1, s.length() - firstComma - 2 );
								float y;
								ss >> y;
								glyph.textureMaxs = Vector2f(x, y);
							}
							else if(std::string(topAttributes.name()).compare("ttfA") == 0)
							{
								glyph.ttfA = topAttributes.as_float();
							}
							else if(std::string(topAttributes.name()).compare("ttfB") == 0)
							{
								glyph.ttfB = topAttributes.as_float();
							}
							else if(std::string(topAttributes.name()).compare("ttfC") == 0)
							{
								glyph.ttfC = topAttributes.as_float();
							}

							topAttributes = topAttributes.next_attribute();
						}


						glyph.yOffset = 0.0f;

						verts[ (glyph.ucsIndex - 32) * 4 ]     = (GlyphVert(Vector2f(0.0f, 0.0f), Vector2f( glyph.textureMins.x, glyph.textureMaxs.y) ) );
						verts[ (glyph.ucsIndex - 32) * 4 + 1] = (GlyphVert(Vector2f(0.0f, 1.0f), Vector2f( glyph.textureMins.x, glyph.textureMins.y) ) );
						verts[ (glyph.ucsIndex - 32) * 4 + 2] = (GlyphVert(Vector2f(glyph.ttfB, 0.0f), Vector2f( glyph.textureMaxs.x, glyph.textureMaxs.y) ) );
						verts[ (glyph.ucsIndex - 32) * 4 + 3] = (GlyphVert(Vector2f(glyph.ttfB, 1.0f), Vector2f( glyph.textureMaxs.x, glyph.textureMins.y) ) );

						if(glyph.ucsIndex > 48 && glyph.ucsIndex <= 57)
						{
							glyph.ttfA = m_characterInfos[48-32].ttfA;
							glyph.ttfB = m_characterInfos[48-32].ttfB;
							glyph.ttfC = m_characterInfos[48-32].ttfC;
						}

						m_characterInfos[glyph.ucsIndex - 32] = glyph;


					}
					glyphNode = glyphNode.next_sibling();
				}

			}

			m_idVBO = LoadBufferObject(GL_ARRAY_BUFFER, verts.data(), sizeof(GlyphVert) * verts.size());

			Texture::TextureFormat tf;

			tf.MagFilter = GL_LINEAR;
			tf.MinFilter = GL_LINEAR_MIPMAP_LINEAR;
			tf.MipMapTexture = true;

			m_fontTexture = Texture::CreateOrGetTexture(s_FontDirectory + fontName.m_string + ".png", tf);

			return true;
		}
	}


	//
	// Static Member Functions ////////////////////////////////////////////
	//
	void Font::LoadShader()
	{
		s_Shader = Shader(TEXT_SHADER_SOURCE_FILE_PATH);
		s_SDFShader = Shader(SDF_FONT_SHADER_SOURCE_FILE_PATH);


	}

	float Font::getLengthOfString( const std::string& textToDraw,
		std::shared_ptr<Font>& pFont,
		float size,
		float aspect)
	{
		if(textToDraw.empty())
			return .0f;

		size_t slength = textToDraw.size();
		float x = 0;
		FontCharacterInfo* c = &pFont->m_characterInfos[textToDraw[0] - 32];
		for(size_t i = 0; i < slength; i++)
		{
			c = &(pFont->m_characterInfos[textToDraw[i] - 32]);
			x += (c->ttfA + c->ttfB + c->ttfC);
		}

		x -= c->ttfC;
		x *= size;
		return x;
	}

	bool Font::DrawTextToScreen( const std::string& textToDraw, 
		std::shared_ptr<Font>& pFont, 
		const RGBA& color, 
		const Vector2f& textPosition, 
		float size,
		ALIGNMENT textAlignment,
		float aspect)
	{
		PROMISES(pFont);

		Shader* shader;

		if( pFont->m_isSDFFont )
		{
			shader = &s_SDFShader;
		}
		else
		{
			shader = &s_Shader;
		}

		Vector2f topLeft = textPosition;
		float textLength = 0;
		switch(textAlignment)
		{
		case ALIGNMENT_CENTER:
			textLength = getLengthOfString(textToDraw, pFont, size);
			topLeft.x -= textLength * .5f;
			break;
		case ALIGNMENT_RIGHT:
			textLength = getLengthOfString(textToDraw, pFont, size);
			topLeft.x -= textLength;
			break;
		case ALIGNMENT_LEFT:
		default:
			break;
		}

		shader->load();
		glDisable(GL_CULL_FACE);
		glBindBuffer(GL_ARRAY_BUFFER, pFont->m_idVBO);
		glUniform4f(shader->getUniformID(HASHED_STRING_u_color), color.x, color.y, color.z, color.w );
		glUniform1f(shader->getUniformID(HASHED_STRING_u_size), size );
		glUniform2f(shader->getUniformID(HASHED_STRING_u_position), topLeft.x, topLeft.y);
		glUniform2f(shader->getUniformID(HASHED_STRING_u_inverseScreenResolution), 1.0f / static_cast<float>(ScreenSize.x), 1.0f / static_cast<float>(ScreenSize.y) );

		glActiveTexture( GL_TEXTURE0 );
		glEnable( GL_TEXTURE_2D);
		Texture::ApplyTexture(pFont->m_fontTexture);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1i(shader->getUniformID(HASHED_STRING_u_diffuse), 0);


		shader->setAttribute(HASHED_STRING_a_position, 2, GL_FLOAT, sizeof(GlyphVert), offsetof(GlyphVert, pos));
		shader->setAttribute(HASHED_STRING_a_texCoords, 2, GL_FLOAT, sizeof(GlyphVert), offsetof(GlyphVert, texCoords));


		size_t slength = textToDraw.size();
		unsigned char upperBound = pFont->m_characterInfos.size() + 31;
		float x = 0;
		for(size_t i = 0; i < slength; i++)
		{
			char charToDraw = textToDraw[i];
			if(charToDraw >= 32 && charToDraw < upperBound )
			{
				FontCharacterInfo* c = &(pFont->m_characterInfos[textToDraw[i] - 32]);
				x += c->ttfA;
				glUniform2f(shader->getUniformID(HASHED_STRING_u_offset), x * size, c->yOffset * size);
				glDrawArrays( GL_QUADS, ( (c->ucsIndex-32) * 4), 4 );

				x += c->ttfB + c->ttfC;
			}
		}

		shader->disableAttribute(HASHED_STRING_a_position );
		shader->disableAttribute(HASHED_STRING_a_texCoords );
		glDisable( GL_TEXTURE_2D);
		glEnable(GL_CULL_FACE);

		return true;
	}

	bool Font::DrawTextToWorldAlignedWorld( const std::string& textToDraw, 
		std::shared_ptr<Font>& pFont,
		Camera* pCamera,
		const RGBA& color, 
		const Vector3f& topLeft,
		const Vector3f& direction,
		float roll,
		float size, 
		float aspect)
	{
		s_Shader.load();
		glDisable(GL_CULL_FACE);
		glBindBuffer(GL_ARRAY_BUFFER, pFont->m_idVBO);
		glUniform1i(s_Shader.getUniformID(HASHED_STRING_u_debugInt), 1 );
		glUniform4f(s_Shader.getUniformID(HASHED_STRING_u_color), color.x, color.y, color.z, color.w );
		glUniform1f(s_Shader.getUniformID(HASHED_STRING_u_size), size );
		glUniformMatrix4fv( s_Shader.getUniformID(HASHED_STRING_u_MV), 1, false, pCamera->getMatrix().m_Matrix );
		glUniform3f(s_Shader.getUniformID(HASHED_STRING_u_position), topLeft.x, topLeft.y, topLeft.z);
		Matrix4f world;
		ConstructRotationMatrixFromDirectionalVector(world, direction );
		glUniformMatrix4fv( s_Shader.getUniformID(HASHED_STRING_u_worldTransform), 1, false, world.m_Matrix );

		glActiveTexture( GL_TEXTURE0 );
		glEnable( GL_TEXTURE_2D);
		Texture::ApplyTexture(pFont->m_fontTexture);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1i(s_Shader.getUniformID(HASHED_STRING_u_diffuse), 0);


		s_Shader.setAttribute(HASHED_STRING_a_position, 2, GL_FLOAT, sizeof(GlyphVert), offsetof(GlyphVert, pos));
		s_Shader.setAttribute(HASHED_STRING_a_texCoords, 2, GL_FLOAT, sizeof(GlyphVert), offsetof(GlyphVert, texCoords));
		size_t slength = textToDraw.size();
		unsigned char upperBound = pFont->m_characterInfos.size() + 31;
		float x = 0;
		for(size_t i = 0; i < slength; i++)
		{
			char charToDraw = textToDraw[i];
			if(charToDraw >= 32 && charToDraw < upperBound )
			{
				FontCharacterInfo* c = &(pFont->m_characterInfos[textToDraw[i] - 32]);
				x += c->ttfA;
				glUniform1f(s_Shader.getUniformID(HASHED_STRING_u_offset), x * size );
				glDrawArrays( GL_QUADS, ( (c->ucsIndex-32) * 4), 4 );

				x += c->ttfB + c->ttfC;
			}
		}

		s_Shader.disableAttribute(HASHED_STRING_a_position );
		s_Shader.disableAttribute(HASHED_STRING_a_texCoords );
		glDisable( GL_TEXTURE_2D);
		glEnable(GL_CULL_FACE);

		return true;
	}

	bool Font::DrawTextToWorldAlignedScreen( const std::string& textToDraw, 
		std::shared_ptr<Font>& pFont, 
		const RGBA& color, 
		const Vector3f& center,
		float size, 
		float aspect )
	{
		// 		s_Shader.load();
		// 		glDisable(GL_CULL_FACE);
		// 		glBindBuffer(GL_ARRAY_BUFFER, pFont->m_idVBO);
		// 		glUniform1i(s_Shader.getUniformID(HASHED_STRING_u_debugInt), 3 );
		// 		glUniform4f(s_Shader.getUniformID(HASHED_STRING_u_color), color.x, color.y, color.z, color.w );
		// 		glUniform1f(s_Shader.getUniformID(HASHED_STRING_u_size), size );
		// 		glUniformMatrix4fv( s_Shader.getUniformID(HASHED_STRING_u_MV), 1, false, OpenGLApp::Instance()->getCamera().getMatrix().m_Matrix );
		// 		glUniform3f(s_Shader.getUniformID(HASHED_STRING_u_position), center.x, center.y, center.z);
		// 		
		// 		Matrix4f world;
		// 		glUniformMatrix4fv( s_Shader.getUniformID(HASHED_STRING_u_WorldTransform), 1, false, world.m_Matrix );
		// 
		// 		glActiveTexture( GL_TEXTURE0 );
		// 		glEnable( GL_TEXTURE_2D);
		// 		Texture::ApplyTexture(pFont->m_fontTexture);
		// 		glEnable(GL_BLEND);
		// 		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// 		glUniform1i(s_Shader.getUniformID(HASHED_STRING_u_diffuse), 0);
		// 
		// 
		// 		s_Shader.setAttribute(HASHED_STRING_a_position, 2, GL_FLOAT, sizeof(GlyphVert), offsetof(GlyphVert, pos));
		// 		s_Shader.setAttribute(HASHED_STRING_a_texCoords, 2, GL_FLOAT, sizeof(GlyphVert), offsetof(GlyphVert, texCoords));
		// 		size_t slength = textToDraw.size();
		// 		unsigned char upperBound = pFont->m_characterInfos.size() + 31;
		// 		float x = 0;
		// 		for(size_t i = 0; i < slength; i++)
		// 		{
		// 			char charToDraw = textToDraw[i];
		// 			if(charToDraw >= 32 && charToDraw < upperBound )
		// 			{
		// 				FontCharacterInfo* c = &(pFont->m_characterInfos[textToDraw[i] - 32]);
		// 				x += c->ttfA;
		// 				glUniform1f(s_Shader.getUniformID(HASHED_STRING_u_offset), x * size );
		// 				glDrawArrays( GL_TRIANGLE_STRIP, ( (c->ucsIndex-32) * 4), 4 );
		// 
		// 				x += c->ttfB + c->ttfC;
		// 			}
		// 		}
		// 
		// 		s_Shader.disableAttribute(HASHED_STRING_a_position );
		// 		s_Shader.disableAttribute(HASHED_STRING_a_texCoords );
		// 		glDisable( GL_TEXTURE_2D);
		// 		glEnable(GL_CULL_FACE);
		// 
		return true;
	}

	static bool DrawTextToWorldAlignedAxis( const std::string& textToDraw, 
		std::shared_ptr<Font>& pFont,
		const RGBA& color, 
		const Vector3f& center,
		const Vector3f& axis,
		float size, 
		float aspect = 1.0f);

	std::shared_ptr<Font> Font::CreateOrGetFont( const HashedString& FontName )
	{
		std::shared_ptr<Font> result = 0;

		auto FontsIterator = Fonts.find(FontName);


		if(FontsIterator != Fonts.end() )
		{
			return FontsIterator->second;
		}
		else
		{
			result = std::make_shared<Font>(Font( FontName ));
			Fonts.insert(std::make_pair(FontName, result) );
			return result;
		}
	}

	static void DestroyFont( std::shared_ptr<Font>& fontToDestroy );
	static void DestroyAllFonts();
	static void SetFontDirectory( const std::string& FontDirectory);


}