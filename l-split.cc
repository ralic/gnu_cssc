/*
 * l-split.c
 *
 * By Ross Ridge
 * Public Domain
 *
 * Functions for non-destructively spliting a string into a list of
 * strings.
 * 
 */

#include "cssc.h"
#include "list.h"

#ifdef CONFIG_SCCS_IDS
static const char rcs_id[] = "CSSC $Id: l-split.cc,v 1.3 1997/05/10 14:49:50 james Exp $";
#endif

list<mystring>
split_mrs(mystring mrs) {
	list<mystring> mr_list;

	if (mrs != NULL) {
		char *s = mrs.xstrdup();

		s = strtok(s, " \t\n");
		while(s != NULL) {
			mr_list.add(s);
			s = strtok(s, NULL);
		}
		free(s);
	}

	return mr_list;
}

list<mystring>
split_comments(mystring comments) {
	list<mystring> comment_list;

	if (comments != NULL) {
		char *s = comments.xstrdup();
		char *start;
		char *end;

		start = s;
		end = strchr(s, '\n');
		while(end != NULL) {
			*end++ = '\0';
			comment_list.add(start);
			start = end;
			end = strchr(start, '\n');
		}

		if (*start != '\0') {
			comment_list.add(start);
		}

		free(s);
	}

	return comment_list;
}

/* Local variables: */
/* mode: c++ */
/* End: */
