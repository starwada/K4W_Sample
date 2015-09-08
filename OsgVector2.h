// All Rights Reserved, Copyright(C) Fujitsu Limited 2003
//-----------------------------------------------------------------------------
//
// 1.分類
//	データ型
//
//
//-----------------------------------------------------------------------------

#ifndef __OSG_VECTOR2__
#define __OSG_VECTOR2__

#include <stdlib.h>
#include <math.h>
#include "Osg_Define.h"

//-----------------------------------------------------------------------------
//
// 1.クラス名
//	ベクトルクラス（２次元）
//
// 2.機能概要
//	２次元ベクトルデータ（double型）の各種計算処理を行う。
//	■コンストラクタ
//	  ・OsgVector2()
//	  ・OsgVector2( double )
//	  ・OsgVector2( const double[2] )
//	  ・OsgVector2( const double, const double )
//	■オペレータ演算子
//	  【データの取得】
//	  ・OsgVector2( int )
//	  【符号】
//	  ・+OsgVector2
//	  ・-OsgVector2
//	  【代入】
//	  ・OsgVector2 = OsgVector2
//	  ・OsgVector2 << double[2]
//	  ・OsgVector2 >> double[2]
//	  ・double[2] << OsgVector2
//	  ・double[2] >> OsgVector2
//	  【比較】
//	  ・OsgVector2 == OsgVector2
//	  ・OsgVector2 != OsgVector2
//	  【代入演算子】
//	  ・OsgVector2 += OsgVector2
//	  ・OsgVector2 -= OsgVector2
//	  ・OsgVector2 *= OsgVector2
//	  ・OsgVector2 /= OsgVector2
//	  【加減乗除】
//	  ・OsgVector2 + OsgVector2
//	  ・OsgVector2 - OsgVector2
//	  ・OsgVector2 * OsgVector2	内積
//	  ・OsgVector2 * double
//	  ・double * OsgVector2
//	  ・OsgVector2 / double
//	  ・OsgVector2 & OsgVector2	外積
//	  【変換】
//	  ・(POINT)OsgVector2		POINT構造体への変換
//	■操作関数
//	  ・abs( OsgVector2& )
//	  ・Angle( OsgVector2& )	本クラスを中心点とする角度
//	  ・Rotate( double )		原点を基準とした回転
//	  ・IsEffective()		有効座標値のチェック
//	  ・Scale( OsgVector2& )	拡縮
//	  ・CalcUnit()			単位ベクトルの取得
//
//-----------------------------------------------------------------------------

class	OsgVector2
{
//-----------------------------------------------------------------------------
//	メンバ変数
//-----------------------------------------------------------------------------
public:
	float		v[2] ;		// ベクトル成分(x,y)

public:
//-----------------------------------------------------------------------------
//	コンストラクタ
//-----------------------------------------------------------------------------
	inline	OsgVector2(){
	 	v[0] = v[1] = 0.0f ;
	}

	inline	OsgVector2(float p){
	 	v[0] = p ;
	 	v[1] = 0.0f ;
	}

	inline	OsgVector2(const float p[2]){
	 	v[0] = p[0] ; v[1] = p[1] ;
	}

