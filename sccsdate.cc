/*
 * sccsdate.c
 *
 * By Ross Ridge
 * Public Domain
 *
 * Members of the class sccs_date.
 *
 */

#ifdef __GNUC__
#pragma implementation "sccsdate.h"
#endif

#include "mysc.h"
#include "sccsdate.h"

#include <ctype.h>

#ifdef CONFIG_SCCS_IDS
static const char sccs_id[] = "@(#) MySC sccsdate.c 1.1 93/11/09 17:17:58";
#endif

#ifdef CONFIG_NO_MKTIME

#define mktime LIDENT(mktime)

#ifdef CONFIG_DECLARE_TIMEZONE
extern long timezone;
#endif
#ifdef CONFIG_DECLARE_TZSET
extern "C" void CDECL tzset();
#endif

/* This is a "good enough for MySC" implementation of mktime. */

static time_t
mktime(struct tm const *tm) {
	const long day_secs = 24L * 60L * 60L;
	int year = tm->tm_year - 70;
	time_t t = (long) year * 365 * day_secs
		   + (long) ((year + 1) / 4) * day_secs;
	static long month_offs[12] = {
		0L,
		31L * day_secs,
		59L * day_secs,
		90L * day_secs,
		120L * day_secs,
		151L * day_secs,
		181L * day_secs,
		212L * day_secs,
		243L * day_secs,
		273L * day_secs,
		304L * day_secs,
		334L * day_secs
	};

	t += month_offs[tm->tm_mon] + (long) (tm->tm_mday - 1) * day_secs;
	if (tm->tm_mon > 2 && tm->tm_year % 4 == 0) {
		t += day_secs;
        }

	t += tm->tm_sec + tm->tm_min * 60 + (long) tm->tm_hour * 60 * 60;

#ifndef CONFIG_NO_TIMEZONE_VAR
	tzset();
	t += timezone;
#endif

	if (localtime(&t)->tm_isdst > 0) {
		t -= 60 * 60;
	}

	return t;
}	       
	
#endif /* CONFIG_NO_MKTIME */   


static int
days_in_month(int mon, int year) {
	switch(mon) {
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		return 31;

	case 4:
	case 6:
	case 9:
	case 11:
		return 30;

	case 2:
		if (year % 4 == 0 && year != 0) {
			return 29;
		}
		return 28;
	}
	return -1;
}

static int
get_part(const char *&s, int def) {
	char c = *s;
	while(c != '\0') {
		if (isdigit(c)) {
			s++;
			if (isdigit(*s)) {
				return (c - '0') * 10 + (*s++ - '0');
			}
			return c - '0';
		}
		c = *++s;
	}
	return def;
}

static int
check_tm(struct tm &tm) {
	if (tm.tm_year < 69) {
		tm.tm_year += 100;
	}

	return tm.tm_mon == -1 || tm.tm_mon > 11
	       || tm.tm_mday > days_in_month(tm.tm_mon + 1, tm.tm_year)
	       || tm.tm_hour > 23 || tm.tm_min > 59 || tm.tm_sec > 59;
}

sccs_date::sccs_date(const char *s) {
	t = (time_t) -1;

	if (s == NULL) {
		return;
	}

	struct tm tm;

	tm.tm_year = get_part(s, -1);
	if (tm.tm_year == -1) {
		return;
	}

#if 1
	if ((tm.tm_year == 19 || tm.tm_year == 20)
            && isdigit(s[0]) && isdigit(s[1])) {
		tm.tm_year = (tm.tm_year - 19) * 100
			     + (s[0] - '0') * 10 + (s[1] - '0');
		s += 2;
	}
#endif
			
	tm.tm_mon = get_part(s, 12) - 1;
	tm.tm_mday = get_part(s, days_in_month(tm.tm_mon + 1, tm.tm_year));
	tm.tm_hour = get_part(s, 23);
	tm.tm_min = get_part(s, 59);
	tm.tm_sec = get_part(s, 59);

	tm.tm_isdst = -1;

	if (check_tm(tm)) {
		return;
	}

	t = mktime(&tm);
}

sccs_date::sccs_date(const char *date, const char *time) {
	struct tm tm;

	t = (time_t) -1;

	if (!isdigit(date[0]) || !isdigit(date[1]) || date[2] != '/') {
		return;
	}
	tm.tm_year = (date[0] - '0') * 10 + (date[1] - '0');

	if (!isdigit(date[3]) || !isdigit(date[4]) || date[5] != '/') {
		return;
	}
	tm.tm_mon = (date[3] - '0') * 10 + (date[4] - '0') - 1;

	if (!isdigit(date[6]) || !isdigit(date[7]) || date[8] != '\0') {
		return;
	}
	tm.tm_mday = (date[6] - '0') * 10 + (date[7] - '0');

	if (!isdigit(time[0]) || !isdigit(time[1]) || time[2] != ':') {
		return;
	}
	tm.tm_hour = (time[0] - '0') * 10 + (time[1] - '0');

	if (!isdigit(time[3]) || !isdigit(time[4]) || time[5] != ':') {
		return;
	}
	tm.tm_min = (time[3] - '0') * 10 + (time[4] - '0');

	if (!isdigit(time[6]) || !isdigit(time[7]) || time[8] != '\0') {
		return;
	}
	tm.tm_sec = (time[6] - '0') * 10 + (time[7] - '0');

	tm.tm_isdst = -1;

	if (check_tm(tm)) {
		return;
	}

	t = mktime(&tm);
}

int
sccs_date::printf(FILE *f, char fmt) const {
	struct tm const *tm = localtime(&t); 

	if (tm == NULL) {
		quit(-1, "localtime() failed.");
	}

	switch(fmt) {
	case 'D':
		return fprintf(f, "%02d/%02d/%02d", tm->tm_year % 100,
			       tm->tm_mon + 1, tm->tm_mday) == EOF;

	case 'H':
		return fprintf(f, "%02d/%02d/%02d", tm->tm_mon + 1,
			       tm->tm_mday, tm->tm_year % 100) == EOF;

	case 'T':
		return fprintf(f, "%02d:%02d:%02d", tm->tm_hour,
			       tm->tm_min, tm->tm_sec) == EOF;

	case 'y':
		return fprintf(f, "%02d", tm->tm_year % 100) == EOF;
	
	case 'o':
		return fprintf(f, "%02d", tm->tm_mon + 1) == EOF;
		
	case 'd':
		return fprintf(f, "%02d", tm->tm_mday) == EOF;

	case 'h':
		return fprintf(f, "%02d", tm->tm_hour) == EOF;

	case 'm':
		return fprintf(f, "%02d", tm->tm_min) == EOF;

	case 's':
		return fprintf(f, "%02d", tm->tm_sec) == EOF;

	default:
		assert(!"sccs_date::printf: Invalid format");
	}

	return 0;
}

int
sccs_date::print(FILE *f) const {
	return printf(f, 'D') || putc(' ', f) == EOF || printf(f, 'T');
}


const char *
sccs_date::as_string() const {
	struct tm const *tm = localtime(&t); 

	if (tm == NULL) {
		quit(-1, "localtime() failed.");
	}

	static char buf[18];

	sprintf(buf, "%02d/%02d/%02d %02d:%02d:%02d",
		tm->tm_year % 100, tm->tm_mon + 1, tm->tm_mday,
		tm->tm_hour, tm->tm_min, tm->tm_sec);

	return buf;
}

/* Local variables: */
/* mode: c++ */
/* End: */
