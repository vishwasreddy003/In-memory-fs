# File System Simulation

This program simulates a basic file system with functionalities such as creating directories, navigating through directories, listing directory contents, creating files, displaying file contents, moving and copying files/directories, and removing files/directories.

## Implemented Functionalities

1. **cd (Change Directory):**
   - Change the current working directory.
   - Usage: `cd <directory_path>` or `cd ..` to go up one level.

2. **mkdir (Make Directory):**
   - Create a new directory.
   - Usage: `mkdir <directory_name>`.

3. **ls (List):**
   - List the contents of the current directory or a specified directory.
   - Usage: `ls [<directory_name>]`.

4. **touch:**
   - Create a new empty file.
   - Usage: `touch <file_name>`.

5. **cat:**
   - Display the contents of a file.
   - Usage: `cat <file_name>`.

6. **mv (Move):**
   - Move a file or directory to another location.
   - Usage: `mv <source_path> <destination_path>`.

7. **cp (Copy):**
   - Copy a file or directory to another location.
   - Usage: `cp <source_path> <destination_path>`.

8. **rm (Remove):**
   - Remove a file or directory.
   - Usage: `rm <target_path>`.

## Improvements Made

1. **Enum for Node Types:**
   - Used an enumeration (`NodeType`) for node types instead of using characters to improve code readability and avoid conflicts.

2. **Improved Error Handling:**
   - Added more informative error messages for better user feedback.
   - Validated user input more thoroughly to prevent potential runtime errors.

3. **Modularized Code:**
   - Broke down the code into smaller functions for better readability and maintainability.

4. **Memory Management:**
   - Used smart pointers (`std::unique_ptr`) for managing memory to reduce the risk of memory leaks.

5. **Additional Commands:**
   - Added `mv` (Move), `cp` (Copy), and `rm` (Remove) functionalities.

## How to Run

1. **Compilation:**
   - Compile the program using a C++ compiler. For example, using g++:
     ```bash
     g++ -o filesystem_simulation main.cpp
     ```

2. **Run the Program:**
   - Execute the compiled program:
     ```bash
     ./filesystem_simulation
     ```

3. **Usage:**
   - Enter commands at the prompt. Supported commands include `cd`, `mkdir`, `ls`, `touch`, `cat`, `mv`, `cp`, `rm`, and `exit`.

4. **Testing:**
   - Test various commands to explore and manipulate the file system.

## Example Usage

```bash
$ ./filesystem_simulation
/ > mkdir documents
/ > cd documents
/documents > touch file.txt
/documents > ls
file.txt -
/documents > cat file.txt
[Content of file.txt]
/documents > cd ..
/ >
