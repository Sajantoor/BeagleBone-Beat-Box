/*
Contains utility functions for common tasks
*/
#include "common/utils.hpp"
#include <stdbool.h>
#include <sys/wait.h>
#include <time.h>
#include <iostream>

void sleepForMs(long long ms) {
	struct timespec sleep_time;
	sleep_time.tv_sec = ms / 1000;
	sleep_time.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&sleep_time, NULL);
}

long long getTimeInMs(void) {
	struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);
	long long seconds = spec.tv_sec;
	long long nano_seconds = spec.tv_nsec;
	long long milli_seconds = seconds * 1000 + nano_seconds / 1000000;
	return milli_seconds;
}

void runCommand(std::string command, bool show_output) {
	FILE* pipe = popen(command.c_str(), "r");
	if (pipe == NULL) {
		std::cerr << "Error running command: " << command << std::endl;
		return;
	}

	char buffer[1024];
	while (!feof(pipe) && !ferror(pipe)) {
		if (fgets(buffer, sizeof(buffer), pipe) != NULL) {
			if (show_output) std::cout << buffer;
		}
	}

	int exit_code = WEXITSTATUS(pclose(pipe));
	if (exit_code != 0) std::cerr << "Error running command: " << command << std::endl;
}
