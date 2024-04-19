#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include <string>

void sleepForMs(long long ms);
long long getTimeInMs(void);
void runCommand(std::string command, bool show_output);

#endif
