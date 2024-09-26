#include "uptest.h"
#include "bstr.h"
#include "log.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Generate a random string of a given length
char *generate_random_string(size_t length)
{
	static const char charset[] =
		"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char *random_string = malloc(length + 1);

	if (random_string) {
		for (size_t n = 0; n < length; n++) {
			int key = rand() % (int)(sizeof(charset) - 1);
			random_string[n] = charset[key];
		}
		random_string[length] = '\0';
	}
	return random_string;
}

char *safe_str_dup(const char *src)
{
	if (src == NULL) return NULL;

	size_t len = strlen(src) + 1;
	char *dest = malloc(len);

	if (dest == NULL) return NULL;

	strcpy(dest, src);
	return dest;
}

// Unit Tests

// Test for bstr_from_cstr and bstr_destroy
static unit_result test_bstr_create_and_destroy(void)
{
	bstr b = bstr_from_cstr("hello");

	UT_ASSERT(b != NULL);
	UT_ASSERT(b->slen == 5);
	UT_ASSERT(b->mlen >= 6);
	UT_ASSERT(strcmp((char *)b->data, "hello") == 0);

	UT_ASSERT(bstr_destroy(b) == BSTR_OK);
	return UNIT_PASS;
}

// Test for bstr_copy
static unit_result test_bstr_copy(void)
{
	bstr original = bstr_from_cstr("test copy");

	UT_ASSERT(original != NULL);

	bstr copy = bstr_copy(original);
	UT_ASSERT(copy != NULL);
	UT_ASSERT(copy->slen == original->slen);
	UT_ASSERT(strcmp((char *)copy->data, (char *)original->data) == 0);

	UT_ASSERT(bstr_destroy(original) == BSTR_OK);
	UT_ASSERT(bstr_destroy(copy) == BSTR_OK);
	return UNIT_PASS;
}

// Test for bstr_concat
static unit_result test_bstr_concat(void)
{
	bstr b1 = bstr_from_cstr("Hello");
	bstr b2 = bstr_from_cstr(" World");

	UT_ASSERT(b1 != NULL && b2 != NULL);

	UT_ASSERT(bstr_concat(b1, b2) == BSTR_OK);
	UT_ASSERT(strcmp((char *)b1->data, "Hello World") == 0);
	UT_ASSERT(b1->slen == 11);

	UT_ASSERT(bstr_destroy(b1) == BSTR_OK);
	UT_ASSERT(bstr_destroy(b2) == BSTR_OK);
	return UNIT_PASS;
}

// Test for bstr_assign
static unit_result test_bstr_assign(void)
{
	bstr b1 = bstr_from_cstr("Initial");
	bstr b2 = bstr_from_cstr("Assigned");

	UT_ASSERT(b1 != NULL && b2 != NULL);

	UT_ASSERT(bstr_assign(b1, b2) == BSTR_OK);
	UT_ASSERT(strcmp((char *)b1->data, "Assigned") == 0);
	UT_ASSERT(b1->slen == b2->slen);

	UT_ASSERT(bstr_destroy(b1) == BSTR_OK);
	UT_ASSERT(bstr_destroy(b2) == BSTR_OK);
	return UNIT_PASS;
}

// Test for bstr_mid
static unit_result test_bstr_mid(void)
{
	bstr b = bstr_from_cstr("Hello, World!");

	UT_ASSERT(b != NULL);

	bstr mid = bstr_mid(b, 7, 5);
	UT_ASSERT(mid != NULL);
	UT_ASSERT(strcmp((char *)mid->data, "World") == 0);
	UT_ASSERT(mid->slen == 5);

	UT_ASSERT(bstr_destroy(b) == BSTR_OK);
	UT_ASSERT(bstr_destroy(mid) == BSTR_OK);
	return UNIT_PASS;
}

// Test for bstr_tolower
static unit_result test_bstr_tolower(void)
{
	bstr b = bstr_from_cstr("LOWERCASE");

	UT_ASSERT(b != NULL);

	UT_ASSERT(bstr_tolower(b) == BSTR_OK);
	UT_ASSERT(strcmp((char *)b->data, "lowercase") == 0);

	UT_ASSERT(bstr_destroy(b) == BSTR_OK);
	return UNIT_PASS;
}

