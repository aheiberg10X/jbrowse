Profiling using valgrind

execute the program using:
	valgrind --tool=callgrind <program+arguments>

view the results:
	kcachegrind

Remember to remove callgrind output between runs. (look for files that look like
callgrind.out.pid)

