// FontConverter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

struct sdf_glyph
{
	int ID;
	int width, height;
	int x, y;
	float xoff, yoff;
	float xadv;
};


int _tmain(int argc, _TCHAR* argv[])
{
	std::string FilePath;

	//std::cin >> FilePath;

	FilePath = "C:/Windows/Fonts/Corbel.ttf";

	FT_Library ft_lib;
	FT_Face face;

	FT_Error error = FT_Init_FreeType( &ft_lib );
	if( error )
	{

	}

	error = FT_New_Face( ft_lib,
		FilePath.c_str(),
		0,
		&face );

	if ( error == FT_Err_Unknown_File_Format )
	{
		//the font file could be opened and read, but it appears
		//that its font format is unsupported

		std::cout << "unsupported Formate\n";
	}
	else if ( error )
	{
		//another error code means that the font file could not
		//be opened or read, or simply that it is broken...

		std::cout << "Error loading File\n";
	}

	//anyway lets handle these faces

	int numFaces = face->num_faces;
	int numGlyphs = face->num_glyphs;

	int size = 4;
	size <<= 1;

	//error = FT_Set_Char_Size(face, 4 / 16, 0);

	return 0;
}

