#include "test_string.hpp"
#include "test_vector.hpp"
#include "test_list.hpp"
#include "test_deque.hpp"
#include "test_stack.hpp"
#include "test_queue.hpp"
#include "test_priority_queue.hpp"
#include "test_set.hpp"
#include "test_map.hpp"
#include "test_multiset.hpp"
#include "test_multimap.hpp"
#include "test_unordered_set.hpp"
#include "test_unordered_map.hpp"
#include "test_unordered_multiset.hpp"
#include "test_unordered_multimap.hpp"
#include "test_forward_list.hpp"
#include "test_array.hpp"

#include "test_stream_iterator.hpp"
#include "test_iterator_traits.hpp"
#include "test_reverse_iterator.hpp"

// 主函数：运行所有测试
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
