// All Rights Reserved, Copyright(C) Fujitsu Limited 2003
//-----------------------------------------------------------------------------
//
// 1.����
//	�f�[�^�^
//
// 2.�쐬��
//	(��)�x�m�ʋ�B�V�X�e���G���W�j�A�����O
//
//-----------------------------------------------------------------------------

#ifndef __OSG_VECTOR3__
#define __OSG_VECTOR3__

#include <stdlib.h>
#include <math.h>
#include "Osg_Define.h"
#include "OsgVector2.h"

//-----------------------------------------------------------------------------
//
// 1.�N���X��
//	�x�N�g���N���X�i�R�����j
//
// 2.�@�\�T�v
//	�R�����x�N�g���f�[�^�idouble�^�j�̊e��v�Z�������s���B
//	���R���X�g���N�^
//	  �EOsgVector3()
//	  �EOsgVector3( double )
//	  �EOsgVector3( const double[3] )
//	  �EOsgVector3( const double, const double, const double )
//	���I�y���[�^���Z�q
//	  �y�f�[�^�̎擾�z
//	  �EOsgVector3( int )
//	  �y�����z
//	  �E+OsgVector3
//	  �E-OsgVector3
//	  �y����z
//	  �EOsgVector3 = OsgVector3
//	  �EOsgVector3 << double[3]
//	  �EOsgVector3 >> double[3]
//	  �Edouble[3] << OsgVector3
//	  �Edouble[3] >> OsgVector3
//	  �y��r�z
//	  �EOsgVector3 == OsgVector3
//	  �EOsgVector3 != OsgVector3
//	  �y������Z�q�z
//	  �EOsgVector3 += OsgVector3
//	  �EOsgVector3 -= OsgVector3
//	  �EOsgVector3 *= OsgVector3
//	  �EOsgVector3 /= OsgVector3
//	  �EOsgVector3 &= OsgVector3
//	  �y�����揜�z
//	  �EOsgVector3 + OsgVector3
//	  �EOsgVector3 - OsgVector3
//	  �EOsgVector3 * OsgVector3	����
//	  �EOsgVector3 * double
//	  �Edouble * OsgVector3
//	  �EOsgVector3 / double
//	  �EOsgVector3 & OsgVector3	�O��
//	������֐�
//	  �Eabs( OsgVector3& )		��Βl
//	  �EIsEffective()		�L�����W�l�̃`�F�b�N
//	  �ECalcUnit()			�P�ʃx�N�g���̎擾
//
//-----------------------------------------------------------------------------

class	OsgVector3
{
//-----------------------------------------------------------------------------
//	�����o�ϐ�
//-----------------------------------------------------------------------------
public:
	double		v[3] ;		// �x�N�g������(x,y,z)

public:
//-----------------------------------------------------------------------------
//	�R���X�g���N�^
//-----------------------------------------------------------------------------
	inline	OsgVector3(){
	 	v[0] = v[1] = v[2] = 0.0f ;
	}

	inline	OsgVector3( double p ){
	 	v[0] = p ;
	 	v[1] = v[2] = 0.0f ;
	}

	inline	OsgVector3( const double p[3] ){
	 	v[0] = p[0] ; v[1] = p[1] ; v[2] = p[2] ;
	}

