/*
 * canonify.cc: Part of GNU CSSC.
 * 
 * 
 *    Copyright (C) 1997, Free Software Foundation, Inc. 
 * 
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 * 
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * 
 * Functions for canonifying filenames.
 */

#include "config.h"

#ifdef STDC_HEADERS
#include <stddef.h>
#include <assert.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif


static const char *
get_current_directory()
{
  size_t len = 1;
  char *p;

  for (;;)
    {
      if (NULL != (p = new char[len]))
	{
	  if ( NULL != getcwd(p, len) )	// success!
	    {
	      return p;
	    }
	  else
	    {
	      len *= 2;		// try a larger buffer.
	    }
	  delete[] p;
	}
      else			// allocation failed.
	{
	  return ".";	// this is a cop-out really.
	}      
    }
}



int main(int argc, char *argv[])
{
  const char newline[1] = { '\n' };
  const char *dir = get_current_directory();
  write(1, dir, strlen(dir));
  write(1, newline, 1);
  return 0;
}
