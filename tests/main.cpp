#include <gtest/gtest.h>

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    const auto status =
        ::testing::UnitTest::GetInstance()->Run();

    return status;
}
