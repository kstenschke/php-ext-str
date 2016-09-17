#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_str.h"
#include "ext/standard/php_string.h"

/* True global resources - no need for thread safety here */
static int le_str;

/* {{{ str_functions[]
 *
 * Every user visible function must have an entry in str_functions[].
 */
const zend_function_entry str_functions[] = {
    PHP_FE(str_startswith,      NULL)
    PHP_FE(str_endswith,        NULL)
    PHP_FE(str_isupper,         NULL)
    PHP_FE(str_islower,         NULL)
    PHP_FE(str_iswhitespace,    NULL)
    PHP_FE(str_swapcase,        NULL)
    PHP_FE(str_contains,        NULL)
    PHP_FE(str_wrap,            NULL)
    PHP_FE(str_startsnumerical, NULL)
    PHP_FE(str_intexplode,      NULL)
    PHP_FE(str_unwrap,          NULL)
    PHP_FE(str_random,          NULL)
    {NULL, NULL, NULL}
};
/* }}} */

/* {{{ str_module_entry
 */
zend_module_entry str_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "str",
    str_functions,
    PHP_MINIT(str),
    PHP_MSHUTDOWN(str),
    NULL,
    NULL,
    PHP_MINFO(str),
#if ZEND_MODULE_API_NO >= 20010901
    "0.1",
#endif
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_STR
ZEND_GET_MODULE(str)
#endif

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(str)
{
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(str)
{
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(str)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "str support", "enabled");
    php_info_print_table_end();
}
/* }}} */

/* {{{ proto bool str_startswith(string haystack, string needle [, bool case_sensitivity])
   Binary safe optionally case sensitive check if haystack starts with needle */
PHP_FUNCTION(str_startswith)
{
    char *needle, *haystack;
    int needle_len, haystack_len, retval;
    zend_bool cs = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|b", &haystack, &haystack_len, &needle, &needle_len, &cs) == FAILURE) {
        RETURN_FALSE;
    }

    if (needle_len > haystack_len) {
        RETURN_FALSE;
    }

    if (cs) {
        retval = zend_binary_strncmp(needle, needle_len, haystack, haystack_len, needle_len);
    } else {
        retval = zend_binary_strncasecmp(needle, needle_len, haystack, haystack_len, needle_len);
    }

    RETURN_BOOL(retval == 0);
}
/* }}} */

/* {{{ proto bool str_endswith(string haystack, string needle [, bool case_sensitivity])
   Binary safe optionally case sensitive check if haystack ends with needle */
PHP_FUNCTION(str_endswith)
{
    char *needle, *haystack;
    int needle_len, haystack_len, retval;
    zend_bool cs = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|b", &haystack, &haystack_len, &needle, &needle_len, &cs) == FAILURE) {
        RETURN_FALSE;
    }

    if (needle_len > haystack_len) {
        RETURN_FALSE;
    }

    if (cs) {
        retval = zend_binary_strncmp(needle, needle_len, haystack + (haystack_len - needle_len), needle_len, needle_len);
    } else {
        retval = zend_binary_strncasecmp(needle, needle_len, haystack + (haystack_len - needle_len), needle_len, needle_len);
    }

    RETURN_BOOL(retval == 0);
}
/* }}} */

/* {{{ proto bool str_contains(string haystack, string needle [, bool case_sensitivity])
   Binary safe optionally case sensitive check if haystack contains needle */
PHP_FUNCTION(str_contains)
{
    char *needle, *haystack, *haystack_dup, *needle_dup;
    char *found = NULL;
    int needle_len, haystack_len;
    zend_bool cs = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|b", &haystack, &haystack_len, &needle, &needle_len, &cs) == FAILURE) {
        RETURN_FALSE;
    }

    if (needle_len > haystack_len || !haystack_len) {
        RETURN_FALSE;
    }

    if (!needle_len) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty delimiter");
        RETURN_FALSE;
    }

    /* TODO: mimick more of the logic from strpos, i.e. make needle
     * a zval.
     */
    if (cs) {
        found = php_memnstr(haystack, needle, needle_len, haystack + haystack_len);
    } else {
        haystack_dup = estrndup(haystack, haystack_len);
        php_strtolower(haystack_dup, haystack_len);

        needle_dup = estrndup(needle, needle_len);
        php_strtolower(needle_dup, needle_len);
        found = php_memnstr(haystack_dup, needle_dup, needle_len, haystack_dup + haystack_len);

        efree(haystack_dup);
        efree(needle_dup);
    }

    RETURN_BOOL(found);
}
/* }}} */

