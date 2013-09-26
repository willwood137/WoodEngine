#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H


//#include "stdafx.h"
#include <iostream>
#include <string>
#include <set>
#include <memory>


namespace woodman
{

	

	class Texture
	{


	public:

		struct TextureCompare
		{
			bool operator()( const std::shared_ptr<Texture>& lhs, const std::shared_ptr<Texture>& rhs ) const;
		};

		struct TextureFormat
		{
			GLint MagFilter;
			GLint MinFilter;
			GLint WrapS;
			GLint WrapT;
			bool MipMapTexture;

			TextureFormat()
				: MagFilter(GL_NEAREST),
				MinFilter(GL_NEAREST_MIPMAP_LINEAR),
				WrapS(GL_REPEAT),
				WrapT(GL_REPEAT),
				MipMapTexture(true)
			{

			}
		};


		typedef unsigned int Size;		// Used for the width and height
		typedef unsigned char Color;	// RGBA color

		//
		// Member Functions ///////////////////////////////////////////////
		//

		Texture();
		Texture( const std::string& texturePath, const TextureFormat& format );
		// REQUIRES( !texturePath.empty() );

		~Texture();

		bool IsLoaded() const;
		// Returns true if the texture has been successfully loaded through 
		// one of the Load...() functions below 
		// and is ready to be applied for rendering.

		std::string& GetFilePath();
		// Returns the path of the file (if any) from which this
		// texture was loaded.
		// Returns an empty string if no file was used in loading this
		// texture or if the texture has not been loaded.

		Size GetWidth() const;
		Size GetHeight() const;
		// REQUIRES( IsLoaded() );
		// PROMISES( IsPowerOfTwo( result ));

		bool HasAlpha() const;
		// REQUIRES( IsLoaded() );
		// Returns true if the loaded texture should be 
		// rendered with alpha blending enabled (slower!).
		// This would be true if the original texture file
		// had an alpha channel (RGBA rather than RGB texture).

		bool LoadFromFile( 
			const std::string& texturePath );
		// REQUIRES( !IsLoaded() );
		// REQUIRES( !texturePath.empty() );
		// PROMISES( IsLoaded() );
		// PROMISES( GetFilePath() == szTexturePath );


		//
		// Static Member Functions ////////////////////////////////////////////
		//

		static bool ApplyTexture( std::shared_ptr<Texture>& pTexture );
		// REQUIRES( pTexture == 0 || pTexture->IsLoaded() );
		// If pTexture is 0, texturing will be disabled.
		// Otherwise, texturing is enabled and the indicated
		// texture is applied.
		// If pTexture->HasAlpha(), alpha blending will also
		// be enabled. Otherwise alpha blending will be disabled.

		static std::shared_ptr<Texture> CreateOrGetTexture( 
			const std::string& texturePath,
			const TextureFormat& format = TextureFormat()  );
		// REQUIRES( !texturePath.empty() );
		// REQUIRES( strlen( szTexturePath ) > 0 );
		// PROMISES( result );
		// PROMISES( result->IsLoaded() );
		// PROMISES( result->GetFilePath() == szTexturePath );
		// If the given path has already been loaded, this function
		// returns the previously loaded texture.
		// Otherwise, this function loads the file into a new texture,
		// records the association between the texture with the path, 
		// and returns the texture.

		static void DestroyTexture( std::shared_ptr<Texture>& texture );
		// REQUIRES( pTexture );
		// PROMISES( pTexture == 0 );
		// Deletes the indicated texture.
		// If this texture was registered through the
		// CreateOrGetTexture() mechanism, DestroyTexture()
		// will remove the texture from this registration.
		// Textures created through CreateOrGetTexture()
		// should always be deleted using DestroyTexture().

		static void DestroyAllTextures();
		// Destroys all textures registered through the CreateOrGetTexture() 
		// function.



	private:
		std::string mTexturePath;
		GLuint m_id;
		bool mIsLoaded;
		bool mFromMemory;
		int mChannels;
		Size mWidth;
		Size mHeight;
		static std::set< std::shared_ptr<Texture>, TextureCompare > Textures;
		TextureFormat m_format;

	};


	inline bool Texture::TextureCompare::operator()( const std::shared_ptr<Texture>& lhs, const std::shared_ptr<Texture>& rhs ) const
	{
		return lhs->mTexturePath.compare(rhs->mTexturePath) < 0;
	}
}
#endif