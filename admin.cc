/*
 * admin.c
 *
 * By Ross Ridge
 * Public Domain
 *
 * Administer and create SCCS files.
 *
 */ 

#include "mysc.h"
#include "sccsfile.h"
#include "sf-chkmr.h"
#include "fileiter.h"
#include "getopt.h"

const char main_sccs_id[] = "@(#) MySC admin.c 1.1 93/11/09 17:17:52";

void
usage() {
	fprintf(stderr,
"usage: %s [-nrzIMTYV] [-a users] [-d flags] [-e users] [-f flags]\n"
"\t[-i file] [-m MRs] [-t file] [-y comments] file ...\n",
	prg_name);
}

int
main(int argc, char **argv) {
	int c;
	release first_release = NULL;		/* -r */
	int new_sccs_file = 0;			/* -n */
	const char *iname = NULL;		/* -i -I */
	const char *file_comment = NULL;	/* -t -T */
	list<mystring> set_flags, unset_flags;	/* -f, -d */
	list<mystring> add_users, erase_users;	/* -a, -e */
	mystring mrs, comments;			/* -m -M, -y -Y */
	const int check = 0;			/* -h */
	int reset_checksum = 0;			/* -z */
	
	if (argc > 0) {
		set_prg_name(argv[0]);
	} else {
		set_prg_name("admin");
	}

	class getopt opts(argc, argv, "ni:Irt:Tf:d:a:e:m:y:hzYMV");
	for(c = opts.next(); c != getopt::END_OF_ARGUMENTS; c = opts.next()) {
		switch (c) {
		default:
			quit(-2, "Unsupported option: '%c'", c);

		case 'n':
			new_sccs_file = 1;
			break;

		case 'i':
			iname = opts.getarg();
			new_sccs_file = 1;
			break;

		case 'I':
			iname = "-";
			new_sccs_file = 1;
			break;

		case 'r':
			first_release = release(opts.getarg());
			if (!first_release.valid()) {
				quit(-2, "Invaild release: '%s'",
				     opts.getarg());
			}
			break;

		case 't':
			file_comment = opts.getarg();
			break;
		       
		case 'T':
			file_comment = "";
			break;

		case 'f':
			set_flags.add(opts.getarg());
			break;

		case 'd':
			unset_flags.add(opts.getarg());
			break;

		case 'a':
			add_users.add(opts.getarg());
			break;

		case 'e':
			erase_users.add(opts.getarg());
			break;

		case 'm':
			mrs = opts.getarg();
			break;

		case 'M':
			mrs = "";
			break;

		case 'y':
			comments = opts.getarg();
			break;

		case 'Y':
			comments = "";
			break;

#if 0
		case 'h':
			check = 1;
			break;
#endif

		case 'z':
			reset_checksum = 1;
			break;

		case 'V':
			version();
			break;
		}
	}

	list<mystring> mr_list, comment_list;
	int first = 1;
	sccs_file_iterator iter(argc, argv, opts.get_index());

	while(iter.next()) {
		sccs_name &name = iter.get_name();

		if (reset_checksum && !check) {
			if (!name.valid()) {
				quit(-1, "%s: Not a SCCS file.",
				     (const char *) name);
			}
			sccs_file::update_checksum(name);
			continue;
		}
			
		/* enum */ sccs_file::_mode mode = sccs_file::UPDATE;
#if 0
		if (check) {
			mode = sccs_file::READ;
		} else
#endif
		if (new_sccs_file) {
			mode = sccs_file::CREATE;
		}

		sccs_file file(name, mode);

#if 0
		if (check) {
			file.check();
			continue;
		}
#endif

		file.admin(file_comment, set_flags, unset_flags,
			   add_users, erase_users);

		if (new_sccs_file) {
			if (iname != NULL && !first) {
				quit(-1, "The 'i' keyletter can't be used with"
					 " multiple files.");
			}

			if (first) {
				if (stdin_is_a_tty()) {
					if (mrs == NULL
					    && file.mr_required()) {
						mrs = prompt_user("MRs? ");
					}
				}
				mr_list = split_mrs(mrs);
				comment_list = split_comments(comments);
				first = 0;
			}

			if (file.mr_required()) {
				if (mr_list.length() == 0) {
					quit(-1, "%s: MR number(s) must be "
						 " supplied.",
					     (const char *) name);
				}
				if (file.check_mrs(mr_list)) {
					quit(-1, "%s: Invalid MR number(s).",
					     (const char *) name);
				}
			}

			file.create(first_release, iname,
				    mr_list, comment_list);
		} else {
			file.update();
		}		
		first = 0;
	}

	return 0;
}

/* Local variables: */
/* mode: c++ */
/* End: */
