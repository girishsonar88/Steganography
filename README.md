# Image Steganography Using LSB Encoding and Decoding

This project implements a basic image steganography technique using Least Significant Bit (LSB) encoding and decoding. The purpose of this project is to hide a secret message (text) inside an image, which can then be extracted using the decoding process.

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [How It Works](#how-it-works)
- [Installation](#installation)

## Introduction

Steganography is the practice of concealing information within another file or medium. In this project, a secret message is encoded into the least significant bits of an image's pixel values, making it nearly invisible to the human eye. The secret message can later be extracted using the decoding process.

The encoding process alters the least significant bits of an image's pixel values, while the decoding process reverses the encoding to reveal the hidden message.

This project is implemented in **C++** using basic file I/O and image handling techniques without the use of external libraries like OpenCV.

## Features

- **LSB Encoding**: Hide secret text messages inside an image.
- **LSB Decoding**: Extract the hidden message from the image.
- **Support for BMP Images**: This project works with `.bmp` image files.
- **Default Secret File**: If no secret file is provided, a default output file (`decodedSecret.txt`) is created, while 
Decoding.
- **Magic String**: Both side a Common Magic String is needed, to encode and to decode the data, it acts as password.

## How It Works

### LSB Encoding:
1. **Text to Binary**: The secret text message is converted into its binary form.
2. **Modify Image**: For each pixel in the image, the least significant bit (LSB) is replaced with a bit from the binary message.
3. **Save Image**: The modified image is saved, and it now contains the hidden message.

### LSB Decoding:
1. **Extract LSBs**: The program reads the LSB from each pixel of the image.
2. **Reconstruct Message**: The extracted bits are used to reconstruct the original message in binary, which is then converted back to text.
3. **Output**: The hidden message is written to a text file.

## Installation

### Prerequisites:
- C++ compiler (e.g., GCC or MSVC), CMake.

### Building steps

1. **Clone the repository:**
   First, clone the project repository to your local machine by running the following command:
   ```bash
   git clone https://github.com/girishsonar88/Steganography.git

   cd Steganography
   mkdir build
   cd build
   make

   output exe will be generated at utils
2. **ENCODING:**
  ./StagnoGraphy.exe -e <image_file> <secret_message_file> <optional_output_image_filename>
  ./StagnoGraphy.exe -e beautiful.bmp secret.txt stago.bmp(optional)
3. **DECODING:**
  StagnoGraphy.exe -d <encoded_image_file> <optional_outputfilename>
  ./StagnoGraphy.exe -d stago.bmp decoded.txt(optional)
