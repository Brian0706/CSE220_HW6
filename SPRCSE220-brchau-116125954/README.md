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

Things to Note:
- All my input and output files are stored in tests.in.org and tests.out.exp respectively

Makefile Changes
- I have modified the makefile
  - Now, make test both compiles the code and runs the test
  - To run my tests that I have made located in student_tests.c, run the command make studentTests
- Summary:
  - To run unit tests, you can skip previous steps and just run make test, running make all first and then make test will also work.
  - To run student tests, just run the command make studentTests.
