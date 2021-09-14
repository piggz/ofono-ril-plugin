/*
 *  oFono - Open Source Telephony
 *
 *  Copyright (C) 2019-2021 Jolla Ltd.
 *  Copyright (C) 2019 Open Mobile Platform LLC.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 */

#include "test_common.h"

#include "ril_ecclist.h"
#include "ril_log.h"

#include <gutil_strv.h>

#include <sys/stat.h>

#define TMP_DIR_TEMPLATE "test-ril_ecclist-XXXXXX"

GLOG_MODULE_DEFINE("ril");

static TestOpt test_opt;

struct ril_ecclist_parse_test {
	const char* name;
	const char* in;
	const char* const* out;
};

static void test_quit_cb(struct ril_ecclist *ecc, gpointer loop)
{
	test_quit_later(loop);
}

static void test_inc_cb(struct ril_ecclist *ecc, gpointer ptr)
{
	(*(int*)ptr)++;
}

/* ==== parse ==== */

static void test_parse(gconstpointer data)
{
	const struct ril_ecclist_parse_test *test = data;
	char *dir = g_dir_make_tmp(TMP_DIR_TEMPLATE, NULL);
	char *file = g_build_filename(dir, "ecclist", NULL);
	struct ril_ecclist *ecc;

	GDEBUG("Created file %s", file);
	g_assert(g_file_set_contents(file, test->in, -1, NULL));
	ecc = ril_ecclist_new(file);
	g_assert(gutil_strv_equal(ecc->list, (char**)test->out));

	ril_ecclist_unref(ecc);
	remove(file);
	remove(dir);
	g_free(file);
	g_free(dir);
}

static const char* null_str = NULL;
static const char single_str_in[] = "911";
static const char* single_str_out[] = { "911", NULL };
static const char double_str_in[] = "911,112";
static const char double2_str_in[] = "911, 112,";
static const char double3_str_in[] = "911, 911, 112 ";
static const char* double_str_out[] = { "112", "911", NULL };
static const char mtk_str_in[] = "112,31;911,31;112,-1;911,-1";
static const char mtk2_str_in[] = "112,31; 911,31; 112; 911 ";

static const struct ril_ecclist_parse_test tests[] = {
	{ "empty", "", &null_str },
	{ "single", single_str_in, single_str_out },
	{ "double", double_str_in, double_str_out },
	{ "double2", double2_str_in, double_str_out },
	{ "double3", double3_str_in, double_str_out },
	{ "mtk", mtk_str_in, double_str_out },
	{ "mtk2", mtk2_str_in, double_str_out }
};

/* ==== file_perm ==== */

static void test_file_perm()
{
	char *dir = g_dir_make_tmp(TMP_DIR_TEMPLATE, NULL);
	char *file = g_build_filename(dir, "ecclist", NULL);
	int count = 0;
	struct ril_ecclist *ecc;
	gulong id[2];
	GMainLoop *loop = g_main_loop_new(NULL, FALSE);

	GDEBUG("Created file %s", file);
	g_assert(g_file_set_contents(file, single_str_in, -1, NULL));
	ecc = ril_ecclist_new(file);
	id[0] = ril_ecclist_add_list_changed_handler(ecc, test_inc_cb, &count);
	id[1] = ril_ecclist_add_list_changed_handler(ecc, test_quit_cb, loop);

	g_assert(id[0]);
	g_assert(id[1]);
	g_assert(gutil_strv_equal(ecc->list, (char**)single_str_out));

	/* Modify the file */
	g_assert(g_file_set_contents(file, double_str_in, -1, NULL));

	/* ril_ecclist needs event loop to process filesystem change events */
	test_run(&test_opt, loop);

	g_assert(count == 1);
	g_assert(gutil_strv_equal(ecc->list, (char**)double_str_out));

	/* Making file unreadable resets the ecc list */
	GDEBUG("Making file %s unreadable", file);
	g_assert(g_file_set_contents(file, single_str_in, -1, NULL));
	g_assert(chmod(file, 0) == 0);
	count = 0;
	g_main_loop_run(loop);
	g_assert(count == 1);
	g_assert(!ecc->list);

	g_main_loop_unref(loop);
	ril_ecclist_remove_handler(ecc, id[0]);
	ril_ecclist_remove_handler(ecc, id[1]);
	ril_ecclist_unref(ecc);
	remove(dir);
	g_free(file);
	g_free(dir);
}

/* ==== file_change ==== */