	inline	OsgVector2(const float x, const float y){
	 	v[0] = x ; v[1] = y ;
	}

//-----------------------------------------------------------------------------
//	デストラクタ
//-----------------------------------------------------------------------------
	virtual	inline	~OsgVector2(){ }

//-----------------------------------------------------------------------------
//	オペレータ演算子
//-----------------------------------------------------------------------------
//	OsgVector2( int )
	inline	float&	operator () (int i){
		return	v[i];
	}

//	+OsgVector2
	inline	OsgVector2&	operator + (){
		return	*this ;
	}

//	-OsgVector2
	inline	OsgVector2	operator - (){
		return	( OsgVector2(-v[0],-v[1]) ) ;
	}

//	OsgVector2 = OsgVector2
	inline	OsgVector2&	operator = ( OsgVector2& w ){
		v[0] = w.v[0] ; v[1] = w.v[1] ;
		return	*this ;
	}

//	OsgVector2 << double[2]
	inline	OsgVector2&	operator << (float* d){
		v[0] = d[0] ; v[1] = d[1] ;
		return	*this ;
	}

//	OsgVector2 >> double[2]
	inline	double*	operator >> (float* d){
		d[0] = v[0] ; d[1] = v[1] ;
		return	d ;
	}

//	double[2] << OsgVector2
	inline	friend	float*	operator << (float* d, OsgVector2& w){
		d[0] = w.v[0] ; d[1] = w.v[1] ;
		return	d ;
	}

//	double[2] >> OsgVector2
	inline	friend	OsgVector2&
		operator >> (float* d, OsgVector2& w){
		w.v[0] = d[0] ; w.v[1] = d[1] ;
		return	w ;
	}

//	OsgVector2 == OsgVector2
	inline	bool operator == ( const OsgVector2& p ){
		OsgVector2	w( v[0]-p.v[0], v[1]-p.v[1] ) ;
		return	( sqrt(w.v[0]*w.v[0] + w.v[1]*w.v[1]) <= OSG_EPS ) ;
	}

//	OsgVector2 != OsgVector2
	inline	bool	operator != ( const OsgVector2 &w ){
		return	!( *this == w ) ;
	}

//	OsgVector2 += OsgVector2
	inline	OsgVector2& operator += ( const OsgVector2& p ){
		v[0] += p.v[0] ;
		v[1] += p.v[1] ;
		return	*this ;
	}

//	OsgVector2 -= OsgVector2
	inline	OsgVector2& operator -= ( const OsgVector2& p ){
		v[0] -= p.v[0] ;
		v[1] -= p.v[1] ;
		return	*this ;
	}

//	OsgVector2 *= double
	inline	OsgVector2& operator *= (const float p){
		v[0] *= p ;
		v[1] *= p ;
		return	*this ;
	}

//	OsgVector2 /= double
	inline	OsgVector2& operator /= (const float p){
		if( fabs(p) <= OSG_DBL_EPS ) return *this ;
		v[0] /= p ;
		v[1] /= p ;
		return	*this ;
	}

//	OsgVector2 + OsgVector2
	inline	OsgVector2	operator + ( const OsgVector2& p ){
		return	OsgVector2( v[0]+p.v[0], v[1]+p.v[1] ) ;
	}

//	OsgVector2 - OsgVector2
	inline	OsgVector2	operator - ( const OsgVector2& p ){
		return	OsgVector2( v[0]-p.v[0], v[1]-p.v[1] ) ;
	}

//	OsgVector2 * OsgVector2	内積
	inline	float operator * (const OsgVector2& p){
		return	( v[0]*p.v[0] + v[1]*p.v[1] ) ;
	}

//	OsgVector2 * double
	inline	OsgVector2	operator * (const float p){
		return	OsgVector2( v[0]*p, v[1]*p ) ;
	}

//	double * OsgVector2
	inline	friend	OsgVector2
		operator * (const float p, const OsgVector2& q){
		return	OsgVector2( p*q.v[0], p*q.v[1] ) ;
	}

//	OsgVector2 / double
	inline	OsgVector2	operator / (const float q){
		if( fabs(q) <= OSG_DBL_EPS ){
			return	OsgVector2( v[0], v[1] ) ;
		}else{
			return	OsgVector2( v[0]/q, v[1]/q ) ;
		}
	}

//	OsgVector2 & OsgVector2	外積
	inline	float	operator & (const OsgVector2& p){
		return	( v[0]*p.v[1] - v[1]*p.v[0] ) ;
	}

//	(POINT)OsgVector2
	inline	operator POINT ( void ) const {
		POINT	data = { (long)v[0], (long)v[1] } ;
		return	data ;
	}

//-----------------------------------------------------------------------------
//	操作関数
//-----------------------------------------------------------------------------
//	abs( OsgVector2& )
	inline	friend float	abs(const OsgVector2& p){
		return	sqrt( p.v[0]*p.v[0]+p.v[1]*p.v[1] ) ;
	}

//	本クラスを中心点とする角度の取得
	inline	float	Angle(OsgVector2& pos){
		float	rad = atan2(pos(1) - v[1], pos(0) - v[0]);
		if( rad < 0 ) rad += OSG_PAI_2 ;
		return	rad ;
	}

//	原点を基準とした回転
	inline	void	Rotate( const double angle ){
		double	x = v[0]*cos(angle) - v[1]*sin(angle) ;
		double	y = v[0]*sin(angle) + v[1]*cos(angle) ;
		v[0] = x ; v[1] = y ;
	}

//	有効座標値のチェック
	inline	bool	IsEffective(){
		if( (v[0] < OSG_MIN) || (v[0] > OSG_MAX) ||
			(v[1] < OSG_MIN) || (v[1] > OSG_MAX) ) return false ;
		return	true ;
	}

//	拡縮
	inline	OsgVector2&	Scale( OsgVector2& s ){
		v[0] *= s.v[0] ;
		v[1] *= s.v[1] ;
		return	*this ;
	}

//	単位ベクトルの取得
	inline	OsgVector2	CalcUnit(){
		double	val = abs(*this) ;
		if( val < OSG_EPS ) return OsgVector2(*this) ;
		OsgVector2	unit_vec ;
		unit_vec(0) = v[0] / val ;
		unit_vec(1) = v[1] / val ;
		return	unit_vec ;
	}
//------------------------------------------------------------------ ADD(S) 001
//	OsgVector2 << MX3_POS_2D 
	inline	 OsgVector2& operator << ( MX3_POS_2D pos ){
		v[0] = pos.x ; v[1] = pos.y ;
		return *this ;
	}

//	OsgVector2 >> MX3_POS_2D 
	inline	 MX3_POS_2D& operator >> ( MX3_POS_2D& pos ){
		pos.x = v[0] ; pos.y = v[1] ;
		return pos ;
	}

//	MX3_POS_2D << OsgVector2 
	inline	friend MX3_POS_2D& operator << ( MX3_POS_2D& mp, OsgVector2& vp ){
		mp.x = vp.v[0] ; mp.y = vp.v[1] ;
		return mp ;
	}

//	MX3_POS_2D >> OsgVector2 
	inline	friend OsgVector2& operator >> ( MX3_POS_2D& mp, OsgVector2& vp ){
		vp.v[0] = mp.x ; vp.v[1] = mp.y ;
		return vp ;
	}

//	OsgVector2 << MX3_VEC_2D 
	inline	 OsgVector2& operator << ( MX3_VEC_2D pos ){
		v[0] = pos.x ; v[1] = pos.y ;
		return *this ;
	}

//	OsgVector2 >> MX3_VEC_2D 
	inline	 MX3_VEC_2D& operator >> ( MX3_VEC_2D& pos ){
		pos.x = v[0] ; pos.y = v[1] ;
		return pos ;
	}

//	MX3_VEC_2D << OsgVector2 
	inline	friend MX3_VEC_2D& operator << ( MX3_VEC_2D& mp, OsgVector2& vp ){
		mp.x = vp.v[0] ; mp.y = vp.v[1] ;
		return mp ;
	}

//	MX3_VEC_2D >> OsgVector2 
	inline	friend OsgVector2& operator >> ( MX3_VEC_2D& mp, OsgVector2& vp ){
		vp.v[0] = mp.x ; vp.v[1] = mp.y ;
		return vp ;
	}

//	２点間距離
	inline	double	Distance( OsgVector2 p ){
		double dx = v[0] - p(0) ;
		double dy = v[1] - p(1) ;

		dx *= dx ;
		dy *= dy ;

		return sqrt( dx + dy ) ;
	}

//	データ設定
	inline	void	Set(const float x, const float y){
		v[0] = x ;
		v[1] = y ;
	}

//	ベクトルがゼロかチェックする
	inline	bool	IsZeroVector(){
		float dx = v[0] * v[0];
		float dy = v[1] * v[1];

		float ds = sqrt(dx + dy);
		if( ds < OSG_EPS ) { return true ; }
		
		return false ;
	}
//------------------------------------------------------------------ ADD(E) 001

//------------------------------------------------------------------ ADD(S) 002
	//	２点間の中心点を求める
	inline	OsgVector2 CalcMidVec( OsgVector2 p1, OsgVector2 p2 ){
		*this =  p1 + ( p2 - p1 ) / 2.0 ;
		return *this ;
	}
//------------------------------------------------------------------ ADD(E) 002
} ;

#endif

