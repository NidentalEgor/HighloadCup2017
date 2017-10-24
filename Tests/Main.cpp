#include "DataStorageTests/LoadDataTests/LoadDataTests.h"
#include "DataStorageTests/GetEntityByIdTests/GetEntityByIdTests.h"
#include "DataStorageTests/GetVisitsByUserIdTests/GetVisitsByUserIdTests.h"
#include "DataStorageTests/UpdateEntityTests/UpdateEntityTests.h"
#include "DataStorageTests/GetAverageLocationMarkTests/GetAverageLocationMarkTests.h"
#include "DataStorageTests/AddEntityTests/AddEntityTests.h"

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}