#include "gmock/gmock.h"

#include <stdio.h>
#include <string>

#include "gtest/gtest.h"

using testing::_;
using testing::AnyNumber;
using testing::Ge;
using testing::InSequence;
using testing::NaggyMock;
using testing::Ref;
using testing::Return;
using testing::Sequence;

class MockFoo {
public:
    MockFoo() {}

    MOCK_METHOD3(Bar, char(const std::string& s, int i, double x));
    MOCK_METHOD2(Bar2, bool(int x, int y));
    MOCK_METHOD2(Bar3, void(int x, int y));

private:
    GTEST_DISALLOW_COPY_AND_ASSIGN_(MockFoo);
};

class GMockOutputTest : public testing::Test {
protected:
    NaggyMock<MockFoo> foo_;
};

TEST_F(GMockOutputTest, ExpectedCall) {
    testing::GMOCK_FLAG(verbose) = "info";

    EXPECT_CALL(foo_, Bar2(0, _));
    foo_.Bar2(0, 0);  // Expected call

    testing::GMOCK_FLAG(verbose) = "warning";
}

TEST_F(GMockOutputTest, ExpectedCallToVoidFunction) {
    testing::GMOCK_FLAG(verbose) = "info";

    EXPECT_CALL(foo_, Bar3(0, _));
    foo_.Bar3(0, 0);  // Expected call

    testing::GMOCK_FLAG(verbose) = "warning";
}

TEST_F(GMockOutputTest, ExplicitActionsRunOut) {
    EXPECT_CALL(foo_, Bar2(_, _))
        .Times(2)
        .WillOnce(Return(false));
    foo_.Bar2(2, 2);
    foo_.Bar2(1, 1);  // Explicit actions in EXPECT_CALL run out.
}

TEST_F(GMockOutputTest, UnexpectedCall) {
    EXPECT_CALL(foo_, Bar2(0, _));

    foo_.Bar2(1, 0);  // Unexpected call
    foo_.Bar2(0, 0);  // Expected call
}

TEST_F(GMockOutputTest, UnexpectedCallToVoidFunction) {
    EXPECT_CALL(foo_, Bar3(0, _));

    foo_.Bar3(1, 0);  // Unexpected call
    foo_.Bar3(0, 0);  // Expected call
}

TEST_F(GMockOutputTest, ExcessiveCall) {
    EXPECT_CALL(foo_, Bar2(0, _));

    foo_.Bar2(0, 0);  // Expected call
    foo_.Bar2(0, 1);  // Excessive call
}

TEST_F(GMockOutputTest, ExcessiveCallToVoidFunction) {
    EXPECT_CALL(foo_, Bar3(0, _));

    foo_.Bar3(0, 0);  // Expected call
    foo_.Bar3(0, 1);  // Excessive call
}

TEST_F(GMockOutputTest, UninterestingCall) {
    foo_.Bar2(0, 1);  // Uninteresting call
}

TEST_F(GMockOutputTest, UninterestingCallToVoidFunction) {
    foo_.Bar3(0, 1);  // Uninteresting call
}

TEST_F(GMockOutputTest, RetiredExpectation) {
    EXPECT_CALL(foo_, Bar2(_, _))
        .RetiresOnSaturation();
    EXPECT_CALL(foo_, Bar2(0, 0));

    foo_.Bar2(1, 1);
    foo_.Bar2(1, 1);  // Matches a retired expectation
    foo_.Bar2(0, 0);
}

TEST_F(GMockOutputTest, UnsatisfiedPrerequisite) {
    {
        InSequence s;
        EXPECT_CALL(foo_, Bar(_, 0, _));
        EXPECT_CALL(foo_, Bar2(0, 0));
        EXPECT_CALL(foo_, Bar2(1, _));
    }

    foo_.Bar2(1, 0);  // Has one immediate unsatisfied pre-requisite
    foo_.Bar("Hi", 0, 0);
    foo_.Bar2(0, 0);
    foo_.Bar2(1, 0);
}

TEST_F(GMockOutputTest, UnsatisfiedPrerequisites) {
    Sequence s1, s2;

    EXPECT_CALL(foo_, Bar(_, 0, _))
        .InSequence(s1);
    EXPECT_CALL(foo_, Bar2(0, 0))
        .InSequence(s2);
    EXPECT_CALL(foo_, Bar2(1, _))
        .InSequence(s1, s2);

    foo_.Bar2(1, 0);  // Has two immediate unsatisfied pre-requisites
    foo_.Bar("Hi", 0, 0);
    foo_.Bar2(0, 0);
    foo_.Bar2(1, 0);
}

