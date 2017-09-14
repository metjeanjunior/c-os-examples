### This holds a very simple terminal written in C

* It uses exec and fork to run the commands.

* cd does work properly. This can easily be tested using the [environ](environ) binary.

* showpid is a built-in command that shows the pid of the last 5 forked proccesses.
 * This is implemented using a simple [queue](https://www.freebsd.org/cgi/man.cgi?query=queue#SINGLY-LINKED_TAIL_QUEUES).
