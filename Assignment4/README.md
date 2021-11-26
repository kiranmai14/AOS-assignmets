## Assignment 4
### Inode base filesystem implementation
.

.
#### How to execute

```g++ main.cpp -o main```

```./main```



#### Operations on the disk
 1. create disk : Creates an empty disk of size 512MB.
 2. mount disk : Opens the specified disk for various file operations.
 3. exit :  Close the application.

 #### Operations on the files present in the mounted disk

 1. create file : creates an empty text file.
2. open file : opens a particular file in read/write/append mode as specified in input,
multiple files can be opened simultaneously.
    - 0 : read mode
    - 1 : write mode
    - 2 : append mode
3. read file : Displays the content of the file.
4. write file : Write fresh data to file(override previous data in file).
5. append file : Append new data to an existing file data.
6. close file : Closes the file.
7. delete file : Deletes the file.
8. list of files : List all files present in the current disk.
9. list of opened files : List all opened files and specify the mode they are open in.
10.unmount : Closes the currently mounted disk.


