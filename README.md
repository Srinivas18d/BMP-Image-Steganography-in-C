# BMP Image Steganography in C

A C-based image steganography project that hides and extracts secret files inside BMP images using **Least Significant Bit (LSB) encoding**.

The project demonstrates low-level file handling, binary data manipulation, bitwise operations, pointers, structures, and modular programming in C.

## Features

* Hide secret files inside BMP images using LSB steganography
* Extract hidden files from stego images
* Support for multiple secret file formats
* Checks image capacity before encoding
* Stores the secret file extension and size inside the image
* Uses a magic string to identify encoded images
* Preserves the BMP header during encoding
* Handles encoding and decoding errors
* Modular implementation using C source and header files

## How It Works

The project uses **Least Significant Bit (LSB) steganography**.

The least significant bits of the image's pixel data are modified to store the secret information. Since only the lowest bit is changed, the modification is generally not visually noticeable.

### Example

```text
Original pixel byte : 10110100
Secret data bit     :        1
Encoded pixel byte  : 10110101
```

The encoded image contains the following information:

```text
+-------------------------+
| BMP Header              |
+-------------------------+
| Magic String            |
+-------------------------+
| Secret File Extension   |
+-------------------------+
| Secret File Size        |
+-------------------------+
| Secret File Data        |
+-------------------------+
| Remaining Image Data    |
+-------------------------+
```

## Encoding Process

```text
Source BMP Image
       |
       v
Check Image Capacity
       |
       v
Copy BMP Header
       |
       v
Encode Magic String
       |
       v
Encode Secret File Extension
       |
       v
Encode Secret File Size
       |
       v
Encode Secret File Data
       |
       v
Copy Remaining Image Data
       |
       v
Stego BMP Image
```

## Decoding Process

```text
Stego BMP Image
       |
       v
Read Magic String
       |
       v
Validate Encoded Image
       |
       v
Decode Secret File Extension
       |
       v
Decode Secret File Size
       |
       v
Extract Secret File Data
       |
       v
Recovered Secret File
```

## Technologies Used

* **Language:** C
* **Image Format:** BMP
* **Technique:** LSB Steganography
* **Compiler:** GCC

### Concepts Used

* File handling
* Binary file I/O
* Bitwise operations
* Pointers
* Structures
* Command-line arguments
* Error handling
* Modular programming
* Dynamic data processing

## Project Structure

```text
BMP-Image-Steganography-in-C/
│
├── main.c
├── encode.c
├── encode.h
├── decode.c
├── decode.h
├── common.h
├── types.h
│
├── beautiful.bmp
├── secret.txt
│
├── README.md
└── .gitignore
```

### Source Files

| File       | Description                                                   |
| ---------- | ------------------------------------------------------------- |
| `main.c`   | Handles command-line arguments and controls encoding/decoding |
| `encode.c` | Implements the encoding process                               |
| `encode.h` | Encoding structures and function declarations                 |
| `decode.c` | Implements the decoding process                               |
| `decode.h` | Decoding structures and function declarations                 |
| `common.h` | Common constants and definitions                              |
| `types.h`  | User-defined data types and status values                     |

## Compilation

Compile the project using GCC:

```bash
gcc main.c encode.c decode.c -o stego
```

For additional compiler warnings:

```bash
gcc -Wall -Wextra -std=c11 main.c encode.c decode.c -o stego
```

## Usage

### Encode a Secret File

```bash
./stego -e <source.bmp> <secret_file> <output.bmp>
```

Example:

```bash
./stego -e beautiful.bmp secret.txt stego.bmp
```

This embeds `secret.txt` inside `beautiful.bmp` and generates `stego.bmp`.

### Decode a Secret File

```bash
./stego -d <stego.bmp>
```

Example:

```bash
./stego -d stego.bmp
```

The decoder extracts the hidden file from the stego image.

## Example Workflow

### Step 1 — Compile

```bash
gcc -Wall -Wextra -std=c11 main.c encode.c decode.c -o stego
```

### Step 2 — Encode

```bash
./stego -e beautiful.bmp secret.txt stego.bmp
```

### Step 3 — Decode

```bash
./stego -d stego.bmp
```

The hidden file is reconstructed from the encoded BMP image.

## Supported Files

The implementation can work with supported secret-file extensions such as:

```text
.txt
.pdf
.docx
.png
.jpg
```

The carrier image is a BMP file.

## Limitations

* The current implementation is designed for BMP images.
* Image capacity limits the maximum size of the secret file.
* Modifying or recompressing the image can destroy the hidden data.
* LSB steganography hides data but does **not provide encryption**.
* The implementation is primarily intended for learning and demonstration.

## Security Note

This project provides **data hiding, not cryptographic security**.

For sensitive information, the secret file should be encrypted before being embedded into the image.

## Learning Outcomes

This project provided practical experience with:

* Binary file handling in C
* BMP image data processing
* Bitwise manipulation
* LSB encoding and decoding
* File pointers
* Structures and pointers
* Command-line arguments
* Modular C programming
* Error handling

## Future Improvements

* Add encryption before embedding
* Support additional image formats
* Add password-based encryption
* Improve BMP format validation
* Add automated testing
* Add support for different BMP bit depths
* Add a graphical user interface
* Improve portability across operating systems

## Author

**Srinivas D**

Electronics & Communication Engineering
