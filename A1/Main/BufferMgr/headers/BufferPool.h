//
// Created by 张滋沛 on 2022/1/29.
//

#ifndef A1_BUFFERPOOL_H
#define A1_BUFFERPOOL_H

class BufferPool{

private:
    size_t totalSlot;
    int[] unusedSlot;
    size_t pageSize;
    size_t totalSize;

public:
    BufferPool(size_t size, size_t num)
        char
        totalSlot = num;
        unusedSlot = myArray[num];
        pageSize = size;
        totalSize = size * num;
    }
};
#endif //A1_BUFFERPOOL_H
