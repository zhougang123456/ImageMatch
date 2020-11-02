#ifndef __IMAGE_MATCH_HPP_
#define __IMAGE_MATCH_HPP_
#define IMAGE_HEIGHT_MAX 1080

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
    ImageMatch();
    ~ImageMatch();
    int do_match(Pixel32Bit* src, Pixel32Bit* dest, int width, int height);
private:
    int match_table_up[IMAGE_HEIGHT_MAX];
    int match_table_down[IMAGE_HEIGHT_MAX];
    int current_x;
    int current_y;
    Pixel32Bit* current_pixel;
    void reset(void);
    void match_line(Pixel32Bit* current_pixel, Pixel32Bit* dest_start, Pixel32Bit* dest_end, int width);
    bool pixel_same(Pixel32Bit* pixel_a, Pixel32Bit* pixel_b);
    int  compute_vector();
};


#endif // __IMAGE_MATCH_HPP_
