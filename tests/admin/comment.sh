#! /bin/sh

# comment.sh:  Testing for comments at initialisation time.

# Import common functions & definitions.
. test-common

s=s.new.txt
remove foo $s new.txt s.1 s.2 command.log


remove foo
echo '%M%' > foo
test `cat foo` = '%M%' || miscarry cannot create file foo.

# Create an empty SCCS file to work on.
docommand C1 "${admin} -ifoo $s" 0 "" ""

# Check the format of the default comment.
echo -n C2...
remove prs.$s
${prs} $s | sed -ne '/^COMMENTS:$/,/$/ p' > prs.$s || fail prs failed.
test `wc -l < prs.$s` -eq 2 || fail wrong comment format.
test `head -1 prs.$s` = "COMMENTS:" || fail Comment doesn\'t start COMMENTS:
tail -1 prs.$s | grep \
 '^date and time created [0-9][0-9]/[0-1][0-9]/[0-3][0-9] [0-2][0-9]:[0-5][0-9]:[0-5][0-9] by ' >/dev/null\
    || fail "default message format error."
echo passed
remove $s prs.$s 

# Force a blank comment and check it was blank.
docommand C3 "${admin} -ifoo -y $s" 0 "" ""
docommand C4 "${prs} $s | \
	    sed -ne '/^COMMENTS:$/,/$/ p'"   0  \
	    "COMMENTS:\n\n" ""
remove $s


# Specify some comment and check it works.
docommand C5 "${admin} -ifoo -yMyComment $s" 0 "" ""
docommand C6 "${prs} $s | \
	    sed -ne '/^COMMENTS:$/,/$/ p'"   0  \
	    "COMMENTS:\nMyComment\n" ""

# Detach the comment arg and check it no longer works.
remove MyComment $s
docommand C7 "${admin} -y MyComment -n $s" 1 "" IGNORE

# Ensure the same form does work normally.
remove MyComment $s
docommand C8 "${admin} -n -yMyComment $s" 0 "" IGNORE


# Can we create multiple files if we don't use -i ?
docommand C9 "${admin} -n s.1 s.2" 0 "" ""

# Check both generated files.
for n in 1 2
do
    stage=C`expr 9 + $n`
    docommand $stage "${prs} \
  -d':B:\n:BF:\n:DI:\n:DL:\n:DT:\n:I:\n:J:\n:LK:\n:MF:\n:MP:\n:MR:\n:Z:' s.1" \
  0                                                                           \
  "\nno\n\n00000/00000/00000\nD\n1.1\nno\nnone\nno\nnone\n\n@(#)\n"       \
  ""
done

docommand C12 "${prs} -d':M:\n' s.1 s.2" 0 "1\n\n2\n\n" ""

# We should only be able to create one file if we use -i.
docommand C13 "${admin} -n -ifoo s.1 s.2" 1 "" IGNORE

remove s.1 s.2 foo command.log $s
success