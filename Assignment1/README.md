## Linux Based File Explorer


### Description of the project
Building a fully functional file explorer application, with restricted features.
The application supports two modes:
* NORMAL MODE
* COMMAND MODE

### Normal Mode
* Displays all the directories and files present in current folder.
* Informations displayed: file name, file size, Ownership (user and group) and Permissions , last modified (in human readable format)
* Key presses and their functionalities:
    * `LEFT ARROW KEY`: Takes to the previously visited directory
    * `RIGHT ARROW KEY`: Takes to next directory
    * `ENTER KEY`: When user presses it, the file/directory at the cursor opens
    * `BACKSPACE KEY`: Takes user up to one level
    * `HOME KEY`: Takes user back to directory where the place has started
    * `UP and DOWN keys`: used for moving cursor up and down to desired file or directory
    * Pressing `:` will take you to command mode
    * Pressing `q` will close the application


### Command Mode
This mode is entered from Normal mode whenever `:` is pressed.
#### Following commands are supported:
* COPY: `copy <source_file(s)> <destination_directory>`
* MOVE: `move <source_file(s)> <destination_directory>`
* RENAME: `rename <old_filename> <new_filename>`
* CREATE FILE: `create_file <file_name> <destination_path>`
* CREATE DIRECTORY: `create_dir <diectory_name> <destination_path>`
* DELETE FILE: `delete_file <file_path>`
* DELETE DIRECTORY: `delete_dir <directory_path>`
* SEARCH FILE: `search <file_name> / directory_name>`
* SEARCH FILE: `search <directory_name>`
* GOTO: `goto <directory_path>`
* Pressing `q` will close the application
* Pressing `ESC KEY` takes user back to Normal Mode.


#### How to execute the program-
* Open the terminal with root of the application folder as the present working directory
* Compile the cpp file: `g++ main.cpp -o main`
* Run the executable file: `./main`

#### Language used-
C++

