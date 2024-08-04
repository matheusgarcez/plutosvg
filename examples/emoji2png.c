#include <plutosvg.h>

#include <stdio.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H

#define handle_error(status) if(status) { error = status; goto cleanup; }

int main(int argc, char* argv[])
{
    if(argc != 4) {
        fprintf(stderr, "Usage: emoji2png font codepoint size\n");
        return -1;
    }

    const char* filename = argv[1];
    FT_ULong codepoint = strtoul(argv[2], NULL, 16);
    FT_ULong size = strtoul(argv[3], NULL, 10);

    FT_Library library = NULL;
    FT_Face face = NULL;
    FT_Error error = FT_Err_Ok;

    handle_error(FT_Init_FreeType(&library));
    handle_error(FT_Property_Set(library, "ot-svg", "svg-hooks", plutosvg_ft_svg_hooks()));
    handle_error(FT_New_Face(library, filename, 0, &face));
    handle_error(FT_Set_Pixel_Sizes(face, 0, size));
    handle_error(FT_Load_Char(face, codepoint, FT_LOAD_RENDER | FT_LOAD_COLOR));

    if(face->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_BGRA) {
        FT_GlyphSlot slot = face->glyph;
        plutovg_surface_t* surface = plutovg_surface_create_for_data(slot->bitmap.buffer, slot->bitmap.width, slot->bitmap.rows, slot->bitmap.pitch);

        char name[64];
        sprintf(name, "emoji-%lx.png", codepoint);
        plutovg_surface_write_to_png(surface, name);
        fprintf(stdout, "Generated Emoji: %s\n", name);
    } else {
        fprintf(stderr, "The glyph for codepoint %lx is not in color mode.\n", codepoint);
    }

cleanup:
    if(error) fprintf(stderr, "freetype error: %s\n", FT_Error_String(error));
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return error;
}