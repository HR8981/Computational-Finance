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
			virtual ~F1() = default;
			double operator()( double x ) override;
			double operator()( double x, double y ) override;
		};

		// test method f(x) = (x - 1)^3
		double F1::operator()( const double x )
		{
			return ( x - 1 ) * ( x - 1 ) * ( x - 1 );
		}
		
		// Not used.
		double F1::operator()( double x, double y )
		{
			return 0;
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

	GTEST_TEST_(SecantSolverTests, F1Bisecion, SecantSolverTests, ::testing::internal::GetTypeId<SecantSolverTests>())
	{
		F1 f;

		const auto expected_root = 1.00266;

		SecantSolver sm( f );

		const auto actual_root = sm.getRoot( -10, 10 );

		EXPECT_NEAR( expected_root, actual_root, .0001 );
	}
}