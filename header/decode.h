#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

typedef struct _DecodeInfo {
    /* Source Image info */
    std::string src_image_fname;
    std::ifstream fstream_src_image;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    std::string secret_fname;
    std::ofstream fstream_secret;
    uint secret_file_size;
    uint secret_fname_extension_size;
    std::string secret_file_extension;

}DecodeInfo;

class Decoding {
private:
	DecodeInfo* m_DecInfo;

public:
    Decoding() {
        m_DecInfo = new DecodeInfo();
    }
    ~Decoding() {
        if (m_DecInfo) {
            delete m_DecInfo;
        }
    }
    /* Encoding function prototype */

    /* Read and validate Encode args from argv */
    Status read_and_validate_encode_args(char* argv[]);

    /* Perform the encoding */
    Status do_decoding();

    /* Get File pointers for i/p and o/p files */
    Status open_files();

    /*  Magic String */
    Status decode_magic_string();

    /* Decode function, which does the real Decoding */
    Status decode_data_from_image(std::string& data, uint size);

    /* Decode a byte from LSB of image data array */
    Status decode_byte_form_lsb(char& ch, char* image_buffer);

    /* Decode secret file extenstion size*/
    Status decode_secret_file_extn_size();

    /* Decode secret file extenstion */
    Status decode_secret_file_extn();

    /* Decode secret file size */
    Status decode_secret_file_size();

    /*Secret file open if not given create default*/
    Status open_secret_file();

    /*Decode Secret File data*/
    Status decode_secret_file_data();
};

#endif