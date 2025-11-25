# Part I am on: Run a program

## Needs to be Done:
- [] If not a builtin command, "1. Search for an executable with the given name in the directories listed in PATH (just like type does)"
- [] If executable found, "2. If found, execute the program"
- [] "3. Pass any arguments from the command line to the program"

## Already Done
- [X] Make is_builtin_cmd() function to see if a given command is one of the builtin functions we have. Also refactor to use it
- [X] Make builtin_name_to_idx() function to map a builtin command from its name to its index in the array. Also refactor to use it
- [X] Add comments to stuff to improve documentation
- [X] Maybe make searching in path stuff a function to reuse in type part and new area part
- [X] Restructure to having main.c just be main, shell.c be implementations of functions and stuff and shell.h being declarations / structs
