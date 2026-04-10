# CSE220_HW6
/*
    Name: Brian Chau
    SBU ID: 116125954
*/

In my hw6 there are several constraints:
- It only can scan for words that are a max length of 20 characters
- It can only handle a line of max 200 characters
  - This means that if a word takes up one line in prefix search of 201, it wouldn't replace it properly
- For the -l argument, it only checks the first possible numbers
  - Ex: When strtok seperates 10,,15, it will check "10" and then "" and nothing else, this results in an error

hw6.c behavior:
- If a write fails, my code will automatically exit. Although there is an error code for this, -3, this is only used to check for early termination. Therefore, main will still return 0.
- If there was a problem with the file pointers at the very end that results in a file error, my program will check for this and return a -1.
- My code will not create an output file if the errors MISSING_ARGUMENT, DUPLICATED_ARGUMENTS, and INPUT_FILE_MISSING are detected. However, if any other errors are present, the output file will have been created.
- When an empty string in passed in for an argument of -s or -r, a S_ARGUMENT_VALID and R_ARGUMENT_INVALID will be thrown for both flags respectively.
- With the -s and -r flag, if the flag is the last argument before the files, it will take the file path as a string either for replacement or search.
  - Ex: -l 1,10 -r test -s input.txt output.txt, the searchString will be input.txt 

Things to Note:
- All my input and output files are stored in tests.in.org and tests.out.exp respectively

Makefile and How to Run Tests
- The makefile has been modified such that make test will both compile the program and tests, and then the tests will be run.
- To run my program and test it you can either
  - Run make or make all, then make test
  - Just run make test 
- Then to run the tests run make tests, which will run both the unit_tests and student_tests as 
make all puts all the tests in one file.
  - First the unit tests will run followed by the student_tests. 