#include <memory>
#include "types.h"
#include "encode.h"
#include "decode.h"

//#include "decode.h"

OperationType check_operation_type(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    //check operation type
    if (check_operation_type(argc, argv) == e_encode){
        std::unique_ptr<Encoding> encoding = std::make_unique<Encoding>();
        std::cout << "Selected Encoding\n";
        //read each required file extensions and if correct, store filename in structure
        if (encoding->read_and_validate_encode_args(argv) == e_success) {
            std::cout << "Read and validate arguments successful\n";
            //Perform Encoding
            if (encoding->do_encoding() == e_success) {
                std::cout << "Encoding is done successfully :)\n";
            }
            else {
                std::cerr << "Failed Encoding the Data\n";
            }
        }
        else {
            std::cerr << "Read and validate arguments unsuccessful\n";
        }
    }else if (check_operation_type(argc, argv) == e_decode)
    {
        printf("Selected Decoding\n");
        std::unique_ptr<Decoding> decoding = std::make_unique<Decoding>();
        //read each required file extensions and if correct, store filename in structure
        if (decoding->read_and_validate_encode_args(argv) == d_success) {
            std::cout << "Read and validate arguments successful\n";
            //Perform Encoding
            if (decoding->do_decoding() == d_success) {
                std::cout << "Decoding is done successfully :)\n";
            }
            else {
                std::cerr << "Failed Decoding the Data\n";
            }
        }
        else{
            printf("Read and validate rguments unsuccessful\n");
        }
    }else{
        std::cerr << "Usage:\nEncoding: ./a.out -e beautiful.bmp secret.txt stego.bmp(optional)" << std::endl;
        std::cerr<<"Decoding: ./a.out -d stago.bmp"<<std::endl;
    }

    return 0;
}

OperationType check_operation_type(int argc,char* argv[])
{
    if (argc < 2) {
        return e_unsupported;
    }
    //a.out -e (check for -e or -d or other)

    if (strcmp(argv[1], "-e") == 0){
        return e_encode;
    }else if (strcmp(argv[1], "-d") == 0){
        return e_decode;
    }else{
        return e_unsupported;
    }
}