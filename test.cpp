#include <cppunit/ui/text/TestRunner.h>

extern CppUnit::TestSuite* handTestSuite();

int main() {
    CppUnit::TextUi::TestRunner runner;
    runner.addTest(handTestSuite());
    bool retcode = runner.run();
    return !retcode;
}
