/*
    Name: Brian Chau
    SBU ID: 116125954
*/

#include "student_tests.h"
#include "hw6.h"
 
static char test_log_outfile[100];

int run_using_system(char *test_name, char *args) {
    char *executable = "./bin/hw6";
    assert(access(executable, F_OK) == 0);

    char cmd[500];
    sprintf(test_log_outfile, "%s/%s.log", TEST_OUTPUT_DIR, test_name);
#if defined(__linux__)
    sprintf(cmd, "ulimit -f 300; ulimit -t 5; valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes --error-exitcode=37 ./bin/hw6 %s >> %s 2>&1",
	    args, test_log_outfile);
#else
    cr_log_warn("Skipping valgrind tests. Run tests on Linux or GitHub for full output.\n");
#endif
    return system(cmd);
}

int run_using_system_no_valgrind(char *test_name, char *args) {
    char executable[100];
    sprintf(executable, "./bin/hw6");
    assert(access(executable, F_OK) == 0);

    char cmd[500];
    sprintf(test_log_outfile, "%s/%s.log", TEST_OUTPUT_DIR, test_name);
    sprintf(cmd, "ulimit -f 300; ulimit -t 5; ./bin/hw6 %s >> %s 2>&1", args, test_log_outfile);
    return system(cmd);
}

void expect_outfile_matches(char *test_name) {
    char cmd[500];
    sprintf(cmd, "diff %s/%s.out.txt %s/%s.out.txt >> %s", TEST_EXPECTED_DIR, test_name, TEST_OUTPUT_DIR, test_name,
        test_log_outfile);
    int err = system(cmd);
    cr_expect_eq(err, 0, "The output was not what was expected (diff exited with status %d).\n", WEXITSTATUS(err));
}

void expect_normal_exit(int status) {
    cr_expect_eq(status, 0, "The program did not exit normally (status = %d).\n", status);
}

void expect_error_exit(int actual_status, int expected_status) {
    cr_expect_eq(WEXITSTATUS(actual_status), expected_status,
		 "The program exited with status %d instead of %d.\n", WEXITSTATUS(actual_status), expected_status);
}

void expect_no_valgrind_errors(int status) {
    cr_expect_neq(WEXITSTATUS(status), 37, "Valgrind reported errors -- see %s", test_log_outfile);
}

void prep_files(char *orig_file, char *input_file) {
    char cmd[200];
    sprintf(cmd, "cp %s/%s %s/%s.in.txt", TEST_ORIG_DIR, orig_file, TEST_INPUT_DIR, input_file);
    system(cmd);
}

static char args[ARGS_TEXT_LEN];

TestSuite(invalid_args_test, .timeout=TEST_TIMEOUT); // return code to OS
TestSuite(output_test, .timeout=TEST_TIMEOUT, .disabled=false);
TestSuite(valgrind_test, .timeout=TEST_TIMEOUT, .disabled=false);

