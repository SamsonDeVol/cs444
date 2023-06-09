#include "image.h"
#include "block.h"
#include "free.h"
#include "mkfs.h"
#include "ctest.h"
#include "inode.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>

void test_image_open(void){
    CTEST_ASSERT(image_open("new_file", 1) != -1, "assert image open succeeds");
    CTEST_ASSERT(image_open("/new_file", 1) == -1, "assert improper image open fails");
}

void test_image_close(void){
    CTEST_ASSERT(image_close() == -1, "assert image close succeeds");
    image_open("test_file", 1);
    CTEST_ASSERT(image_close() != -1, "assert image close succeeds");
}

void test_bwrite_and_bread(void){
    unsigned char write_block[BLOCK_SIZE] = "hello";
    unsigned char read_block[BLOCK_SIZE];
    CTEST_ASSERT(memcmp(write_block, read_block, BLOCK_SIZE) != 0, "assert originating buffers don't match");
    bwrite(3, write_block);
    bread(3, read_block);
    CTEST_ASSERT(memcmp(write_block, read_block, BLOCK_SIZE) == 0, "assert write and read buffers match");
}

void test_find_free(void){
    unsigned char test_buf[BLOCK_SIZE] = {1};
    CTEST_ASSERT(find_free(test_buf) == 1, "assert find_free() returns 1 for 0b0000001");
    unsigned char test_buf_2[BLOCK_SIZE] = {191};
    CTEST_ASSERT(find_free(test_buf_2) == 6, "assert find_free() returns 6 for int 0b10111111");
    unsigned char test_buf_3[BLOCK_SIZE] = {255, 1};
    CTEST_ASSERT(find_free(test_buf_3) == 9, "assert find_free() returns 9 for 0b0000010111111111");
}

void test_set_free(void){
    unsigned char test_block_1[BLOCK_SIZE] = {5};
    unsigned char compare_block_1[BLOCK_SIZE] = {5};
    unsigned char compare_block_1a[BLOCK_SIZE] = {4};
    CTEST_ASSERT(memcmp(test_block_1, compare_block_1, BLOCK_SIZE) == 0, "assert test and compare block match");
    
    set_free(test_block_1, 0, 0);
    CTEST_ASSERT(memcmp(test_block_1, compare_block_1a, BLOCK_SIZE) == 0, "assert set_free() clears first bit for 0b00000101");
    
    // set test block to 0b0000010100000101
    unsigned char test_block_2[BLOCK_SIZE] = {5, 5};
    unsigned char compare_block_2[BLOCK_SIZE] = {5, 4};
    set_free(test_block_2, 8, 0);
    CTEST_ASSERT(memcmp(test_block_2, compare_block_2, BLOCK_SIZE) == 0, "assert set_free() clears first bit of second byte for 0b0000010100000101");
  
    // set test block to 0b00000101
    unsigned char test_block_3[BLOCK_SIZE] = {5};
    unsigned char compare_block_3[BLOCK_SIZE] = {7};
    set_free(test_block_3, 1, 1);
    CTEST_ASSERT(memcmp(test_block_3, compare_block_3, BLOCK_SIZE) == 0, "assert set_free() sets second bit to 1 for 0b00000101");
}

void test_alloc(void){

    unsigned char read_block[BLOCK_SIZE];
    unsigned char compare_block[BLOCK_SIZE] = {1};
    int alloc_return;

    image_open("test_file", DO_TRUNCATE);

    alloc_return = alloc();
    CTEST_ASSERT(alloc_return == 0, "assert alloc() returns 0 first");
    
    bread(DATA_MAP_NUM, read_block);
    CTEST_ASSERT(memcmp(read_block, compare_block, BLOCK_SIZE) == 0, "assert written memory matches alloc");

    alloc_return = alloc();
    CTEST_ASSERT(alloc_return == 1, "assert alloc() returns 1 second");
    
    alloc_return = alloc();
    CTEST_ASSERT(alloc_return == 2, "assert alloc() returns 2 third");
}

void test_ialloc(void){

    unsigned char read_block[BLOCK_SIZE];
    unsigned char compare_block[BLOCK_SIZE] = {1};
    int ialloc_return;

    image_open("test_file", DO_TRUNCATE);

    ialloc_return = ialloc();
    CTEST_ASSERT(ialloc_return == 0, "assert ialloc() returns 0 first");
    
    bread(DATA_MAP_NUM, read_block);
    CTEST_ASSERT(memcmp(read_block, compare_block, BLOCK_SIZE) == 0, "assert written memory matches ialloc");

    ialloc_return = ialloc();
    CTEST_ASSERT(ialloc_return == 1, "assert ialloc() returns 1 second");
    
    ialloc_return = ialloc();
    CTEST_ASSERT(ialloc_return == 2, "assert ialloc() returns 2 third");
}

void test_mkfs(void){

    image_open("test_file", DO_TRUNCATE);
    unsigned char read_block[BLOCK_SIZE];

    mkfs();
   
    int end_of_file = lseek(image_fd, 0, SEEK_END);
    CTEST_ASSERT(end_of_file == 4194304, "assert file size is 4096*1024 = 4194304");
   
    bread(DATA_MAP_NUM, read_block);
    int free_block_num = find_free(read_block);
    CTEST_ASSERT(free_block_num == 7, "assert 6 blocks reserved on making file system");
}

int main(){

    CTEST_VERBOSE(1);
    test_image_open();
    test_image_close();
    test_mkfs();
    test_alloc();
    test_ialloc();
    test_bwrite_and_bread();
    test_find_free();
    test_set_free();
    CTEST_RESULTS();
    CTEST_EXIT();

    return 0;
}
