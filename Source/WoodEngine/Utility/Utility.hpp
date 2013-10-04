#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <set>
#include <list>
#include <memory>

#include "..\Math\Vector2.hpp"
#include "..\Math\Vector3.hpp"
#include "..\Math\Vector4.hpp"
#include "../stdafx.h"



namespace woodman
{
	enum PropertyType
	{
		PROPERTYTYPE_VECTOR,
		PROPERTYTYPE_MATRIX,
		PROPERTYTYPE_SAMPLER2D,
		PROPERTYTYPE_STRING,
		PROPERTYTYPE_LIST,
		PROPERTYTYPE_FLOAT,
		PROPERTYTYPE_SIZE,
	};

	struct PropertyList
	{
		PropertyType type;
		std::string name;		
		std::list< std::unique_ptr<PropertyList> > children;
	};

	struct DataType
	{
		PropertyType type;
		unsigned int minSize;
		unsigned int maxSize;
		unsigned int currentSize;
	};


	void ConvertToType( const DataType& d1, const DataType& d2, const std::string& variableName, std::string& output );

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	//unsigned int readStringFromFile(std::string& outString, std::ifstream& in, unsigned int offset);

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	bool getFileText( const std::string& filePath, std::string& outText );

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void getShaderLog( GLuint idShader, std::string& outLog);

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void getProgramLog( GLuint idShader, std::string& outLog);


	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	template< typename T>
	inline T clamp( T x, T min, T max)
	{
		return ( x < min ) ? min : ( x > max ) ? max : x;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	template< typename T>
	inline T calcMin( T anyValue1, T anyValue2 )
	{
		return ( anyValue1 < anyValue2 ) ? anyValue1 : anyValue2;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	template< typename T>
	inline T calcMax( T anyValue1, T anyValue2 )
	{
		return ( anyValue1 > anyValue2 ) ? anyValue1 : anyValue2;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	template< typename T>
	inline void mapValueToRange( const T& inValue, const T& inStart, const T& inEnd, const T& outStart, const T& outEnd, T& outValue)
	{
		if( inEnd == inStart )
		{
			outValue = 0.5f * ( outEnd + outStart );
			return;
		}
		outValue = inValue - inStart;
		outValue /= ( inEnd - inStart);
		outValue *= ( outEnd - outStart );
		outValue += outStart;
	}
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	template< typename T >
	void getValueFromString( const std::string& theString, T& Value )
	{
		if (theString.empty())
		{
			return;
		}
		else
		{
			std::stringstream converter(theString);
			converter >> Value;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	unsigned int loadTexture( const char* szTexturePath );

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	template< typename vec_t >
	unsigned int loadVertexBufferObject( const vec_t& vertices )
	{
		return loadVertexBufferObject( vertices.data(),
			vertices.size() * sizeof( vertices.front() ));
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	template< typename vec_t >
	unsigned int loadIndexBufferObject( const vec_t& vertices )
	{
		return loadIndexBufferObject( vertices.data(),
			vertices.size() * sizeof( vertices.front() ));
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	unsigned int loadVertexBufferObject( const void* pVertices, size_t nBytes );
	unsigned int loadIndexBufferObject( const void* pVertices, size_t nBytes );
	unsigned int LoadBufferObject( GLenum bufferType, const void* vertices, size_t bytes );

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	float Easein( float inValue, float inStart, float inEnd, float outStart, float outEnd );

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	inline bool isKeyDown( int virtualKey )
	{
		return ( GetAsyncKeyState( virtualKey ) & 0x8000 ) == 0x8000;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	inline RGBA getColorFromPlayerID(unsigned int id)
	{
		switch(id)
		{
		case 2: // Blue
			return RGBA(.0f, .0f, 1.0f, 1.0f);
		case 3: // Dark Orange
			return RGBA(0.80078125f, 0.51953125f, 0.0f, 1.0f);
		case 4: // Green
			return RGBA(0.0f, 0.390625f, 0.0f, 1.0f);
		case 5: // Muave
			return RGBA(1.0f, 0.6796875, 0.75390625f, 1.0f);
		case 6: // Red
			return RGBA(0.80078125f, 0.0f, 0.0f, 1.0f);
		case 7: // SummerSky
			return RGBA(0.52734375f, 0.8046875f, 0.9765625f, 1.0f);
		case 8: // Indigo
			return RGBA(0.29296875f, 0.0f, 0.5078125f, 1.0f);
		case 9: // Golden Fizz
			return RGBA(1.0f, 0.60546875f, 0.11328125f, 1.0f );
		case 10: // Med Blue
			return RGBA(0.0f, 0.0f, 0.54296875f, 1.0f );
		default:
			return RGBA(0.54296875f, 0.3984375f, 0.54296875f, 1.0f );
		}

	}
}