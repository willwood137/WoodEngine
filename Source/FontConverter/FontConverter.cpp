// FontConverter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "EasyBMP.h"
#include "BinPacker.hpp"
#include "lodepng.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

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

bool gen_pack_list(
	FT_Face &ft_face,
	int pixel_size,
	int pack_tex_size,
	const std::vector< int > &render_list,
	std::vector< sdf_glyph > &packed_glyphs );

void get_SDF_radial(
	unsigned char *fontmap,
	int locationX, 
	int locationY,
	int originalWidth,
	int originalHeight,
	const sdf_glyph& glyphData,
	int FullTextureSize,
	unsigned char& signedDistance,
	unsigned char& xDirection,
	unsigned char& yDirection);

int save_png_SDFont(
	const char* orig_filename,
	const char* font_name,
	int img_width, int img_height,
	const std::vector< unsigned char > &img_data,
	const std::vector< sdf_glyph > &packed_glyphs );

int scaler = 16;

int _tmain(int argc, _TCHAR* argv[])
{
	std::string FilePath;

	//std::cin >> FilePath;

	FilePath = "C:/Windows/Fonts/Corbel.ttf";

	FT_Library ft_lib;
	FT_Face ft_face;

	FT_Error error = FT_Init_FreeType( &ft_lib );
	FT_Error ft_err;
	if( error )
	{

	}

	error = FT_New_Face( ft_lib,
		FilePath.c_str(),
		0,
		&ft_face );

	if ( error == FT_Err_Unknown_File_Format )
	{
		//the font file could be opened and read, but it appears
		//that its font format is unsupported

		std::cout << "unsupported Format\n";
	}
	else if ( error )
	{
		//another error code means that the font file could not
		//be opened or read, or simply that it is broken...

		std::cout << "Error loading File\n";
	}

	//anyway lets handle these faces

	int numFaces = ft_face->num_faces;
	int numGlyphs = ft_face->num_glyphs;
	int texture_size = 1024;
	int max_unicode_char = std::min(numGlyphs, 128);

	std::vector< int > render_list;
	for( int char_idx = 0; char_idx <= max_unicode_char; ++char_idx )
	{
		render_list.push_back( char_idx );
	}
	//	find the perfect size
	printf( "\nDetermining ideal font pixel size: " );
	std::vector< sdf_glyph > all_glyphs;
	//	initial guess for the size of the Signed Distance Field font
	//	(intentionally low, the first trial will be at sz*2, so 8x8)
	int sz = 4;
	bool keep_going = true;
	while( keep_going )
	{
		sz <<= 1;
		printf( " %i", sz );
		keep_going = gen_pack_list( ft_face, sz, texture_size, render_list, all_glyphs );
	}
	int sz_step = sz >> 2;
	while( sz_step )
	{
		if( keep_going )
		{
			sz += sz_step;
		} else
		{
			sz -= sz_step;
		}
		printf( " %i", sz );
		sz_step >>= 1;
		keep_going = gen_pack_list( ft_face, sz, texture_size, render_list, all_glyphs );
	}
	//	just in case
	while( (!keep_going) && (sz > 1) )
	{
		--sz;
		printf( " %i", sz );
		keep_going = gen_pack_list( ft_face, sz, texture_size, render_list, all_glyphs );
	}
	printf( "\nResult = %i pixels\n", sz );

	if( !keep_going )
	{
		printf( "The data will not fit in a texture %i^2\n", texture_size );
		system( "pause" );
		return -1;
	}

	//set up memory to store our textures
	std::vector<unsigned char> signedData( 4 * texture_size * texture_size, 0 );

	//	render all the glyphs individually
	printf( "\nRendering characters into a packed %i^2 image:\n", texture_size );
	int packed_glyph_index = 0;

	for( unsigned int char_index = 0; char_index < render_list.size(); ++char_index )
	{
		int glyph_index = FT_Get_Char_Index( ft_face, render_list[char_index] );
		if( glyph_index )
		{
			

			ft_err = FT_Load_Glyph( ft_face, glyph_index, 0 );
			if( !ft_err )
			{
				ft_err = FT_Render_Glyph( ft_face->glyph, FT_RENDER_MODE_MONO );
				if( !ft_err )
				{
					//	we have the glyph, already rendered, get the data about it
					int widthOriginal = ft_face->glyph->bitmap.width;
					int heightOriginal = ft_face->glyph->bitmap.rows;
					int p = ft_face->glyph->bitmap.pitch;

					unsigned char *buf = ft_face->glyph->bitmap.buffer;

					char* filename = new char[50];
					sprintf(filename, "c%d.bmp", glyph_index);

					//std::ofstream outfile(filename, std::ofstream::binary);
					//outfile.write(fontmap, glyphData.width * glyphData.height);

					int sw = widthOriginal + scaler * 4;
					int sh = heightOriginal + scaler * 4;
					unsigned char *smooth_buf = new unsigned char[sw*sh];
					for( int i = 0; i < sw*sh; ++i )
					{
						smooth_buf[i] = 0;
					}

					//	copy the glyph into the buffer to be smoothed
					for( int j = 0; j < heightOriginal; ++j )
					{
						for( int i = 0; i < widthOriginal; ++i )
						{
							smooth_buf[scaler*2+i+(j+scaler*2)*sw] = 255 * ((buf[j*p+(i>>3)] >> (7 - (i & 7))) & 1);
						}
					}

// 					BMP Image;
// 					Image.SetSize(sw, sh);
// 					//Image.SetBitDepth(32);
// 
// 					if(heightOriginal > 0)
// 					{
// 						for(int j = 0; j < sh; ++j)
// 						{
// 							for(int i = 0; i < sw; ++i)
// 							{
// 								RGBApixel p;
// 								int pLoc = i + j*sw;
// 								//p.Red = 255 * ( buf[ (i + j * widthOriginal ) / 4] & ( 1 << ( ( i + j*widthOriginal) % 4 ) ) );
// 								p.Red = smooth_buf[pLoc];
// 								p.Green = p.Red;
// 								p.Blue = p.Red;
// 								p.Alpha = p.Red;
// 								Image.SetPixel(i, j, p);
// 							}
// 						}
// 						Image.WriteToFile(filename);
// 					}

					int sdfWidth = all_glyphs[packed_glyph_index].width;
					int sdfHeight = all_glyphs[packed_glyph_index].height;

					int sdfx = all_glyphs[packed_glyph_index].x;
					int sdfy = all_glyphs[packed_glyph_index].y;

					for( int j = 0; j < sdfHeight; ++j )
					{
						for( int i = 0; i < sdfWidth; ++i )
						{
							int pixelID = (i+sdfx+(j+sdfy)*texture_size) * 4;

							//get_SDF
							unsigned char signedDistance, xDirection, yDirection;

							get_SDF_radial( smooth_buf, 
								i * scaler + (scaler >>1), 
								j * scaler + (scaler >>1),
								sw,
								sh,
								all_glyphs[packed_glyph_index],
								texture_size * scaler, 
								signedDistance, 
								xDirection, 
								yDirection);

							signedData[pixelID] = xDirection;
							signedData[pixelID + 1] = yDirection;
							signedData[pixelID + 2] = signedDistance;
							signedData[pixelID + 3] = 255;

						}
					}
					++packed_glyph_index;

				}
				printf( "%i ", render_list[char_index] );
			}
		}
	}

	save_png_SDFont(
		FilePath.c_str(), ft_face->family_name,
		texture_size, texture_size,
		signedData, all_glyphs );

	return 0;
}

