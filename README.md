# Assembler-C

Assembler-C is a project that provides an implementation of an assembler for a hypothetical assembly language, written in C. This project was developed in collaboration with a fellow student as part of a data science workshop.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Examples](#examples)
- [Contributing](#contributing)
- [License](#license)

## Introduction

Assembler-C is a tool designed to convert assembly language source code into machine code. The project aims to provide a clear understanding of the process of assembly, parsing, and code generation. This implementation was created to facilitate learning and experimentation with assembler concepts.

## Features

- **Assembly Code Parsing**: Parses assembly language instructions and labels.
- **Symbol Table Management**: Maintains and resolves labels and symbols.
- **Code Generation**: Translates assembly instructions into machine code.
- **Error Handling**: Detects and reports errors in assembly code.

## Installation

To run Assembler-C, you'll need to have a C compiler installed. Follow these steps to set up the project:

1. Clone the repository:
   ```bash
   git clone https://github.com/Shay-Gabison/Assembler-C.git
   ```

2. Navigate to the project directory:
   ```bash
   cd Assembler-C
   ```

3. Compile the code:
   ```bash
   gcc -o assembler assembler.c
   ```

## Usage

To use Assembler-C, provide the assembler source file as an argument to the executable:

```bash
./assembler source.asm
```

The assembler will generate an output file with the machine code.

## Examples

Here are some example assembly source files and their corresponding outputs:

- **Example 1**: A simple program that adds two numbers.
- **Example 2**: A program that demonstrates label usage and jumps.

## Contributing

This project was developed in collaboration with a fellow student as part of a data science workshop. Contributions to the project are welcome. Please follow these guidelines:

1. Fork the repository.
2. Create a feature branch.
3. Commit your changes.
4. Push to the branch.
5. Create a pull request.

