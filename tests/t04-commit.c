/*
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2,
 * as published by the Free Software Foundation.
 *
 * In addition to the permissions in the GNU General Public License,
 * the authors give you unlimited permission to link the compiled
 * version of this file into combinations with other programs,
 * and to distribute those combinations without any restriction
 * coming from the use of this file.  (The General Public License
 * restrictions do apply in other respects; for example, they cover
 * modification of the file, and distribution when not linked into
 * a combined executable.)
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "test_lib.h"
#include "test_helpers.h"

#include "commit.h"
#include "signature.h"

static char *test_commits_broken[] = {

/* empty commit */
"",

/* random garbage */
"asd97sa9du902e9a0jdsuusad09as9du098709aweu8987sd\n",

/* broken endlines 1 */
"tree f6c0dad3c7b3481caa9d73db21f91964894a945b\r\n\
parent 05452d6349abcd67aa396dfb28660d765d8b2a36\r\n\
author Vicent Marti <tanoku@gmail.com> 1273848544 +0200\r\n\
committer Vicent Marti <tanoku@gmail.com> 1273848544 +0200\r\n\
\r\n\
a test commit with broken endlines\r\n",

/* broken endlines 2 */
"tree f6c0dad3c7b3481caa9d73db21f91964894a945b\
parent 05452d6349abcd67aa396dfb28660d765d8b2a36\
author Vicent Marti <tanoku@gmail.com> 1273848544 +0200\
committer Vicent Marti <tanoku@gmail.com> 1273848544 +0200\
\
another test commit with broken endlines",

/* starting endlines */
"\ntree f6c0dad3c7b3481caa9d73db21f91964894a945b\n\
parent 05452d6349abcd67aa396dfb28660d765d8b2a36\n\
author Vicent Marti <tanoku@gmail.com> 1273848544 +0200\n\
committer Vicent Marti <tanoku@gmail.com> 1273848544 +0200\n\
\n\
a test commit with a starting endline\n",

/* corrupted commit 1 */
"tree f6c0dad3c7b3481caa9d73db21f91964894a945b\n\
parent 05452d6349abcd67aa396df",

/* corrupted commit 2 */
"tree f6c0dad3c7b3481caa9d73db21f91964894a945b\n\
parent ",

/* corrupted commit 3 */
"tree f6c0dad3c7b3481caa9d73db21f91964894a945b\n\
parent ",

/* corrupted commit 4 */
"tree f6c0dad3c7b3481caa9d73db21f91964894a945b\n\
par",

};


static char *test_commits_working[] = {
/* simple commit with no message */
"tree 1810dff58d8a660512d4832e740f692884338ccd\n\
author Vicent Marti <tanoku@gmail.com> 1273848544 +0200\n\
committer Vicent Marti <tanoku@gmail.com> 1273848544 +0200\n\
\n",

/* simple commit, no parent */
"tree 1810dff58d8a660512d4832e740f692884338ccd\n\
author Vicent Marti <tanoku@gmail.com> 1273848544 +0200\n\
committer Vicent Marti <tanoku@gmail.com> 1273848544 +0200\n\
\n\
a simple commit which works\n",

/* simple commit, no parent, no newline in message */
"tree 1810dff58d8a660512d4832e740f692884338ccd\n\
author Vicent Marti <tanoku@gmail.com> 1273848544 +0200\n\
committer Vicent Marti <tanoku@gmail.com> 1273848544 +0200\n\
\n\
a simple commit which works",

/* simple commit, 1 parent */
"tree 1810dff58d8a660512d4832e740f692884338ccd\n\
parent e90810b8df3e80c413d903f631643c716887138d\n\
author Vicent Marti <tanoku@gmail.com> 1273848544 +0200\n\
committer Vicent Marti <tanoku@gmail.com> 1273848544 +0200\n\
\n\
a simple commit which works\n",
};

BEGIN_TEST(parse0, "parse the OID line in a commit")

	git_oid oid;

#define TEST_OID_PASS(string, header) { \
	const char *ptr = string;\
	const char *ptr_original = ptr;\
	size_t len = strlen(ptr);\
	must_pass(git__parse_oid(&oid, &ptr, ptr + len, header));\
	must_be_true(ptr == ptr_original + len);\
}