bool gen_pack_list(
	FT_Face &ft_face,
	int pixel_size,
	int pack_tex_size,
	const std::vector< int > &render_list,
	std::vector< sdf_glyph > &packed_glyphs )
{
	int ft_err;
	packed_glyphs.clear();
	ft_err = FT_Set_Pixel_Sizes( ft_face, pixel_size*scaler, 0 );
	std::vector< int > rectangle_info;
	std::vector< std::vector<int> > packed_glyph_info;
	for( unsigned int char_index = 0; char_index < render_list.size(); ++char_index )
	{
		int glyph_index = FT_Get_Char_Index( ft_face, render_list[char_index] );
		if( glyph_index )
		{
			ft_err = FT_Load_Glyph( ft_face, glyph_index, 0 );
			if( !ft_err )
			{

				ft_err = FT_Render_Glyph( ft_face->glyph, FT_RENDER_MODE_MONO );
				if( !ft_err )
				{
					sdf_glyph add_me;
					//	we have the glyph, already rendered, get the data about it
					int w = ft_face->glyph->bitmap.width;
					int h = ft_face->glyph->bitmap.rows;
					//	oversize the holding buffer so I can smooth it!
					int sw = w + scaler * 4;
					int sh = h + scaler * 4;
					//	do the SDF
					int sdfw = sw / scaler;
					int sdfh = sh / scaler;
					rectangle_info.push_back( sdfw );
					rectangle_info.push_back( sdfh );
					//	add in the data I already know
					add_me.ID = render_list[char_index];
					add_me.width = sdfw;
					add_me.height = sdfh;
					//	these need to be filled in later (after packing)
					add_me.x = -1;
					add_me.y = -1;
					//	these need scaling...
					add_me.xoff = ft_face->glyph->bitmap_left;
					add_me.yoff = ft_face->glyph->bitmap_top;
					add_me.xadv = ft_face->glyph->advance.x / 64.0;
					//	so scale them (the 1.5's have to do with the padding
					//	border and the sampling locations for the SDF)
					add_me.xoff = add_me.xoff / scaler - 1.5;
					add_me.yoff = add_me.yoff / scaler + 1.5;
					add_me.xadv = add_me.xadv / scaler;
					//	add it to my list
					packed_glyphs.push_back( add_me );
				}
			}
		}
	}
	const bool dont_allow_rotation = false;
	BinPacker bp;
	bp.Pack( rectangle_info, packed_glyph_info, pack_tex_size, dont_allow_rotation );
	//	populate the actual coordinates
	if( packed_glyph_info.size() == 1 )
	{
		//	it all fit into one!
		unsigned int lim = packed_glyph_info[0].size();
		for( unsigned int i = 0; i < lim; i += 4 )
		{
			//	index, x, y, rotated
			unsigned int idx = packed_glyph_info[0][i+0];
			packed_glyphs[idx].x = packed_glyph_info[0][i+1];
			packed_glyphs[idx].y = packed_glyph_info[0][i+2];
		}
		return true;
	}
	return false;
}

