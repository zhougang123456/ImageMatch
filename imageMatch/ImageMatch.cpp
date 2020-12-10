#include "ImageMatch.hpp"
#include <memory>

#define MAX_PIXEL_SAME 3
#define HORIZONTAL_JUMP_SAMPLES 40
#define VERTICAL_JUMP_SAMPLES   15
#define SAME_SAMPLE_RATIO       10

ImageMatch::ImageMatch(int surface_height)
{
    max_height = surface_height;
    match_table_up = (int*)malloc(sizeof(int) * surface_height);
    match_table_down = (int*)malloc(sizeof(int) * surface_height);
}

ImageMatch::~ImageMatch()
{   
    if (match_table_up) {
        free(match_table_up);
    }
    if (match_table_down) {
        free(match_table_down);
    }
}

void ImageMatch::reset(void)
{
    current_x = 0;
    current_y = 0;
    sample_num = 0;
    memset(match_table_up, 0, sizeof(int) * max_height);
    memset(match_table_down, 0, sizeof(int) * max_height);
}

bool ImageMatch::pixel_same(Pixel32Bit* pixel_a, Pixel32Bit* pixel_b)
{   
    int i = 0;
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

bool ImageMatch::pixel_is_colorful(Pixel32Bit* pixel)
{   
    if (pixel->r == pixel->g  && pixel->g == pixel->b) {
        return false;
    }
    return true;
}

int ImageMatch::compute_vector(void)
{   
    int tmp = 0, vector = -1, flag;
    /* find the nearest position except for the same position */
    for (int i = 1; i < max_height; i++) {
        if (match_table_up[i] >= tmp) {
            tmp = match_table_up[i];
            vector = i;
            flag = -1;
        }
        if (match_table_down[i] >= tmp) {
            tmp = match_table_down[i];
            vector = i;
            flag = 1;
        }
        //printf("%d tmp %d %d\n", i, match_table_up[i], match_table_down[i]);
    }
    
    if (match_table_down[0] >= sample_num / 2 && match_table_down[0] > tmp) {
        return 0;
    }

    int score = sample_num / SAME_SAMPLE_RATIO ? sample_num / SAME_SAMPLE_RATIO : 1;
    if (tmp < score) {
        if (match_table_down[0] > score) {
            vector = 0;
        } else {
            vector = -1;
        }
    }

    if (vector == -1) {
        return max_height;
    } 

    return vector * flag;
}

void ImageMatch::match_line(Pixel32Bit* current_pixel, Pixel32Bit* dest_start, Pixel32Bit* dest_end, int dest_y, int dest_width)
{   
    Pixel32Bit* dest_now = dest_start;
    while (dest_now < dest_end) {
        if (pixel_same(current_pixel, dest_now)) {
            if (current_y > dest_y) {
                match_table_up[current_y - dest_y]++;
            } else {
                match_table_down[dest_y - current_y]++;
            }
        }
        dest_y++;
        dest_now += dest_width;
    }
}

int ImageMatch::do_match(Pixel32Bit* src, int src_x, int src_y, int src_width, int src_height,
                         Pixel32Bit* dest, int dest_x, int dest_y, int dest_width, int dest_height)
{   
    /* compute match area needed */
    int left = src_x > dest_x ? src_x : dest_x;
    int right = src_x + src_width > dest_y + dest_width ? dest_y + dest_width : src_x + src_width;
    int top = src_y > dest_y ? src_y : dest_y;
    int bottom = src_y + src_height > dest_y + dest_height ? dest_y + dest_height : src_y + src_height;
    int width = right - left;
    int height = bottom - top;
    if (width <= 0 || height <= 0) {
        return max_height;
    }
    /* compute horizontal and vertical sample jump */
    horizontal_jump = (width / HORIZONTAL_JUMP_SAMPLES) > MAX_PIXEL_SAME ? (width / HORIZONTAL_JUMP_SAMPLES) : MAX_PIXEL_SAME;
    vertical_jump = (height / VERTICAL_JUMP_SAMPLES) ? (height / VERTICAL_JUMP_SAMPLES) : 1;

    /* init match table score */
    reset();

    int src_offset, dest_offset;
    Pixel32Bit *dest_start, *dest_end;
    for (current_x = left + horizontal_jump; current_x < right - horizontal_jump; current_x += horizontal_jump) {
        src_offset = current_x - src_x;
        dest_offset = current_x - dest_x;
        dest_start = dest + dest_offset;
        dest_end = dest + dest_width * dest_height + dest_offset;
        for (current_y = top + vertical_jump; current_y < bottom - vertical_jump; current_y += vertical_jump)
        {
            current_pixel = src + src_width * (current_y - src_y) + src_offset;
            if (pixel_is_colorful(current_pixel)) {
                match_line(current_pixel, dest_start, dest_end, dest_y, dest_width);
                sample_num++;
            }
            
        }
    }

    /* compute match vector according to match table score */
    return compute_vector();
}