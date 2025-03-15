#include "encode.h"
Status Encoding::read_and_validate_encode_args(char* argv[])
{
    // Get .bmp file - ./a.out -e beautiful.bmp
    fs::path srcImagePath;
    std::string extension;
    if (argv[2] != nullptr) {
        srcImagePath = argv[2];
        extension = srcImagePath.extension().string();
        if (!extension.empty() && extension == ".bmp") {
            m_EncInfo->src_image_fname = srcImagePath.string();
        }else {
            return e_failure;
        }
    }
    else {
        return e_failure;
    }

    // Get .txt file - ./a.out -e beautiful.bmp secret.txt
    if (argv[3] != nullptr) {
        srcImagePath =argv[3];
        extension = srcImagePath.extension().string();
        if (!extension.empty() && extension == ".txt") {
            m_EncInfo->secret_fname = srcImagePath.string();
        }else {
            return e_failure;
        }
    }
    else {
        return e_failure;
    }

    // Get stago.bmp file - ./a.out -e beautiful.bmp secret.txt stago.bmp (optional)
    if (argv[4] != nullptr) {
        srcImagePath = argv[4];
        extension = srcImagePath.extension().string();
        if (!extension.empty() && extension == ".bmp") {
            m_EncInfo->stego_image_fname = srcImagePath.string();
        }
        else {
            return e_failure;
        }
    }
    else {
        fs::path utilsDir = UTILS_DIR;
        fs::path secretFilePath = utilsDir / "stago.bmp";
        
        // Ensure that the "utils" directory exists
        if (!fs::exists(utilsDir)) {
            fs::create_directory(utilsDir);
        }
        m_EncInfo->stego_image_fname = secretFilePath.string();
    }

    return e_success;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: fstream variables for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status Encoding::open_files()
{
    // Src Image file
    m_EncInfo->fstream_src_image.open(m_EncInfo->src_image_fname, std::ios::in | std::ios::binary);

    // Do Error handling
    if (!m_EncInfo->fstream_src_image.is_open())
    {
        std::cerr<<"fopen\n"; 
        std::cerr<<"ERROR: Unable to open file "<< m_EncInfo->src_image_fname<<'\n';
        return e_failure;
    }

    // Secret file
    m_EncInfo->fstream_secret.open(m_EncInfo->secret_fname, std::ios::in);
    // Do Error handling
    if (!m_EncInfo->fstream_secret.is_open())
    {
        std::cerr <<"fopen\n";
        std::cerr <<"ERROR: Unable to open file "<< m_EncInfo->secret_fname<<'\n';
        return e_failure;
    }

    //also hold secret file extension size
    fs::path srcImagePath(m_EncInfo->secret_fname);
    std::string extension = srcImagePath.extension().string();
    if (extension.empty()) {
        std::cerr << "Error: File does not have an extension: " << m_EncInfo->secret_fname << std::endl;
        return e_failure;
    }
    m_EncInfo->secret_fname_extension_size = extension.size();
    
    // Stego Image file
    m_EncInfo->fstream_stego_image.open(m_EncInfo->stego_image_fname, std::ios::out | std::ios::binary);
    // Do Error handling
    if (!m_EncInfo->fstream_stego_image.is_open())
    {
        std::cerr <<"fopen\n";
        std::cerr <<"ERROR: Unable to open file "<< m_EncInfo->stego_image_fname<<'\n';
        return e_failure;
    }

    return e_success;
}

Status Encoding::check_capacity()
{
    uint bmp_img_size = get_image_size_for_bmp();
    if (bmp_img_size == e_failure) {
        return e_failure;
    }
    m_EncInfo->image_capacity = bmp_img_size;
    m_EncInfo->size_secret_file = get_file_size();

    //header data-54, magic string length ,secretfileExtensionSize-4(to hold extensionsize 4byte enough),
    //secretfileExtensionlength(real extension length of secretfile(m_EncInfo->secret_fname_extension_size)),
    //, secretfilesize-4(to hold secret file size 4byte is enough),
    if (m_EncInfo->image_capacity > (54 + (strlen(MAGIC_STRING) + 4 + m_EncInfo->secret_fname_extension_size + 4 + m_EncInfo->size_secret_file) * 8)) {
        return e_success;
    }else{
        return e_failure;
    }
}

/* Get image size
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint Encoding::get_image_size_for_bmp()
{
    uint width{}, height{};
    // Seek to 18th byte
    m_EncInfo->fstream_src_image.seekg(18, std::ios::beg);

    if (m_EncInfo->fstream_src_image.fail()) {
        std::cerr << "ERROR: seeking the positon!\n";
        return e_failure;
    }

    // Read the width (4 bytes)
    m_EncInfo->fstream_src_image.read(reinterpret_cast<char*>(&width), sizeof(uint));

    if (m_EncInfo->fstream_src_image.fail()) {
        std::cerr << "ERROR: Reading the width failed!\n";
        return e_failure;
    }

    // Read the height (4 bytes)
    m_EncInfo->fstream_src_image.read(reinterpret_cast<char*>(&height), sizeof(uint));
    if (m_EncInfo->fstream_src_image.fail()) {
        std::cerr << "ERROR: Reading the height failed!\n";
        return e_failure;
    }


    // Seek to the 28th byte for the bits per pixel value
    m_EncInfo->fstream_src_image.seekg(28, std::ios::beg);

    if (m_EncInfo->fstream_src_image.fail()) {
        std::cerr << "ERROR: Seeking to bits per pixel position failed!\n";
        return e_failure;
    }

    // Read the bits per pixel (2 bytes)
    m_EncInfo->fstream_src_image.read(reinterpret_cast<char*>(&m_EncInfo->bits_per_pixel), sizeof(uint16_t));

    if (m_EncInfo->fstream_src_image.fail()) {
        std::cerr << "ERROR: Reading bits per pixel failed!\n";
        return e_failure;
    }

    // Return image capacity
    return width * height * (m_EncInfo->bits_per_pixel / 8);
    //if CMYK --> 
}

uint Encoding::get_file_size()
{
    m_EncInfo->fstream_secret.seekg(0, std::ios::end);
    return m_EncInfo->fstream_secret.tellg();
}

Status Encoding::copy_bmp_header()
{
    char header_data[54];

    m_EncInfo->fstream_src_image.seekg(0, std::ios::beg);
    m_EncInfo->fstream_src_image.read(header_data, 54);
    m_EncInfo->fstream_stego_image.write(header_data, 54);

    return e_success;
}

Status Encoding::encode_magic_string()
{
    //every encoding will call encode_data_to_image
    encode_data_to_image(MAGIC_STRING, strlen(MAGIC_STRING));
    return e_success;
}

Status Encoding::encode_data_to_image(const std::string data, uint size)
{
    for (uint i{}; i < size; i++){
        //read 8 bytes from beautiful.bmp and store it in image_data
        m_EncInfo->fstream_src_image.read(m_EncInfo->image_data, 8);
        //encode the data
        encode_byte_to_lsb(data[i], m_EncInfo->image_data);
        m_EncInfo->fstream_stego_image.write(m_EncInfo->image_data, 8);
    }


    return e_success;
}

Status Encoding::encode_byte_to_lsb(char data, char* image_buffer)
{
    uint mask = 1 << 7;
    for (int i{}; i < 8; i++) {
        //image_buffer[i] - fetching each byte from source .bmp image 
        //0XFE used to clear LSB bit of image_buffer[i]
        image_buffer[i] = ((image_buffer[i] & 0xFE) | ((data & mask) >> (7 - i)));
        mask = mask >> 1;
    }
    return e_success;
}

Status Encoding::encode_size()
{
    //encode secret file extension size
    char str[32];   //to encode 4 bytes
    m_EncInfo->fstream_src_image.read(str, 32);
    encode_size_to_lsb(str, m_EncInfo->secret_fname_extension_size);
    m_EncInfo->fstream_stego_image.write(str, 32);
    return e_success;
}
Status Encoding::encode_size_to_lsb(char* buffer, uint size)
{
    uint mask = 1 << 7;
    for (int i = 0; i < 32; i++){
        buffer[i] = ((buffer[i] & 0xFE) | ((size & mask) >> (7 - i)));
        mask = mask >> 1;
    }
    return e_success;
}
Status Encoding::encode_secret_file_extn()
{
    std::string extension = ".txt";  //default
    fs::path srcImagePath(m_EncInfo->secret_fname);
    extension = srcImagePath.extension().string();
    if (extension.empty()) {
        std::cerr << "Error: File does not have an extension: " << m_EncInfo->secret_fname << std::endl;
        return e_failure;
    }
    encode_data_to_image(extension, extension.size());
    return e_success;
}
Status Encoding::encode_secret_file_size(uint file_size)
{
    //encode secret file size
    char str[32];   //to encode 4 bytes
    m_EncInfo->fstream_src_image.read(str, 32);
    encode_size_to_lsb(str, file_size);
    m_EncInfo->fstream_stego_image.write(str, 32);
    return e_success;
}
Status Encoding::encode_secret_file_data()
{
    char ch;
    m_EncInfo->fstream_secret.seekg(0, std::ios::beg);  //move cursor to starting in secret file 
    for (int i{}; i < m_EncInfo->size_secret_file ; i++) {
        //read 8 bytes from beautiful.bmp and store it in image_data
        m_EncInfo->fstream_src_image.read(m_EncInfo->image_data, 8);
        m_EncInfo->fstream_secret.read(&ch, 1);
        //encode the data
        encode_byte_to_lsb(ch, m_EncInfo->image_data);
        m_EncInfo->fstream_stego_image.write(m_EncInfo->image_data, 8);
    }
    return e_success;
}

Status Encoding::copy_remaining_img_data()
{
    char ch;
    while (m_EncInfo->fstream_src_image.read(&ch, 1)) {
        m_EncInfo->fstream_stego_image.write(&ch, 1);
    }
    return e_success;
}
Status Encoding::do_encoding()
{
    //rest of the encoding function
    //open files
    if (open_files() == e_success){
        std::cout<<"Opened all the required Files\n";
        //check capacity
        if (check_capacity() == e_success){
            std::cout<<"Check capacity is success..Can proceed to Encode the data\n";
            //copy header info from src to dst
            if (copy_bmp_header() == e_success){
                std::cout << "Copied Header successfully\n";
                //Encode magic string
                if (encode_magic_string() == e_success){
                    std::cout << "Encoded magic string successfully\n";
                    //encode secret file extension size
                    if (encode_size() == e_success) {
                            std::cout << "Encode secret file extension size successful\n";
                            //Encode secret file extension
                            if (encode_secret_file_extn() == e_success){
                                std::cout << "Encode secret file extension successful\n";
                                //encode secret file size
                                if (encode_secret_file_size(m_EncInfo->size_secret_file) == e_success){
                                    std::cout<<"Successfully encoded secret file size\n";
                                    //encode secret file data
                                    if (encode_secret_file_data() == e_success){
                                        std::cout << "Successfully encoded secret data.... :)\n";
                                        //Copy remaining image bytes from src to stego image after encoding
                                        if (copy_remaining_img_data() == e_success){
                                            std::cout<<"Copied remaining bytes successfully.. :)\n";
                                        }else {
                                            std::cerr<<"Failed to copy remaining bytes.\n";
                                            return e_failure;
                                        }
                                    }else {
                                        std::cerr<<"Failed encoding secret data.. :( \n";
                                        return e_failure;
                                    }
                                }else {
                                    std::cerr<<"Failed encoding secret file size\n";
                                    return e_failure;
                                }
                            }else {
                                std::cerr<<"Encode secret file extension Unsuccessful\n";
                                return e_failure;
                            }
                    }else {
                        std::cerr << "Failed Encoding secret file extension size\n";
                        return e_failure;
                    }
                }else {
                    std::cerr << "Falied to Encode magic string\n";
                    return e_failure;
                }
            }else {
                std::cerr << "Failure: Copying Header\n";
                return e_failure;
            }
        }else {
            std::cerr << "Not possible to encode the data\n";
            return e_failure;
        }
    }else {
        std::cerr << "Falied opening the Files\n";
        return e_failure;
    }
    return e_success;
}