void get_SDF_radial(
	unsigned char *fontmap,
	int locationX, 
	int locationY,
	int originalWidth,
	int originalHeight,
	const sdf_glyph& glyphData,
	int FullTextureSize,
	unsigned char& signedDistance,
	unsigned char& xDirection,
	unsigned char& yDirection )
{
	//first check if we are a value or not
	unsigned char value;
	bool inside = false;


	unsigned int location = locationX + locationY * originalWidth;
	value = fontmap[location];

	unsigned char pixelValue = value;

	if(value > 0)
		inside = true;

	
	//now check for the closest value
	int maxRadius = scaler * 16;

	float shortestDistance = static_cast<float>(maxRadius);
	int shortestX = 0, shortestY = 0;

	bool thresholdFound = false;

	for(int radius = 1; radius < maxRadius; radius++)
	{
		bool MorePossibleShorterDistances = false;

		//starting from left check spiral
		for(int x = -radius; x <= radius; ++x )
		{
			
			if(locationX + x >= 0 && locationX + x < originalWidth)
			{
				int y = radius - abs(x);
				float distance = sqrt( static_cast<float>( x * x + y * y) );
				if(distance < shortestDistance)
				{
					MorePossibleShorterDistances = true;
				}

				if( (locationY + y) < originalHeight)
				{
					//check top

					unsigned int testLocation = (locationX + x) + (locationY + y ) * originalWidth;
					value = fontmap[testLocation];
					if(value != pixelValue)
					{
						thresholdFound = true;
						//we found a break!
						if(distance < shortestDistance)
						{
							shortestDistance = distance;
							shortestX = x;
							shortestY = y;
						}
					}
				}
				//check bottom
				if(y != 0)
				{
					if( locationY - y >= 0 )
					{
						unsigned int testLocation = (locationX + x) + (locationY - y ) * originalWidth;
						value = fontmap[testLocation];
					
						if(value != pixelValue)
						{
							thresholdFound = true;
							//we found a break!
							if(distance < shortestDistance)
							{
								shortestDistance = distance;
								shortestX = x;
								shortestY = -y;
							}
						}
					}
				}
			}
		}

		//when we find a value that passes we need to check all pixels that are of shorter distance than that
		// if there are no more pixels to check, break out
		if(!MorePossibleShorterDistances)
			break;
	}

	//shortest distance is in pixels, convert that to [0, 1] for the whole texture
	float distanceTexture = std::max( 0.0f, std::min( 1.0f, (shortestDistance / static_cast<float>(maxRadius) ) ) );

	if(inside)
		signedDistance = ( (distanceTexture + 1.0f ) * .5f ) * 255.0f;
	else
		signedDistance = ( (1.0f - distanceTexture) * .5f) * 255.0f;

	float x = shortestX;
	float y = shortestY;


	//normalize distance vector
	float lengthsquared = x * x + y * y;
	if( lengthsquared != 1 )
	{
		if( lengthsquared != 0)
		{
			float ilength = 1 / sqrt( lengthsquared );
			x *= ilength;
			y *= ilength;
		}
		else
		{
			x = 0.0f; //.707106781f;
			y = 0.0f; //.707106781f;
		}
	}

	xDirection = 255 * ( (x + 1) * .5 );
	yDirection = 255 * ( (y + 1) * .5 );

}

int save_png_SDFont(
	const char* orig_filename,
	const char* font_name,
	int img_width, int img_height,
	const std::vector< unsigned char > &img_data,
	const std::vector< sdf_glyph > &packed_glyphs )
{
	//	save my image
	int fn_size = strlen( orig_filename ) + 100;
	char *fn = new char[ fn_size ];
	sprintf( fn, "s_sdf.png", orig_filename );
	printf( "'%s'\n", fn );
	LodePNG::Encoder encoder;
	encoder.addText("Comment", "Signed Distance Font: lonesock tools, Modified by William Wood");
	encoder.getSettings().zlibsettings.windowSize = 512; //	faster, not much worse compression
	std::vector<unsigned char> buffer;

	encoder.encode( buffer, img_data.empty() ? 0 : &img_data[0], img_width, img_height );
	LodePNG::saveFile( buffer, fn );
	
	//	now save the acompanying info
	sprintf( fn, "_sdf.xml", orig_filename );
	for( unsigned int i = 0; i < packed_glyphs.size(); ++i )
	{
			
			
			
	
	}
	delete [] fn;
	return 0;
}