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

Test(invalid_args_test, args_missing01, .description="Argument missing.") {
    char *test_name = "args_missing01";
    sprintf(args, "-s end %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, MISSING_ARGUMENT);
}

Test(invalid_args_test, s_arg_missing01, .description="S argument missing, but if after R argument which is also missing.") {
    char *test_name = "s_argument_missing01";
    prep_files("unix.txt", test_name);  
    sprintf(args, "-r -s -l 1,10 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
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

Test(invalid_args_test, input_arg_missing01, .description="Input File is Missing") {
    char *test_name = "input_arg_missing01";
    prep_files("unix.txt", test_name);  
    sprintf(args, "-r -s test -l 1,10 %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
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

Test(invalid_args_test, output_file_unwritable02, .description="Output file is unwritable.") {
    char *test_name = "output_file_unwritable02"; 
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s the -r WOLFIE %s/%s.in.txt %s/out.txt", TEST_INPUT_DIR, test_name, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, OUTPUT_FILE_UNWRITABLE);
}