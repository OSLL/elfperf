#define TESTCLASS1_A 5
#define TESTCLASS1_B -15
#define TESTCLASS1_C 0.5

const int TEST_FUNCTION_2_ARG0 = 115;
const double TEST_FUNCTION_2_ARG1 = 0.125;
const float TEST_FUNCTION_2_RESULT = 0.999;

#define EPS 0.00000001

class TestClass1
{
private:
    int m_a;
    int m_b;
    double m_c;

public:
    TestClass1(const int& a, const int& b, const double& c);
    
    void setA(const int& a);
    void setB(const int& b);
    void setC(const double& c);

    int getA();
    int getB();
    double getC();

    ~TestClass1();
};

void testFunction11();
float testFunction12(int arg0, double arg1);
