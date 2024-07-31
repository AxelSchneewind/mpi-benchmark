/** @file cmdline.h
 *  @brief The header file for the command line option parser
 *  generated by GNU Gengetopt version 2.23
 *  http://www.gnu.org/software/gengetopt.
 *  DO NOT modify this file, since it can be overwritten
 *  @author GNU Gengetopt */

#ifndef CMDLINE_H
#define CMDLINE_H

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h> /* for FILE */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef CMDLINE_PARSER_PACKAGE
/** @brief the program name (used for printing errors) */
#define CMDLINE_PARSER_PACKAGE "mpi benchmark"
#endif

#ifndef CMDLINE_PARSER_PACKAGE_NAME
/** @brief the complete program name (used for help and version) */
#define CMDLINE_PARSER_PACKAGE_NAME "mpi benchmark"
#endif

#ifndef CMDLINE_PARSER_VERSION
/** @brief the program version */
#define CMDLINE_PARSER_VERSION "0.1"
#endif

enum enum_send_patterns { send_patterns__NULL = -1, send_patterns_arg_Linear = 0, send_patterns_arg_LinearInverse, send_patterns_arg_Stride2, send_patterns_arg_Stride128, send_patterns_arg_Stride1K, send_patterns_arg_Stride16K, send_patterns_arg_Random, send_patterns_arg_RandomBurst128, send_patterns_arg_RandomBurst1K, send_patterns_arg_RandomBurst16K, send_patterns_arg_GridBoundary };

/** @brief Where the command line options are stored */
struct gengetopt_args_info
{
  const char *help_help; /**< @brief Print help and exit help description.  */
  const char *version_help; /**< @brief Print version and exit help description.  */
  int buffer_size_arg;	/**< @brief the log of the buffer size in bytes (default='23').  */
  char * buffer_size_orig;	/**< @brief the log of the buffer size in bytes original value given at command line.  */
  const char *buffer_size_help; /**< @brief the log of the buffer size in bytes help description.  */
  int iteration_count_arg;	/**< @brief the number of iterations per test case (default='100').  */
  char * iteration_count_orig;	/**< @brief the number of iterations per test case original value given at command line.  */
  const char *iteration_count_help; /**< @brief the number of iterations per test case help description.  */
  char ** modes_arg;	/**< @brief a comma separated list containing the benchmarks to run (default='all').  */
  char ** modes_orig;	/**< @brief a comma separated list containing the benchmarks to run original value given at command line.  */
  unsigned int modes_min; /**< @brief a comma separated list containing the benchmarks to run's minimum occurreces */
  unsigned int modes_max; /**< @brief a comma separated list containing the benchmarks to run's maximum occurreces */
  const char *modes_help; /**< @brief a comma separated list containing the benchmarks to run help description.  */
  int* min_partition_size_arg;	/**< @brief the logs of the minimal partition sizes for each mode (default='0').  */
  char ** min_partition_size_orig;	/**< @brief the logs of the minimal partition sizes for each mode original value given at command line.  */
  unsigned int min_partition_size_min; /**< @brief the logs of the minimal partition sizes for each mode's minimum occurreces */
  unsigned int min_partition_size_max; /**< @brief the logs of the minimal partition sizes for each mode's maximum occurreces */
  const char *min_partition_size_help; /**< @brief the logs of the minimal partition sizes for each mode help description.  */
  int* max_partition_size_arg;	/**< @brief the logs of the maximal partition sizes for each mode (default='0').  */
  char ** max_partition_size_orig;	/**< @brief the logs of the maximal partition sizes for each mode original value given at command line.  */
  unsigned int max_partition_size_min; /**< @brief the logs of the maximal partition sizes for each mode's minimum occurreces */
  unsigned int max_partition_size_max; /**< @brief the logs of the maximal partition sizes for each mode's maximum occurreces */
  const char *max_partition_size_help; /**< @brief the logs of the maximal partition sizes for each mode help description.  */
  int different_partition_sizes_flag;	/**< @brief flag to enable different partition sizes for send and receive sides (default=off).  */
  const char *different_partition_sizes_help; /**< @brief flag to enable different partition sizes for send and receive sides help description.  */
  int* min_thread_count_arg;	/**< @brief log2 of the minimal thread counts for each mode (default='0').  */
  char ** min_thread_count_orig;	/**< @brief log2 of the minimal thread counts for each mode original value given at command line.  */
  unsigned int min_thread_count_min; /**< @brief log2 of the minimal thread counts for each mode's minimum occurreces */
  unsigned int min_thread_count_max; /**< @brief log2 of the minimal thread counts for each mode's maximum occurreces */
  const char *min_thread_count_help; /**< @brief log2 of the minimal thread counts for each mode help description.  */
  int* max_thread_count_arg;	/**< @brief log2 of the maximal thread counts for each mode (default='0').  */
  char ** max_thread_count_orig;	/**< @brief log2 of the maximal thread counts for each mode original value given at command line.  */
  unsigned int max_thread_count_min; /**< @brief log2 of the maximal thread counts for each mode's minimum occurreces */
  unsigned int max_thread_count_max; /**< @brief log2 of the maximal thread counts for each mode's maximum occurreces */
  const char *max_thread_count_help; /**< @brief log2 of the maximal thread counts for each mode help description.  */
  enum enum_send_patterns *send_patterns_arg;	/**< @brief send patterns to use for all test cases (default='Linear').  */
  char ** send_patterns_orig;	/**< @brief send patterns to use for all test cases original value given at command line.  */
  unsigned int send_patterns_min; /**< @brief send patterns to use for all test cases's minimum occurreces */
  unsigned int send_patterns_max; /**< @brief send patterns to use for all test cases's maximum occurreces */
  const char *send_patterns_help; /**< @brief send patterns to use for all test cases help description.  */
  char * bench_name_arg;	/**< @brief name of this benchmark (default='').  */
  char * bench_name_orig;	/**< @brief name of this benchmark original value given at command line.  */
  const char *bench_name_help; /**< @brief name of this benchmark help description.  */
  char ** output_file_arg;	/**< @brief list of files (corresponding to the respective rank) that the results will be written to (csv format).  */
  char ** output_file_orig;	/**< @brief list of files (corresponding to the respective rank) that the results will be written to (csv format) original value given at command line.  */
  unsigned int output_file_min; /**< @brief list of files (corresponding to the respective rank) that the results will be written to (csv format)'s minimum occurreces */
  unsigned int output_file_max; /**< @brief list of files (corresponding to the respective rank) that the results will be written to (csv format)'s maximum occurreces */
  const char *output_file_help; /**< @brief list of files (corresponding to the respective rank) that the results will be written to (csv format) help description.  */
  