#define TEST_OID_FAIL(string, header) { \
	const char *ptr = string;\
	size_t len = strlen(ptr);\
	must_fail(git__parse_oid(&oid, &ptr, ptr + len, header));\
}

	TEST_OID_PASS("parent 05452d6349abcd67aa396dfb28660d765d8b2a36\n", "parent ");
	TEST_OID_PASS("tree 05452d6349abcd67aa396dfb28660d765d8b2a36\n", "tree ");
	TEST_OID_PASS("random_heading 05452d6349abcd67aa396dfb28660d765d8b2a36\n", "random_heading ");
	TEST_OID_PASS("stuck_heading05452d6349abcd67aa396dfb28660d765d8b2a36\n", "stuck_heading");
	TEST_OID_PASS("tree 5F4BEFFC0759261D015AA63A3A85613FF2F235DE\n", "tree ");
	TEST_OID_PASS("tree 1A669B8AB81B5EB7D9DB69562D34952A38A9B504\n", "tree ");
	TEST_OID_PASS("tree 5B20DCC6110FCC75D31C6CEDEBD7F43ECA65B503\n", "tree ");
	TEST_OID_PASS("tree 173E7BF00EA5C33447E99E6C1255954A13026BE4\n", "tree ");

	TEST_OID_FAIL("parent 05452d6349abcd67aa396dfb28660d765d8b2a36", "parent ");
	TEST_OID_FAIL("05452d6349abcd67aa396dfb28660d765d8b2a36\n", "tree ");
	TEST_OID_FAIL("parent05452d6349abcd67aa396dfb28660d765d8b2a6a\n", "parent ");
	TEST_OID_FAIL("parent 05452d6349abcd67aa396dfb280d765d8b2a6\n", "parent ");
	TEST_OID_FAIL("tree  05452d6349abcd67aa396dfb28660d765d8b2a36\n", "tree ");
	TEST_OID_FAIL("parent 0545xd6349abcd67aa396dfb28660d765d8b2a36\n", "parent ");
	TEST_OID_FAIL("parent 0545xd6349abcd67aa396dfb28660d765d8b2a36FF\n", "parent ");
	TEST_OID_FAIL("", "tree ");
	TEST_OID_FAIL("", "");

#undef TEST_OID_PASS
#undef TEST_OID_FAIL

END_TEST

BEGIN_TEST(parse1, "parse the signature line in a commit")

#define TEST_SIGNATURE_PASS(_string, _header, _name, _email, _time, _offset) { \
	const char *ptr = _string; \
	size_t len = strlen(_string);\
	git_signature person = {NULL, NULL, {0, 0}}; \
	must_pass(git_signature__parse(&person, &ptr, ptr + len, _header));\
	must_be_true(strcmp(_name, person.name) == 0);\
	must_be_true(strcmp(_email, person.email) == 0);\
	must_be_true(_time == person.when.time);\
	must_be_true(_offset == person.when.offset);\
	free(person.name); free(person.email);\
}