// Test for bstr_list_create and bstr_list_destroy
static unit_result test_bstr_list_create_and_destroy(void)
{
	bstr_list *list = bstr_list_create();

	UT_ASSERT(list != NULL);
	UT_ASSERT(list->qty == 0);
	UT_ASSERT(list->mlen >= 1);
	UT_ASSERT(list->entry != NULL);

	UT_ASSERT(bstr_list_destroy(list) == BSTR_OK);
	return UNIT_PASS;
}

// Test for bstr_list_alloc
static unit_result test_bstr_list_alloc(void)
{
	bstr_list *list = bstr_list_create();

	UT_ASSERT(list != NULL);

	int result = bstr_list_alloc(list, 10);
	UT_ASSERT(result == BSTR_OK);
	UT_ASSERT(list->mlen >= 10);

	UT_ASSERT(bstr_list_destroy(list) == BSTR_OK);
	return UNIT_PASS;
}

// Test for bstr_list_alloc_min
static unit_result test_bstr_list_alloc_min(void)
{
	bstr_list *list = bstr_list_create();

	UT_ASSERT(list != NULL);

	int result = bstr_list_alloc(list, 10);
	UT_ASSERT(result == BSTR_OK);
	UT_ASSERT(list->mlen >= 10);

	// Allocate minimum size that should not be less than the current qty
	result = bstr_list_alloc_min(list, 5);
	UT_ASSERT(result == BSTR_OK);
	UT_ASSERT(list->mlen >= 5);

	UT_ASSERT(bstr_list_destroy(list) == BSTR_OK);
	return UNIT_PASS;
}

// Test for bstr_split
static unit_result test_bstr_split(void)
{
	bstr str = bstr_from_cstr("split,this,string");

	UT_ASSERT(str != NULL);

	bstr_list *list = bstr_split(str, ',');
	UT_ASSERT(list != NULL);
	UT_ASSERT(list->qty == 3);
	UT_ASSERT(strcmp((char *)list->entry[0]->data, "split") == 0);
	UT_ASSERT(strcmp((char *)list->entry[1]->data, "this") == 0);
	UT_ASSERT(strcmp((char *)list->entry[2]->data, "string") == 0);

	UT_ASSERT(bstr_destroy(str) == BSTR_OK);
	UT_ASSERT(bstr_list_destroy(list) == BSTR_OK);
	return UNIT_PASS;
}

// Test for bstr_splits
static unit_result test_bstr_splits(void)
{
	bstr str = bstr_from_cstr("split;this,string");
	bstr split_chars = bstr_from_cstr(",;");

	UT_ASSERT(str != NULL && split_chars != NULL);

	bstr_list *list = bstr_splits(str, split_chars);
	UT_ASSERT(list != NULL);
	UT_ASSERT(list->qty == 3);
	UT_ASSERT(strcmp((char *)list->entry[0]->data, "split") == 0);
	UT_ASSERT(strcmp((char *)list->entry[1]->data, "this") == 0);
	UT_ASSERT(strcmp((char *)list->entry[2]->data, "string") == 0);

	UT_ASSERT(bstr_destroy(str) == BSTR_OK);
	UT_ASSERT(bstr_destroy(split_chars) == BSTR_OK);
	UT_ASSERT(bstr_list_destroy(list) == BSTR_OK);
	return UNIT_PASS;
}

