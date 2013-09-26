#pragma once

#include "Vector3.hpp"
#include <memory>

namespace woodman
{
	const float PI = 3.1415926538f;
	const float CONVERT_TO_RADIANS = PI / 180.0f;

	const float CONVERT_TO_DEGREES = 180.0f / PI;
	
	template< typename T_ObjType>
	class Matrix4
	{
	public:
		float m_Matrix[16];

		//		ROW-major memory layout: from float[16] to matrix
		//		0  1  2  3   |  ix  iy  iz  0
		//		4  5  6  7   |  jx  jy  jz  0
		//		8  9  10 11  |  kx  ky  kz  0
		//		12 13 14 15  |  Tx  Ty  Tz  1

		//constructors
		Matrix4();
		Matrix4( T_ObjType mat[] );
		Matrix4( const Matrix4& m);
		Matrix4( T_ObjType i, T_ObjType j, T_ObjType k );
		//Matrix4( const Vector3<T_ObjType>& iv, const Vector3<T_ObjType>& jv, const Vector3<T_ObjType>& kv);


		// Accessors / const Matrix Transformations
		Matrix4 getRotationMatrix() const;
		Matrix4 getTranslationMatrix() const;
		Matrix4 getInverseMatrix() const;
		Matrix4 getTranslatedMatrix( const Vector3<T_ObjType>& V ) const;
		Matrix4 getTranslatedMatrix( T_ObjType iOffset, T_ObjType jOffset, T_ObjType kOffset) const;
		Matrix4 getScaledMatrix( T_ObjType uniformScale );
		Matrix4 getScaledMatrix( const Vector3<T_ObjType>& nonUniformScale ) const;
		Matrix4 getScaledMatrix( T_ObjType iScale, T_ObjType jScale, T_ObjType kScale ) const;

		

		// Non-Const Matrix Transformations
		//
		Matrix4& Invert();
		Matrix4& Transpose();
		Matrix4& NegateTranslation();
		Matrix4& translateMatrix( const Vector3<T_ObjType>& V );
		Matrix4& uniformScaleMatrix( T_ObjType uniformScale );
		Matrix4& nonUniformScaleMatrix( const Vector3<T_ObjType>& nonUniformScale );
		Matrix4& TransposeRotation();
		Matrix4& rotateMatrixDegrees( float YawDegrees, float PitchDegrees, float RollDegrees );
		Matrix4& rotateMatrixRadians( float YawRadians, float PitchRadians, float RollRadians );

		// vector manipulation
		//
		Vector3<T_ObjType>& transformVector( Vector3<T_ObjType>& v ) const;
		Vector3<T_ObjType> getTransformedVector( const Vector3<T_ObjType>& v) const;

		// operators
		Matrix4& operator=( const Matrix4<T_ObjType>& m );
		Matrix4& operator*=(const Matrix4<T_ObjType>& m);
		T_ObjType operator[]( size_t i ) const { return m_Matrix[i];}
		T_ObjType& operator[]( size_t i ){ return m_Matrix[i];}

		Matrix4 operator*( const Matrix4& m2) const;
	};
	

	typedef Matrix4<float> Matrix4f;
	typedef Matrix4<double> Matrix4d;

	//////////////////////////////////////////////////////////////////////////////////////////
	// Constructors																			//
	//////////////////////////////////////////////////////////////////////////////////////////

	template <typename T_ObjType>
	inline Matrix4<T_ObjType>::Matrix4()
	{
		memset(m_Matrix, 0, 16 * (int)sizeof(float));

		m_Matrix[0] = 1.0f;
		m_Matrix[5] = 1.0f;
		m_Matrix[10] = 1.0f;
		m_Matrix[15] = 1.0f;

	}

	template <typename T_ObjType>
	inline Matrix4<T_ObjType>::Matrix4( T_ObjType mat[] )
	{


		memset(m_Matrix, 0, 16 * (int)sizeof(float));

		// not done with a for loop cause it is slower (maybe)
		m_Matrix[0] = mat[0];
		m_Matrix[1] = mat[1];
		m_Matrix[2] = mat[2];
		m_Matrix[3] = mat[3];
		m_Matrix[4] = mat[4];
		m_Matrix[5] = mat[5];
		m_Matrix[6] = mat[6];
		m_Matrix[7] = mat[7];
		m_Matrix[8] = mat[8];
		m_Matrix[9] = mat[9];
		m_Matrix[10] = mat[10];
		m_Matrix[11] = mat[11];
		m_Matrix[12] = mat[12];
		m_Matrix[13] = mat[13];
		m_Matrix[14] = mat[14];
		m_Matrix[15] = mat[15];
	}


