#include <glog/logging.h>
#include <gtest/gtest.h>

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();
    testing::InitGoogleTest(&argc, argv);
    int const return_val = RUN_ALL_TESTS();
    std::cout << "RUN_ALL_TESTS return value: " << return_val << std::endl;
    return return_val;
}
