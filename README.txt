Disk file system stimulation
Authored by Coral Bahofrker


==Description==
The code file contains 3 classes which are:

1. fsInode -> Its function is to keep the block numbers are stored in a file + getters & setters + private methods.

2. FileDescriptor -> Saves a filename and a pointer to the file's inode.

3. fsDisk -> the disk itself.

The purpose of the program is to simulate the management of the disk file system.
Our disk is a file containing up to 512 inclusive.
The file system contains only one folder and all files were created under this folder.
The user enters options between 1 and 10, in an infinite loop, where each case (1-10) represents an action that needs to be performed on the disk:

case 0 -> exit. treminate the program and call the destructor.

case 1 -> print the disk content, calling listAll method.

case 2 -> Formate, initialize the disk & check whether the input block size is legal.

case 3 -> CreateFile, the function receives the name of the file that we want to create, performs tests on the input (if it already exists, if it is not empty, etc.) and creates a new file on the disk, 
          taking into account finding free space first and only if there is no space then we will add a new space.

case 4 -> OpenFile,the function receives the name of the file we want to open and checks several cases (is it already open, does it exist, etc.).
          After these tests, the function looks for the location of the file it must open and then looks for whether there is free space before opening it, 
          if there is not then it is opened in its original location. 

case 5 -> CloseFile,the function receives the fd of the file we want to close and checks, among other things, whether the file is already closed, whether it exists and whether the fd is valid at all.
          After these tests, if everything is correct, the function closes the file and marks it as unused. 

case 6 -> WriteToFile, the function receives the fd of the file we want to write to, the text we want to write and the length of our input.
		  we'll run in a loop and first, we will check and calculate our position in the file and the offset.
          After that we will check which block we are in and then we will check if the block is allocated or not.
          If the block is not allocated, then we will allocate it using the helper function blockallocation.
          After that, we will move our cursor to the appropriate position in the file and write one character at a time until we finish writing everything according to the position in each iteration of the loop.
          In the single and double block, we use auxiliary variables to direct us to which location we should go with the pointer/pointers and which block to access and then which physical location to write to.

case 7 -> ReadFromFile, the function receives the fd of the file we want to read from, the length we want to read and a buffer to read to it.
		  First the function performs various input checks to make sure everything is correct and legal.
          If so, we will run a loop over the length of the input, calculate the offset and its position in the file, then we will check each time which block we are in and move the name accordingly and read one character each time until we finish the loop.
          If the user asked to read more than what is written, then we will read what is possible.

case 8 -> DelFile, the function receives the name of the file that we want to delete from the disk.
          First the function performs the various tests in order to verify the correctness of the input.
          After that, we will run in a loop from the end to the beginning and check according to the calculation of the offset and the position in the file, the blocks used by the file and release the blocks and pointers accordingly.
          As in the writing function, here too In the single and double block, we use auxiliary variables to direct us to which location we should go with the pointer/pointers and which block to access and then which physical location to delete from
          At the end of the function, we will update the file name to be an empty string, its size to 0.

case 9 -> CopyFile, the function accepts the name of a destination file and a source file.
		  Performs initial input checks in order to avoid errors.
		  If everything is correct according to the requirements of the exercise, the function will find the location of the source file, check if it is closed and, if so, also check if the target file exists.
		  If there is, overwrite it.
		  If it does not exist, you will create a new file named name and copy the contents of the source file into it.
		  At the end of copying, both files are closed.

case 10 -> RenameFile, The function receives the name of the file you want to change and the new name you want to give it.
		   First we will perform initial input tests according to the requirements of the exercise.
           If everything is correct, the function will find the location of the file and save it, check if it is open or closed. If everything is closed fine and you can continue, 
           we will save the old name in the auxiliary variable, delete it from the MainDir and update the new name.

main functions:
1. listAll - print the disk content.
2. fsFormat - initialize the disk.
3. CreateFile - create a new file with given file name.
4. OpenFile - open an existing file.
5. CloseFile - close an existing file.
6. WriteToFile - write data to the file according to blocks calculation.
7. DelFile - free the file's blocks and pointers & set his name to an empty string & update his size to 0.
8. ReadFromFile - read data to the file according to blocks calculation.
9. GetFileSize - return the file's size.
10. CopyFile - create a copy of the file.
11. RenameFile - change the name of the file.
12. ~fsDisk - destructor.

==Program Files==
stub_code.cpp- the file contain the full program that runs the stimulation & its functions.

==How to compile?==
compile: gcc stub_code.cpp -o stub_code
run: ./stub_code

==Input:==
reads commands the user type.

==Output:==
Execute the disk file system commands.


