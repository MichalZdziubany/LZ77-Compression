# LZ77 Compression Tool

A C-based command-line utility for lossless text file compression and decompression using the LZ77 algorithm. Developed as part of a third-year final project for the BSc in Computing in Software Development.

---

## Project Overview

This tool implements the LZ77 compression technique, which identifies repeated sequences in data and replaces them with back-references to reduce the file size. It supports compressing and decompressing `.txt` files using a sliding window mechanism.

---

## Features

- Compress any text file using the LZ77 algorithm.
- Decompress `.lz77` binary files back to original content.
- Lightweight and written in pure C.
- Handles files of varying sizes with dynamic memory allocation.
- Includes debugging output for token tracking.

---

## Technologies Used

- **Language**: C
- **Compiler**: GCC
- **IDE**: Visual Studio Code
- **Version Control**: Git
- **Libraries**: Standard C libraries (`stdio.h`, `stdlib.h`, `string.h`)

---

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/MichalZdziubany/LZ77-Compression.git
   cd LZ77-Compression
   ```

2. Compile the code:
   ```bash
   gcc Lz77.c -o lz77
   ```

3. Run the program:
   ```bash
   ./lz77
   ```

---

## Example Usage

## Compress a file:
```bash
Choose an option:
1. Compress
2. Decompress
> 1
Enter the name of the file to compress: sample.txt
Enter the name of the output file: compressed.lz77
```

## Decompress a file:
```bash
Choose an option:
1. Compress
2. Decompress
> 2
Enter the name of the file to decompress: compressed.lz77
Enter the name of the output file: output.txt
```

---

## Author

**Michal Zdziubany**  
Student ID: G00419292  
Supervised by: Ian McLoughlin  
BSc in Computing in Software Development – Year 3

---