	inline	OsgVector3( const double x, const double y, const double z ){
	 	v[0] = x ; v[1] = y ; v[2] = z ;
	}

//-----------------------------------------------------------------------------
//	�f�X�g���N�^
//-----------------------------------------------------------------------------
	virtual	inline	~OsgVector3(){ }

//-----------------------------------------------------------------------------
//	�I�y���[�^���Z�q
//-----------------------------------------------------------------------------
//	OsgVector3( int )
	inline	double&	operator () ( int i ){
		return	v[i];
	}

//	+OsgVector3
	inline	OsgVector3&	operator + (){
		return	*this ;
	}

//	-OsgVector3
	inline	OsgVector3	operator - (){
		return	( OsgVector3(-v[0],-v[1],-v[2]) ) ;
	}

//	OsgVector3 = OsgVector3
	inline	OsgVector3&	operator = ( const OsgVector3& w ){
		v[0] = w.v[0] ; v[1] = w.v[1] ; v[2] = w.v[2] ;
		return	*this ;
	}

//	OsgVector3 << double[3]
	inline	OsgVector3&	operator << ( double* d ){
		v[0] = d[0] ; v[1] = d[1] ; v[2] = d[2] ;
		return	*this ;
	}

//	OsgVector3 >> double[3]
	inline	double*	operator >> ( double* d ){
		d[0] = v[0] ; d[1] = v[1] ; d[2] = v[2] ;
		return	d ;
	}

//	double[3] << OsgVector3
	inline	friend	double*	operator << ( double* d, OsgVector3& w ){
		d[0] = w.v[0] ; d[1] = w.v[1] ; d[2] = w.v[2] ;
		return	d ;
	}

//	double[3] >> OsgVector3
	inline	friend	OsgVector3&
			operator >> ( double* d, OsgVector3& w ){
		w.v[0] = d[0] ; w.v[1] = d[1] ; w.v[2] = d[2] ;
		return	w ;
	}
//----------------------------------------2004.08.31 (S) ADD HSK) H.Tsuruga (6)
//	OsgVector3 << MX3_POS_3D 
	inline	 OsgVector3& operator << ( MX3_POS_3D pos ){
		v[0] = pos.x ; v[1] = pos.y ; v[2] = pos.z ;
		return *this ;
	}

//	OsgVector3 >> MX3_POS_3D 
	inline	 MX3_POS_3D& operator >> ( MX3_POS_3D& pos ){
		pos.x = v[0] ; pos.y = v[1] ; pos.z = v[2] ;
		return pos ;
	}

//	MX3_POS_3D << OsgVector3 
	inline	friend MX3_POS_3D& operator << ( MX3_POS_3D& mp, OsgVector3& vp ){
		mp.x = vp.v[0] ; mp.y = vp.v[1] ; mp.z = vp.v[2] ;
		return mp ;
	}

//	MX3_POS_3D >> OsgVector3 
	inline	friend OsgVector3& operator >> ( MX3_POS_3D& mp, OsgVector3& vp ){
		vp.v[0] = mp.x ; vp.v[1] = mp.y ; vp.v[2] = mp.z ;
		return vp ;
	}

//	OsgVector3 << MX3_VEC_3D 
	inline	 OsgVector3& operator << ( MX3_VEC_3D pos ){
		v[0] = pos.x ; v[1] = pos.y ; v[2] = pos.z ;
		return *this ;
	}

//	OsgVector3 >> MX3_VEC_3D 
	inline	 MX3_VEC_3D& operator >> ( MX3_VEC_3D& pos ){
		pos.x = v[0] ; pos.y = v[1] ; pos.z = v[2] ;
		return pos ;
	}

//	MX3_VEC_3D << OsgVector3
	inline	friend MX3_VEC_3D& operator << ( MX3_VEC_3D& mp, OsgVector3& vp ){
		mp.x = vp.v[0] ; mp.y = vp.v[1] ; mp.z = vp.v[2] ;
		return mp ;
	}

//	MX3_VEC_3D >> OsgVector3
	inline	friend OsgVector3& operator >> ( MX3_VEC_3D& mp, OsgVector3& vp ){
		vp.v[0] = mp.x ; vp.v[1] = mp.y ; vp.v[2] = mp.z ;
		return vp ;
	}
//----------------------------------------2004.08.31 (E) ADD HSK) H.Tsuruga (6)

//	OsgVector3 == OsgVector3
	inline	bool operator == ( const OsgVector3& p ){
		OsgVector3	w( v[0]-p.v[0], v[1]-p.v[1], v[2]-p.v[2] ) ;
		return	( sqrt(w.v[0]*w.v[0]+
				w.v[1]*w.v[1]+w.v[2]*w.v[2]) <= OSG_EPS ) ;
	}

//	OsgVector3 != OsgVector3
	inline	bool	operator != ( const OsgVector3 &w ){
		return	!( *this == w ) ;
	}

//	OsgVector3 += OsgVector3
	inline	OsgVector3& operator += ( const OsgVector3& p ){
		v[0] += p.v[0] ;
		v[1] += p.v[1] ;
		v[2] += p.v[2] ;
		return	*this ;
	}

//	OsgVector3 -= OsgVector3
	inline	OsgVector3& operator -= ( const OsgVector3& p ){
		v[0] -= p.v[0] ;
		v[1] -= p.v[1] ;
		v[2] -= p.v[2] ;
		return	*this ;
	}

//	OsgVector3 *= double
	inline	OsgVector3& operator *= ( const double p ){
		v[0] *= p ;
		v[1] *= p ;
		v[2] *= p ;
		return	*this ;
	}

//	OsgVector3 /= double
	inline	OsgVector3& operator /= ( const double p ){
		if( fabs(p) <= OSG_DBL_EPS ) return *this ;
		v[0] /= p ;
		v[1] /= p ;
		v[2] /= p ;
		return	*this ;
	}

//	OsgVector3 &= OsgVector3
	inline	OsgVector3& operator &= ( const OsgVector3& p ){
		double	x = v[0] ;
		double	y = v[1] ;
		double	z = v[2] ;
		v[0] = y*p.v[2] - z*p.v[1] ;
		v[1] = z*p.v[0] - x*p.v[2] ;
		v[2] = x*p.v[1] - y*p.v[0] ;
		return	*this ;
	}

//	OsgVector3 + OsgVector3
	inline	OsgVector3	operator + ( const OsgVector3& p ){
		return	OsgVector3( v[0]+p.v[0], v[1]+p.v[1], v[2]+p.v[2] ) ;
	}

//	OsgVector3 - OsgVector3
	inline	OsgVector3	operator - ( const OsgVector3& p ){
		return	OsgVector3( v[0]-p.v[0], v[1]-p.v[1], v[2]-p.v[2] ) ;
	}

//	OsgVector3 * OsgVector3	����
	inline	double operator * ( const OsgVector3& p ){
		return	( v[0]*p.v[0] + v[1]*p.v[1] + v[2]*p.v[2] ) ;
	}

//	OsgVector3 * double
	inline	OsgVector3	operator * ( const double p ){
		return	OsgVector3( v[0]*p, v[1]*p, v[2]*p ) ;
	}

//	double * OsgVector3
	inline	friend	OsgVector3
			operator * ( const double p, const OsgVector3& q ){
		return	OsgVector3( p*q.v[0], p*q.v[1], p*q.v[2] ) ;
	}

//	OsgVector3 / double
	inline	OsgVector3	operator / ( const double q ){
		if( fabs(q) <= OSG_DBL_EPS ){
			return	OsgVector3( v[0], v[1], v[2] ) ;
		}else{
			return	OsgVector3( v[0]/q, v[1]/q, v[2]/q ) ;
		}
	}

//	(OsgVector2)OsgVector3
	inline	operator OsgVector2 ( void ){
		OsgVector2	data( v[0], v[1] ) ;
		return	data ;
	}

//	OsgVector3 & OsgVector3	�O��
	inline	OsgVector3	operator & ( const OsgVector3& p ){
		return	OsgVector3( v[1]*p.v[2]-v[2]*p.v[1],
			v[2]*p.v[0]-v[0]*p.v[2], v[0]*p.v[1]-v[1]*p.v[0] ) ;
	}

//-----------------------------------------------------------------------------
//	����֐�
//-----------------------------------------------------------------------------
//	abs( OsgVector3& )
	inline	friend double	abs( const OsgVector3& p ){
		return	sqrt( p.v[0]*p.v[0]+p.v[1]*p.v[1]+p.v[2]*p.v[2] ) ;
	}

//	�L�����W�l�̃`�F�b�N
	inline	bool	IsEffective(){
		if( (v[0] < OSG_MIN) || (v[0] > OSG_MAX) ||
			(v[1] < OSG_MIN) || (v[1] > OSG_MAX) ||
			(v[2] < OSG_MIN) || (v[2] > OSG_MAX) ) return false ;
		return	true ;
	}

//	�P�ʃx�N�g���̎擾
	inline	OsgVector3	CalcUnit(){
		double	val = abs(*this) ;
		if( val < OSG_EPS ) return OsgVector3(*this) ;
		OsgVector3	unit_vec ;
		unit_vec(0) = v[0] / val ;
		unit_vec(1) = v[1] / val ;
		unit_vec(2) = v[2] / val ;
		return	unit_vec ;
	}

//----------------------------------------2004.08.31 (S) ADD HSK) H.Tsuruga (6)
//	�x�N�g�����[�����`�F�b�N����
	inline	bool	IsZeroVector(){
		double dx = v[0] * v[0] ;
		double dy = v[1] * v[1] ;
		double dz = v[2] * v[2] ;

		double ds = sqrt( dx + dy + dz ) ;
		if( ds < OSG_EPS ) { return true ; }
		
		return false ;
	}

//	�f�[�^�ݒ�
	inline	void	Set( const double x, const double y, const double z ){
		v[0] = x ;
		v[1] = y ;
		v[2] = z ;
	}
//----------------------------------------2004.08.31 (E) ADD HSK) H.Tsuruga (6)
} ;

#endif