static prop_result prop_bstr_concat(void *env)
{
	(void)env; // Unused parameter

	char *str1 = generate_random_string(8);
	char *str2 = generate_random_string(12);

	if (!str1 || !str2) {
		free(str1);
		free(str2);
		return PROP_FAIL;
	}

	bstr b1 = bstr_from_cstr(str1);
	bstr b2 = bstr_from_cstr(str2);

	if (!b1 || !b2) {
		bstr_destroy(b1);
		bstr_destroy(b2);
		free(str1);
		free(str2);
		return PROP_FAIL;
	}

	int initial_len = b1->slen;
	int total_len = b1->slen + b2->slen;

	int result = bstr_concat(b1, b2);
	if (result != BSTR_OK) {
		bstr_destroy(b1);
		bstr_destroy(b2);
		free(str1);
		free(str2);
		return PROP_FAIL;
	}

	int concat_length_check = (b1->slen == total_len);

	if (initial_len + b2->slen <= b1->mlen && b2->slen <= b1->slen) {
		int concat_content_check = (memcmp(b1->data + initial_len,
						   b2->data, b2->slen) == 0);

		bstr_destroy(b1);
		bstr_destroy(b2);
		free(str1);
		free(str2);

		return (concat_length_check &&
			concat_content_check) ? PROP_PASS : PROP_FAIL;
	} else {
		bstr_destroy(b1);
		bstr_destroy(b2);
		free(str1);
		free(str2);
		return PROP_FAIL; // Data overread prevention
	}
}

// Test for bstr_split_str
static unit_result test_bstr_split_str(void)
{
	bstr str = bstr_from_cstr("splitthisstring");
	bstr split_str = bstr_from_cstr("this");

	UT_ASSERT(str != NULL && split_str != NULL);

	bstr_list *list = bstr_split_str(str, split_str);
	UT_ASSERT(list != NULL);
	UT_ASSERT(list->qty == 2);
	UT_ASSERT(strcmp((char *)list->entry[0]->data, "split") == 0);
	UT_ASSERT(strcmp((char *)list->entry[1]->data, "string") == 0);

	UT_ASSERT(bstr_destroy(str) == BSTR_OK);
	UT_ASSERT(bstr_destroy(split_str) == BSTR_OK);
	UT_ASSERT(bstr_list_destroy(list) == BSTR_OK);
	return UNIT_PASS;
}

// Test for bstr_join
static unit_result test_bstr_join(void)
{
	bstr_list *list = bstr_list_create();

	UT_ASSERT(list != NULL);

	bstr entry1 = bstr_from_cstr("join");
	bstr entry2 = bstr_from_cstr("these");
	bstr entry3 = bstr_from_cstr("strings");

	UT_ASSERT(bstr_list_alloc(list, 3) == BSTR_OK);
	list->entry[0] = entry1;
	list->entry[1] = entry2;
	list->entry[2] = entry3;
	list->qty = 3;

	bstr sep = bstr_from_cstr(" ");
	bstr joined = bstr_join(list, sep);
	UT_ASSERT(joined != NULL);
	UT_ASSERT(strcmp((char *)joined->data, "join these strings") == 0);

	UT_ASSERT(bstr_destroy(sep) == BSTR_OK);
	UT_ASSERT(bstr_destroy(joined) == BSTR_OK);
	UT_ASSERT(bstr_list_destroy(list) == BSTR_OK);
	return UNIT_PASS;
}

// Property Tests

// Property: Creating a bstr from a C string should preserve the
// string's content and length
static prop_result prop_bstr_from_cstr(void *env)
{
	(void)env; // Unused parameter

	char *rand_str = generate_random_string(10);
	if (!rand_str) return PROP_FAIL;

	bstr b = bstr_from_cstr(rand_str);
	if (!b) {
		free(rand_str);
		return PROP_FAIL;
	}

	int cmp_result = strcmp((char *)b->data, rand_str);
	int length_result = (b->slen == (int)strlen(rand_str));

	bstr_destroy(b);
	free(rand_str);

	return (cmp_result == 0 && length_result) ? PROP_PASS : PROP_FAIL;
}

// Property: Copying a bstr should result in two different objects
// with the same content
static prop_result prop_bstr_copy(void *env)
{
	(void)env; // Unused parameter

	char *rand_str = generate_random_string(15);
	if (!rand_str) return PROP_FAIL;

	bstr b1 = bstr_from_cstr(rand_str);
	if (!b1) {
		free(rand_str);
		return PROP_FAIL;
	}

	bstr b2 = bstr_copy(b1);
	if (!b2) {
		bstr_destroy(b1);
		free(rand_str);
		return PROP_FAIL;
	}

	int cmp_result = strcmp((char *)b1->data, (char *)b2->data);
	int different_objects = (b1 != b2);

	bstr_destroy(b1);
	bstr_destroy(b2);
	free(rand_str);

	return (cmp_result == 0 && different_objects) ? PROP_PASS : PROP_FAIL;
}