#define TEST_SIGNATURE_FAIL(_string, _header) { \
	const char *ptr = _string; \
	size_t len = strlen(_string);\
	git_signature person = {NULL, NULL, {0, 0}}; \
	must_fail(git_signature__parse(&person, &ptr, ptr + len, _header));\
	free(person.name); free(person.email);\
}

	TEST_SIGNATURE_PASS(
		"author Vicent Marti <tanoku@gmail.com> 12345 \n",
		"author ",
		"Vicent Marti",
		"tanoku@gmail.com",
		12345,
		0);

	TEST_SIGNATURE_PASS(
		"author Vicent Marti <> 12345 \n",
		"author ",
		"Vicent Marti",
		"",
		12345,
		0);

	TEST_SIGNATURE_PASS(
		"author Vicent Marti <tanoku@gmail.com> 231301 +1020\n",
		"author ",
		"Vicent Marti",
		"tanoku@gmail.com",
		231301,
		620);

	TEST_SIGNATURE_PASS(
		"author Vicent Marti with an outrageously long name \
		which will probably overflow the buffer <tanoku@gmail.com> 12345 \n",
		"author ",
		"Vicent Marti with an outrageously long name \
		which will probably overflow the buffer",
		"tanoku@gmail.com",
		12345,
		0);

	TEST_SIGNATURE_PASS(
		"author Vicent Marti <tanokuwithaveryveryverylongemail\
		whichwillprobablyvoverflowtheemailbuffer@gmail.com> 12345 \n",
		"author ",
		"Vicent Marti",
		"tanokuwithaveryveryverylongemail\
		whichwillprobablyvoverflowtheemailbuffer@gmail.com",
		12345,
		0);

	TEST_SIGNATURE_PASS(
		"committer Vicent Marti <tanoku@gmail.com> 123456 +0000 \n",
		"committer ",
		"Vicent Marti",
		"tanoku@gmail.com",
		123456,
		0);

	TEST_SIGNATURE_PASS(
		"committer Vicent Marti <tanoku@gmail.com> 123456 +0100 \n",
		"committer ",
		"Vicent Marti",
		"tanoku@gmail.com",
		123456,
		60);

	TEST_SIGNATURE_PASS(
		"committer Vicent Marti <tanoku@gmail.com> 123456 -0100 \n",
		"committer ",
		"Vicent Marti",
		"tanoku@gmail.com",
		123456,
		-60);

	TEST_SIGNATURE_FAIL(
		"committer Vicent Marti <tanoku@gmail.com> 123456 -1500 \n",
		"committer ");

	TEST_SIGNATURE_FAIL(
		"committer Vicent Marti <tanoku@gmail.com> 123456 +0163 \n",
		"committer ");

	TEST_SIGNATURE_FAIL(
		"author Vicent Marti <tanoku@gmail.com> 12345 \n",
		"author  ");

	TEST_SIGNATURE_FAIL(
		"author Vicent Marti <tanoku@gmail.com> 12345 \n",
		"committer ");

	TEST_SIGNATURE_FAIL(
		"author Vicent Marti 12345 \n",
		"author ");

	TEST_SIGNATURE_FAIL(
		"author Vicent Marti <broken@email 12345 \n",
		"author ");

	TEST_SIGNATURE_FAIL(
		"author Vicent Marti <tanoku@gmail.com> notime \n",
		"author ");

	TEST_SIGNATURE_FAIL(
		"author Vicent Marti <tanoku@gmail.com>\n",
		"author ");

	TEST_SIGNATURE_FAIL(
		"author ",
		"author ");

#undef TEST_SIGNATURE_PASS
#undef TEST_SIGNATURE_FAIL

END_TEST

/* External declaration for testing the buffer parsing method */
int commit_parse_buffer(git_commit *commit, void *data, size_t len, unsigned int parse_flags);

BEGIN_TEST(parse2, "parse a whole commit buffer")
	const int broken_commit_count = sizeof(test_commits_broken) / sizeof(*test_commits_broken);
	const int working_commit_count = sizeof(test_commits_working) / sizeof(*test_commits_working);

	int i;
	git_repository *repo;

	must_pass(git_repository_open(&repo, REPOSITORY_FOLDER));

	for (i = 0; i < broken_commit_count; ++i) {
		git_commit *commit;
		commit = git__malloc(sizeof(git_commit));
		memset(commit, 0x0, sizeof(git_commit));
		commit->object.repo = repo;

		must_fail(commit_parse_buffer(
					commit,
					test_commits_broken[i],
					strlen(test_commits_broken[i]),
					0x1)
				);

		git_commit__free(commit);
	}

	for (i = 0; i < working_commit_count; ++i) {
		git_commit *commit;

		commit = git__malloc(sizeof(git_commit));
		memset(commit, 0x0, sizeof(git_commit));
		commit->object.repo = repo;

		must_pass(commit_parse_buffer(
					commit,
					test_commits_working[i],
					strlen(test_commits_working[i]),
					0x0)
				);

		git_commit__free(commit);

		commit = git__malloc(sizeof(git_commit));
		memset(commit, 0x0, sizeof(git_commit));
		commit->object.repo = repo;

		must_pass(commit_parse_buffer(
					commit,
					test_commits_working[i],
					strlen(test_commits_working[i]),
					0x1)
				);

		git_commit__free(commit);
	}

	git_repository_free(repo);
END_TEST

static const char *commit_ids[] = {
	"a4a7dce85cf63874e984719f4fdd239f5145052f", /* 0 */
	"9fd738e8f7967c078dceed8190330fc8648ee56a", /* 1 */
	"4a202b346bb0fb0db7eff3cffeb3c70babbd2045", /* 2 */
	"c47800c7266a2be04c571c04d5a6614691ea99bd", /* 3 */
	"8496071c1b46c854b31185ea97743be6a8774479", /* 4 */
	"5b5b025afb0b4c913b4c338a42934a3863bf3644", /* 5 */
};

