
#include "..\stdafx.h"
#include "Texture.hpp"
#include <cassert>

#define REQUIRES(x) assert(x); 
#define PROMISES(x) assert(x);


#include <iostream>
#include <atlbase.h>
#include <atlstr.h>

namespace woodman
{

#define STBI_HEADER_FILE_ONLY
#include "stb_image.c"


	std::set<std::shared_ptr<Texture>, Texture::TextureCompare > Texture::Textures;

	static std::string EmptyString("");

	bool IsPowerOf2( unsigned );


	bool IsPowerOfTwo ( unsigned inputValue )
	{
		// not used because its too hard to find textures to fit this condition
		return true;

		const unsigned MASK = 1 ;

		while ( ! ( inputValue & MASK ) )
			inputValue >>= 1 ;

		if ( inputValue == 1 )
			return true ;
		else
			return false ;
	}

	Texture::Texture()
	{
		mIsLoaded = false;
		mFromMemory = false;

	}

	Texture::Texture( const std::string& texturePath, const TextureFormat& format )
		:m_format(format)
	{
		REQUIRES( !texturePath.empty() );
		mIsLoaded = false;

		if( !texturePath.empty() )
		{
			LoadFromFile( texturePath );
		}

	}

	Texture::~Texture()
	{

	}



	bool Texture::IsLoaded() const
	{
		// Returns true if the texture has been successfully loaded through 
		// one of the Load...() functions below 
		// and is ready to be applied for rendering.

		return mIsLoaded;
	}


	std::string& Texture::GetFilePath()
	{
		// Returns the path of the file (if any) from which this
		// texture was loaded.
		// Returns an empty string if no file was used in loading this
		// texture or if the texture has not been loaded.

		if( mIsLoaded || !mFromMemory )
			return mTexturePath;

		return EmptyString;
	}

	Texture::Size Texture::GetWidth() const
	{
		REQUIRES( IsLoaded() );

		unsigned int result = -1;

		if( mIsLoaded && IsPowerOfTwo( mWidth ) )
		{
			result = mWidth;
		}

		PROMISES( IsPowerOfTwo( result ));

		return result;
	}

	Texture::Size Texture::GetHeight() const
	{
		REQUIRES( IsLoaded() );

		unsigned int result = -1;

		if( mIsLoaded && IsPowerOfTwo( mHeight ) )
		{
			result = mHeight;
		}

		PROMISES( IsPowerOfTwo( result ));

		return result;
	}


	bool Texture::HasAlpha() const
	{
		REQUIRES( IsLoaded() );

		// Returns true if the loaded texture should be 
		// rendered with alpha blending enabled (slower!).
		// This would be true if the original texture file
		// had an alpha channel (RGBA rather than RGB texture).
		//
		if( mChannels == 2 || mChannels == 4 )
			return true;

		return false;
	}

