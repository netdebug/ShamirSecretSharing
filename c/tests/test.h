#pragma once

#define massert(x) do { \
	if (!(x)) { \
		fprintf(stderr, "Assertion: " #x "\n"); \
	} } while(0);

