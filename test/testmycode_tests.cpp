#include <QtTest>

class TestMyCodeTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase(); // called before the first test function is executed
    void init(); // called before each test function is executed
    void cleanup(); // called after every test function
    void cleanupTestCase(); // called after the last test function was executed

    // Declare unit test prototype definitions here
    void testExample1();
    void testExample2();
};


void TestMyCodeTest::initTestCase()
{
    // called before the first test function is executed
}

void TestMyCodeTest::init() {
    // called before each test function is executed
}

void TestMyCodeTest::cleanup() {
    // called after every test function
}

void TestMyCodeTest::cleanupTestCase()
{
    // called after the last test function was executed
}

// Implement unit test functions here.

void TestMyCodeTest::testExample1()
{
    QVERIFY2(true, "Failure");
}

void TestMyCodeTest::testExample2()
{
    QVERIFY2(true, "Failure");
}

// To make our test case a stand-alone executable, expand QtTest main macro
QTEST_MAIN(TestMyCodeTest)
// Include meta object compiler output because we have no header file
#include "testmycode_tests.moc"
