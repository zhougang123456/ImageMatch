#include "ImageMatch.hpp"
#include <memory>
#define HORIZONTAL_JUMP 100
#define VERTICAL_JUMP   100
#define MAX_PIXEL_SAME 3
ImageMatch::ImageMatch()
{
}

ImageMatch::~ImageMatch()
{
}

void ImageMatch::reset(void)
{
    current_x = 0;
    current_y = 0;
    memset(match_table_up, 0, sizeof(int) * IMAGE_HEIGHT_MAX);
    memset(match_table_down, 0, sizeof(int) * IMAGE_HEIGHT_MAX);
}

bool ImageMatch::pixel_same(Pixel32Bit* pixel_a, Pixel32Bit* pixel_b)
{   
    int i = 0;
    if (pixel_a->r == 0 && pixel_a->g == 0 && pixel_a->b == 0) {
        return false;
    }
    if (pixel_a->r == 255 && pixel_a->g == 255 && pixel_a->b == 255) {
        return false;
    }
   
    while (i < MAX_PIXEL_SAME) {
        if (pixel_a->r != pixel_b->r || pixel_a->g != pixel_b->g || pixel_a->b != pixel_b->b) {
            return false;
        }
        i++;
        pixel_a++;
        pixel_b++;
    }
 
    return true;
}

int ImageMatch::compute_vector()
{   
    int tmp = 0, vector = -1, flag;
    for (int i = 0; i < IMAGE_HEIGHT_MAX; i++) {
        if (match_table_up[i] > tmp) {
            tmp = match_table_up[i];
            vector = i;
            flag = -1;
        }
        if (match_table_down[i] > tmp) {
            tmp = match_table_down[i];
            vector = i;
            flag = 1;
        }
    }
    if (vector == -1) {
        return IMAGE_HEIGHT_MAX;
    } 
    return vector * flag;
}

void ImageMatch::match_line(Pixel32Bit* current_pixel, Pixel32Bit* dest_start, Pixel32Bit* dest_end, int width)
{   
    int line = -1;
    Pixel32Bit* dest_now = dest_start;
    int dest_current_y = 0;
    while (dest_now < dest_end) {
        if (pixel_same(current_pixel, dest_now)) {
            if (current_y > dest_current_y) {
                match_table_up[current_y - dest_current_y]++;
            } else {
                match_table_down[dest_current_y - current_y]++;
            }
        }
        dest_current_y++;
        dest_now += width;
    }
}

int ImageMatch::do_match(Pixel32Bit* src, Pixel32Bit* dest, int width, int height)
{   
    reset();
    for (current_x = 100; current_x < width - 100; current_x += HORIZONTAL_JUMP)
        for (current_y = 100; current_y < height - 100; current_y += VERTICAL_JUMP)
        {   
            current_pixel = src + width * current_y + current_x;
            match_line(current_pixel, dest + current_x, dest + width * height + current_x, width);    
        }
    return compute_vector();
}