// Property: Converting a string to lowercase should match the result
// of a manual tolower conversion
static prop_result prop_bstr_tolower(void *env)
{
	(void)env; // Unused parameter

	char *rand_str = generate_random_string(15);
	if (!rand_str) return PROP_FAIL;

	bstr b = bstr_from_cstr(rand_str);
	if (!b) {
		free(rand_str);
		return PROP_FAIL;
	}

	char *lower_str = safe_str_dup(rand_str);
	if (!lower_str) {
		bstr_destroy(b);
		free(rand_str);
		return PROP_FAIL;
	}

	// Convert the duplicated string to lowercase manually
	for (int i = 0; lower_str[i]; i++)
		lower_str[i] = (char)tolower((unsigned char)lower_str[i]);

	int result = bstr_tolower(b);
	int tolower_check = (strcmp((char *)b->data, lower_str) == 0);

	bstr_destroy(b);
	free(rand_str);
	free(lower_str);

	return (result == BSTR_OK && tolower_check) ? PROP_PASS : PROP_FAIL;
}

// Property: Allocating more memory should not decrease the current length and should be >= requested size
static prop_result prop_bstr_list_alloc(void *env)
{
	(void)env; // Unused parameter

	bstr_list *list = bstr_list_create();
	if (!list)
		return PROP_FAIL;

	int initial_size = 5;


	if (bstr_list_alloc(list, initial_size) != BSTR_OK) {
		bstr_list_destroy(list);
		return PROP_FAIL;
	}

	// The allocated size should be at least the requested size
	if (list->mlen < initial_size) {
		bstr_list_destroy(list);
		return PROP_FAIL;
	}

	int min_size = 2;


	if (bstr_list_alloc_min(list, min_size) != BSTR_OK) {
		bstr_list_destroy(list);
		return PROP_FAIL;
	}

	// The allocated size should still be at least the initial allocation size
	if (list->mlen < initial_size) {
		bstr_list_destroy(list);
		return PROP_FAIL;
	}

	bstr_list_destroy(list);
	return PROP_PASS;
}


// Property: Inserting a substring at various positions should yield expected results
static prop_result prop_bstr_insert(void *env)
{
	(void)env; // Unused parameter

	char *base_str = generate_random_string(10);
	char *insert_str = generate_random_string(5);
	if (!base_str || !insert_str) return PROP_FAIL;

	bstr base = bstr_from_cstr(base_str);
	bstr insert = bstr_from_cstr(insert_str);
	if (!base || !insert) {
		free(base_str);
		free(insert_str);
		return PROP_FAIL;
	}

	int original_length = base->slen;
	int insert_pos = rand() % (original_length + 1); // Insert at a random valid position

	if (bstr_insert(base, insert_pos, insert, ' ') != BSTR_OK) {
		bstr_destroy(base);
		bstr_destroy(insert);
		free(base_str);
		free(insert_str);
		return PROP_FAIL;
	}

	// Check if the length has increased correctly
	if (base->slen != original_length + insert->slen) {
		bstr_destroy(base);
		bstr_destroy(insert);
		free(base_str);
		free(insert_str);
		return PROP_FAIL;
	}

	// Verify content correctness after insertion
	if (memcmp(base->data + insert_pos, insert->data, insert->slen) != 0) {
		bstr_destroy(base);
		bstr_destroy(insert);
		free(base_str);
		free(insert_str);
		return PROP_FAIL;
	}

	bstr_destroy(base);
	bstr_destroy(insert);
	free(base_str);
	free(insert_str);
	return PROP_PASS;
}

