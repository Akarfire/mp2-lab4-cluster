#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "Cluster.h"
#include <gtest.h>

void Update(CCluster* InCluster)
{
	TProgramCall Call("Program", 10, 25);

	InCluster->CallProgramExecution(Call);
}


TEST(TCluster, can_call_valid_program)
{
	CCluster Cluster(100, 32);

	TProgramCall Call("Program", 10, 25);

	ASSERT_NO_THROW(Cluster.CallProgramExecution(Call));
}

TEST(TCluster, throws_when_calling_program_with_too_many_processors)
{
	CCluster Cluster(100, 32);
	
	TProgramCall Call("Program", 64, 25);

	ASSERT_ANY_THROW(Cluster.CallProgramExecution(Call));
}

TEST(TCluster, throws_when_calling_program_with_zero_processors)
{
	CCluster Cluster(100, 32);

	TProgramCall Call("Program", 0, 25);
	ASSERT_ANY_THROW(Cluster.CallProgramExecution(Call));
}

TEST(TCluster, throws_when_calling_program_with_zero_or_negative_execution_time)
{
	CCluster Cluster(100, 32);

	TProgramCall Call("Program", 5, 0);
	ASSERT_ANY_THROW(Cluster.CallProgramExecution(Call));
}

TEST(TCluster, throws_when_calling_program_with_empty_name)
{
	CCluster Cluster(100, 32);

	TProgramCall Call("", 25, 25);

	ASSERT_ANY_THROW(Cluster.CallProgramExecution(Call));
}

TEST(TCluster, can_start_and_work)
{
	CCluster Cluster(100, 32);
	ASSERT_NO_THROW(Cluster.Start(Update));
}