/* {{{ proto bool str_isupper(string input)
   Checks if all cased characters in the given input string are uppercase and if the
   string contains at least one cased character. */
PHP_FUNCTION(str_isupper)
{
    unsigned char *input, *end;
    unsigned char c;
    int input_len;
    int has_cased_char = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &input, &input_len) == FAILURE) {
        RETURN_FALSE;
    }

    /* A string must at least contain one cased character in order to
     * yield a positive result.
     */
    if (input_len == 0) {
        RETURN_FALSE;
    }

    /* Shortcut for strings that consist of just a single character. */
    if (input_len == 1) {
        RETURN_BOOL(isupper(*input) != 0)
    }

    for (end = input + input_len; input < end; input++) {
        c = *input;

        if (islower(c)) {
            RETURN_FALSE;
        }

        if (!has_cased_char && isupper(c)) {
            has_cased_char = 1;
        }
    }

    RETURN_BOOL(has_cased_char == 1);
}
/* }}} */

/* {{{ proto bool str_islower(string input)
   Checks if all cased characters in the given input string are lowercase and if the
   string contains at least one cased character. */
PHP_FUNCTION(str_islower)
{
    unsigned char *input, *end;
    unsigned char c;
    int input_len;
    int has_cased_char = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &input, &input_len) == FAILURE) {
        RETURN_FALSE;
    }

    // A string must at least contain one cased character in order to yield a positive result.
    if (input_len == 0) {
        RETURN_FALSE;
    }

    // Shortcut for strings that consist of just a single character.
    if (input_len == 1) {
        RETURN_BOOL(islower(*input) != 0)
    }

    for (end = input + input_len; input < end; input++) {
        c = *input;

        if (isupper(c)) {
            RETURN_FALSE;
        }

        if (!has_cased_char && islower(c)) {
            has_cased_char = 1;
        }
    }

    RETURN_BOOL(has_cased_char == 1);
}
/* }}} */

/* {{{ proto bool str_iswhitespace(string input)
   Checks if the given string is empty or contains whitespace characters
   only. */
PHP_FUNCTION(str_iswhitespace)
{
    unsigned char *input, *end;
    int input_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &input, &input_len) == FAILURE) {
        RETURN_FALSE;
    }

    // Empty strings yield a positive result.  Need to think about this some more.
    if (input_len == 0) {
        RETURN_TRUE;
    }

    // Shortcut for strings that consist of just a single character.
    if (input_len == 1) {
        RETURN_BOOL(isspace(*input))
    }

    for (end = input + input_len; input < end; input++) {
        if (!isspace(*input)) {
            RETURN_FALSE;
        }
    }

    RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool str_swapcase(string input)
   Converts uppercase characters in the given string to their lowercase representations and vice versa. */
PHP_FUNCTION(str_swapcase)
{
    char *input;
    unsigned char *c, *end;
    int input_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &input, &input_len) == FAILURE) {
        RETURN_FALSE;
    }

    input = estrndup(input, input_len);

    c = (unsigned char *)input;
    end = (unsigned char *)c + input_len;

    while (c < end) {
        if (isupper(*c)) {
            *c = tolower(*c);
        } else if (islower(*c)) {
            *c = toupper(*c);
        }

        c++;
    }

    RETURN_STRINGL(input, input_len, 0)
}
/* }}} */

bool startsWith(const char *pre, const char *str)
{
	if (str == NULL || pre == NULL) return 0;

    size_t len_pre = strlen(pre),
           len_str = strlen(str);

	if (len_pre == 0) return 1;

    return len_str < len_pre ? false : strncmp(pre, str, len_pre) == 0;
}

