#include "main.h"

/**
 * bfree - frees a pointer as well as NULLs the address
 * @ptr: address of the pointer to be freed
 *
 * Return: 1(success), 0(failed)
 */
int bfree(void **ptr)
{
	if (ptr && *ptr)
	{
		free(*ptr);
		*ptr = NULL;
		return (1);
	}
	return (0);
}