static void test_file_change()
{
	char *dir = g_dir_make_tmp(TMP_DIR_TEMPLATE, NULL);
	char *file = g_build_filename(dir, "ecclist", NULL);
	int count = 0;
	struct ril_ecclist *ecc;
	gulong id[2];
	GMainLoop *loop = g_main_loop_new(NULL, FALSE);

	GDEBUG("Created file %s", file);
	g_assert(g_file_set_contents(file, single_str_in, -1, NULL));
	ecc = ril_ecclist_new(file);
	id[0] = ril_ecclist_add_list_changed_handler(ecc, test_inc_cb, &count);
	id[1] = ril_ecclist_add_list_changed_handler(ecc, test_quit_cb, loop);

	g_assert(id[0]);
	g_assert(id[1]);
	g_assert(gutil_strv_equal(ecc->list, (char**)single_str_out));

	/* Modify the file */
	g_assert(g_file_set_contents(file, double_str_in, -1, NULL));

	/* ril_ecclist needs event loop to process filesystem change events */
	test_run(&test_opt, loop);

	g_assert(count == 1);
	g_assert(gutil_strv_equal(ecc->list, (char**)double_str_out));

	/* Removing the file resets the ecc list */
	GDEBUG("Removing file %s", file);
	g_assert(remove(file) == 0);
	count = 0;
	g_main_loop_run(loop);
	g_assert(count == 1);
	g_assert(!ecc->list);

	g_main_loop_unref(loop);
	ril_ecclist_remove_handler(ecc, id[0]);
	ril_ecclist_remove_handler(ecc, id[1]);
	ril_ecclist_unref(ecc);
	remove(dir);
	g_free(file);
	g_free(dir);
}

/* ==== dir_change ==== */

static void test_dir_change()
{
	char *dir = g_dir_make_tmp(TMP_DIR_TEMPLATE, NULL);
	char *file = g_build_filename(dir, "ecclist", NULL);
	int count = 0;
	struct ril_ecclist *ecc;
	gulong id[3];
	GMainLoop *loop = g_main_loop_new(NULL, FALSE);

	GDEBUG("Created directory %s", dir);
	ecc = ril_ecclist_new(file);
	id[0] = ril_ecclist_add_list_changed_handler(ecc, test_inc_cb, &count);
	id[1] = ril_ecclist_add_list_changed_handler(ecc, test_quit_cb, loop);

	g_assert(id[0]);
	g_assert(id[1]);
	g_assert(!ecc->list);
	GDEBUG("Created file %s", file);
	g_assert(g_file_set_contents(file, single_str_in, -1, NULL));

	/* ril_ecclist needs event loop to process filesystem change events */
	test_run(&test_opt, loop);

	g_assert(count == 1);
	g_assert(gutil_strv_equal(ecc->list, (char**)single_str_out));

	/* Removing the directory resets the ecc list */
	GDEBUG("Removing directory %s", dir);
	g_assert(remove(file) == 0);
	g_assert(remove(dir) == 0);
	count = 0;
	g_main_loop_run(loop);
	g_assert(count == 1);
	g_assert(!ecc->list);

	g_main_loop_unref(loop);
	ril_ecclist_remove_handler(ecc, id[0]);
	ril_ecclist_remove_handler(ecc, id[1]);
	ril_ecclist_unref(ecc);
	g_free(file);
	g_free(dir);
}

/* ==== null ==== */

static void test_null(void)
{
	char *dir = g_dir_make_tmp(TMP_DIR_TEMPLATE, NULL);
	char *file = g_build_filename(dir, "ecclist", NULL);
	struct ril_ecclist *ecc;

	/* Make sure neither directory nor file exist */
	remove(dir);
	ecc = ril_ecclist_new(file);
	g_assert(ecc);
	g_assert(!ecc->list);
	g_assert(!ril_ecclist_new(NULL));
	g_assert(!ril_ecclist_ref(NULL));
	g_assert(!ril_ecclist_add_list_changed_handler(NULL, NULL, NULL));
	g_assert(!ril_ecclist_add_list_changed_handler(ecc, NULL, NULL));
	ril_ecclist_unref(NULL);
	ril_ecclist_remove_handler(NULL, 0);
	ril_ecclist_remove_handler(ecc, 0);
	ril_ecclist_unref(ril_ecclist_ref(ecc));
	ril_ecclist_unref(ecc);

	g_free(file);
	g_free(dir);
}

#define TEST_(name) "/ril_ecclist/" name

int main(int argc, char *argv[])
{
	int i;

	g_test_init(&argc, &argv, NULL);
	for (i = 0; i < G_N_ELEMENTS(tests); i++) {
		const struct ril_ecclist_parse_test* test = tests + i;
		char* path = g_strconcat(TEST_("parse/"), test->name, NULL);

		g_test_add_data_func(path, test, test_parse);
		g_free(path);
	}
	g_test_add_func(TEST_("null"), test_null);
	g_test_add_func(TEST_("file_perm"), test_file_perm);
	g_test_add_func(TEST_("file_change"), test_file_change);
	g_test_add_func(TEST_("dir_change"), test_dir_change);
	test_init(&test_opt, argc, argv);
	return g_test_run();
}

/*
 * Local Variables:
 * mode: C
 * c-basic-offset: 8
 * indent-tabs-mode: t
 * End:
 */
