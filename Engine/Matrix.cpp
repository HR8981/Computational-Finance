#include "stdafx.h"
#include "Matrix.h"
#include <utility>
#include "Common.h"

using namespace Common;

namespace NumericalMethods
{
    #pragma region Constructor / Deconstructor

    Matrix::Matrix( int size )
    {
        for( auto index = 0; index < size; ++index )
        {
            m_rows.emplace_back( size, 0 );
        }
    }

    Matrix::Matrix( const int m, const int n )
    {
        for( auto index = 0; index < m; ++index )
        {
            m_rows.emplace_back( n, 0 );
        }
    }

    Matrix::Matrix( const Matrix &m )
    {
        m_rows = m.m_rows;
    }

    Matrix::Matrix( vector<Row> rows )
        : m_rows( std::move( rows ) )
    {
    }

    Matrix& Matrix::operator=( const Matrix &m )
    {
        if( this != &m )
        {
            m_rows = m.m_rows;
        }

        return *this;
    }

	#pragma endregion

    void Matrix::transpose()
    {
        vector<Row> rows;

        for( auto i = 0; i < m_rows.size(); ++i )
        {
            Row row;

            for( auto j = 0; j < m_rows[ 0 ].size(); ++j )
            {
                row[ j ] = m_rows[ j ][ i ];
            }

            rows.push_back( row );
        }

        m_rows.swap( rows );
    }

    double Matrix::trace()
    {
        if( !m_rows.empty() && m_rows.size() != m_rows[ 0 ].size() )
            return Zero;

        auto total = 0.0;

        for( auto index = 0; index < m_rows.size(); ++index )
        {
            total += m_rows[ index ][ index ];
        }

        return total;
    }

    void Matrix::add( const Matrix &m )
    {
        if( m_rows.size() != m.m_rows.size() || m_rows[ 0 ].size() != m_rows[ 0 ].size() )
            throw runtime_error( "Invalid Matrix Dimensions" );

        for( auto i = 0; i < m_rows.size(); ++i )
        {
            for( auto j = 0; j < m.m_rows[ 0 ].size(); ++j )
            {
                m_rows[ i ][ j ] += m.m_rows[ i ][ j ];
            }
        }
    }

    void Matrix::subtract( const Matrix &m )
    {
        if( m_rows.size() != m.m_rows.size() || m_rows[ 0 ].size() != m_rows[ 0 ].size() )
            throw runtime_error( "Invalid Matrix Dimensions" );

        for( auto i = 0; i < m_rows.size(); ++i )
        {
            for( auto j = 0; j < m.m_rows[ 0 ].size(); ++j )
            {
                m_rows[ i ][ j ] -= m.m_rows[ i ][ j ];
            }
        }
    }

    void Matrix::multiply( const Matrix &m )
    {
        if( m_rows[ 0 ].size() != m.m_rows.size() )
            throw runtime_error( "Invalid Matrix Dimensions" );

        vector<Row> rows;

        for( auto &m_row : m_rows )
		{
            Row row;

            for( auto j = 0; j < m.m_rows.size(); ++j )
            {
                auto MiJ = Zero;

                for( auto k = 0; k < m_rows[ 0 ].size(); ++k )
                {
                    MiJ += m_row[ k ] * m.m_rows[ k ][ j ];
                }

                row.push_back( MiJ );
            }

            rows.push_back( row );
        }

        m_rows.swap( rows );
    }

    vector<Matrix::Row> Matrix::getRows() const
    {
        return m_rows;
    }

    Matrix::Row& Matrix::operator[]( const int pos )
    {
        return m_rows[ pos ];
    }

    bool Matrix::operator==( const Matrix &m ) const
    {
        return m_rows == m.m_rows;
    }

	size_t Matrix::numRows() const
	{
		return m_rows.size();
	}

	Matrix operator+( const Matrix &m1, const Matrix &m2 )
    {
        auto tmp( m1 );
        tmp.add( m2 );
        return tmp;
    }

    Matrix operator*( const Matrix &m1, const Matrix &m2 )
    {
        auto tmp( m1 );
        tmp.multiply( m2 );
        return tmp;
    }

    Matrix operator-( const Matrix &m1, const Matrix &m2 )
    {
        auto tmp( m1 );
        tmp.subtract( m2 );
        return tmp;
    }
}