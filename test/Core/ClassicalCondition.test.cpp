#include <iostream>
#include <limits>
#include "Core/Utilities/Tools/OriginCollection.h"
#include "Core/VirtualQuantumProcessor/NoiseQPU/NoiseModel.h"
#include "Core/VirtualQuantumProcessor/CPUImplQPU.h"
#include "QPanda.h"
#include "gtest/gtest.h"
#include "Core/QuantumCircuit/QNodeDeepCopy.h"
USING_QPANDA
using namespace std;

TEST(ClassicalCondition, testClassicalConditionADD)
{    
    return;
    CPUQVM* m_qvm = new CPUQVM();
    m_qvm->init();
    auto c1 = m_qvm->allocateCBit();
    auto c2 = m_qvm->allocateCBit();

    c1.setValue(10);
    c2.setValue(20);
    ASSERT_EQ(c1.eval(),10);
    ASSERT_EQ(c2.eval(),20);
    // cc3 is classicalCondition
    auto cc3 = c1+c2;
    ASSERT_EQ(cc3.eval(), c1.eval()+ c2.eval());
    // cc4 is classicalCondition
    auto cc4 = c1+10;
    ASSERT_EQ(cc4.eval(), c1.eval()+10);

    // cc5 is classicalCondition
    auto cc5 = 10+c2;
    ASSERT_EQ(cc5.eval(), c2.eval()+10);

    auto prog = QProg();
    auto m_prog = CreateEmptyQProg();
    prog<<(c1=c1+1)<<(c2=c2+c1+cc3+cc4);
    auto qwhile = CreateWhileProg(c1<11,prog);
    m_prog<<qwhile;
    m_qvm->directlyRun(prog);
    ASSERT_EQ(c1.eval(),11);
    ASSERT_EQ(c2.eval(),83);
    
    m_qvm->finalize();
    delete m_qvm;
}

TEST(ClassicalCondition, testClassicalConditionSUB)
{    
    return;
    CPUQVM* m_qvm = new CPUQVM();
    m_qvm->init();
    auto c1 = m_qvm->allocateCBit();
    auto c2 = m_qvm->allocateCBit();

    c1.setValue(10);
    c2.setValue(20);

    // cc3 is classicalCondition
    auto cc3 = c2-c1;
    ASSERT_EQ(cc3.eval(),10);
    // cc4 is classicalCondition
    auto cc4 = c1-10;
    ASSERT_EQ(cc4.eval(),0);
    // cc5 is classicalCondition
    auto cc5 = 20-c2;
    ASSERT_EQ(cc5.eval(),0);

    QProg prog;
    auto m_prog = CreateEmptyQProg();
    prog<<(c1=c1+1)<<(c2=c2-c1-cc3-cc4);
    auto qwhile = CreateWhileProg(c1<11,prog);
    m_prog<<qwhile;
    m_qvm->directlyRun(prog);
    ASSERT_EQ(c1.eval(),11);
    ASSERT_EQ(c2.eval(),-1);
    m_qvm->finalize();
    delete m_qvm;
}

TEST(ClassicalCondition, testClassicalConditionMUL)
{    
    return;
    CPUQVM* m_qvm = new CPUQVM();
    m_qvm->init();
    auto c1 = m_qvm->allocateCBit();
    auto c2 = m_qvm->allocateCBit();

    c1.setValue(10);
    c2.setValue(20);

    // cc3 is classicalCondition
    auto cc3 = c2*c1;
    ASSERT_EQ(cc3.eval(),200);
    // cc4 is classicalCondition
    auto cc4 = c1*10;
    ASSERT_EQ(cc4.eval(),100);
    // cc5 is classicalCondition
    auto cc5 = 20*c2;
    ASSERT_EQ(cc5.eval(),400);

    auto prog = QProg();
    auto m_prog = CreateEmptyQProg();
    prog<<(c1=c1+1)<<(c2=c2*c1*cc3*cc4);
    auto qwhile = CreateWhileProg(c1<11,prog);
    m_prog<<qwhile;
    m_qvm->directlyRun(prog);
    std::cout <<c2.eval()<<std::endl;
    ASSERT_EQ(c1.eval(),11);
    ASSERT_EQ(c2.eval(),5324000);
    m_qvm->finalize();
    delete m_qvm;
}

TEST(ClassicalCondition, testClassicalConditionDIV)
{    
    return;
    CPUQVM* m_qvm = new CPUQVM();
    m_qvm->init();
    auto c1 = m_qvm->allocateCBit();
    auto c2 = m_qvm->allocateCBit();

    c1.setValue(10);
    c2.setValue(20);

    // cc3 is classicalCondition
    auto cc3 = c2/c1;
    ASSERT_EQ(cc3.eval(),2);
    // cc4 is classicalCondition
    //ASSERT_THROW(auto cc4 = c1/0,std::invalid_argument);
    // cc5 is classicalCondition
    auto cc5 = 20/c2;
    ASSERT_EQ(cc5.eval(),1);

    auto prog = QProg();
    auto m_prog = CreateEmptyQProg();
    prog<<(c1=c1+1)<<(c2 = c2 + 2)<<(c2=c2/c1*(c1/11));
    auto qwhile = CreateWhileProg(c1<11,prog);
    m_prog<<qwhile;
    m_qvm->directlyRun(prog);
    ASSERT_EQ(c1.eval(),11);
    ASSERT_EQ(c2.eval(),2);
    m_qvm->finalize();
    delete m_qvm;
}