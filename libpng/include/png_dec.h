
#include "png.h"

typedef enum {
        COLOR_TYPE_GRAY = 0,
        COLOR_TYPE_RGB = 1,
        COLOR_TYPE_PALETTE = 2,
        COLOR_TYPE_GRAY_ALPHA = 3,
        COLOR_TYPE_RGB_ALPHA = 4
} PNG_COLOR_TYPE;

typedef struct {
        unsigned char   red;
        unsigned char   green;
        unsigned char   blue;
} PNG_COLOR;

typedef struct {
        unsigned long   width, height;          // png_uint_32
        unsigned long   rowbytes;                // png_uint_32
        PNG_COLOR_TYPE  Color_Type;
        int             num_pass;
        unsigned char   interlace_type;         // png_byte
        int             num_palette;            // png_uint_16
        PNG_COLOR       *palette;               // png_colorp
        unsigned char   bit_depth;              // png_byte
        unsigned char   *row_buf;               // png_byte
        unsigned char   *image_buf;
	unsigned char   tR, tG, tB;
        //PNG_COLOR       *trans_values;          // png_colorp
} PNG_IMAGE;

int decodePng(char *rawData, PNG_IMAGE *pngImage, int dataLength);
void ClearPngImage(PNG_IMAGE *pngImage);

