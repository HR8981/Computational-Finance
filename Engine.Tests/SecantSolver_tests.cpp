#include "stdafx.h"

#include <gtest/gtest.h>
#include "Common.h"
#include "MathFunction.h"
#include "SecantSolver.h"

using namespace std;
using namespace Common;
using namespace NumericalMethods;

namespace NumericalTests
{
	namespace
	{
		class F1 : public MathFunction<double>
		{
		public:
			virtual ~F1();
			double operator()(double value) override;
		};

		F1::~F1()
		{
		}

		// test method f(x) = (x - 1)^3
		double F1::operator()(double x)
		{
			return (x - 1) * (x - 1) * (x - 1);
		}
	}

	class SecantSolverTests :
		public ::testing::Test
	{
	protected:
		void SetUp() override
		{
		}

		void TearDown() override
		{
		}
	};

	TEST_F(SecantSolverTests, F1Bisecion)
	{
		F1 f;

		auto expected_root = 1.00266;

		SecantSolver sm(f);

		auto actual_root = sm.getRoot(-10, 10);

		EXPECT_NEAR(expected_root, actual_root, .0001);
	}
}