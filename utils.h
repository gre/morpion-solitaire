#ifndef _UTILS_H
#define _UTILS_H
/**
 * Utils module
 */

int util_abs(int);
int util_inRangeExclusif(int i, int a, int b);



/**
 * Trim a string (remove extra spaces around words)
 * @return the trimed string
 */
extern char* str_trim(char *s);
extern void str_truncate(char* str, int length);
extern void str_formatOnlyAlphaAndUnderscore(char* str);

/// Args utils ///

/**
 * Check if argv contains key
 * @param argv, argc : given from the main()
 * @param key : the string to search
 * @return
 *  1: if argv contains the string key
 *  0: else
 */
extern int util_containsArg(int argc, char * argv[], char* key);

/**
 * Get an integer from argv following the key arg
 * example : ./prog arg1 arg2 --level 8 arg3 ...
 * The function will also consume args '--level' and '8'.
 * 
 * @param argv, argc : given from the main()
 * @param key : the key to search (preceding the value)
 * @param value : the dest value getted after the arg key
 * @return a status:
 *  0: arg successfully getted
 *  1: arg was not found in argv
 *  2: no digits were found in the string
 *  3: the arg value is invalid
 */
extern int util_getArgValue(int argc, char * argv[], char * key, int * value);

/**
 * Get a string arg following the key arg from argv
 * example : ./prog arg1 arg2 -d dictionaries/fr.txt arg3 ...
 * The function will also consume taken args ('-d' and 'dictionaries/fr.txt').
 * 
 * @param argv, argc : given from the main()
 * @param key : the key to search (preceding the value)
 * @param value : the dest value getted after the arg key
 * @return a status:
 *  0: arg successfully getted
 *  1: arg was not found in argv
 */
extern int util_getArgString(int argc, char * argv[], char * key, char** value);


#endif
