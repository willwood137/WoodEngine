
#include "../stdafx.h"

#include <utility>
#include <fstream>
#include <memory>

#include "Utility.hpp"

namespace woodman
{
	void ConvertToType( const DataType& out, const DataType& in, const std::string& variableName, std::string& output )
	{
		assert( out.type == PROPERTYTYPE_FLOAT && in.type == PROPERTYTYPE_FLOAT);

		if(out.currentSize == 1)
		{
			if(in.currentSize == 1)
			{
				output = variableName;
			}
			else if(in.currentSize == 2)
			{
				output = "vec2( " + variableName + ", " + variableName + " )";
			}
			else if(in.currentSize == 3)
			{
				output = "vec3( " + variableName + ", " + variableName + ", " + variableName + " )";
			}
			else if(in.currentSize == 4)
			{
				output = "vec4( " + variableName + ", " + variableName + ", " + variableName + ", " + variableName + " )";
			}
		}
		else if(out.currentSize == 2)
		{
			if(in.currentSize == 1)
			{
				output = variableName + ".x";
			}
			else if(in.currentSize == 2)
			{
				output = variableName;
			}
			else if(in.currentSize == 3)
			{
				output = "vec3( " + variableName + ", 0.0 )";
			}
			else if(in.currentSize == 4)
			{
				output = "vec4( " + variableName + ", 0.0, 1.0 )";
			}
		}
		else if(out.currentSize == 3)
		{
			if(in.currentSize == 1)
			{
				output = variableName + ".x";
			}
			else if(in.currentSize == 2)
			{
				output = variableName + ".xy";
			}
			else if(in.currentSize == 3)
			{
				output = variableName;
			}
			else if(in.currentSize == 4)
			{
				output = "vec4( " + variableName + ", 1.0 )";
			}
		}
		else if(out.currentSize == 4)
		{
			if(in.currentSize == 1)
			{
				output = variableName + ".x";
			}
			else if(in.currentSize == 2)
			{
				output = variableName + ".xy";
			}
			else if(in.currentSize == 3)
			{
				output = variableName + ".xyz";
			}
			else if(in.currentSize == 4)
			{
				output = variableName;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	// readStringFromFile(std::string& outString, std::ifstream& in, unsigned int offset)
	//
	// Reads a string from a file starting at offset.  format is assumed that first is a unsigned int giving
	// the length (number of chars) of the string followed by a string, all in binary format.
	//
	// return length of string;
	//
	// 	unsigned int readStringFromFile(std::string& outString, std::ifstream& in, unsigned int offset)
	// 	{
	// 		if(offset != 0)
	// 			in.seekg(offset, std::ios::beg);
	// 
	// 		unsigned int stringLength;
	// 
	// 		in.read(reinterpret_cast<char*>(&stringLength), sizeof(unsigned int) );
	// 		if( stringLength > 0)
	// 		{
	// 			outString.resize(stringLength);
	// 			in.read(reinterpret_cast<char*>(&outString[0]), sizeof(char)*stringLength );
	// 		}
	// 
	// 		return stringLength;
	// 	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


	bool getFileText( const std::string& filePath, std::string& outText )
	{

		// Open the file
		//
		std::ifstream sourceFile( filePath.c_str() );


		if(sourceFile.good() )
		{

			// Read the full contents of the file into a string. (may throw.)
			//
			sourceFile.seekg( 0, std::ios::end );
			outText.reserve( static_cast< size_t > ( sourceFile.tellg() ));
			sourceFile.seekg( 0, std::ios::beg );

			outText.assign(( std::istreambuf_iterator<char>(sourceFile )),
				std::istreambuf_iterator<char>() );

			sourceFile.close();
			return true;
		}
		return false;
	}

	void getShaderLog( GLuint idShader, std::string& outLog)
	{
		GLint lenLog;
		glGetShaderiv( idShader, GL_INFO_LOG_LENGTH, &lenLog );

		if( lenLog > 0 )
		{
			// Report errors or warnings.
			//
			std::unique_ptr< char > szLogText( new char[ lenLog + 1 ] );

			glGetShaderInfoLog( idShader, lenLog, &lenLog, szLogText.get() );
			outLog += szLogText.get();


		}
	}

	void getProgramLog( GLuint idProgram, std::string& outLog)
	{
		assert( idProgram);


		GLint lenLog;
		glGetShaderiv( idProgram, GL_INFO_LOG_LENGTH, &lenLog );

		if( lenLog > 0 )
		{
			// Report errors or warnings.
			//
			std::unique_ptr< char > szLogText( new char[ lenLog + 1 ] );

			glGetProgramInfoLog( idProgram, lenLog, &lenLog, szLogText.get() );
			outLog += szLogText.get();
		}
	}

	unsigned int loadVertexBufferObject( const void* pVertices, size_t nBytes )
	{

		GLuint idVBO;
		glGenBuffers( 1, &idVBO );
		glBindBuffer( GL_ARRAY_BUFFER, idVBO );

		glBufferData( GL_ARRAY_BUFFER, nBytes, pVertices, GL_STATIC_DRAW );

		return idVBO;
	}

	unsigned int loadIndexBufferObject( const void* pVertices, size_t nBytes )
	{

		GLuint idVBO;
		glGenBuffers( 1, &idVBO );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, idVBO );

		glBufferData( GL_ELEMENT_ARRAY_BUFFER, nBytes, pVertices, GL_STATIC_DRAW );

		return idVBO;
	}

	float Easein( float inValue, float inStart, float inEnd, float outStart, float outEnd )
	{
		float outValue = inValue -inStart;
		if( inEnd == inStart )
			return 0.5f * ( outEnd + outStart );
		outValue /= ( inEnd - inStart);

		//outValue = 1 - (1-outValue) * (1-outValue);
		//outValue = 2* outValue * outValue - outValue*outValue*outValue;

		outValue *= ( outEnd - outStart );
		outValue += outStart;

		// if outValue is outside of out range, do not set it to the boundary values, 
		// this allows filter functions that step out of the boundaries


		return outValue;
	} 

	unsigned int LoadBufferObject( GLenum bufferType, const void* vertices, size_t bytes )
	{
		assert( bufferType == GL_ARRAY_BUFFER || bufferType == GL_ELEMENT_ARRAY_BUFFER );
		//VBO or IBO

		GLuint idVBO;

		//Bind if of our buffer
		glGenBuffers( 1, &idVBO );
		glBindBuffer( bufferType, idVBO );

		//Feed the data to it. Not going to modify it, but want to draw it.
		glBufferData( bufferType, bytes, vertices, GL_STATIC_DRAW );


		return idVBO;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	int StringToWString(std::wstring &ws, const std::string &s)
	{
		std::wstring wsTmp(s.begin(), s.end());

		ws = wsTmp;

		return 0;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


}