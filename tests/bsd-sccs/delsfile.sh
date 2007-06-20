#! /bin/sh
#
# This is a test for a bug in sccs.c where the command 
#   sccs unedit /tmp/SCCS/s.foo
# causes the deletion of s.foo (instead, the file ./foo should be deleted).

. ../common/test-common
. ../common/not-root


# If LANG is defined but the system is misconfigured, we will produce
# the error message "Error setting locale: No such file or directory".
# If that happens, the test suite will fail.  For this reason, we
# unset the LANG environment variable.  Of course, things being
# printed out in the wrong language would also mess up the results of
# the test suite.
# We want to prevent setlocale(LC_ALL, "") failing:
unset LANG

# We assume that all the files we want to work on are in the current
# directory (unless we specify a full path, which in fact we do).

unset PROJECTDIR

echo "Using the driver program ${sccs}"

files="foo"
sfiles="s.foo"


cleanup () {
    if [ -d /tmp/SCCS ] 
    then
	for i in $files; do /bin/rm -f /tmp/SCCS/[spzd].$i $i; done
	rmdir /tmp/SCCS
    fi
    rm -f $files
}

cleanup
remove command.log log log.stdout log.stderr 
mkdir /tmp/SCCS

echo "Creating the input files..."
${admin} -n /tmp/SCCS/s.foo
${admin} -n s.foo

docommand d1 "test -f s.foo" 0 "" IGNORE
docommand d2 "${sccs} edit /tmp/SCCS/s.foo" 0 IGNORE IGNORE
docommand d3 "test -f foo" 0 "" IGNORE

# When we have the bug, this step will probably fail, because the delete
# removes the wrong file, so the subsequent get finds that ./foo exists and 
# is writable, so it fails.
docommand d4 "${sccs} unedit /tmp/SCCS/s.foo" 0 IGNORE IGNORE

# This is the heart of the test; make sure sccs.c deleted the right file.
# (the file should have been recreated as read-only).
docommand d5 "test -r foo"   0 "" IGNORE
docommand d5 "test -w foo"   1 "" IGNORE

# make sure we didn't delete the innocent bystander file s.foo.
docommand d6 "test -f s.foo" 0 "" IGNORE

cleanup
success


    