	bool Texture::LoadFromFile( 
		const std::string& texturePath )
	{
		REQUIRES( !IsLoaded() );
		REQUIRES( !texturePath.empty() );

		if (IsLoaded() || texturePath.empty() )
			return false;

		int x, y, n;
		unsigned char *data = stbi_load( texturePath.c_str(), &x, &y, &n, 4 );

		if( !data )
		{
			CStringW path( texturePath.c_str() );
			MessageBox( NULL, L"Error Loading File: " + path, L"File Load Error", MB_OK | MB_ICONERROR );	
			return 0;
		}



		// Create the OpenGL texture.
		//
		m_id = 0;
		glGenTextures( 1, &m_id );
		glBindTexture( GL_TEXTURE_2D, m_id );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_format.MagFilter );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_format.MinFilter );	
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_format.WrapS );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_format.WrapT );	
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );

		if(m_format.MipMapTexture)
			glGenerateMipmap( GL_TEXTURE_2D );

		GLenum err = glGetError();
		//if( err != GL_NO_ERROR )
		//{
		//	MessageBox( NULL, L"Texture Not Loaded", L"Texture Load Error", MB_OK | MB_ICONERROR );
		//	return false;
		//}
		mWidth = x;
		mHeight = y;
		mChannels = n;
		mIsLoaded = true;
		mFromMemory = false;
		mTexturePath = texturePath;


		stbi_image_free(data);
		return true;
	}

	//
	// Static Member Functions ////////////////////////////////////////////
	//

	bool Texture::ApplyTexture( std::shared_ptr<Texture>& pTexture )
	{
		REQUIRES( pTexture == 0 || pTexture->IsLoaded() );

		// If pTexture is 0, texturing will be disabled.
		// Otherwise, texturing is enabled and the indicated
		// texture is applied.
		// If pTexture->HasAlpha(), alpha blending will also
		// be enabled. Otherwise alpha blending will be disabled.

		if( pTexture == 0 )
		{
			glDisable( GL_TEXTURE_2D );
			return false;
		}
		else if( pTexture->IsLoaded() )
		{
			glEnable( GL_TEXTURE_2D );

			// Configure the texture object.
			//
			glBindTexture( GL_TEXTURE_2D, pTexture->m_id );

			// Enable/Disable Alpha Blending
			//
			if( pTexture->HasAlpha() )
			{
				glEnable( GL_BLEND );
				glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			}
			else
			{
				glDisable( GL_BLEND );
			}

			GLenum err = glGetError();
			//			if( err != GL_NO_ERROR )
			//			{
			//				MessageBox( NULL, L"OpenGL Error: Texture Not Applied", L"OpenGL Error", MB_OK | MB_ICONERROR );
			//			}
		}
		else
		{
			MessageBox( NULL, L"Texture Not Loaded", L"Texture Load Error", MB_OK | MB_ICONERROR );
			return false;
		}

		return true;
	}



	std::shared_ptr<Texture> Texture::CreateOrGetTexture( const std::string& texturePath, const TextureFormat& format )
	{
		REQUIRES( !texturePath.empty() );
		REQUIRES( strlen( texturePath.c_str() ) > 0 );

		bool found = false;
		std::shared_ptr<Texture> result = 0;

		std::set<std::shared_ptr<Texture>, Texture::TextureCompare >::iterator TexturesIterator;
		TexturesIterator = Textures.begin();

		while(TexturesIterator != Textures.end() )
		{
			if( (*TexturesIterator)->GetFilePath().compare(texturePath) == 0 ) 
			{
				found = true;
				result = (*TexturesIterator);
				break;
			}
			++TexturesIterator;
		}
		if(!found)
		{	
			result = std::make_shared<Texture>(Texture( texturePath, format ));
			Textures.insert(result);
		}



		PROMISES( result );
		PROMISES( result->IsLoaded() );
		PROMISES( result->GetFilePath().compare( texturePath ) == 0 );

		if(!result || !result->IsLoaded() || !( result->GetFilePath() == texturePath ) )
		{
			// TODO
			// Throw exception
		}


		return result;
	}

	void Texture::DestroyTexture( std::shared_ptr<Texture>& pTexture )
	{
		//REQUIRES( pTexture );

		if(pTexture != 0)
		{
			if(pTexture.use_count() <= 2 )
			{
				glDeleteTextures(1, &pTexture->m_id);
				Textures.erase(pTexture);
				pTexture = 0;

			}
			else
			{
				// Texture is still being used by another object
				pTexture = 0;

			}
		}
		PROMISES( pTexture == 0 );
	}

	void Texture::DestroyAllTextures()
	{
		// Destroys all textures registered through the CreateOrGetTexture() 
		// function.
		std::set< std::shared_ptr<Texture>, Texture::TextureCompare  >::iterator TexturesIterator;
		TexturesIterator = Texture::Textures.begin();

		while(TexturesIterator != Texture::Textures.end() )
		{
			Textures.erase(TexturesIterator);
			TexturesIterator = Texture::Textures.begin();
		}

	}
}