Test(output_test, simple_search01, .description="Perform a simple replacement over entire file.") {
    char *test_name = "simple_student_search01";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s ipsum -r DONE %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, simple_search02, .description="Perform a simple replacement over entire file. Extra flags should be ignored.") {
    char *test_name = "simple_student_search02";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s Lorem -r HELLO -h -g test %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, simple_search03, .description="Perform a simple replacement over just the first paragraph.") {
    char *test_name = "simple_student_search03";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s Lorem -r HELLO -l 1,7 test %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, simple_search04, .description="Perform a replacement of a substring of a work.") {
    char *test_name = "simple_student_search04";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s ip -r NUM test %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, simple_search05, .description="Perform a simple replacement over a range larger than the file.") {
    char *test_name = "simple_student_search05";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s Lorem -r HELLO -l 1,30 test %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, simple_search06, .description="Perform a simple replacement over a range that is equal to the file.") {
    char *test_name = "simple_student_search06";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s Lorem -r HELLO -l 1,23 test %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, simple_search07, .description="Try to replace a word that is not in the text.") {
    char *test_name = "simple_student_search07";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-s Holy -r COW test %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, simple_search08, .description="Perform a simple replacement with the replacement string having the wildcard characters") {
    char *test_name = "simple_student_search08";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-s Romulus -r *ROM test %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, simple_search09, .description="Perform a simple replacement with the search string having the wildcard character") {
    char *test_name = "simple_student_search09";
    prep_files("wildcard.txt", test_name);    
    sprintf(args, "-s *Hello -r HI_THERE test %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, simple_search10, .description="Perform a simple replacement with the replacement string having non-alphatetical characters") {
    char *test_name = "simple_student_search10";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-s Remus -r 1234_ test %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, simple_search11, .description="Perform a simple replacement with a different order") {
    char *test_name = "simple_student_search11";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-l 1,23 -r HELLO -s Lorem %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, simple_search12, .description="Perform a simple replacement with a different order again") {
    char *test_name = "simple_student_search12";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-r HELLO -l 1,23 -s Lorem %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, simple_search13, .description="Perform a simple replacement with a word of max search length") {
    char *test_name = "simple_student_search13";
    prep_files("maxsearch.txt", test_name);    
    sprintf(args, "-r WORKS -l 1,23 -s ThisTwentyCharacters %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, simple_search14, .description="Perform a simple replacement where searchString overlaps with another string.") {
    char *test_name = "simple_student_search14";
    prep_files("overlap.txt", test_name);    
    sprintf(args, "-r HI -s aa -l 1,1 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, simple_search15, .description="Perform a simple replacement where file is empty") {
    char *test_name = "simple_student_search15";
    prep_files("empty.txt", test_name);    
    sprintf(args, "-r Lop -l 1,23 -s Lo %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, simple_search16, .description="Perform a simple replacement where search string is a substring of replace string") {
    char *test_name = "simple_student_search16";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-r Lop -l 1,23 -s Lo %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, simple_search17, .description="Perform a simple replacement where replacement is the name of the input file.") {
    char *test_name = "simple_student_search17";
    prep_files("lorem.txt", test_name);  
    sprintf(args, "-s Lorem -l 1,23 -r %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, simple_search18, .description="Perform a simple replacement where L argument is not just numbers.") {
    char *test_name = "simple_student_search18";
    prep_files("lorem.txt", test_name);  
    sprintf(args, "-s Lorem -l 1ab,15 -r HI %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, simple_search19, .description="Perform a simple replacement where L's endline argument is not just numbers.") {
    char *test_name = "simple_student_search19";
    prep_files("lorem.txt", test_name);  
    sprintf(args, "-s Lorem -l 1,15ab -r HI %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search01, .description="Perform a wildcard prefix replacement over some lines. Terms have different lengths.") {
    char *test_name = "wildcard_student_search01";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s Lor* -r HI -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search02, .description="Perform a wildcard prefix replacement over some lines. Multiple different types of words have the prefix.") {
    char *test_name = "wildcard_student_search02";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s con* -r TEST -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search03, .description="Perform a wildcard prefix replacement over some lines. No word has the prefix.") {
    char *test_name = "wildcard_student_search03";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s uyp* -r TEST -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search04, .description="Perform a wildcard prefix replacement over some lines. Range is the middle of the file.") {
    char *test_name = "wildcard_student_search04";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s Lor* -r HI -l 9,15 -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search05, .description="Perform a wildcard prefix replacement over some lines. Multiple different types of words have the prefix.") {
    char *test_name = "wildcard_student_search05";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s con* -r TEST -w -l 1,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search06, .description="Perform a wildcard prefix replacement over some lines. Used a different file.") {
    char *test_name = "wildcard_student_search06";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-s Rom* -r RAIN -w -l 1,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search07, .description="Perform a wildcard prefix replacement over entire file. Used a different order.") {
    char *test_name = "wildcard_student_search07";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-s Rom* -r RAIN -l 1,23 -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search08, .description="Perform a wildcard prefix replacement over entire file. Used a different order again.") {
    char *test_name = "wildcard_student_search08";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-s Rom* -w -r RAIN -l 1,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search09, .description="Perform a wildcard prefix replacement over entire file. Used a different order again.") {
    char *test_name = "wildcard_student_search09";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-w -r RAIN -s Rom* -l 1,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search10, .description="Perform a wildcard prefix replacement over entire file. Prefix used is actually a suffix.") {
    char *test_name = "wildcard_student_search10";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-s us* -r RAIN -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search11, .description="Perform a wildcard suffix replacement over some lines. Terms have different lengths.") {
    char *test_name = "wildcard_student_search11";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s *rem -r HI -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search12, .description="Perform a wildcard suffix replacement over some lines. Suffix is one letter.") {
    char *test_name = "wildcard_student_search12";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s *m -r TEST -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search13, .description="Perform a wildcard suffix replacement over some lines. Range is last paragraph.") {
    char *test_name = "wildcard_student_search13";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s *rem -r HI -w -l 17,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search14, .description="Perform a wildcard suffix replacement over some lines. Range is whole file.") {
    char *test_name = "wildcard_student_search14";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s *rem -r HI -w -l 1,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search15, .description="Perform a wildcard suffix replacement over entire file. Suffix does not exist.") {
    char *test_name = "wildcard_student_search15";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s *test -r HI -w -l 1,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search16, .description="Perform a wildcard suffix replacement over entire file. Checking for a word replace right before newline.") {
    char *test_name = "wildcard_student_search16";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-s *us -r USA -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search17, .description="Perform a wildcard suffix replacement over entire file. Using a different order.") {
    char *test_name = "wildcard_student_search17";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-r USA -s *us -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search18, .description="Perform a wildcard suffix replacement over entire file. Using a different order.") {
    char *test_name = "wildcard_student_search18";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-r USA -w -s *us %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search19, .description="Perform a wildcard suffix replacement over entire file. Using a different order.") {
    char *test_name = "wildcard_student_search19";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-w -r USA -s *us -l 1,15 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search20, .description="Perform a wildcard suffix replacement over some lines. Range past file's end.") {
    char *test_name = "wildcard_student_search20";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-w -r USA -s *us -l 1,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search21, .description="Perform a wildcard suffix replacement over some lines. Word is at beginning of range.") {
    char *test_name = "wildcard_student_search21";
    prep_files("oneword.txt", test_name);    
    sprintf(args, "-w -r TEST -s *work -l 1,2 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search22, .description="Perform a wildcard prefix replacement over some lines. Word is at end of range.") {
    char *test_name = "wildcard_student_search22";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-w -r TEST -s Stan* -l 1,15 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search23, .description="Perform a wildcard prefix replacement over some lines. File is empty.") {
    char *test_name = "wildcard_student_search23";
    prep_files("empty.txt", test_name);    
    sprintf(args, "-w -r TEST -s Stan* -l 1,15 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search24, .description="Perform a wildcard suffix replacement over some lines. File is empty.") {
    char *test_name = "wildcard_student_search24";
    prep_files("empty.txt", test_name);    
    sprintf(args, "-w -r TEST -s *hi -l 1,15 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search25, .description="Perform a wildcard prefix replacement over some lines. Prefix is one letter.") {
    char *test_name = "wildcard_student_search25";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-w -r PUPPY -s p* -l 1,15 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search26, .description="Perform a wildcard prefix replacement over some lines. Prefix is just a word.") {
    char *test_name = "wildcard_student_search26";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-w -r TESTING -s Lorem* -l 1,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(output_test, wildcard_search27, .description="Perform a wildcard prefix replacement over some lines. Suffix is just a word.") {
    char *test_name = "wildcard_student_search27";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-w -r TESTING -s *Lorem -l 1,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

/*Valgrind Tests*/
Test(valgrind_test, simple_search01, .description="Perform a simple replacement over entire file.") {
    char *test_name = "simple_student_search01";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s ipsum -r DONE %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, simple_search02, .description="Perform a simple replacement over entire file. Extra flags should be ignored.") {
    char *test_name = "simple_student_search02";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s Lorem -r HELLO -h -g test %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, simple_search03, .description="Perform a simple replacement over just the first paragraph.") {
    char *test_name = "simple_student_search03";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s Lorem -r HELLO -l 1,7 test %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, simple_search04, .description="Perform a replacement of a substring of a work.") {
    char *test_name = "simple_student_search04";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s ip -r NUM test %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, simple_search05, .description="Perform a simple replacement over a range larger than the file.") {
    char *test_name = "simple_student_search05";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s Lorem -r HELLO -l 1,30 test %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, simple_search06, .description="Perform a simple replacement over a range that is equal to the file.") {
    char *test_name = "simple_student_search06";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s Lorem -r HELLO -l 1,23 test %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, simple_search07, .description="Try to replace a word that is not in the text.") {
    char *test_name = "simple_student_search07";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-s Holy -r COW test %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, simple_search08, .description="Perform a simple replacement with the replacement string having the wildcard characters") {
    char *test_name = "simple_student_search08";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-s Romulus -r *ROM test %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, simple_search09, .description="Perform a simple replacement with the search string having the wildcard character") {
    char *test_name = "simple_student_search09";
    prep_files("wildcard.txt", test_name);    
    sprintf(args, "-s *Hello -r HI_THERE test %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, simple_search10, .description="Perform a simple replacement with the replacement string having non-alphatetical characters") {
    char *test_name = "simple_student_search10";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-s Remus -r 1234_ test %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, simple_search11, .description="Perform a simple replacement with a different order") {
    char *test_name = "simple_student_search11";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-l 1,23 -r HELLO -s Lorem %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, simple_search12, .description="Perform a simple replacement with a different order again") {
    char *test_name = "simple_student_search12";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-r HELLO -l 1,23 -s Lorem %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, simple_search13, .description="Perform a simple replacement with a word of max search length") {
    char *test_name = "simple_student_search13";
    prep_files("maxsearch.txt", test_name);    
    sprintf(args, "-r WORKS -l 1,23 -s ThisTwentyCharacters %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, simple_search14, .description="Perform a simple replacement where searchString overlaps with another string.") {
    char *test_name = "simple_student_search14";
    prep_files("overlap.txt", test_name);    
    sprintf(args, "-r HI -s aa -l 1,1 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, simple_search15, .description="Perform a simple replacement where file is empty") {
    char *test_name = "simple_student_search15";
    prep_files("empty.txt", test_name);    
    sprintf(args, "-r Lop -l 1,23 -s Lo %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, simple_search16, .description="Perform a simple replacement where search string is a substring of replace string") {
    char *test_name = "simple_student_search16";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-r Lop -l 1,23 -s Lo %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, simple_search17, .description="Perform a simple replacement where replacement is the name of the input file.") {
    char *test_name = "simple_search17";
    prep_files("unix.txt", test_name);  
    sprintf(args, "-s test -l -r %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, simple_search18, .description="Perform a simple replacement where L argument is not just numbers.") {
    char *test_name = "simple_student_search18";
    prep_files("lorem.txt", test_name);  
    sprintf(args, "-s Lorem -l 1ab,15 -r HI %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, simple_search19, .description="Perform a simple replacement where L's endline argument is not just numbers.") {
    char *test_name = "simple_student_search19";
    prep_files("lorem.txt", test_name);  
    sprintf(args, "-s Lorem -l 1,15ab -r HI %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search01, .description="Perform a wildcard prefix replacement over some lines. Terms have different lengths.") {
    char *test_name = "wildcard_student_search01";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s Lor* -r HI -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}
Test(valgrind_test, wildcard_search02, .description="Perform a wildcard prefix replacement over some lines. Multiple different types of words have the prefix.") {
    char *test_name = "wildcard_student_search02";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s con* -r TEST -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}
Test(valgrind_test, wildcard_search03, .description="Perform a wildcard prefix replacement over some lines. No word has the prefix.") {
    char *test_name = "wildcard_student_search03";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s uyp* -r TEST -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search04, .description="Perform a wildcard prefix replacement over some lines. Range is the middle of the file.") {
    char *test_name = "wildcard_student_search04";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s Lor* -r HI -l 9,15 -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search05, .description="Perform a wildcard prefix replacement over some lines. Multiple different types of words have the prefix.") {
    char *test_name = "wildcard_student_search05";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s con* -r TEST -w -l 1,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search06, .description="Perform a wildcard prefix replacement over some lines. Used a different file.") {
    char *test_name = "wildcard_student_search06";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-s Rom* -r RAIN -w -l 1,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search07, .description="Perform a wildcard prefix replacement over entire file. Used a different order.") {
    char *test_name = "wildcard_student_search07";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-s Rom* -r RAIN -l 1,23 -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search08, .description="Perform a wildcard prefix replacement over entire file. Used a different order again.") {
    char *test_name = "wildcard_student_search08";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-s Rom* -w -r RAIN -l 1,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search09, .description="Perform a wildcard prefix replacement over entire file. Used a different order again.") {
    char *test_name = "wildcard_student_search09";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-w -r RAIN -s Rom* -l 1,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search10, .description="Perform a wildcard prefix replacement over entire file. Prefix used is actually a suffix.") {
    char *test_name = "wildcard_student_search10";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-s us* -r RAIN -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search11, .description="Perform a wildcard suffix replacement over some lines. Terms have different lengths.") {
    char *test_name = "wildcard_student_search11";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s *rem -r HI -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search12, .description="Perform a wildcard suffix replacement over some lines. Suffix is one letter.") {
    char *test_name = "wildcard_student_search12";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s *m -r TEST -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search13, .description="Perform a wildcard suffix replacement over some lines. Range is last paragraph.") {
    char *test_name = "wildcard_student_search13";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s *rem -r HI -w -l 17,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}
Test(valgrind_test, wildcard_search14, .description="Perform a wildcard suffix replacement over some lines. Range is whole file.") {
    char *test_name = "wildcard_student_search14";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s *rem -r HI -w -l 1,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search15, .description="Perform a wildcard suffix replacement over entire file. Suffix does not exist.") {
    char *test_name = "wildcard_student_search15";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-s *test -r HI -w -l 1,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search16, .description="Perform a wildcard suffix replacement over entire file. Checking for a word replace right before newline.") {
    char *test_name = "wildcard_student_search16";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-s *us -r USA -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search17, .description="Perform a wildcard suffix replacement over entire file. Using a different order.") {
    char *test_name = "wildcard_student_search17";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-r USA -s *us -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search18, .description="Perform a wildcard suffix replacement over entire file. Using a different order.") {
    char *test_name = "wildcard_student_search18";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-r USA -w -s *us %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search19, .description="Perform a wildcard suffix replacement over entire file. Using a different order.") {
    char *test_name = "wildcard_student_search19";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-w -r USA -s *us -l 1,15 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search20, .description="Perform a wildcard suffix replacement over some lines. Range past file's end.") {
    char *test_name = "wildcard_student_search20";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-w -r USA -s *us -l 1,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search21, .description="Perform a wildcard suffix replacement over some lines. Word is at edge of range.") {
    char *test_name = "wildcard_student_search21";
    prep_files("oneword.txt", test_name);    
    sprintf(args, "-w -r TEST -s *work -l 1,2 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search22, .description="Perform a wildcard prefix replacement over some lines. Word is at end of range.") {
    char *test_name = "wildcard_student_search22";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-w -r TEST -s Stan* -l 1,15 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search23, .description="Perform a wildcard prefix replacement over some lines. File is empty.") {
    char *test_name = "wildcard_student_search23";
    prep_files("empty.txt", test_name);    
    sprintf(args, "-w -r TEST -s Stan* -l 1,15 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}


Test(valgrind_test, wildcard_search24, .description="Perform a wildcard suffix replacement over some lines. File is empty.") {
    char *test_name = "wildcard_student_search24";
    prep_files("empty.txt", test_name);    
    sprintf(args, "-w -r TEST -s *hi -l 1,15 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search25, .description="Perform a wildcard prefix replacement over some lines. Prefix is one letter.") {
    char *test_name = "wildcard_student_search25";
    prep_files("rome.txt", test_name);    
    sprintf(args, "-w -r PUPPY -s p* -l 1,15 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search26, .description="Perform a wildcard prefix replacement over some lines. Prefix is just a word.") {
    char *test_name = "wildcard_student_search26";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-w -r TESTING -s Lorem* -l 1,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(valgrind_test, wildcard_search27, .description="Perform a wildcard prefix replacement over some lines. Suffix is just a word.") {
    char *test_name = "wildcard_student_search27";
    prep_files("lorem.txt", test_name);    
    sprintf(args, "-w -r TESTING -s *Lorem -l 1,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

/*Invalid arguments test*/
Test(invalid_args_test, args_missing01, .description="Argument missing.") {
    char *test_name = "args_missing01";
    sprintf(args, "-s end %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, MISSING_ARGUMENT);
}

Test(invalid_args_test, args_missing02, .description="Argument missing, but also duplicate.") {
    char *test_name = "args_missing02";
    sprintf(args, "-s -s");
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, MISSING_ARGUMENT);
}

Test(invalid_args_test, args_missing03, .description="Argument missing, but also WILD_CARD INVALID.") {
    char *test_name = "args_missing03";
    sprintf(args, "-s -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, MISSING_ARGUMENT);
}

Test(invalid_args_test, args_missing04, .description="Argument missing, but also INPUT_FILE_MISSING.") {
    char *test_name = "args_missing04";
    sprintf(args, "-s the -r end");
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, MISSING_ARGUMENT);
}

Test(invalid_args_test, args_missing05, .description="No arguments.") {
    char *test_name = "args_missing05";
    sprintf(args, " ");
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, MISSING_ARGUMENT);
}

Test(invalid_args_test, args_missing06, .description="Just input and output files.") {
    char *test_name = "args_missing06";
    sprintf(args, "%s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, MISSING_ARGUMENT);
}

Test(invalid_args_test, duplicated_args01, .description="S flag appears twice.") {
    char *test_name = "duplicated_args01";
    sprintf(args, "-s end -s -r %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, DUPLICATE_ARGUMENT);
}

Test(invalid_args_test, duplicated_args02, .description="R flag appears twice.") {
    char *test_name = "duplicated_args02";
    sprintf(args, "-s end -r hello -r %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, DUPLICATE_ARGUMENT);
}

Test(invalid_args_test, duplicated_args03, .description="L flag appears twice.") {
    char *test_name = "duplicated_args03";
    sprintf(args, "-s end -r hello -l 1,10 -l %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, DUPLICATE_ARGUMENT);
}

Test(invalid_args_test, duplicated_args04, .description="W flag appears twice.") {
    char *test_name = "duplicated_args04";
    sprintf(args, "-s end -r hello -w -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, DUPLICATE_ARGUMENT);
}

Test(invalid_args_test, duplicated_args05, .description="Input File error is also present.") {
    char *test_name = "duplicated_args05";
    sprintf(args, "-s end -r hello -w -w %s/%s.out.txt", TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, DUPLICATE_ARGUMENT);
}

Test(invalid_args_test, s_arg_missing01, .description="S flag is not present.") {
    char *test_name = "s_argument_missing01";
    prep_files("unix.txt", test_name);  
    sprintf(args, "-r tests -l 1,10 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, S_ARGUMENT_MISSING);
}

Test(invalid_args_test, s_arg_missing02, .description="S argument missing, but if after R argument which is also missing.") {
    char *test_name = "s_argument_missing02";
    prep_files("unix.txt", test_name);  
    sprintf(args, "-r -s -l 1,10 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, S_ARGUMENT_MISSING);
}

Test(invalid_args_test, s_arg_missing03, .description="S argument missing, but if after L argument which is also missing.") {
    char *test_name = "s_argument_missing03";
    prep_files("unix.txt", test_name);  
    sprintf(args, "-r test -s -l %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, S_ARGUMENT_MISSING);
}

Test(invalid_args_test, s_arg_missing04, .description="S argument missing, but if after wildcard is also invalid.") {
    char *test_name = "s_argument_missing04";
    prep_files("unix.txt", test_name);  
    sprintf(args, "-r test -l -s -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, S_ARGUMENT_MISSING);
}

Test(invalid_args_test, s_arg_missing05, .description="S argument missing, but if after wildcard is also invalid.") {
    char *test_name = "s_argument_missing04";
    prep_files("unix.txt", test_name);  
    sprintf(args, "-r test -l -s \"\" -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, S_ARGUMENT_MISSING);
}

Test(invalid_args_test, r_arg_missing01, .description="R argument missing, but if after L argument is invalid.") {
    char *test_name = "r_arg_missing01";
    prep_files("unix.txt", test_name);  
    sprintf(args, "-r -s test -l 15,1 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, R_ARGUMENT_MISSING);
}

Test(invalid_args_test, r_arg_missing02, .description="R flag is not present") {
    char *test_name = "r_arg_missing02";
    prep_files("unix.txt", test_name);  
    sprintf(args, "-s test -l 15,1 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, R_ARGUMENT_MISSING);
}

Test(invalid_args_test, r_arg_missing03, .description="R argument missing, but if after W argument is invalid.") {
    char *test_name = "r_arg_missing03s";
    prep_files("unix.txt", test_name);  
    sprintf(args, "-r -s test -l 1,15 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, R_ARGUMENT_MISSING);
}

Test(invalid_args_test, r_arg_missing04, .description="R argument is empty string.") {
    char *test_name = "r_arg_missing04";
    prep_files("unix.txt", test_name);  
    sprintf(args, "-r \"\" -s test -l 1,15 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, R_ARGUMENT_MISSING);
}

Test(invalid_args_test, input_arg_missing01, .description="Input File is Missing and rest is valid") {
    char *test_name = "input_arg_missing01";
    prep_files("unix.txt", test_name);  
    sprintf(args, "-r end -s test -l 1,10 %s/%s.out.txt", TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, INPUT_FILE_MISSING);
}

Test(invalid_args_test, input_arg_missing02, .description="Input File is Missing and missing argument for R") {
    char *test_name = "_input_arg_missing02";
    prep_files("unix.txt", test_name);  
    sprintf(args, "-r -s test -l 15,1 %s/.in.txt %s/%s.out.txt", TEST_INPUT_DIR, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, INPUT_FILE_MISSING);
}

Test(invalid_args_test, input_arg_missing03, .description="Input File is Missing and invalid args for L and W") {
    char *test_name = "_input_arg_missing03";
    prep_files("unix.txt", test_name);  
    sprintf(args, "-r end -s *the* -l xyz10,15 -w %s/.in.txt %s/%s.out.txt", TEST_INPUT_DIR, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, INPUT_FILE_MISSING);
}

Test(invalid_args_test, output_file_unwritable01, .description="Output file is unwritable.") {
    char *test_name = "output_file_unwritable01"; 
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s the -r WOLFIE %s/%s.in.txt %s/.out.txt", TEST_INPUT_DIR, test_name, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, OUTPUT_FILE_UNWRITABLE);
}

Test(invalid_args_test, output_file_unwritable02, .description="Output file is missing.") {
    char *test_name = "output_file_unwritable02"; 
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s the -r WOLFIE %s/%s.in.txt %s/out.txt", TEST_INPUT_DIR, test_name, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, OUTPUT_FILE_UNWRITABLE);
}

Test(invalid_args_test, output_file_unwritable03, .description="Output file is unwritable.") {
    char *test_name = "output_file_unwritable03"; 
    prep_files("unix.txt", test_name);   
    char cmd[200]; 
    sprintf(cmd, "touch %s/%s.out.txt; chmod 400 %s/%s.out.txt", TEST_OUTPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    system(cmd);
    sprintf(args, "-s the -r WOLFIE %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, OUTPUT_FILE_UNWRITABLE);
}

Test(invalid_args_test, output_file_unwritable04, .description="Can't write to root directory.") {
    char *test_name = "output_file_unwritable04"; 
    prep_files("unix.txt", test_name);   
    sprintf(args, "-s the -r WOLFIE %s/%s.in.txt /%s.out.txt", TEST_INPUT_DIR, test_name, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, OUTPUT_FILE_UNWRITABLE);
}

Test(invalid_args_test, l_argument_invalid01, .description="l argument is invalid. Both negative line numbers.") {
    char *test_name = "l_argument_invalid01";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s bar -r test -l -10,-25 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, L_ARGUMENT_INVALID);
}

Test(invalid_args_test, l_argument_invalid02, .description="l argument is invalid. One line number unparsable.") {
    char *test_name = "l_argument_invalid02";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s bar -r test -l hey,25 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, L_ARGUMENT_INVALID);
}

Test(invalid_args_test, l_argument_invalid03, .description="l argument is invalid. Second line number unparsable.") {
    char *test_name = "l_argument_invalid03";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s bar -r test -l 10,hey %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, L_ARGUMENT_INVALID);
}

Test(invalid_args_test, l_argument_invalid04, .description="l argument is invalid. Both line numbers unparsable.") {
    char *test_name = "l_argument_invalid04";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s bar -r test -l hey,there %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, L_ARGUMENT_INVALID);
}

Test(invalid_args_test, l_argument_invalid05, .description="l argument is invalid. Only one line number.") {
    char *test_name = "l_argument_invalid05";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s bar -r test -l 10 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, L_ARGUMENT_INVALID);
}

Test(invalid_args_test, l_argument_invalid06, .description="l argument is invalid. no line number.") {
    char *test_name = "l_argument_invalid06";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-l -s bar -r test %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, L_ARGUMENT_INVALID);
}

Test(invalid_args_test, l_argument_invalid07, .description="Start greater than end.") {
    char *test_name = "l_argument_invalid07";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s bar -r test -l 10,9 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, L_ARGUMENT_INVALID);
}

Test(invalid_args_test, l_argument_invalid08, .description="Invalid argument has a number in it.") {
    char *test_name = "l_argument_invalid08";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s bar -r test -l hey9,10 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, L_ARGUMENT_INVALID);
}

Test(invalid_args_test, l_argument_invalid09, .description="Startline is 0.") {
    char *test_name = "l_argument_invalid09";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s bar -r test -l 0,10 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, L_ARGUMENT_INVALID);
}

Test(invalid_args_test, l_argument_invalid10, .description="Two numbers seperated by a lot of commas.") {
    char *test_name = "l_argument_invalid10";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s bar -r test -l 9,,,,,,,,,-10 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, L_ARGUMENT_INVALID);
}


Test(invalid_args_test, l_argument_invalid11, .description="Endline is 0.") {
    char *test_name = "l_argument_invalid11";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s bar -r test -l 10,0 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, L_ARGUMENT_INVALID);
}

Test(invalid_args_test, l_argument_invalid12, .description="Wildcard Argument Invalid is also in the arguments") {
    char *test_name = "l_argument_invalid12";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s bar -r test -l 10,0 -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, L_ARGUMENT_INVALID);
}

Test(invalid_args_test, l_argument_invalid13, .description="L flag is right before inputfile") {
    char *test_name = "l_argument_invalid13";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s bar -r test -l %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, L_ARGUMENT_INVALID);
}

Test(invalid_args_test, l_argument_invalid14, .description="Another seperator besides comma") {
    char *test_name = "l_argument_invalid14";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s bar -r test -l 1-10 -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, L_ARGUMENT_INVALID);
}


Test(invalid_args_test, wildcard_invalid01, .description="Search text is two wildcard indicators.") {
    char *test_name = "wildcard_invalid01";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s ** -r HELLO -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, WILDCARD_INVALID);
}

Test(invalid_args_test, wildcard_invalid02, .description="Search text is missing.") {
    char *test_name = "wildcard_invalid02";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s -r HELLO -w -l 1,2 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, S_ARGUMENT_MISSING);
}

Test(invalid_args_test, wildcard_invalid03, .description="Just a wildcard marker.") {
    char *test_name = "wildcard_invalid03";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s * -r HELLO -w -l 1,2 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, WILDCARD_INVALID);
}

Test(invalid_args_test, wildcard_invalid04, .description="No wildcard marker.") {
    char *test_name = "wildcard_invalid04";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s a -r HELLO -w -l 1,2 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, WILDCARD_INVALID);
}

Test(invalid_args_test, wildcard_invalid05, .description="Space inside search term of prefix.") {
    char *test_name = "wildcard_invalid05";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s \"a b*\" -r HELLO -w -l 1,2 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, WILDCARD_INVALID);
}

Test(invalid_args_test, wildcard_invalid06, .description="Period inside search term of prefix.") {
    char *test_name = "wildcard_invalid06";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s a.b* -r HELLO -w -l 1,2 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, WILDCARD_INVALID);
}

Test(invalid_args_test, wildcard_invalid07, .description="Asterick inside search term of suffix.") {
    char *test_name = "wildcard_invalid07";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s *a*b -r HELLO -w -l 1,2 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, WILDCARD_INVALID);
}

Test(invalid_args_test, wildcard_invalid08, .description="Space inside search term of suffix.") {
    char *test_name = "wildcard_invalid08";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s \"*a b\" -r HELLO -w -l 1,2 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, WILDCARD_INVALID);
}

Test(invalid_args_test, wildcard_invalid09, .description="Search term is input file") {
    char *test_name = "wildcard_invalid09";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-r HELLO -w -l 1,2 -s %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, WILDCARD_INVALID);
}