// Property: Comparing strings should respect lexicographical order and case sensitivity
static prop_result prop_bstr_cmp_icmp(void *env)
{
	(void)env; // Unused parameter

	char *str1 = generate_random_string(10);
	char *str2 = safe_str_dup(str1);
	if (!str1 || !str2) return PROP_FAIL;

	bstr b1 = bstr_from_cstr(str1);
	bstr b2 = bstr_from_cstr(str2);
	if (!b1 || !b2) {
		free(str1);
		free(str2);
		return PROP_FAIL;
	}

	// Check that identical strings return 0
	if (bstr_cmp(b1, b2) != 0) {
		bstr_destroy(b1);
		bstr_destroy(b2);
		free(str1);
		free(str2);
		return PROP_FAIL;
	}

	// Modify one string to differ only in case
	for (int i = 0; str2[i]; i++)
		str2[i] = tolower(str2[i]);
	bstr_icmp(b1, b2);

	if (bstr_icmp(b1, b2) != 0) {
		bstr_destroy(b1);
		bstr_destroy(b2);
		free(str1);
		free(str2);
		return PROP_FAIL;
	}

	bstr_destroy(b1);
	bstr_destroy(b2);
	free(str1);
	free(str2);
	return PROP_PASS;
}

// Property: Splitting a string by a character should yield expected substrings
static prop_result prop_bstr_split_splits(void *env)
{
	(void)env; // Unused parameter

	char *rand_str = generate_random_string(15);
	if (!rand_str) return PROP_FAIL;

	bstr str = bstr_from_cstr(rand_str);
	if (!str) {
		free(rand_str);
		return PROP_FAIL;
	}

	unsigned char split_char = rand_str[0]; // Split by the first character
	bstr_list *list = bstr_split(str, split_char);
	if (!list) {
		bstr_destroy(str);
		free(rand_str);
		return PROP_FAIL;
	}

	int combined_length = 0;
	for (int i = 0; i < list->qty; i++)
		combined_length += list->entry[i]->slen;

	// Check that combined length of substrings matches original
	if (combined_length != str->slen - (list->qty - 1)) {
		bstr_destroy(str);
		bstr_list_destroy(list);
		free(rand_str);
		return PROP_FAIL;
	}

	bstr_destroy(str);
	bstr_list_destroy(list);
	free(rand_str);
	return PROP_PASS;
}

// Property: Truncating a string should result in the correct length and content
static prop_result prop_bstr_trunc(void *env)
{
	(void)env; // Unused parameter

	char *rand_str = generate_random_string(20);
	if (!rand_str) return PROP_FAIL;

	bstr str = bstr_from_cstr(rand_str);
	if (!str) {
		free(rand_str);
		return PROP_FAIL;
	}

	int new_length = rand() % (str->slen + 1); // Random length <= original length

	if (bstr_trunc(str, new_length) != BSTR_OK) {
		bstr_destroy(str);
		free(rand_str);
		return PROP_FAIL;
	}

	// Check that the new length is as expected
	if (str->slen != new_length) {
		bstr_destroy(str);
		free(rand_str);
		return PROP_FAIL;
	}

	// Ensure the truncated content is preserved correctly
	if (memcmp(str->data, rand_str, new_length) != 0) {
		bstr_destroy(str);
		free(rand_str);
		return PROP_FAIL;
	}

	bstr_destroy(str);
	free(rand_str);
	return PROP_PASS;
}

