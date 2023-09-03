#include <QtTest>
#include "ImageFlipTest.h"
#include "ImageRotationTest.h"

template <typename TestClass>
void runTests(int argc, char* argv[], int* status)
{
    ::QTest::Internal::callInitMain<TestClass>();
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);
    QTEST_DISABLE_KEYPAD_NAVIGATION TestClass tc;
    QTEST_SET_MAIN_SOURCE_PATH
    *status |= QTest::qExec(&tc, argc, argv);
}


int main(int argc, char *argv[])
{
    int status = 0;

    runTests<ImageRotationTest>(argc, argv, &status);
    runTests<ImageFlipTest>(argc, argv, &status);

    return status;
}