  unsigned int help_given ;	/**< @brief Whether help was given.  */
  unsigned int version_given ;	/**< @brief Whether version was given.  */
  unsigned int buffer_size_given ;	/**< @brief Whether buffer-size was given.  */
  unsigned int iteration_count_given ;	/**< @brief Whether iteration-count was given.  */
  unsigned int modes_given ;	/**< @brief Whether modes was given.  */
  unsigned int min_partition_size_given ;	/**< @brief Whether min-partition-size was given.  */
  unsigned int max_partition_size_given ;	/**< @brief Whether max-partition-size was given.  */
  unsigned int different_partition_sizes_given ;	/**< @brief Whether different-partition-sizes was given.  */
  unsigned int min_thread_count_given ;	/**< @brief Whether min-thread-count was given.  */
  unsigned int max_thread_count_given ;	/**< @brief Whether max-thread-count was given.  */
  unsigned int send_patterns_given ;	/**< @brief Whether send-patterns was given.  */
  unsigned int bench_name_given ;	/**< @brief Whether bench-name was given.  */
  unsigned int output_file_given ;	/**< @brief Whether output-file was given.  */

} ;

/** @brief The additional parameters to pass to parser functions */
struct cmdline_parser_params
{
  int override; /**< @brief whether to override possibly already present options (default 0) */
  int initialize; /**< @brief whether to initialize the option structure gengetopt_args_info (default 1) */
  int check_required; /**< @brief whether to check that all required options were provided (default 1) */
  int check_ambiguity; /**< @brief whether to check for options already specified in the option structure gengetopt_args_info (default 0) */
  int print_errors; /**< @brief whether getopt_long should print an error message for a bad option (default 1) */
} ;

/** @brief the purpose string of the program */
extern const char *gengetopt_args_info_purpose;
/** @brief the usage string of the program */
extern const char *gengetopt_args_info_usage;
/** @brief the description string of the program */
extern const char *gengetopt_args_info_description;
/** @brief all the lines making the help output */
extern const char *gengetopt_args_info_help[];

/**
 * The command line parser
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser (int argc, char **argv,
  struct gengetopt_args_info *args_info);

/**
 * The command line parser (version with additional parameters - deprecated)
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @param override whether to override possibly already present options
 * @param initialize whether to initialize the option structure my_args_info
 * @param check_required whether to check that all required options were provided
 * @return 0 if everything went fine, NON 0 if an error took place
 * @deprecated use cmdline_parser_ext() instead
 */
int cmdline_parser2 (int argc, char **argv,
  struct gengetopt_args_info *args_info,
  int override, int initialize, int check_required);

/**
 * The command line parser (version with additional parameters)
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @param params additional parameters for the parser
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_ext (int argc, char **argv,
  struct gengetopt_args_info *args_info,
  struct cmdline_parser_params *params);

/**
 * Save the contents of the option struct into an already open FILE stream.
 * @param outfile the stream where to dump options
 * @param args_info the option struct to dump
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_dump(FILE *outfile,
  struct gengetopt_args_info *args_info);

/**
 * Save the contents of the option struct into a (text) file.
 * This file can be read by the config file parser (if generated by gengetopt)
 * @param filename the file where to save
 * @param args_info the option struct to save
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_file_save(const char *filename,
  struct gengetopt_args_info *args_info);

/**
 * Print the help
 */
void cmdline_parser_print_help(void);
/**
 * Print the version
 */
void cmdline_parser_print_version(void);

/**
 * Initializes all the fields a cmdline_parser_params structure 
 * to their default values
 * @param params the structure to initialize
 */
void cmdline_parser_params_init(struct cmdline_parser_params *params);

/**
 * Allocates dynamically a cmdline_parser_params structure and initializes
 * all its fields to their default values
 * @return the created and initialized cmdline_parser_params structure
 */
struct cmdline_parser_params *cmdline_parser_params_create(void);

/**
 * Initializes the passed gengetopt_args_info structure's fields
 * (also set default values for options that have a default)
 * @param args_info the structure to initialize
 */
void cmdline_parser_init (struct gengetopt_args_info *args_info);
/**
 * Deallocates the string fields of the gengetopt_args_info structure
 * (but does not deallocate the structure itself)
 * @param args_info the structure to deallocate
 */
void cmdline_parser_free (struct gengetopt_args_info *args_info);

/**
 * Checks that all the required options were specified
 * @param args_info the structure to check
 * @param prog_name the name of the program that will be used to print
 *   possible errors
 * @return
 */
int cmdline_parser_required (struct gengetopt_args_info *args_info,
  const char *prog_name);

extern const char *cmdline_parser_send_patterns_values[];  /**< @brief Possible values for send-patterns. */


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* CMDLINE_H */
