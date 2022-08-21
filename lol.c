void *realloc(void *pointer, size_t size)
{
    if (!pointer)
    {
	return malloc(size);
    }

    if (size == 0)
    {
	free(pointer);

	return NULL;
    }

    size_t old_size = 0;

    if (((uint64_t)pointer & 0xFFF) != 0)
    {
        malloc_metadata_t *metadata = pointer;
        size_t index = metadata->size;

	old_size = slab_cache_index_to_size(index);
    }
    else
    {
        malloc_metadata_t *metadata = pointer;
        size_t page_count = metadata->size;

	old_size = page_count / PAGE_SIZE;
    }

    // it's not aligned (i.e. too precise, thus not the same)
    // if (old_size == size)
    // {
    //     return pointer;
    // }

    void *new_pointer = malloc(size);

    if (!new_pointer)
    {
	// `pointer` is not freed, as this is the responsibility of caller
	return NULL;
    }

    if (old_size > size)
    {
	memcpy(new_pointer, pointer, size);
    }
    else
    {
	memcpy(new_pointer, pointer, old_size);
    }

    free(pointer);

    return new_pointer;
}
