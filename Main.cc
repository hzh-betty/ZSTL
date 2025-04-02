#include "string.hpp"
#include <gtest/gtest.h>
TEST(mystring, basic)
{
    zstl::string str1("hello world");
    EXPECT_EQ(strcmp("hello world", str1.c_str()), 0);
    zstl::string str2(str1);
    EXPECT_EQ(strcmp("hello world", str2.c_str()), 0);
    zstl::string str3;
    str3 = str2;
    EXPECT_EQ(strcmp("hello world", str2.c_str()), 0);
}
int main(int argc, char *argv[])
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}