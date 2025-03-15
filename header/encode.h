#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" // Contains user defined types
/*
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

typedef struct _EncodeInfo
{
    /* Source Image info */
    std::string src_image_fname;
    std::ifstream fstream_src_image;
    uint image_capacity;
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    std::string secret_fname;
    std::ifstream fstream_secret;
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;
    uint secret_fname_extension_size;

    /* Stego Image Info */
    std::string stego_image_fname;
    std::ofstream fstream_stego_image;

} EncodeInfo;


class Encoding{
private:
    EncodeInfo* m_EncInfo;

public:
    Encoding() {
        m_EncInfo = new EncodeInfo();
    }
    ~Encoding() {
        if (m_EncInfo) {
            delete m_EncInfo;
        }
    }

    /* Encoding function prototype */

    /* Read and validate Encode args from argv */
    Status read_and_validate_encode_args(char* argv[]);

    /* Perform the encoding */
    Status do_encoding();

    /* Get File pointers for i/p and o/p files */
    Status open_files();

    /* check capacity */
    Status check_capacity();

    /* Get image size */
    uint get_image_size_for_bmp();

    /* Get file size */
    uint get_file_size();

    /* Copy bmp image header */
    Status copy_bmp_header();

    /* Store Magic String */
    Status encode_magic_string();

    /* Encode function, which does the real encoding */
    Status encode_data_to_image(const std::string data, uint size);

    /* Encode secret file extension size*/
    Status encode_size();

    /* Encode secret file size to lab*/
    Status encode_size_to_lsb(char* buffer, uint size);

    /* Encode secret file extenstion */
    Status encode_secret_file_extn();

    /* Encode secret file size */
    Status encode_secret_file_size(uint file_size);

    /* Encode secret file data*/
    Status encode_secret_file_data();

    /* Encode a byte into LSB of image data array */
    Status encode_byte_to_lsb(char data, char* image_buffer);

    /* Copy remaining image bytes from src to stego image after encoding */
    Status copy_remaining_img_data();

};

#endif
