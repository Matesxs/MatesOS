#include "strcmp.hpp"

int strcmp(const char *l, const char *r)
{
	for (; *l==*r && *l; l++, r++);
	return *(unsigned char *)l - *(unsigned char *)r;
}

int strcmp(const char *l, const char *r, size_t len)
{
	for (size_t i = 0; i < len && *l && *r; i++)
	{
		if (*l != *r) return *l - *r;
		l++; r++;
	}
	return 0;
}