	template <typename T_ObjType>
	inline Matrix4<T_ObjType>::Matrix4( const Matrix4<T_ObjType>& m)
	{
		for(int i = 0; i < 16; i++ )
			m_Matrix[i] = m.m_Matrix[i];
	}

	template <typename T_ObjType>
	inline Matrix4<T_ObjType>::Matrix4( T_ObjType i, T_ObjType j, T_ObjType k )
	{
		memset(m_Matrix, 0, 16 * (int)sizeof(float));

		m_Matrix[0] = i;
		m_Matrix[5] = j;
		m_Matrix[10] = k;
		m_Matrix[15] = 1.0f;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Transformation Functions																//
	//////////////////////////////////////////////////////////////////////////////////////////

	template <typename T_ObjType>
	inline Matrix4<T_ObjType>& Matrix4<T_ObjType>::translateMatrix( const Vector3<T_ObjType>& V )
	{
		Matrix4<T_ObjType> temp;
		ConstructTranslationMatrix<T_ObjType>(temp, V);
		*this *= temp;
		return *this;
	}

	template <typename T_ObjType>
	inline Matrix4<T_ObjType>& Matrix4<T_ObjType>::TransposeRotation()
	{
		std::swap(m_Matrix[1], m_Matrix[4]);
		std::swap(m_Matrix[2], m_Matrix[8]);
		std::swap(m_Matrix[6], m_Matrix[9]);
		return *this;
	}


	template <typename T_ObjType>
	inline Matrix4<T_ObjType>& Matrix4<T_ObjType>::NegateTranslation()
	{
		m_Matrix[12] = -m_Matrix[12];
		m_Matrix[13] = -m_Matrix[13];
		m_Matrix[14] = -m_Matrix[14];

		return *this;
	}

	template <typename T_ObjType>
	inline Matrix4<T_ObjType>& Matrix4<T_ObjType>::Invert()
	{

		// OpenGL matrices are column major and can be quite confusing to access
		// when stored in the typical, one-dimensional array often used by the API.
		// Here are some shorthand conversion macros, which convert a row/column
		// combination into an array index.

#define MAT(m_Matrix,r,c) m_Matrix[c*4+r]

#define m11 MAT(m_Matrix,0,0)
#define m12 MAT(m_Matrix,0,1)
#define m13 MAT(m_Matrix,0,2)
#define m14 MAT(m_Matrix,0,3)
#define m21 MAT(m_Matrix,1,0)
#define m22 MAT(m_Matrix,1,1)
#define m23 MAT(m_Matrix,1,2)
#define m24 MAT(m_Matrix,1,3)
#define m31 MAT(m_Matrix,2,0)
#define m32 MAT(m_Matrix,2,1)
#define m33 MAT(m_Matrix,2,2)
#define m34 MAT(m_Matrix,2,3)
#define m41 MAT(m_Matrix,3,0)
#define m42 MAT(m_Matrix,3,1)
#define m43 MAT(m_Matrix,3,2)
#define m44 MAT(m_Matrix,3,3)

		// Inverse = adjoint / det. (See linear algebra texts.)

		// pre-compute 2x2 dets for last two rows when computing
		// cofactors of first two rows.
		float d12 = (m31 * m42 - m41 * m32);
		float d13 = (m31 * m43 - m41 * m33);
		float d23 = (m32 * m43 - m42 * m33);
		float d24 = (m32 * m44 - m42 * m34);
		float d34 = (m33 * m44 - m43 * m34);
		float d41 = (m34 * m41 - m44 * m31);

		T_ObjType tmp[16];

		tmp[0] =  (m22 * d34 - m23 * d24 + m24 * d23);
		tmp[1] = -(m21 * d34 + m23 * d41 + m24 * d13);
		tmp[2] =  (m21 * d24 + m22 * d41 + m24 * d12);
		tmp[3] = -(m21 * d23 - m22 * d13 + m23 * d12);

		// Compute determinant as early as possible using these cofactors.
		float det = m11 * tmp[0] + m12 * tmp[1] + m13 * tmp[2] + m14 * tmp[3];

		// Run singularity test.
		if( det == 0.0 )
		{
			//cout << "Warning: Call to invertMatrix produced a Singular matrix." << endl;

			float identity[16] =
			{
				1.0, 0.0, 0.0, 0.0,
				0.0, 1.0, 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0,
				0.0, 0.0, 0.0, 1.0
			};

			memcpy( m_Matrix, identity, 16*sizeof(float) );
		}
		else
		{
			float invDet = 1.0f / det;

			// Compute rest of inverse.
			tmp[0] *= invDet;
			tmp[1] *= invDet;
			tmp[2] *= invDet;
			tmp[3] *= invDet;

			tmp[4] = -(m12 * d34 - m13 * d24 + m14 * d23) * invDet;
			tmp[5] =  (m11 * d34 + m13 * d41 + m14 * d13) * invDet;
			tmp[6] = -(m11 * d24 + m12 * d41 + m14 * d12) * invDet;
			tmp[7] =  (m11 * d23 - m12 * d13 + m13 * d12) * invDet;

			// Pre-compute 2x2 dets for first two rows when computing cofactors
			// of last two rows.
			d12 = m11 * m22 - m21 * m12;
			d13 = m11 * m23 - m21 * m13;
			d23 = m12 * m23 - m22 * m13;
			d24 = m12 * m24 - m22 * m14;
			d34 = m13 * m24 - m23 * m14;
			d41 = m14 * m21 - m24 * m11;

			tmp[8]  =  (m42 * d34 - m43 * d24 + m44 * d23) * invDet;
			tmp[9]  = -(m41 * d34 + m43 * d41 + m44 * d13) * invDet;
			tmp[10] =  (m41 * d24 + m42 * d41 + m44 * d12) * invDet;
			tmp[11] = -(m41 * d23 - m42 * d13 + m43 * d12) * invDet;
			tmp[12] = -(m32 * d34 - m33 * d24 + m34 * d23) * invDet;
			tmp[13] =  (m31 * d34 + m33 * d41 + m34 * d13) * invDet;
			tmp[14] = -(m31 * d24 + m32 * d41 + m34 * d12) * invDet;
			tmp[15] =  (m31 * d23 - m32 * d13 + m33 * d12) * invDet;

			memcpy( m_Matrix, tmp, 16*sizeof(float) );
		}

#undef m11
#undef m12
#undef m13
#undef m14
#undef m21
#undef m22
#undef m23
#undef m24
#undef m31
#undef m32
#undef m33
#undef m34
#undef m41
#undef m42
#undef m43
#undef m44
#undef MAT

		return *this;
	}

	template <typename T_ObjType>
	inline Matrix4<T_ObjType> Matrix4<T_ObjType>::getRotationMatrix() const
	{
		Matrix4<T_ObjType> m;

		// | 0  1  2  3|
		// | 4  5  6  7|
		// | 8  9 10 11|
		// |12 13 14 15|
		m[0] = m_Matrix[0];
		m[1] = m_Matrix[1];
		m[2] = m_Matrix[2];
		m[4] = m_Matrix[4];
		m[5] = m_Matrix[5];
		m[6] = m_Matrix[6];
		m[8] = m_Matrix[8];
		m[9] = m_Matrix[9];
		m[10] = m_Matrix[10];



		return m;
	}

	template <typename T_ObjType>
	inline Matrix4<T_ObjType> Matrix4<T_ObjType>::getTranslationMatrix() const
	{
		Matrix4<T_ObjType> m;

		m[12] = m_Matrix[12];
		m[13] = m_Matrix[13];
		m[14] = m_Matrix[14];

		return m;
	}



	//////////////////////////////////////////////////////////////////////////////////////////
	// Transformation Functions															//
	//////////////////////////////////////////////////////////////////////////////////////////

	template <typename T_ObjType>
	inline Vector3<T_ObjType>& Matrix4<T_ObjType>::transformVector( Vector3<T_ObjType>& v ) const
	{
		v.x = v.x * m_Matrix[0] + v.y * m_Matrix[4] + v.z * m_Matrix[8] + m_Matrix[12];

		v.y = v.x * m_Matrix[1] + v.y * m_Matrix[5] + v.z * m_Matrix[9] + m_Matrix[13];

		v.z = v.x * m_Matrix[2] + v.y * m_Matrix[6] + v.z * m_Matrix[10] + m_Matrix[14];

		return v;
	}

	template <typename T_ObjType>
	inline Vector3<T_ObjType> Matrix4<T_ObjType>::getTransformedVector( const Vector3<T_ObjType>& v ) const
	{
		float x, y, z;

		x = v.x * m_Matrix[0] + v.y * m_Matrix[4] + v.z * m_Matrix[8] + m_Matrix[12];

		y = v.x * m_Matrix[1] + v.y * m_Matrix[5] + v.z * m_Matrix[9] + m_Matrix[13];

		z = v.x * m_Matrix[2] + v.y * m_Matrix[6] + v.z * m_Matrix[10] + m_Matrix[14];

		return Vector3<T_ObjType>(x, y, z);

	}
	//////////////////////////////////////////////////////////////////////////////////////////
	// Operators																			//
	//////////////////////////////////////////////////////////////////////////////////////////

	template <typename T_ObjType>
	inline Matrix4<T_ObjType>& Matrix4<T_ObjType>::operator=( const Matrix4<T_ObjType>& m )
	{
		for(int i = 0; i < 16; i++ )
			m_Matrix[i] = m.m_Matrix[i];

		return *this;
	}

	template <typename T_ObjType>
	inline Matrix4<T_ObjType> Matrix4<T_ObjType>::operator*( const Matrix4<T_ObjType>& rhs) const
	{	
		Matrix4<T_ObjType> result;

		result[0] = m_Matrix[0]*rhs[0] + m_Matrix[1]*rhs[4] + m_Matrix[2]*rhs[8] + m_Matrix[3]*rhs[12];
		result[1] = m_Matrix[0]*rhs[1] + m_Matrix[1]*rhs[5] + m_Matrix[2]*rhs[9] + m_Matrix[3]*rhs[13];
		result[2] = m_Matrix[0]*rhs[2] + m_Matrix[1]*rhs[6] + m_Matrix[2]*rhs[10] + m_Matrix[3]*rhs[14];
		result[3] = m_Matrix[0]*rhs[3] + m_Matrix[1]*rhs[7] + m_Matrix[2]*rhs[11] + m_Matrix[3]*rhs[15];

		result[4] = m_Matrix[4]*rhs[0] + m_Matrix[5]*rhs[4] + m_Matrix[6]*rhs[8] + m_Matrix[7]*rhs[12];
		result[5] = m_Matrix[4]*rhs[1] + m_Matrix[5]*rhs[5] + m_Matrix[6]*rhs[9] + m_Matrix[7]*rhs[13];
		result[6] = m_Matrix[4]*rhs[2] + m_Matrix[5]*rhs[6] + m_Matrix[6]*rhs[10] + m_Matrix[7]*rhs[14];
		result[7] = m_Matrix[4]*rhs[3] + m_Matrix[5]*rhs[7] + m_Matrix[6]*rhs[11] + m_Matrix[7]*rhs[15];

		result[8] = m_Matrix[8]*rhs[0] + m_Matrix[9]*rhs[4] + m_Matrix[10]*rhs[8] + m_Matrix[11]*rhs[12];
		result[9] = m_Matrix[8]*rhs[1] + m_Matrix[9]*rhs[5] + m_Matrix[10]*rhs[9] + m_Matrix[11]*rhs[13];
		result[10] = m_Matrix[8]*rhs[2] + m_Matrix[9]*rhs[6] + m_Matrix[10]*rhs[10] + m_Matrix[11]*rhs[14];
		result[11] = m_Matrix[8]*rhs[3] + m_Matrix[9]*rhs[7] + m_Matrix[10]*rhs[11] + m_Matrix[11]*rhs[15];

		result[12] = m_Matrix[12]*rhs[0] + m_Matrix[13]*rhs[4] + m_Matrix[14]*rhs[8] + m_Matrix[15]*rhs[12];
		result[13] = m_Matrix[12]*rhs[1] + m_Matrix[13]*rhs[5] + m_Matrix[14]*rhs[9] + m_Matrix[15]*rhs[13];
		result[14] = m_Matrix[12]*rhs[2] + m_Matrix[13]*rhs[6] + m_Matrix[14]*rhs[10] + m_Matrix[15]*rhs[14];
		result[15] = m_Matrix[12]*rhs[3] + m_Matrix[13]*rhs[7] + m_Matrix[14]*rhs[11] + m_Matrix[15]*rhs[15];

		return result;
	}

	template <typename T_ObjType>
	inline Matrix4<T_ObjType>& Matrix4<T_ObjType>::operator*=(const Matrix4<T_ObjType>& rhs) 
	{	
		float m[16];

		m[0] = m_Matrix[0]*rhs[0] + m_Matrix[1]*rhs[4] + m_Matrix[2]*rhs[8] + m_Matrix[3]*rhs[12];
		m[1] = m_Matrix[0]*rhs[1] + m_Matrix[1]*rhs[5] + m_Matrix[2]*rhs[9] + m_Matrix[3]*rhs[13];
		m[2] = m_Matrix[0]*rhs[2] + m_Matrix[1]*rhs[6] + m_Matrix[2]*rhs[10] + m_Matrix[3]*rhs[14];
		m[3] = m_Matrix[0]*rhs[3] + m_Matrix[1]*rhs[7] + m_Matrix[2]*rhs[11] + m_Matrix[3]*rhs[15];

		m[4] = m_Matrix[4]*rhs[0] + m_Matrix[5]*rhs[4] + m_Matrix[6]*rhs[8] + m_Matrix[7]*rhs[12];
		m[5] = m_Matrix[4]*rhs[1] + m_Matrix[5]*rhs[5] + m_Matrix[6]*rhs[9] + m_Matrix[7]*rhs[13];
		m[6] = m_Matrix[4]*rhs[2] + m_Matrix[5]*rhs[6] + m_Matrix[6]*rhs[10] + m_Matrix[7]*rhs[14];
		m[7] = m_Matrix[4]*rhs[3] + m_Matrix[5]*rhs[7] + m_Matrix[6]*rhs[11] + m_Matrix[7]*rhs[15];

		m[8] = m_Matrix[8]*rhs[0] + m_Matrix[9]*rhs[4] + m_Matrix[10]*rhs[8] + m_Matrix[11]*rhs[12];
		m[9] = m_Matrix[8]*rhs[1] + m_Matrix[9]*rhs[5] + m_Matrix[10]*rhs[9] + m_Matrix[11]*rhs[13];
		m[10] = m_Matrix[8]*rhs[2] + m_Matrix[9]*rhs[6] + m_Matrix[10]*rhs[10] + m_Matrix[11]*rhs[14];
		m[11] = m_Matrix[8]*rhs[3] + m_Matrix[9]*rhs[7] + m_Matrix[10]*rhs[11] + m_Matrix[11]*rhs[15];

		m[12] = m_Matrix[12]*rhs[0] + m_Matrix[13]*rhs[4] + m_Matrix[14]*rhs[8] + m_Matrix[15]*rhs[12];
		m[13] = m_Matrix[12]*rhs[1] + m_Matrix[13]*rhs[5] + m_Matrix[14]*rhs[9] + m_Matrix[15]*rhs[13];
		m[14] = m_Matrix[12]*rhs[2] + m_Matrix[13]*rhs[6] + m_Matrix[14]*rhs[10] + m_Matrix[15]*rhs[14];
		m[15] = m_Matrix[12]*rhs[3] + m_Matrix[13]*rhs[7] + m_Matrix[14]*rhs[11] + m_Matrix[15]*rhs[15];

		for(int i = 0; i < 16; i++ )
			m_Matrix[i] = m[i];

		return *this;
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	//**																					**//
	//** Creation Funcitons																	**//
	//**																					**//
	////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////////
	// Matrix Creation																		//
	//////////////////////////////////////////////////////////////////////////////////////////

	template <typename T_ObjType>
	inline Matrix4<T_ObjType>& ConstructTranslationMatrix( Matrix4<T_ObjType>& outMatrix, T_ObjType iOffset, T_ObjType jOffset, T_ObjType kOffset )
	{
		memset(outMatrix.m_Matrix, 0, 16 * static_cast<int>( sizeof(T_ObjType) ) );

		outMatrix.m_Matrix[0] = static_cast<T_ObjType>(1.);
		outMatrix.m_Matrix[5] = static_cast<T_ObjType>(1.);
		outMatrix.m_Matrix[10] = static_cast<T_ObjType>(1.);
		
		outMatrix.m_Matrix[12] = iOffset;
		outMatrix.m_Matrix[13] = jOffset;
		outMatrix.m_Matrix[14] = kOffset;
		outMatrix.m_Matrix[15] = static_cast<T_ObjType>(1.);

		return outMatrix;

	}

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	template <typename T_ObjType>
	inline Matrix4<T_ObjType>& ConstructTranslationMatrix( Matrix4<T_ObjType>& outMatrix, const Vector3<T_ObjType>& V )
	{
		memset(outMatrix.m_Matrix, 0, 16 * static_cast<int>( sizeof(T_ObjType) ) );

		outMatrix.m_Matrix[0] = static_cast<T_ObjType>(1.);
		outMatrix.m_Matrix[5] = static_cast<T_ObjType>(1.);
		outMatrix.m_Matrix[10] = static_cast<T_ObjType>(1.);

		outMatrix.m_Matrix[12] = V.x;
		outMatrix.m_Matrix[13] = V.y;
		outMatrix.m_Matrix[14] = V.z;
		outMatrix.m_Matrix[15] = static_cast<T_ObjType>(1.);

		return outMatrix;
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	template <typename T_ObjType>
	inline Matrix4<T_ObjType>& ConstructScaleMatrix( Matrix4<T_ObjType>& outMatrix, float uniformScale )
	{
		memset(outMatrix.m_Matrix, 0, 16 * static_cast<int>( sizeof(T_ObjType) ) );

		outMatrix.m_Matrix[0] = static_cast<T_ObjType>(uniformScale);
		outMatrix.m_Matrix[5] = static_cast<T_ObjType>(uniformScale);
		outMatrix.m_Matrix[10] = static_cast<T_ObjType>(uniformScale);
		outMatrix.m_Matrix[15] = static_cast<T_ObjType>(1.);

		return outMatrix;
	}

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	template <typename T_ObjType>
	inline Matrix4<T_ObjType>& ConstructScaleMatrix( Matrix4<T_ObjType>& outMatrix, float iScale, float jScale, float kScale )
	{
		memset(outMatrix.m_Matrix, 0, 16 * static_cast<int>( sizeof(T_ObjType) ) );

		outMatrix.m_Matrix[0] = static_cast<T_ObjType>(iScale);
		outMatrix.m_Matrix[5] = static_cast<T_ObjType>(jScale);
		outMatrix.m_Matrix[10] = static_cast<T_ObjType>(kScale);
		outMatrix.m_Matrix[15] = static_cast<T_ObjType>(1.);

		return outMatrix;
	}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	template <typename T_ObjType>
	inline Matrix4<T_ObjType>& ConstructScaleMatrix( Matrix4<T_ObjType>& outMatrix, const Vector3<float>& nonUniformScale )
	{
		memset(outMatrix.m_Matrix, 0, 16 * static_cast<int>( sizeof(T_ObjType) ) );

		outMatrix.m_Matrix[0] = static_cast<T_ObjType>(nonUniformScale.x);
		outMatrix.m_Matrix[5] = static_cast<T_ObjType>(nonUniformScale.y);
		outMatrix.m_Matrix[10] = static_cast<T_ObjType>(nonUniformScale.z);
		outMatrix.m_Matrix[15] = static_cast<T_ObjType>(1.);

		return outMatrix;
	}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	template <typename T_ObjType>
	inline Matrix4<T_ObjType>& ConstructRotationMatrixDegrees( Matrix4<T_ObjType>& outMatrix, const Vector3<float>& Degrees )
	{
		memset(outMatrix.m_Matrix, 0, 16 * static_cast<int>( sizeof(T_ObjType) ) );

		outMatrix.m_Matrix[0] = 1.0f;
		outMatrix.m_Matrix[5] = 1.0f;
		outMatrix.m_Matrix[10] = 1.0f;
		outMatrix.m_Matrix[15] = 1.0f;

		if( Degrees.z != 0)
		{
			Matrix4f RollMatrix;

			float RollRadians = Degrees.z * CONVERT_TO_RADIANS;

			float rcos = cos(RollRadians);
			float rsin = sin(RollRadians);

			RollMatrix.m_Matrix[0] = rcos; 
			RollMatrix.m_Matrix[5] = rcos;
			RollMatrix.m_Matrix[4] = -rsin;
			RollMatrix.m_Matrix[1] = rsin;

			outMatrix *= RollMatrix;
		}

		if( Degrees.y != 0)
		{
			Matrix4f PitchMatrix;
			float PitchRadians = Degrees.y * CONVERT_TO_RADIANS;

			float pcos = cos(PitchRadians);
			float psin = sin(PitchRadians);

			PitchMatrix.m_Matrix[5] = pcos; 
			PitchMatrix.m_Matrix[10] = pcos;
			PitchMatrix.m_Matrix[9] = -psin;
			PitchMatrix.m_Matrix[6] = psin;

			outMatrix *= PitchMatrix;
		}

		if( Degrees.x != 0 )
		{
			Matrix4f YawMatrix;
			float YawRadians = Degrees.x * CONVERT_TO_RADIANS;

			float ycos = cos(YawRadians);
			float ysin = sin(YawRadians);

			YawMatrix.m_Matrix[0] = ycos; 
			YawMatrix.m_Matrix[10] = ycos;
			YawMatrix.m_Matrix[2] = -ysin;
			YawMatrix.m_Matrix[8] = ysin;

			outMatrix *= YawMatrix;
		}

		return outMatrix;
	}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	template <typename T_ObjType>
	inline Matrix4<T_ObjType>& ConstructRotationMatrixDegrees( Matrix4<T_ObjType>& outMatrix, float YawDegrees, float PitchDegrees, float RollDegrees )
	{
		memset(outMatrix.m_Matrix, 0, 16 * static_cast<int>( sizeof(T_ObjType) ) );

		outMatrix.m_Matrix[0] = 1.0f;
		outMatrix.m_Matrix[5] = 1.0f;
		outMatrix.m_Matrix[10] = 1.0f;
		outMatrix.m_Matrix[15] = 1.0f;

		if( RollDegrees != 0)
		{
			Matrix4f RollMatrix;

			float RollRadians = RollDegrees * CONVERT_TO_RADIANS;

			float rcos = cos(RollRadians);
			float rsin = sin(RollRadians);

			RollMatrix.m_Matrix[0] = rcos; 
			RollMatrix.m_Matrix[5] = rcos;
			RollMatrix.m_Matrix[4] = -rsin;
			RollMatrix.m_Matrix[1] = rsin;

			outMatrix *= RollMatrix;
		}

		if( PitchDegrees != 0)
		{
			Matrix4f PitchMatrix;
			float PitchRadians = PitchDegrees * CONVERT_TO_RADIANS;

			float pcos = cos(PitchRadians);
			float psin = sin(PitchRadians);

			PitchMatrix.m_Matrix[5] = pcos; 
			PitchMatrix.m_Matrix[10] = pcos;
			PitchMatrix.m_Matrix[9] = -psin;
			PitchMatrix.m_Matrix[6] = psin;

			outMatrix *= PitchMatrix;
		}

		if( YawDegrees != 0 )
		{
			Matrix4f YawMatrix;
			float YawRadians = YawDegrees * CONVERT_TO_RADIANS;

			float ycos = cos(YawRadians);
			float ysin = sin(YawRadians);

			YawMatrix.m_Matrix[0] = ycos; 
			YawMatrix.m_Matrix[10] = ycos;
			YawMatrix.m_Matrix[2] = -ysin;
			YawMatrix.m_Matrix[8] = ysin;

			outMatrix *= YawMatrix;
		}

		return outMatrix;
	}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	template <typename T_ObjType>
	inline Matrix4<T_ObjType>& ConstructRotationMatrixRadians( Matrix4<T_ObjType>& outMatrix, const Vector3<float>& Radians )
	{
		memset(outMatrix.m_Matrix, 0, 16 * static_cast<int>( sizeof(T_ObjType) ) );

		outMatrix.m_Matrix[0] = 1.0f;
		outMatrix.m_Matrix[5] = 1.0f;
		outMatrix.m_Matrix[10] = 1.0f;
		outMatrix.m_Matrix[15] = 1.0f;

		if( Radians.z != 0)
		{
			Matrix4f RollMatrix;

			float rcos = cos(Radians.z);
			float rsin = sin(Radians.z);

			RollMatrix.m_Matrix[0] = rcos; 
			RollMatrix.m_Matrix[5] = rcos;
			RollMatrix.m_Matrix[4] = -rsin;
			RollMatrix.m_Matrix[1] = rsin;

			outMatrix *= RollMatrix;
		}

		if( Radians.y != 0)
		{
			Matrix4f PitchMatrix;

			float pcos = cos(Radians.y);
			float psin = sin(Radians.y);

			PitchMatrix.m_Matrix[5] = pcos; 
			PitchMatrix.m_Matrix[10] = pcos;
			PitchMatrix.m_Matrix[9] = -psin;
			PitchMatrix.m_Matrix[6] = psin;

			outMatrix *= PitchMatrix;
		}

		if( Radians.x != 0 )
		{
			Matrix4f YawMatrix;

			float ycos = cos(Radians.x);
			float ysin = sin(Radians.x);

			YawMatrix.m_Matrix[0] = ycos; 
			YawMatrix.m_Matrix[10] = ycos;
			YawMatrix.m_Matrix[2] = -ysin;
			YawMatrix.m_Matrix[8] = ysin;

			outMatrix *= YawMatrix;
		}

		return outMatrix;
	}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	template <typename T_ObjType>
	inline Matrix4<T_ObjType>& ConstructRotationMatrixRadians( Matrix4<T_ObjType>& outMatrix, float YawRadians, float PitchRadians, float RollRadians )
	{
		memset(outMatrix.m_Matrix, 0, 16 * static_cast<int>( sizeof(T_ObjType) ) );

		outMatrix.m_Matrix[0] = 1.0f;
		outMatrix.m_Matrix[5] = 1.0f;
		outMatrix.m_Matrix[10] = 1.0f;
		outMatrix.m_Matrix[15] = 1.0f;

		if( RollRadians != 0)
		{
			Matrix4f RollMatrix;

			float rcos = cos(RollRadians);
			float rsin = sin(RollRadians);

			RollMatrix.m_Matrix[0] = rcos; 
			RollMatrix.m_Matrix[5] = rcos;
			RollMatrix.m_Matrix[4] = -rsin;
			RollMatrix.m_Matrix[1] = rsin;

			outMatrix *= RollMatrix;
		}

		if( PitchRadians != 0)
		{
			Matrix4f PitchMatrix;

			float pcos = cos(PitchRadians);
			float psin = sin(PitchRadians);

			PitchMatrix.m_Matrix[5] = pcos; 
			PitchMatrix.m_Matrix[10] = pcos;
			PitchMatrix.m_Matrix[9] = -psin;
			PitchMatrix.m_Matrix[6] = psin;

			outMatrix *= PitchMatrix;
		}

		if( YawRadians != 0 )
		{
			Matrix4f YawMatrix;

			float ycos = cos(YawRadians);
			float ysin = sin(YawRadians);

			YawMatrix.m_Matrix[0] = ycos; 
			YawMatrix.m_Matrix[10] = ycos;
			YawMatrix.m_Matrix[2] = -ysin;
			YawMatrix.m_Matrix[8] = ysin;

			outMatrix *= YawMatrix;
		}

		return outMatrix;
	}

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	template <typename T_ObjType>
	inline Matrix4<T_ObjType>& ConstructRotationMatrixFromDirectionalVector( Matrix4<T_ObjType>& outMatrix, const Vector3<T_ObjType>& direction)
	{
		Vector3<T_ObjType> tempUp( 0, 1, 0);
		Vector3<T_ObjType> directiontemp(direction);
		directiontemp.Normalize();
		Vector3<T_ObjType> right(directiontemp.CrossProduct(tempUp));
		right.Normalize();
		Vector3<T_ObjType> up(right.CrossProduct(directiontemp));
		
		memset(outMatrix.m_Matrix, 0, 16 * static_cast<int>( sizeof(T_ObjType) ) );
		outMatrix[0]  = right.x;
		outMatrix[4]  = right.y;
		outMatrix[8]  = right.z;
		outMatrix[2]  = directiontemp.x;
		outMatrix[6]  = directiontemp.y;
		outMatrix[10]  = directiontemp.z;
		outMatrix[1]  = up.x;
		outMatrix[5]  = up.y;
		outMatrix[9] = up.z;
		outMatrix[15] = 1;

		return outMatrix;
	}


}