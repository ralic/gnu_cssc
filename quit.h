/*
 * quit.h
 *
 * By Ross Ridge
 * Public Domain
 *
 * @(#) MySC quit.h 1.1 93/11/09 17:17:49
 *
 */

#ifndef __QUIT_H__
#define __QUIT_H__

#ifdef __GNUC__
#pragma interface
#endif

class cleanup {
	static class cleanup *head;
	static int running;
	static int all_disabled;
#ifndef CONFIG_NO_FORK
	static int in_child_flag;
#endif

	class cleanup *next;
	
protected:
	cleanup();
	virtual void do_cleanup() = 0;

#ifdef __GNUC__
	virtual /* not needed but it gets rid of an anoying warning */
#endif
	~cleanup();

public:

	static void run_cleanups();
	static int active() { return running; }
	static void disable_all() { all_disabled++; }
#ifndef CONFIG_NO_FORK
	static void set_in_child() { in_child_flag = 1; disable_all(); }
	static int in_child() { return in_child_flag; }
#endif
};

extern const char *prg_name;

void set_prg_name(const char *name);

#ifdef __GNUC__
NORETURN quit(int err, const char *fmt, ...)
	__attribute__((format(printf, 2, 3))) POSTDECL_NORETURN;
#else
NORETURN quit(int err, const char *fmt, ...)  POSTDECL_NORETURN;
#endif

NORETURN nomem()  POSTDECL_NORETURN;
NORETURN assert_failed(const char *file, int line, const char *test) POSTDECL_NORETURN;

#define assert(test) ((test) ? (void) 0					\
		             : assert_failed(__FILE__, __LINE__, #test))

extern void usage();

#endif /* __QUIT_H__ */

/* Local variables: */
/* mode: c++ */
/* End: */
