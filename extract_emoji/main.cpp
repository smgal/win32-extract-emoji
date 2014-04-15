
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H

#define PNG_SKIP_SETJMP_CHECK
#include <png.h>


#define CHECK_COND(cond) if (!(cond)) return false
#define CHECK_PASS(cond) if (!(cond)) continue;


bool saveToPng32FromBuffer(unsigned long* p_buffer32, int width, int height, int bytes_per_line, const char* sz_file_name)
{
	FILE* p_file = fopen(sz_file_name, "wb");
	CHECK_COND(p_file);

	png_structp p_png_struct = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	CHECK_COND(p_png_struct);

	png_infop p_png_info = png_create_info_struct(p_png_struct);
	CHECK_COND(p_png_info);

	unsigned char* p_buffer08 = (unsigned char*)p_buffer32;

	CHECK_COND(!setjmp(png_jmpbuf(p_png_struct)));

	png_set_IHDR(p_png_struct, p_png_info, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	png_init_io(p_png_struct, p_file);

	png_byte** pp_rows = static_cast<png_byte**>(png_malloc(p_png_struct, height * sizeof(png_byte*)));

	for (int y = 0; y < height; ++y)
	{
		png_byte* p_row = pp_rows[y] = (png_byte*)(png_malloc(p_png_struct, bytes_per_line));

		// little endian only
		memcpy(p_row, p_buffer08 + bytes_per_line * y, width * 4);
	}

	png_set_rows(p_png_struct, p_png_info, pp_rows);
	png_write_png(p_png_struct, p_png_info, PNG_TRANSFORM_BGR, 0);

	for (int y = 0; y < height; y++)
		png_free(p_png_struct, pp_rows[y]);

	png_free(p_png_struct, pp_rows);

	png_destroy_write_struct(&p_png_struct, &p_png_info);

	fclose(p_file);

	return true;
}

bool extractEmoji(const char* sz_font_file)
{
	FT_Library ft_library = 0;
	FT_Face ft_face = 0;
	FT_Error error;

	error = FT_Init_FreeType(&ft_library);
	CHECK_COND(error == 0);

	error = FT_New_Face(ft_library, sz_font_file, 0, &ft_face);
	CHECK_COND(error == 0);

	{
		FT_ULong length = 0;

		FT_Load_Sfnt_Table(ft_face, FT_MAKE_TAG('C', 'B', 'D', 'T'), 0, NULL, &length);
		CHECK_COND(length != 0);
	}

	if (ft_face->num_fixed_sizes > 0)
	{
		FT_Select_Size(ft_face, 0);
	}

	for (FT_ULong code = 0x0; code < 0xFFFFF; code++)
	{
		FT_UInt glyph_index = FT_Get_Char_Index(ft_face, code);
		CHECK_PASS(glyph_index > 0);

		error = FT_Load_Glyph(ft_face, glyph_index, FT_LOAD_COLOR);
		CHECK_PASS(error == 0);

		error = FT_Render_Glyph(ft_face->glyph, FT_RENDER_MODE_NORMAL);
		CHECK_PASS(error == 0);

		if (ft_face->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_BGRA)
		{
			char file_name[256];
			sprintf(file_name, "u%05x.png", code);

			bool r = saveToPng32FromBuffer((unsigned long*)ft_face->glyph->bitmap.buffer, ft_face->glyph->bitmap.width, ft_face->glyph->bitmap.rows, ft_face->glyph->bitmap.pitch, file_name);
		}
	}

	FT_Done_FreeType(ft_library);

	return true;
}

int _main(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("Usage: extract_emoji.exe file_name.ttf\n");

		return 1;
	}

	if (!extractEmoji(argv[1]))
	{
		printf("%s is not a colored emoji TTF\n", argv[1]);

		return 2;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#	define _CRTDBG_MAP_ALLOC
#	include <stdlib.h>
#	include <crtdbg.h>
#	define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#	define ENABLE_MEMORY_MANAGER _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#else
#	define ENABLE_MEMORY_MANAGER
#endif

int main(int argc, char* argv[])
{
	ENABLE_MEMORY_MANAGER
	//_CrtSetBreakAlloc(ALLOCATED_COUNT);

#if defined(TEST_CASE)
	{
		const int TEST_INDEX = 0;

		switch (TEST_INDEX)
		{
		case 0:
			extractEmoji("PhantomOpenEmoji.ttf");
			break;
		case 1:
			extractEmoji("FruityGirl.ttf");
			break;
		case 2:
			extractEmoji("Funkster.ttf");
			break;
		}

		return 0;
	}
#endif

	return _main(argc, argv);
}
