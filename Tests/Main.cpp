#include "gtest/gtest.h"

#include "DataStorageTests/LoadDataTests/LoadDataTests.h"
#include "DataStorageTests/GetEntityByIdTests/GetEntityByIdTests.h"
#include "DataStorageTests/GetVisitsByUserIdTests/GetVisitsByUserIdTests.h"
#include "DataStorageTests/UpdateEntityTests/UpdateEntityTests.h"
#include "DataStorageTests/GetAverageLocationMarkTests/GetAverageLocationMarkTests.h"
#include "DataStorageTests/AddEntityTests/AddEntityTests.h"
#include "DataStorageTests/ComplexTests/ComplexTests.h"

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	// ::testing::GTEST_FLAG(filter) = "DataStorageComplexTest.CheckVisitsByUserIdAfterVisitUpdateTest";
	// ::testing::GTEST_FLAGetVisitsAmountG(filter) = "DataStorageComplexTest.CheckAverageLocationAfterVisitUpdateTest";
	return RUN_ALL_TESTS();
}