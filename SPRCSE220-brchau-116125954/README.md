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
- If a write fails, my code will automatically exit. Although there is an error code for this, -3, this is only used to check for early termination. Therefore, main will still return 0.
- For the -s, -r, -l arguments, they will check if their argument would have been the input or output file.
  -Ex: For this set of arguments: "-s test -l -r %s/%s.in.txt %s/%s.out.txt", the error would be R_ARGUMENT_MISSING because the -r is right next to the input file

Things to Note:
- All my input and output files are stored in tests.in.org and tests.out.exp respectively

Makefile Changes
- I have modified the makefile
  - Now, make test both compiles the code and runs the test
  - To run my tests that I have made located in student_tests.c, run the command make studentTests