BEGIN_TEST(details0, "query the details on a parsed commit")
	const size_t commit_count = sizeof(commit_ids) / sizeof(const char *);

	unsigned int i;
	git_repository *repo;

	must_pass(git_repository_open(&repo, REPOSITORY_FOLDER));
	
	for (i = 0; i < commit_count; ++i) {
		git_oid id;
		git_commit *commit;

		const git_signature *author, *committer;
		const char *message, *message_short;
		git_time_t commit_time;
		unsigned int parents, p;
		git_commit *parent = NULL, *old_parent = NULL;

		git_oid_mkstr(&id, commit_ids[i]);

		must_pass(git_commit_lookup(&commit, repo, &id));

		message = git_commit_message(commit);
		message_short = git_commit_message_short(commit);
		author = git_commit_author(commit);
		committer = git_commit_committer(commit);
		commit_time = git_commit_time(commit);
		parents = git_commit_parentcount(commit);

		must_be_true(strcmp(author->name, "Scott Chacon") == 0);
		must_be_true(strcmp(author->email, "schacon@gmail.com") == 0);
		must_be_true(strcmp(committer->name, "Scott Chacon") == 0);
		must_be_true(strcmp(committer->email, "schacon@gmail.com") == 0);
		must_be_true(strchr(message, '\n') != NULL);
		must_be_true(strchr(message_short, '\n') == NULL);
		must_be_true(commit_time > 0);
		must_be_true(parents <= 2);
		for (p = 0;p < parents;p++) {
			if (old_parent != NULL)
				git_commit_close(old_parent);

			old_parent = parent;
			must_pass(git_commit_parent(&parent, commit, p));
			must_be_true(parent != NULL);
			must_be_true(git_commit_author(parent) != NULL); // is it really a commit?
		}
		git_commit_close(old_parent);
		git_commit_close(parent);

		must_fail(git_commit_parent(&parent, commit, parents));
		git_commit_close(commit);
	}

	git_repository_free(repo);
END_TEST

#define COMMITTER_NAME "Vicent Marti"
#define COMMITTER_EMAIL "vicent@github.com"
#define COMMIT_MESSAGE "This commit has been created in memory\n\
This is a commit created in memory and it will be written back to disk\n"

static const char *tree_oid = "1810dff58d8a660512d4832e740f692884338ccd";


BEGIN_TEST(write0, "write a new commit object from memory to disk")
	git_repository *repo;
	git_commit *commit;
	git_oid tree_id, parent_id, commit_id;
	const git_signature *author, *committer;
	/* char hex_oid[41]; */

	must_pass(git_repository_open(&repo, REPOSITORY_FOLDER));


	git_oid_mkstr(&tree_id, tree_oid);
	git_oid_mkstr(&parent_id, commit_ids[4]);

	/* create signatures */
	committer = git_signature_new(COMMITTER_NAME, COMMITTER_EMAIL, 123456789, 60);
	must_be_true(committer != NULL);

	author = git_signature_new(COMMITTER_NAME, COMMITTER_EMAIL, 987654321, 90);
	must_be_true(author != NULL);

	must_pass(git_commit_create_v(
		&commit_id, /* out id */
		repo,
		NULL, /* do not update the HEAD */
		author,
		committer,
		COMMIT_MESSAGE,
		&tree_id,
		1, &parent_id));

	git_signature_free((git_signature *)committer);
	git_signature_free((git_signature *)author);

	must_pass(git_commit_lookup(&commit, repo, &commit_id));

	/* Check attributes were set correctly */
	author = git_commit_author(commit);
	must_be_true(author != NULL);
	must_be_true(strcmp(author->name, COMMITTER_NAME) == 0);
	must_be_true(strcmp(author->email, COMMITTER_EMAIL) == 0);
	must_be_true(author->when.time == 987654321);
	must_be_true(author->when.offset == 90);

	committer = git_commit_committer(commit);
	must_be_true(committer != NULL);
	must_be_true(strcmp(committer->name, COMMITTER_NAME) == 0);
	must_be_true(strcmp(committer->email, COMMITTER_EMAIL) == 0);
	must_be_true(committer->when.time == 123456789);
	must_be_true(committer->when.offset == 60);

	must_be_true(strcmp(git_commit_message(commit), COMMIT_MESSAGE) == 0);

	must_pass(remove_loose_object(REPOSITORY_FOLDER, (git_object *)commit));

	git_commit_close(commit);
	git_repository_free(repo);
END_TEST


BEGIN_SUITE(commit)
	ADD_TEST(parse0);
	ADD_TEST(parse1);
	ADD_TEST(parse2);
	ADD_TEST(details0);

	ADD_TEST(write0);
	//ADD_TEST(write1);
END_SUITE