TEST_F(GMockOutputTest, UnsatisfiedWith) {
    EXPECT_CALL(foo_, Bar2(_, _)).With(Ge());
}

TEST_F(GMockOutputTest, UnsatisfiedExpectation) {
    EXPECT_CALL(foo_, Bar(_, _, _));
    EXPECT_CALL(foo_, Bar2(0, _))
        .Times(2);

    foo_.Bar2(0, 1);
}

TEST_F(GMockOutputTest, MismatchArguments) {
    const std::string s = "Hi";
    EXPECT_CALL(foo_, Bar(Ref(s), _, Ge(0)));

    foo_.Bar("Ho", 0, -0.1);  // Mismatch arguments
    foo_.Bar(s, 0, 0);
}

TEST_F(GMockOutputTest, MismatchWith) {
    EXPECT_CALL(foo_, Bar2(Ge(2), Ge(1)))
        .With(Ge());

    foo_.Bar2(2, 3);  // Mismatch With()
    foo_.Bar2(2, 1);
}

TEST_F(GMockOutputTest, MismatchArgumentsAndWith) {
    EXPECT_CALL(foo_, Bar2(Ge(2), Ge(1)))
        .With(Ge());

    foo_.Bar2(1, 3);  // Mismatch arguments and mismatch With()
    foo_.Bar2(2, 1);
}

TEST_F(GMockOutputTest, UnexpectedCallWithDefaultAction) {
    ON_CALL(foo_, Bar2(_, _))
        .WillByDefault(Return(true));   // Default action #1
    ON_CALL(foo_, Bar2(1, _))
        .WillByDefault(Return(false));  // Default action #2

    EXPECT_CALL(foo_, Bar2(2, 2));
    foo_.Bar2(1, 0);  // Unexpected call, takes default action #2.
    foo_.Bar2(0, 0);  // Unexpected call, takes default action #1.
    foo_.Bar2(2, 2);  // Expected call.
}

TEST_F(GMockOutputTest, ExcessiveCallWithDefaultAction) {
    ON_CALL(foo_, Bar2(_, _))
        .WillByDefault(Return(true));   // Default action #1
    ON_CALL(foo_, Bar2(1, _))
        .WillByDefault(Return(false));  // Default action #2

    EXPECT_CALL(foo_, Bar2(2, 2));
    EXPECT_CALL(foo_, Bar2(1, 1));

    foo_.Bar2(2, 2);  // Expected call.
    foo_.Bar2(2, 2);  // Excessive call, takes default action #1.
    foo_.Bar2(1, 1);  // Expected call.
    foo_.Bar2(1, 1);  // Excessive call, takes default action #2.
}

TEST_F(GMockOutputTest, UninterestingCallWithDefaultAction) {
    ON_CALL(foo_, Bar2(_, _))
        .WillByDefault(Return(true));   // Default action #1
    ON_CALL(foo_, Bar2(1, _))
        .WillByDefault(Return(false));  // Default action #2

    foo_.Bar2(2, 2);  // Uninteresting call, takes default action #1.
    foo_.Bar2(1, 1);  // Uninteresting call, takes default action #2.
}

TEST_F(GMockOutputTest, ExplicitActionsRunOutWithDefaultAction) {
    ON_CALL(foo_, Bar2(_, _))
        .WillByDefault(Return(true));   // Default action #1

    EXPECT_CALL(foo_, Bar2(_, _))
        .Times(2)
        .WillOnce(Return(false));
    foo_.Bar2(2, 2);
    foo_.Bar2(1, 1);  // Explicit actions in EXPECT_CALL run out.
}

TEST_F(GMockOutputTest, CatchesLeakedMocks) {
    MockFoo* foo1 = new MockFoo;
    MockFoo* foo2 = new MockFoo;

    // Invokes ON_CALL on foo1.
    ON_CALL(*foo1, Bar(_, _, _)).WillByDefault(Return('a'));

    // Invokes EXPECT_CALL on foo2.
    EXPECT_CALL(*foo2, Bar2(_, _));
    EXPECT_CALL(*foo2, Bar2(1, _));
    EXPECT_CALL(*foo2, Bar3(_, _)).Times(AnyNumber());
    foo2->Bar2(2, 1);
    foo2->Bar2(1, 1);

    // Both foo1 and foo2 are deliberately leaked.
}

TEST_F(GMockOutputTest, TestCatchesLeakedMocksInAdHocTests) {
    MockFoo* foo = new MockFoo;

    // Invokes EXPECT_CALL on foo.
    EXPECT_CALL(*foo, Bar2(_, _));
    foo->Bar2(2, 1);

    // foo is deliberately leaked.
}
