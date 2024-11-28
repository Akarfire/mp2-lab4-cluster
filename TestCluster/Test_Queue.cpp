#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "Queue.h"
#include <gtest.h>

TEST(TQueue, can_create_queue)
{
	ASSERT_NO_THROW(TQueue<int> Q);
}

TEST(TQueue, empty_by_default)
{
	TQueue<int> Queue;
	EXPECT_EQ(true, Queue.empty());
}

TEST(TQueue, can_put_items)
{
	TQueue<int> Queue;

	ASSERT_NO_THROW(Queue.Put(5));
}

TEST(TQueue, can_check_first_item)
{
	TQueue<int> Queue;

	ASSERT_NO_THROW(Queue.Put(5));

	int OutValue = 0;
	ASSERT_NO_THROW(OutValue = Queue.Check());
	EXPECT_EQ(5, OutValue);
}

TEST(TQueue, can_pop_items)
{
	TQueue<int> Queue;

	ASSERT_NO_THROW(Queue.Put(5));
	ASSERT_NO_THROW(Queue.Pop());
}

TEST(TQueue, first_in_first_out)
{
	TQueue<int> Queue;

	ASSERT_NO_THROW(Queue.Put(5));
	ASSERT_NO_THROW(Queue.Put(4));

	int OutValue = 0;

	ASSERT_NO_THROW(OutValue = Queue.Check());
	ASSERT_NO_THROW(Queue.Pop());
	EXPECT_EQ(5, OutValue);

	ASSERT_NO_THROW(OutValue = Queue.Check());
	ASSERT_NO_THROW(Queue.Pop());
	EXPECT_EQ(4, OutValue);
}


TEST(TQueue, can_check_further_items)
{
	TQueue<int> Queue;

	ASSERT_NO_THROW(Queue.Put(5));
	ASSERT_NO_THROW(Queue.Put(4));
	ASSERT_NO_THROW(Queue.Put(3));
	ASSERT_NO_THROW(Queue.Put(2));
	ASSERT_NO_THROW(Queue.Put(1));

	int OutValue = 0;

	ASSERT_NO_THROW(OutValue = Queue.Check(1));
	EXPECT_EQ(4, OutValue);

	ASSERT_NO_THROW(OutValue = Queue.Check(2));
	EXPECT_EQ(3, OutValue);

	ASSERT_NO_THROW(OutValue = Queue.Check(3));
	EXPECT_EQ(2, OutValue);

	ASSERT_NO_THROW(OutValue = Queue.Check(4));
	EXPECT_EQ(1, OutValue);
}


TEST(TQueue, empty_value_is_correct)
{
	TQueue<int> Queue;

	EXPECT_EQ(true, Queue.empty());

	ASSERT_NO_THROW(Queue.Put(5));

	EXPECT_EQ(false, Queue.empty());
}

TEST(TQueue, size_value_is_correct)
{
	TQueue<int> Queue;

	EXPECT_EQ(0, Queue.size());

	ASSERT_NO_THROW(Queue.Put(5));

	EXPECT_EQ(1, Queue.size());

	ASSERT_NO_THROW(Queue.Put(4));
	ASSERT_NO_THROW(Queue.Put(3));
	ASSERT_NO_THROW(Queue.Put(2));
	ASSERT_NO_THROW(Queue.Put(1));

	EXPECT_EQ(5, Queue.size());

	ASSERT_NO_THROW(Queue.Pop());
	ASSERT_NO_THROW(Queue.Pop());

	EXPECT_EQ(3, Queue.size());
}

TEST(TQueue, throws_when_popping_empty)
{
	TQueue<int> Queue;
	ASSERT_ANY_THROW(Queue.Pop());
}

TEST(TQueue, throws_when_popping_invalid_index)
{
	TQueue<int> Queue;
	ASSERT_NO_THROW(Queue.Put(4));
	ASSERT_ANY_THROW(Queue.Pop(1));
	ASSERT_ANY_THROW(Queue.Pop(-1));
}

TEST(TQueue, throws_when_checking_empty)
{
	TQueue<int> Queue;
	ASSERT_ANY_THROW(Queue.Check());
}

TEST(TQueue, throws_when_checking_invalid_index)
{
	TQueue<int> Queue;
	ASSERT_NO_THROW(Queue.Put(4));
	ASSERT_ANY_THROW(Queue.Check(1));
	ASSERT_ANY_THROW(Queue.Check(-1));
}
