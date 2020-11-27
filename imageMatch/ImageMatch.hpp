#ifndef __IMAGE_MATCH_HPP_
#define __IMAGE_MATCH_HPP_

typedef struct Pixel32Bit
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
}Pixel32Bit;
class ImageMatch
{
public:
    ImageMatch(int surface_height);
    ~ImageMatch();
    int  do_match(Pixel32Bit* src, int src_x, int src_y, int src_width, int src_height, 
                  Pixel32Bit* dest, int dest_x, int dest_y, int dest_width, int dest_height);
private:
    int  max_height;
    int  horizontal_jump;
    int  vertical_jump;
    int* match_table_up;
    int* match_table_down;
    int  current_x;
    int  current_y;
    Pixel32Bit* current_pixel;
    void reset(void);
    void match_line(Pixel32Bit* current_pixel, Pixel32Bit* dest_start, Pixel32Bit* dest_end, int dest_y, int dest_width);
    bool pixel_same(Pixel32Bit* pixel_a, Pixel32Bit* pixel_b);
    int  compute_vector(void);
};


#endif // __IMAGE_MATCH_HPP_
