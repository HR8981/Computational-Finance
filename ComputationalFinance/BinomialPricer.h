#pragma once

namespace NumericalMethods
{

	class BinomialPricer
	{
	public:
		BinomialPricer( double price, double up, double down, double rate );
		BinomialPricer &operator= ( const BinomialPricer &p );

		~BinomialPricer();

		// Compute the risk-neutral probability
		double RiskNeutProb();

		// computing the stock price at node n, i
		double S( double S0, double U, double D, int n, int i );

	protected:

		double m_price, m_rate, m_up, m_down;
	};
}