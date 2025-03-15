#include "decode.h"

Status Decoding::read_and_validate_encode_args(char* argv[])
{
    // Get .bmp file - ./a.out -d beautiful.bmp(encoded image)
    fs::path srcImagePath;
    std::string extension;
    if (argv[2] != nullptr) {
        srcImagePath = argv[2];
        extension = srcImagePath.extension().string();
        if (!extension.empty() && extension == ".bmp") {
            m_DecInfo->src_image_fname = srcImagePath.string();
        }
        else {
            return d_failure;
        }
    }
    else {
        return d_failure;
    }
    
    // Get .txt file - ./a.out -e beautiful.bmp secret.txt(optional)
    if (argv[3] != nullptr) {
        srcImagePath = argv[3];
        extension = srcImagePath.extension().string();
        if (!extension.empty() && extension == ".txt") {
            m_DecInfo->secret_fname = srcImagePath.string();
        }
        else {
            return d_failure;
        }
    }
   
    return d_success;
}

Status Decoding::open_files()
{
    // Src Image file
    m_DecInfo->fstream_src_image.open(m_DecInfo->src_image_fname, std::ios::in | std::ios::binary);

    // Do Error handling
    if (!m_DecInfo->fstream_src_image.is_open())
    {
        std::cerr << "fopen\n";
        std::cerr << "ERROR: Unable to open file " << m_DecInfo->src_image_fname << '\n';
        return d_failure;
    }

    //NOTE:opening of secret file handling saparatly 
    return d_success;

}

Status Decoding::decode_magic_string()
{
    std::string magicString;
    //move file pointer to 54th pos, 54 bytes are header size of bmp image
    m_DecInfo->fstream_src_image.seekg(54, std::ios::beg);

    decode_data_from_image(magicString, strlen(MAGIC_STRING));

    if (magicString.compare(MAGIC_STRING) != 0) {
        std::cerr << "ERROR: MagicString is not equal\n";
        return d_failure;
    }
    return d_success;
}

Status Decoding::decode_data_from_image(std::string& data, uint size)
{
    char ch{};
    for (uint i{}; i < size; i++) {
        //read 8 bytes from beautiful.bmp and store it in image_data
        m_DecInfo->fstream_src_image.read(m_DecInfo->image_data, 8);
        //decode the data
        decode_byte_form_lsb(ch, m_DecInfo->image_data);
        data.push_back(ch);
    }
    return d_success;
}

Status Decoding::decode_byte_form_lsb(char& ch, char* image_buffer)
{
    ch = 0;
    for (int i{}; i < 8; i++) {
        ch = ( ch | ((image_buffer[i] & 1) << (7-i)) );
    }
    return d_success;
}

Status Decoding::decode_secret_file_extn_size()
{
    //decode secret file extension size
    m_DecInfo->secret_fname_extension_size = 0;

    std::string extSize;
    decode_data_from_image(extSize, 4); //4- int 
    for (int i = 0; i < 4; i++)
    {
        m_DecInfo->secret_fname_extension_size = (m_DecInfo->secret_fname_extension_size | ((int)extSize[i] << (8 * i)));
    }

    if (m_DecInfo->secret_fname_extension_size > MAX_FILE_SUFFIX)
    {
        return d_failure;
    }

    return d_success;
}

Status Decoding::decode_secret_file_extn()
{
    m_DecInfo->secret_file_extension.clear();

    decode_data_from_image(m_DecInfo->secret_file_extension, m_DecInfo->secret_fname_extension_size);
    
    return d_success;
}

Status Decoding::decode_secret_file_size()
{
    m_DecInfo->secret_file_size = 0;
    
    std::string fileSize;
    decode_data_from_image(fileSize, 4); //4- int 
    for (int i = 0; i < 4; i++)
    {
        m_DecInfo->secret_file_size = (m_DecInfo->secret_file_size | ((int)fileSize[i] << (8 * i)));
    }

    return d_success;
}

Status Decoding::open_secret_file()
{
    if(m_DecInfo->secret_fname.empty()) {   //if file not given , default create a file decodedSecret.txt
       fs::path utilsDir = UTILS_DIR;
       std::string filename = "decodedSecret" + m_DecInfo->secret_file_extension;
       fs::path secretFilePath = utilsDir / filename;

       // Ensure that the "utils" directory exists
       if (!fs::exists(utilsDir)) {
           fs::create_directory(utilsDir);
       }
       m_DecInfo->secret_fname = secretFilePath.string();
    }

    //openfile
    m_DecInfo->fstream_secret.open(m_DecInfo->secret_fname, std::ios::out);

    if (!m_DecInfo->fstream_secret.is_open())
    {
        std::cerr << "fopen\n";
        std::cerr << "ERROR: Unable to open file " << m_DecInfo->secret_fname << '\n';
        return d_failure;
    }

    return d_success;
}

Status Decoding::decode_secret_file_data()
{
    char ch{};
    for (int i{}; i < m_DecInfo->secret_file_size; i++) {
        //read 8 bytes from .bmp and store it in image_data
        m_DecInfo->fstream_src_image.read(m_DecInfo->image_data, 8);
        //decode the data
        decode_byte_form_lsb(ch, m_DecInfo->image_data);

        m_DecInfo->fstream_secret.write(&ch, 1);
    }
    return d_success;
}

Status Decoding::do_decoding()
{
    //rest of the decoding function
    //open files
    if (open_files() == d_success) {
        std::cout << "Opened all the required Files\n";
        //Decode magic string
        if (decode_magic_string() == d_success) {
            std::cout << "Decoded magic string Done\n";
            //decode secret file extension size
            if (decode_secret_file_extn_size() == d_success) {
                std::cout << "Decoded secret file extension size Done\n";
                //decode secret file extension itself
                if (decode_secret_file_extn() == d_success) {
                    std::cout << "Decoded secret file extension Done\n";
                    //decode secret file size
                    if (decode_secret_file_size() == d_success) {
                        std::cout << "Decoded secret file size Done\n";
                        //open secret file to decode
                        if (open_secret_file() == d_success) {
                            std::cout << "opening secret file Done\n";
                            if (decode_secret_file_data() == d_success){
                                std::cout << "decoded secret file data\n";
                                return d_success;
                            }else {
                                std::cerr << "Failed to decode secret file data\n";
                                return d_failure;
                            }
                        }else {
                            std::cerr << "Failed to open secret file\n";
                            return d_failure;
                        }
                    }else {
                        std::cerr << "Failed to decode secret file size\n";
                        return d_failure;
                    }
                }else {
                    std::cerr << "Failed to decode secret file extension\n";
                    return d_failure;
                }
            }else {
                std::cerr << "Failed to decode secret file extension size\n";
                return d_failure;
            }
        }else {
            std::cerr<<"Failed to decode magic string\n";
            return d_failure;
        }
    }else {
        std::cerr << "Failed to open required files\n";
        return d_failure;
    }
    return d_success;
}