// Property: Joining strings should yield a correctly concatenated result
static prop_result prop_bstr_join(void *env)
{
	(void)env; // Unused parameter

	bstr_list *list = bstr_list_create();
	if (!list) return PROP_FAIL;

	// Create random strings and add them to the list
	for (int i = 0; i < 5; i++) {
		char *rand_str = generate_random_string(5);
		if (!rand_str) {
			bstr_list_destroy(list);
			return PROP_FAIL;
		}
		bstr str = bstr_from_cstr(rand_str);
		free(rand_str);
		if (!str || bstr_list_alloc(list, list->qty + 1) != BSTR_OK) {
			bstr_destroy(str);
			bstr_list_destroy(list);
			return PROP_FAIL;
		}
		list->entry[list->qty++] = str;
	}

	// Use a random separator
	char *sep_str = generate_random_string(2);
	bstr sep = bstr_from_cstr(sep_str);
	free(sep_str);
	if (!sep) {
		bstr_list_destroy(list);
		return PROP_FAIL;
	}

	bstr joined = bstr_join(list, sep);
	if (!joined) {
		bstr_destroy(sep);
		bstr_list_destroy(list);
		return PROP_FAIL;
	}

	// Verify the length of the joined string
	int expected_length = 0;
	for (int i = 0; i < list->qty; i++)
		expected_length += list->entry[i]->slen;
	expected_length += (list->qty - 1) * sep->slen;

	if (joined->slen != expected_length) {
		bstr_destroy(sep);
		bstr_destroy(joined);
		bstr_list_destroy(list);
		return PROP_FAIL;
	}

	bstr_destroy(sep);
	bstr_destroy(joined);
	bstr_list_destroy(list);
	return PROP_PASS;
}

// Define the property tests using the macros
PROP_TEST(prop_bstr_from_cstr_test, prop_bstr_from_cstr);
PROP_TEST(prop_bstr_copy_test, prop_bstr_copy);
PROP_TEST(prop_bstr_concat_test, prop_bstr_concat);
PROP_TEST(prop_bstr_trunc_test, prop_bstr_trunc);
PROP_TEST(prop_bstr_tolower_test, prop_bstr_tolower);
PROP_TEST(prop_bstr_list_alloc_test, prop_bstr_list_alloc);
PROP_TEST(prop_bstr_insert_test, prop_bstr_insert);
PROP_TEST(prop_bstr_cmp_icmp_test, prop_bstr_cmp_icmp);
PROP_TEST(prop_bstr_split_splits_test, prop_bstr_split_splits);
PROP_TEST(prop_bstr_join_test, prop_bstr_join);

// Define the unit tests using the macros
UNIT_TEST(test_bstr_create_and_destroy_test, test_bstr_create_and_destroy);
UNIT_TEST(test_bstr_copy_test, test_bstr_copy);
UNIT_TEST(test_bstr_concat_test, test_bstr_concat);
UNIT_TEST(test_bstr_assign_test, test_bstr_assign);
UNIT_TEST(test_bstr_mid_test, test_bstr_mid);
UNIT_TEST(test_bstr_tolower_test, test_bstr_tolower);
UNIT_TEST(test_bstr_list_create_and_destroy_test,
	  test_bstr_list_create_and_destroy);
UNIT_TEST(test_bstr_list_alloc_test, test_bstr_list_alloc);
UNIT_TEST(test_bstr_list_alloc_min_test, test_bstr_list_alloc_min);
UNIT_TEST(test_bstr_split_test, test_bstr_split);
UNIT_TEST(test_bstr_splits_test, test_bstr_splits);
UNIT_TEST(test_bstr_split_str_test, test_bstr_split_str);
UNIT_TEST(test_bstr_join_test, test_bstr_join);

// Main function to run all tests
int main(void)
{
	RUN_UNIT_TESTS(
		test_bstr_create_and_destroy_test,
		test_bstr_copy_test,
		test_bstr_concat_test,
		test_bstr_assign_test,
		test_bstr_mid_test,
		test_bstr_tolower_test,
		test_bstr_list_create_and_destroy_test,
		test_bstr_list_alloc_test,
		test_bstr_list_alloc_min_test,
		test_bstr_split_test,
		test_bstr_splits_test,
		test_bstr_split_str_test,
		test_bstr_join_test
		);

	RUN_PROP_TESTS(
		prop_bstr_from_cstr_test,
		prop_bstr_copy_test,
		prop_bstr_concat_test,
		prop_bstr_trunc_test,
		prop_bstr_tolower_test,
		prop_bstr_list_alloc_test,
		prop_bstr_insert_test,
		prop_bstr_cmp_icmp_test,
		prop_bstr_split_splits_test,
		prop_bstr_join_test
		);

	uptest_summary(); // Print the unified summary

	return 0;
}
