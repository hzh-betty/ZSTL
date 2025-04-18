#include "test_string.hpp"
#include "test_vector.hpp"
#include "test_list.hpp"
#include "test_deque.hpp"
#include "test_stack.hpp"
#include "test_queue.hpp"
#include "test_priority_queue.hpp"

// 主函数：运行所有测试
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
