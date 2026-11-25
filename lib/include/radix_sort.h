#if !defined(RADIX_SORT_HEADER)
#define RADIX_SORT_HEADER

/* Radix sort for general structs by 16-bit unsigned int sort indecies.
This is made specifically for sorting triangles in screen space for our scanline rendering, so we can have some assumptions to improve our performance:
 - We have at most ~4K rows for modern display, so a 16-bit (=65536) range is far more than enough covering a modern display.
 - Our pixel coordinates are non-negative, so we don't need to have a separate case for two's compliment for the sort to be correct.

Radix sort is a good choice as our sort algorithm as we know the range of the sort keys is bounded, so we get O(n) complexity (in this implementation, we have exactly four passes over the array, and two passes over the buckets). This is much better than any other general sort algorithm.
Note that we're passing offsets into the structs, libc has a quicksort implementation that accepts a function pointer instead - this is very slow and prevents a lot of compiler optimizations, but it enables more general sort patterns that we don't need.

Usage example for sorting Vec2i via y coordinate:
Vec2i points[4] = {(Vec2i){0, 2}, (Vec2i){3, 8}, (Vec2i){4, 3}, (Vec2i){5, 1}, };
RadixSort(points, 4, offsetof(Vec2i, y), RADIX_SORT_FORWARD);

Note that offsetof supports internal structs as well, i.e. offsetof(Mat2, r1.y) is the same as offsetof(Mat2, r1) + offsetof(Vec2, y)
*/

#include <stddef.h> // for offsetof

typedef enum RadixSortDirection {
    RADIX_SORT_FORWARD  = 1,
    RADIX_SORT_BACKWARD = 2,
} RadixSortDirection;

#define RadixSort(array, count, keyOffset, direction) RadixSort_Internal((void *)array, sizeof(*array), count, keyOffset, direction);
void RadixSort_Internal(void *base, size_t elementSize, size_t count, size_t sortKeyOffset, RadixSortDirection direction);

#endif // RADIX_SORT_HEADER

#if defined(RADIX_SORT_IMPLEMENTATION)
#if !defined(RADIX_SORT_IMPLEMENTED)
#define RADIX_SORT_IMPLEMENTED

void RadixSort_Internal(void *base, size_t elementSize, size_t count, size_t sortKeyOffset, RadixSortDirection direction) {
    // Using persistant memory in order to prevent heavy memory operations (alloc/free)
    // every time we sort. This way the memory footprint grows to the maximum array size we sort,
    // and stays that way. Recycling memory is key for performance.
    // Note that there's no need to free, leaking here is fine - the OS will clean it up anyway
    // after the program is terminated.
    
    static size_t capacity = 0;
    static void *temp = NULL;
    if (capacity < count * elementSize) {
        temp = realloc(temp, count * elementSize);
        capacity = count * elementSize;
    }
    
    void *source = base;
    void *dest   = temp;
    
    if (count > 256*256/2) {
        // Optimize for a single pass using more buckets for a large array
        static const size_t bucketCount = 256*256;
        size_t sortOffsets[bucketCount] = {}; // 2^8-bit buckets
        
#define RADIX_INDEX(element) (*(unsigned short *)(element + sortKeyOffset) & 0xFFFF)
        
        // First pass - counting edges for each bucket
        for (int i = 0; i < count; ++i) {
            void *element = source + i * elementSize;
            sortOffsets[RADIX_INDEX(element)] += 1;
        }
        
        // Convert the counts into indices
        int currentOffset = 0;
        switch (direction) {
            case RADIX_SORT_FORWARD: {
                for (int i = 0; i < bucketCount; ++i) {
                    int bucketSize = sortOffsets[i];
                    sortOffsets[i] = currentOffset;
                    currentOffset += bucketSize;
                }
            } break;
            case RADIX_SORT_BACKWARD: {
                for (int i = bucketCount - 1; i >= 0; --i) {
                    int bucketSize = sortOffsets[i];
                    sortOffsets[i] = currentOffset;
                    currentOffset += bucketSize;
                }
            }
        }
        
        // Second pass - sort the elements using the current byte part
        for (int i = 0; i < count; ++i) {
            void *element = source + i * elementSize;
            size_t elementIndex = sortOffsets[RADIX_INDEX(element)];
            memcpy(dest + elementIndex * elementSize, element, elementSize);
            sortOffsets[RADIX_INDEX(element)] += 1;
        }
#undef RADIX_INDEX
        
        // copy dest back to source
        memcpy(source, dest, count * elementSize);
    } else {
        // Optimize for less buckets, as going through the buckets becomes more expensive relative to the size of the array
        
        // Sorting using 2 8-bit radix passes
        for (int byteIndex = 0; byteIndex < 2; ++byteIndex) {
            static const size_t bucketCount = 256;
            size_t sortOffsets[bucketCount] = {}; // 2^8-bit buckets
            
#define RADIX_INDEX(element) ((*(unsigned short *)(element + sortKeyOffset) >> (byteIndex*8)) & 0xFF)
            
            // First pass - counting edges for each bucket
            for (int i = 0; i < count; ++i) {
                void *element = source + i * elementSize;
                sortOffsets[RADIX_INDEX(element)] += 1;
            }
            
            // Convert the counts into indices
            int currentOffset = 0;
            switch (direction) {
                case RADIX_SORT_FORWARD: {
                    for (int i = 0; i < bucketCount; ++i) {
                        int bucketSize = sortOffsets[i];
                        sortOffsets[i] = currentOffset;
                        currentOffset += bucketSize;
                    }
                } break;
                case RADIX_SORT_BACKWARD: {
                    for (int i = bucketCount - 1; i >= 0; --i) {
                        int bucketSize = sortOffsets[i];
                        sortOffsets[i] = currentOffset;
                        currentOffset += bucketSize;
                    }
                }
            }
            
            // Second pass - sort the elements using the current byte part
            for (int i = 0; i < count; ++i) {
                void *element = source + i * elementSize;
                size_t elementIndex = sortOffsets[RADIX_INDEX(element)];
                memcpy(dest + elementIndex * elementSize, element, elementSize);
                sortOffsets[RADIX_INDEX(element)] += 1;
            }
#undef RADIX_INDEX
            
            // Swap source and dest for the next iteration
            void *swap = dest;
            dest = source;
            source = swap;
        }
        
        // No need to copy back as we finish sorting into the original array
    }
}

#endif // RADIX_SORT_IMPLEMENTED
#endif // RADIX_SORT_IMPLEMENTATION