char* subString (const char* input, int offset, int len, char* dest)
{
  int input_len = strlen (input);

  if (offset + len > input_len) {
     return NULL;
  }

  strncpy (dest, input + offset, len);
  return dest;
}

bool endsWith(const char *post, const char *str)
{
    if (str == NULL || post == NULL) return 0;

    size_t len_post = strlen(post),
           len_str = strlen(str);

    if (len_post == 0) return 1;

	return len_str > len_post && 0 == strcmp(str + len_str - len_post, post);
}

/* {{{ proto string str_wrap(string input, string lhs, string rhs)
   Wraps the given string into given left- and right-hand-side strings */
PHP_FUNCTION(str_wrap)
{
    char *input, *lhs, *rhs;
    zend_bool prevent_rewrap = 0;

    unsigned char *c, *end;
    int input_len, lhs_len, rhs_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss|b", &lhs, &lhs_len, &input, &input_len, &rhs, &rhs_len, &prevent_rewrap) == FAILURE) {
        RETURN_FALSE;
    }

    input = estrndup(input, input_len);
    lhs = estrndup(lhs, lhs_len);
    rhs = estrndup(rhs, rhs_len);

	if (prevent_rewrap == 1 && startsWith(lhs, input) && endsWith(rhs, input)) {
        RETURN_STRINGL(input, input_len, 0)
	}

	int result_len = lhs_len + input_len + rhs_len;
    char *result = malloc(result_len);
	result = estrndup(result, result_len);

    strcpy(result, lhs); /* use strcpy() here to initialize the result buffer */
    result = strcat(result, input);
    result = strcat(result, rhs);

    RETURN_STRINGL(result, result_len, 0)
}
/* }}} */

/* {{{ proto bool str_startsnumerical(string input)
   Checks whether the given string starts with a number, or optional a negative number. */
PHP_FUNCTION(str_startsnumerical)
{
    char *haystack;
    int haystack_len;
    zend_bool allow_negative = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|bb", &haystack, &haystack_len, &allow_negative) == FAILURE) {
        RETURN_FALSE;
    }

    if (haystack_len == 0) {
        RETURN_FALSE;
    }

    RETURN_BOOL( isdigit(haystack[0]) || (allow_negative && haystack_len > 1 && ('-' == haystack[0]) && isdigit(haystack[1])) );
}
/* }}} */

long strToLong(char *str) {
   if (str == NULL) return 0;

   char *ptr; /* pointer to offset after initial number(s) */

   return strtol(str, &ptr, 10);
}

/* {{{ proto bool str_intexplode(string input)
   Split the given string by "," or the optional given delimiter, into an array of integers. */
PHP_FUNCTION(str_intexplode)
{
    char *haystack, *delimiter = ",";
    zend_bool unique = false;
    int haystack_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|bs", &haystack, &haystack_len, &unique, &delimiter) == FAILURE) {
        RETURN_FALSE;
    }

    zval *arr;
    MAKE_STD_ZVAL(arr);
    array_init(arr);

    if (haystack_len > 0) {
    	int delimiter_len = strlen(delimiter);

    	if (delimiter_len > 0) {
			char *item;
			char *itemWrappedInDelimiter;

			int size = 0;
            long number;
            bool foundDelimiter = false;
			while ( (item = strsep(&haystack, delimiter)) ) {
			    foundDelimiter = true;
				number = atol(item);
				//if (unique == false || size == 0) {
				// @todo implement unique filter
                    add_next_index_long(arr, number);
                    size++;
				//}
			}
			if (foundDelimiter == false) {
			    // haystack is single number, w/o any delimiter
			    number = atol(haystack);    // @todo ensure haystack is numeric
			    add_next_index_long(arr, number);
			}
		}
    }

    RETVAL_ZVAL(arr, 0, 0);
    efree(arr);
}
/* }}} */

/* {{{ proto string str_unwrap(string input, string lhs, string rhs)
   Unwraps the given string (removes given left- and right-hand-side strings if found) */
PHP_FUNCTION(str_unwrap)
{
    char *input, *lhs, *rhs;
    int input_len, lhs_len, rhs_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss|b", &input, &input_len, &lhs, &lhs_len, &rhs, &rhs_len) == FAILURE) {
        RETURN_FALSE;
    }

    input = estrndup(input, input_len);
    lhs = estrndup(lhs, lhs_len);
    rhs = estrndup(rhs, rhs_len);

    bool startsWithLhs = startsWith(lhs, input);
    bool endsWithRhs   = endsWith(rhs, input);

	int result_len_fin = input_len - (startsWithLhs ? lhs_len : 0) - (endsWithRhs ? rhs_len : 0);
    char *result = malloc(result_len_fin);
	result = estrndup(result, result_len_fin);

    if (startsWithLhs) {
        strncpy(result, input + lhs_len, input_len - lhs_len);
        // if both LHS + RHS are found, RHS does not need to be removed, as it's cut-off via the available buffer length
    } else {
        if (endsWithRhs) {
            strncpy(result, input, input_len - rhs_len);
        } else {
            result = input;
        }
    }

    RETURN_STRINGL(result, result_len_fin, 0);
}
/* }}} */

/* {{{ proto string str_random(int length, bool upper, bool number, bool special)
   Generates a (mostly) speakable random string */
PHP_FUNCTION(str_random)
{
    int len;
    zend_bool upper, number, special;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|bbb", &len, &upper, &number, &special) == FAILURE) {
        RETURN_FALSE;
    }

    // len + 1 for nul character.
    char *str = emalloc(len + 1);

    bool has_upper   = false;
    bool has_number  = false;

    int offset, type;
    int last = 0;
    int offset_special = random() % (len - 1);
    int i = 0;
    char c, last_vowel = '-';

    while ((upper && has_upper == false) || (number && has_number == false)) {
        has_upper   = false;
        has_number  = false;

        last = 0;
        i = 0;

        while (i <= len) {
            type = random() % 20;

            // Avoid some cases
            if(last == 3) {
                type = 1;
            } else if(last == 2) {
                type = 1;
            } else if(last == 1) {
                type = (rand() % 5) == 1 ? 10 : 2;
            }

            // Generate
            if(type < 4) {
                // Vowels, numbers
                if (upper && !has_upper && (random() % 10) > 8) {
                    str[i] = "AEIOU"[random() % 5];
                    has_upper = true;
                } else if (number && !has_number && (random() % 10) > 8) {
                    str[i] = "0123456789"[random() % 10];
                    has_number = true;
                } else {
                    do {
                        str[i] = "aeiou"[random() % 5];
                        // prevent repeating the same lower-case vowel
                    } while(str[i] == last_vowel);

                    last_vowel = str[i];
                }
                i++;
                last = 1;
            } else if(type < 9) {
                // Consonants
                if (upper && !has_upper && (random() % 10) > 8) {
                    str[i] = "BCDFGHJKLMNOPQRSTVWXZ"[random() % 21];
                    has_upper = true;
                } else {
                    str[i] = "bcdfghjklmnopqrstvwxz"[random() % 21];
                }
                i++;
                last = 2;
            } else {
                // Speakable consonant pairs
                offset = (((random () % 32) + 1) * 2) - 1;
                str[i]     = "blbrclcrdrdwflfrglgrgwknkrkwmrphplpnprpsscshskslsmsnspstsvswtrtswh"[offset];
                i++;
                if (i <= len) {
                    str[i] = "blbrclcrdrdwflfrglgrgwknkrkwmrphplpnprpsscshskslsmsnspstsvswtrtswh"[offset + 1];
                    i++;
                }
                last = 3;
            }
        }
    }

    if (special) {
        str[random() % (len - 1)] = "!#$%&()*+,-.:;<=>?[]_|"[random() % 22];
    }

    str[len] = 0;

    RETURN_STRINGL(str, len, 0);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
