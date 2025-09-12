
function v3_dot(a, b) { return (a[0]*b[0]) + (a[1]*b[1]) + (a[2]*b[2]); }

function v3_length2(a) { return (a[0]*a[0]) + (a[1]*a[1]) + (a[2]*a[2]); }

function v3_length(a) { return Math.sqrt((a[0]*a[0])+(a[1]*a[1])+(a[2]*a[2])); }

function v3_add_skalar(a, b, o) { o = o || new Float32Array(3);
   o[0] = a[0] + b;        
   o[1] = a[1] + b;
   o[2] = a[2] + b;
   return o;
}

function v3_mul_skalar(a, b, o) { o = o || new Float32Array(3);
   o[0] = a[0] * b;        
   o[1] = a[1] * b;         
   o[2] = a[2] * b;
   return o;
}

function v3_add(a, b, o) { o = o || new Float32Array(3);
   o[0] = a[0] + b[0];     
   o[1] = a[1] + b[1];     
   o[2] = a[2] + b[2];
   return o;
}
/*
// sub(a,b) = b - a
function v3_sub(a, b, o) { o = o || new Float32Array(3);
   o[0] = b[0] - a[0];     
   o[1] = b[1] - a[1];
   o[2] = b[2] - a[2];
   return o;
}
*/

// sub(a,b) = b - a
function v3_sub(a, b, dst) 
{
   dst = dst || new Float32Array(3);
   dst[0] = a[0] + b[0];
   dst[1] = a[1] + b[1];
   dst[2] = a[2] + b[2];
   return dst;
}

function v3_normalize( v, o ) { o = o || new Float32Array(3);
   var len = v3_length( v );
   if (len < 0.000001) { len = 0.000001; } // prevent divide by 0.
   o[0] = v[0] / len;
   o[1] = v[1] / len;
   o[2] = v[2] / len;
   return o;
}

function v3_cross(a, b, o) { o = o || new Float32Array(3);
   o[0] = a[1] * b[2] - a[2] * b[1]; // AyBz - AzBy
   o[1] = a[2] * b[0] - a[0] * b[2]; // AzBx - AxBz
   o[2] = a[0] * b[1] - a[1] * b[0]; // AxBy - AyBx
   return o;
}

function m4_mul_m4( a, b, o ) { o = o || new Float32Array(16);
   var b00 = b[0]; var b01 = b[1]; var b02 = b[2]; var b03 = b[3];
   var b10 = b[4]; var b11 = b[5]; var b12 = b[6]; var b13 = b[7];
   var b20 = b[8]; var b21 = b[9]; var b22 = b[10];var b23 = b[11];
   var b30 = b[12];var b31 = b[13];var b32 = b[14];var b33 = b[15];
   var a00 = a[0]; var a01 = a[1]; var a02 = a[2]; var a03 = a[3];
   var a10 = a[4]; var a11 = a[5]; var a12 = a[6]; var a13 = a[7];
   var a20 = a[8]; var a21 = a[9]; var a22 = a[10];var a23 = a[11];
   var a30 = a[12];var a31 = a[13];var a32 = a[14];var a33 = a[15];
   o[ 0] = b00 * a00 + b01 * a10 + b02 * a20 + b03 * a30;
   o[ 1] = b00 * a01 + b01 * a11 + b02 * a21 + b03 * a31;
   o[ 2] = b00 * a02 + b01 * a12 + b02 * a22 + b03 * a32;
   o[ 3] = b00 * a03 + b01 * a13 + b02 * a23 + b03 * a33;
   o[ 4] = b10 * a00 + b11 * a10 + b12 * a20 + b13 * a30;
   o[ 5] = b10 * a01 + b11 * a11 + b12 * a21 + b13 * a31;
   o[ 6] = b10 * a02 + b11 * a12 + b12 * a22 + b13 * a32;
   o[ 7] = b10 * a03 + b11 * a13 + b12 * a23 + b13 * a33;
   o[ 8] = b20 * a00 + b21 * a10 + b22 * a20 + b23 * a30;
   o[ 9] = b20 * a01 + b21 * a11 + b22 * a21 + b23 * a31;
   o[10] = b20 * a02 + b21 * a12 + b22 * a22 + b23 * a32;
   o[11] = b20 * a03 + b21 * a13 + b22 * a23 + b23 * a33;
   o[12] = b30 * a00 + b31 * a10 + b32 * a20 + b33 * a30;
   o[13] = b30 * a01 + b31 * a11 + b32 * a21 + b33 * a31;
   o[14] = b30 * a02 + b31 * a12 + b32 * a22 + b33 * a32;
   o[15] = b30 * a03 + b31 * a13 + b32 * a23 + b33 * a33;
   return o;
}

function m4_mul_v4( m, v, o ) { o = o || new Float32Array(4);
   for (var j = 0; j < 4; ++j) 
   {
      o[j] = 0.0;
      for (var i = 0; i < 4; ++i) { o[j] += v[i] * m[i * 4 + j]; }
   }
}

function m4_mul_v3( m, v ) {  return m4_mul_v4(m,[v[0],v[1],v[2],1]);  }

function m4_translate( x, y, z ) { o = new Float32Array(16);
   o[0] = 1;   o[1] = 0;   o[2] = 0;   o[3] = 0;
   o[4] = 0;   o[5] = 1;   o[6] = 0;   o[7] = 0;
   o[8] = 0;   o[9] = 0;   o[10] = 1;  o[11] = 0;
   o[12] = x;  o[13] = y;  o[14] = z;  o[15] = 1;
   return o;
}

function m4_identity() { o = new Float32Array(16);
   o[0] = 1;  o[1] = 0;    o[2] = 0;   o[3] = 0;
   o[4] = 0;  o[5] = 1;    o[6] = 0;   o[7] = 0;
   o[8] = 0;  o[9] = 0;    o[10] = 1;  o[11] = 0;
   o[12] = 0; o[13] = 0;   o[14] = 0;  o[15] = 1;
   return o;
}

function m4_transpose(m) { o = new Float32Array(16);
   o[ 0] = m[0];   o[ 1] = m[4];   o[ 2] = m[8];   o[ 3] = m[12];
   o[ 4] = m[1];   o[ 5] = m[5];   o[ 6] = m[9];   o[ 7] = m[13];
   o[ 8] = m[2];   o[ 9] = m[6];   o[10] = m[10];  o[11] = m[14];
   o[12] = m[3];   o[13] = m[7];   o[14] = m[11];  o[15] = m[15];
   return o;
}

/*
function m4_inverse(m) { o = new Float32Array(16);
   var m0 = m[0];    var m1 = m[1];    var m2 = m[2];    var m3 = m[3];
   var m4 = m[4];    var m5 = m[5];    var m6 = m[6];    var m7 = m[7];
   var m8 = m[8];    var m9 = m[9];    var mA = m[10];   var mB = m[11];
   var mC = m[12];   var mD = m[13];   var mE = m[14];   var mF = m[15];
   var t0 = mA * mF; var t1 = mE * mB; var t2 = m6 * mF; var t3 = mE * m7;
   var t4 = m6 * mB; var t5 = mA * m7; var t6 = m2 * mF; var t7 = mE * m3;
   var t8 = m2 * mB; var t9 = mA * m3; var tA = m2 * m7; var tB = m6 * m3;
   var tC = m8 * mD; var tD = mC * m9; var tE = m4 * mD; var tF = mC * m5;
   var tG = m4 * m9; var tH = m8 * m5; var tI = m0 * mD; var tJ = mC * m1;
   var tK = m0 * m9; var tL = m8 * m1; var tM = m0 * m5; var tN = m4 * m1;
   var t0 = (t0 * m5 + t3 * m9 + t4 * mD) - (t1 * m5 + t2 * m9 + t5 * mD);
   var t1 = (t1 * m1 + t6 * m9 + t9 * mD) - (t0 * m1 + t7 * m9 + t8 * mD);
   var t2 = (t2 * m1 + t7 * m5 + tA * mD) - (t3 * m1 + t6 * m5 + tB * mD);
   var t3 = (t5 * m1 + t8 * m5 + tB * m9) - (t4 * m1 + t9 * m5 + tA * m9);
   var d = 1.0 / (m0 * t0 + m4 * t1 + m8 * t2 + mC * t3);
   o[ 0] = d * t0;
   o[ 1] = d * t1;
   o[ 2] = d * t2;
   o[ 3] = d * t3;
   o[ 4] = d * ((t1 * m4 + t2 * m8 + t5 * mC) - (t0 * m4 + t3 * m8 + t4 * mC));
   o[ 5] = d * ((t0 * m0 + t7 * m8 + t8 * mC) - (t1 * m0 + t6 * m8 + t9 * mC));
   o[ 6] = d * ((t3 * m0 + t6 * m4 + tB * mC) - (t2 * m0 + t7 * m4 + tA * mC));
   o[ 7] = d * ((t4 * m0 + t9 * m4 + tA * m8) - (t5 * m0 + t8 * m4 + tB * m8));
   o[ 8] = d * ((tC * m7 + tF * mB + tG * mF) - (tD * m7 + tE * mB + tH * mF));
   o[ 9] = d * ((tD * m3 + tI * mB + tL * mF) - (tC * m3 + tJ * mB + tK * mF));
   o[10] = d * ((tE * m3 + tJ * m7 + tM * mF) - (tF * m3 + tI * m7 + tN * mF));
   o[11] = d * ((tH * m3 + tK * m7 + tN * mB) - (tG * m3 + tL * m7 + tM * mB));
   o[12] = d * ((tE * mA + tH * mE + tD * m6) - (tG * mE + tC * m6 + tF * mA));
   o[13] = d * ((tK * mE + tC * m2 + tJ * mA) - (tI * mA + tL * mE + tD * m2));
   o[14] = d * ((tI * m6 + tN * mE + tF * m2) - (tM * mE + tE * m2 + tJ * m6));
   o[15] = d * ((tM * mA + tG * m2 + tL * m6) - (tK * m6 + tN * mA + tH * m2));
   return o;
}
*/

function m4_inverse(m) 
{
   dst = new Float32Array(16);
   var m00 = m[0 * 4 + 0];
   var m01 = m[0 * 4 + 1];
   var m02 = m[0 * 4 + 2];
   var m03 = m[0 * 4 + 3];
   var m10 = m[1 * 4 + 0];
   var m11 = m[1 * 4 + 1];
   var m12 = m[1 * 4 + 2];
   var m13 = m[1 * 4 + 3];
   var m20 = m[2 * 4 + 0];
   var m21 = m[2 * 4 + 1];
   var m22 = m[2 * 4 + 2];
   var m23 = m[2 * 4 + 3];
   var m30 = m[3 * 4 + 0];
   var m31 = m[3 * 4 + 1];
   var m32 = m[3 * 4 + 2];
   var m33 = m[3 * 4 + 3];
   var t0  = m22 * m33;
   var t1  = m32 * m23;
   var t2  = m12 * m33;
   var t3  = m32 * m13;
   var t4  = m12 * m23;
   var t5  = m22 * m13;
   var t6  = m02 * m33;
   var t7  = m32 * m03;
   var t8  = m02 * m23;
   var t9  = m22 * m03;
   var t10 = m02 * m13;
   var t11 = m12 * m03;
   var t12 = m20 * m31;
   var t13 = m30 * m21;
   var t14 = m10 * m31;
   var t15 = m30 * m11;
   var t16 = m10 * m21;
   var t17 = m20 * m11;
   var t18 = m00 * m31;
   var t19 = m30 * m01;
   var t20 = m00 * m21;
   var t21 = m20 * m01;
   var t22 = m00 * m11;
   var t23 = m10 * m01;

   var t0 = (t0 * m11 + t3 * m21 + t4 * m31) - (t1 * m11 + t2 * m21 + t5 * m31);
   var t1 = (t1 * m01 + t6 * m21 + t9 * m31) - (t0 * m01 + t7 * m21 + t8 * m31);
   var t2 = (t2 * m01 + t7 * m11 + t10 * m31) - (t3 * m01 + t6 * m11 + t11 * m31);
   var t3 = (t5 * m01 + t8 * m11 + t11 * m21) - (t4 * m01 + t9 * m11 + t10 * m21);

   var d = 1.0 / (m00 * t0 + m10 * t1 + m20 * t2 + m30 * t3);
   dst[ 0] = d * t0;
   dst[ 1] = d * t1;
   dst[ 2] = d * t2;
   dst[ 3] = d * t3;
   dst[ 4] = d * ((t1 * m10 + t2 * m20 + t5 * m30) - (t0 * m10 + t3 * m20 + t4 * m30));
   dst[ 5] = d * ((t0 * m00 + t7 * m20 + t8 * m30) - (t1 * m00 + t6 * m20 + t9 * m30));
   dst[ 6] = d * ((t3 * m00 + t6 * m10 + t11 * m30) - (t2 * m00 + t7 * m10 + t10 * m30));
   dst[ 7] = d * ((t4 * m00 + t9 * m10 + t10 * m20) - (t5 * m00 + t8 * m10 + t11 * m20));
   dst[ 8] = d * ((t12 * m13 + t15 * m23 + t16 * m33) - (t13 * m13 + t14 * m23 + t17 * m33));
   dst[ 9] = d * ((t13 * m03 + t18 * m23 + t21 * m33) - (t12 * m03 + t19 * m23 + t20 * m33));
   dst[10] = d * ((t14 * m03 + t19 * m13 + t22 * m33) - (t15 * m03 + t18 * m13 + t23 * m33));
   dst[11] = d * ((t17 * m03 + t20 * m13 + t23 * m23) - (t16 * m03 + t21 * m13 + t22 * m23));
   dst[12] = d * ((t14 * m22 + t17 * m32 + t13 * m12) - (t16 * m32 + t12 * m12 + t15 * m22));
   dst[13] = d * ((t20 * m32 + t12 * m02 + t19 * m22) - (t18 * m22 + t21 * m32 + t13 * m02));
   dst[14] = d * ((t18 * m12 + t23 * m32 + t15 * m02) - (t22 * m32 + t14 * m02 + t19 * m12));
   dst[15] = d * ((t22 * m22 + t16 * m02 + t21 * m12) - (t20 * m12 + t23 * m22 + t17 * m02));
   return dst;
}

/*
template<typename T, qualifier Q> 
GLM_FUNC_QUALIFIER mat<4, 4, T, Q> 
lookAtRH(vec<3, T, Q> const& eye, vec<3, T, Q> const& center, vec<3, T, Q> const& up)
{
   vec<3, T, Q> const f(normalize(center - eye));
   vec<3, T, Q> const s(normalize(cross(f, up)));
   vec<3, T, Q> const u(cross(s, f));

   mat<4, 4, T, Q> Result(1);
   Result[0][0] = s.x;
   Result[1][0] = s.y;
   Result[2][0] = s.z;
   Result[0][1] = u.x;
   Result[1][1] = u.y;
   Result[2][1] = u.z;
   Result[0][2] =-f.x;
   Result[1][2] =-f.y;
   Result[2][2] =-f.z;
   Result[3][0] =-dot(s, eye);
   Result[3][1] =-dot(u, eye);
   Result[3][2] = dot(f, eye);
   return Result;
}
*/

function m4_lookAtRH(pos, target, up, o)
{
   const f = v3_normalize( v3_sub( pos, target ) );
   const s = v3_normalize( v3_cross( f, up ) );
   const u = v3_cross( s, f );

   o = o || new Float32Array(16);
   o[0] = s.x; 
   o[1] = s.y; 
   o[2] = s.z; 
   o[3] = 0; 
   o[4] = u.x; 
   o[5] = u.y; 
   o[6] = u.z; 
   o[7] = 0;
   o[8] =-f.x; 
   o[9] =-f.y; 
   o[10]=-f.z; 
   o[11]= 0;
   o[12]=-v3_dot(s, pos);
   o[13]=-v3_dot(u, pos);
   o[14]= v3_dot(f, pos);
   o[15]= 1;
   return o;
}
/*
template<typename T, qualifier Q>
GLM_FUNC_QUALIFIER mat<4, 4, T, Q> 
lookAtLH(vec<3, T, Q> const& eye, vec<3, T, Q> const& center, vec<3, T, Q> const& up)
{
   vec<3, T, Q> const f(normalize(center - eye));
   vec<3, T, Q> const s(normalize(cross(up, f)));
   vec<3, T, Q> const u(cross(f, s));

   mat<4, 4, T, Q> Result(1);
   Result[0][0] = s.x;
   Result[1][0] = s.y;
   Result[2][0] = s.z;
   Result[0][1] = u.x;
   Result[1][1] = u.y;
   Result[2][1] = u.z;
   Result[0][2] = f.x;
   Result[1][2] = f.y;
   Result[2][2] = f.z;
   Result[3][0] = -dot(s, eye);
   Result[3][1] = -dot(u, eye);
   Result[3][2] = -dot(f, eye);
   return Result;
}
*/

function m4_lookAtLH(pos, target, up, o)
{
   const f = v3_normalize( v3_sub( pos, target ) );
   const s = v3_normalize( v3_cross( up, f ) );
   const u = v3_cross( f, s );

   o = o || new Float32Array(16);
   o[0] = s[0]; 
   o[1] = s[1]; 
   o[2] = s[2]; 
   o[3] = 0; 
   o[4] = u[0]; 
   o[5] = u[1]; 
   o[6] = u[2]; 
   o[7] = 0;
   o[8] = f[0]; 
   o[9] = f[1]; 
   o[10]= f[2]; 
   o[11]= 0;
   //o[12]= -v3_dot(s, pos);
   //o[13]= -v3_dot(u, pos);
   //o[14]= -v3_dot(f, pos);
   o[12]= pos[0];
   o[13]= pos[1];
   o[14]= pos[2];
   o[15]= 1;
   return o;
}

function m4_lookAt(camPos, target, up, o) 
{
   o = o || new Float32Array(16);
   var f = v3_normalize(v3_sub(camPos,target));
   var s = v3_normalize(v3_cross(up, f));
   var u = v3_normalize(v3_cross(f, s));

   o[ 0] = s[0];
   o[ 1] = s[1];
   o[ 2] = s[2];
   o[ 3] = 0;
   o[ 4] = u[0];
   o[ 5] = u[1];
   o[ 6] = u[2];
   o[ 7] = 0;
   o[ 8] = f[0];
   o[ 9] = f[1];
   o[10] = f[2];
   o[11] = 0;
   o[12] = camPos[0];
   o[13] = camPos[1];
   o[14] = camPos[2];
   o[15] = 1;

   return o;
}
/*
	template<typename T>
	GLM_FUNC_QUALIFIER mat<4, 4, T, defaultp> ortho(T left, T right, T bottom, T top)
	{
		mat<4, 4, T, defaultp> Result(static_cast<T>(1));
		Result[0][0] = static_cast<T>(2) / (right - left);
		Result[1][1] = static_cast<T>(2) / (top - bottom);
		Result[2][2] = - static_cast<T>(1);
		Result[3][0] = - (right + left) / (right - left);
		Result[3][1] = - (top + bottom) / (top - bottom);
		return Result;
	}
*/

function m4_orthographic(left, right, bottom, top, near, far, o) 
{
   o = o || new Float32Array(16);
   o[ 0] = 2 / (right - left);
   o[ 1] = 0;
   o[ 2] = 0;
   o[ 3] = 0;
   o[ 4] = 0;
   o[ 5] = 2 / (top - bottom);
   o[ 6] = 0;
   o[ 7] = 0;
   o[ 8] = 0;
   o[ 9] = 0;
   o[10] = 2 / (near - far);
   o[11] = 0;
   o[12] = (left + right) / (left - right);
   o[13] = (bottom + top) / (bottom - top);
   o[14] = (near + far) / (near - far);
   o[15] = 1;
   return o;
}

/*
	template<typename T>	GLM_FUNC_QUALIFIER mat<4, 4, T, defaultp> 
   perspectiveFovLH_NO(T fov, T width, T height, T zNear, T zFar)
	{
		assert(width > static_cast<T>(0));
		assert(height > static_cast<T>(0));
		assert(fov > static_cast<T>(0));

		T const rad = fov;
		T const h = glm::cos(static_cast<T>(0.5) * rad) / glm::sin(static_cast<T>(0.5) * rad);
		T const w = h * height / width; ///todo max(width , Height) / min(width , Height)?

		mat<4, 4, T, defaultp> Result(static_cast<T>(0));
		Result[0][0] = w;
		Result[1][1] = h;
		Result[2][2] = (zFar + zNear) / (zFar - zNear);
		Result[2][3] = static_cast<T>(1);
		Result[3][2] = - (static_cast<T>(2) * zFar * zNear) / (zFar - zNear);
		return Result;
	}

*/
function m4_perspectiveFovLH_NO(fieldOfViewInRadians, aspect, near, far, o) 
{
//   o = o || new Float32Array(16);
   var f = Math.tan(Math.PI * 0.5 - 0.5 * fieldOfViewInRadians);
   const rangeInv = 1.0 / (far-near);
   const rad = 0.5 * fieldOfViewInRadians;
   const h = 1.0 / Math.tan(rad);
   const w = h / aspect; ///todo max(width , Height) / min(width , Height)?

   o[ 0] = w                            //(0,0 | f / aspect
   o[ 1] = 0;                           // 1,0               
   o[ 2] = 0;                           // 2,0   
   o[ 3] = 0;                           // 3,0
   o[ 4] = 0;                           // 0,1
   o[ 5] = h;                           //(1,1)| f
   o[ 6] = 0;                           // 2,1
   o[ 7] = 0;                           // 3,1
   o[ 8] = 0;                           // 0,2
   o[ 9] = 0;                           // 1,2
   o[10] = (near + far) * rangeInv;     //(2,2)
   o[11] = 1;                           //(3,2)
   o[12] = 0;                           // 0,3
   o[13] = 0;                           // 1,3
   o[14] = -2 * near * far * rangeInv;  //(2,3)
   o[15] = 0;                           // 3,3

   return o;
}
/*
void extractDataFromMatrices()
{
   glm::mat4 const & viewMat = m_viewMat;
   glm::mat4 const & viewMatInv = glm::inverse( m_viewMat );
   //glm::vec3 camPos2 = glm::vec3(viewMatInv * glm::vec4( 0,0,0,1 ));
   glm::vec3 camPos2(viewMatInv[3][0], viewMatInv[3][1], viewMatInv[3][2]);
   //glm::vec3 camPos2( -viewMat[0][3], -viewMat[1][3], -viewMat[2][3] );
   glm::vec3 camUp2( viewMat[0][1], viewMat[1][1], viewMat[2][1] );
   glm::vec3 camRight2( viewMat[0][0], viewMat[1][0], viewMat[2][0] );
}
*/

function m4_perspective(fieldOfViewInRadians, aspect, near, far, o) 
{
   o = o || new Float32Array(16);
   var f = Math.tan(Math.PI * 0.5 - 0.5 * fieldOfViewInRadians);
   var rangeInv = 1.0 / (near - far);

   o[ 0] = f / aspect;
   o[ 1] = 0;
   o[ 2] = 0;
   o[ 3] = 0;
   o[ 4] = 0;
   o[ 5] = f;
   o[ 6] = 0;
   o[ 7] = 0;
   o[ 8] = 0;
   o[ 9] = 0;
   o[10] = (near + far) * rangeInv;
   o[11] = -1;
   o[12] = 0;
   o[13] = 0;
   o[14] = near * far * rangeInv * 2;
   o[15] = 0;

   return o;
}

/*
template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER mat<4, 4, T, Q> lookAtRH(vec<3, T, Q> const& eye, vec<3, T, Q> const& center, vec<3, T, Q> const& up)
	{
		vec<3, T, Q> const f(normalize(center - eye));
		vec<3, T, Q> const s(normalize(cross(f, up)));
		vec<3, T, Q> const u(cross(s, f));

		mat<4, 4, T, Q> Result(1);
		Result[0][0] = s.x;
		Result[1][0] = s.y;
		Result[2][0] = s.z;
		Result[0][1] = u.x;
		Result[1][1] = u.y;
		Result[2][1] = u.z;
		Result[0][2] =-f.x;
		Result[1][2] =-f.y;
		Result[2][2] =-f.z;
		Result[3][0] =-dot(s, eye);
		Result[3][1] =-dot(u, eye);
		Result[3][2] = dot(f, eye);
		return Result;
	}

	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER mat<4, 4, T, Q> lookAtLH(vec<3, T, Q> const& eye, vec<3, T, Q> const& center, vec<3, T, Q> const& up)
	{
		vec<3, T, Q> const f(normalize(center - eye));
		vec<3, T, Q> const s(normalize(cross(up, f)));
		vec<3, T, Q> const u(cross(f, s));

		mat<4, 4, T, Q> Result(1);
		Result[0][0] = s.x;
		Result[1][0] = s.y;
		Result[2][0] = s.z;
		Result[0][1] = u.x;
		Result[1][1] = u.y;
		Result[2][1] = u.z;
		Result[0][2] = f.x;
		Result[1][2] = f.y;
		Result[2][2] = f.z;
		Result[3][0] = -dot(s, eye);
		Result[3][1] = -dot(u, eye);
		Result[3][2] = -dot(f, eye);
		return Result;
	}

   //std::cout << "perspectiveFovLH_NO()" << std::endl;
		assert(width > static_cast<T>(0));
		assert(height > static_cast<T>(0));
		assert(fov > static_cast<T>(0));

		T const rad = fov;
		T const h = glm::cos(static_cast<T>(0.5) * rad) / glm::sin(static_cast<T>(0.5) * rad);
		T const w = h * height / width; ///todo max(width , Height) / min(width , Height)?

		mat<4, 4, T, defaultp> Result(static_cast<T>(0));
		Result[0][0] = w;
		Result[1][1] = h;
		Result[2][2] = (zFar + zNear) / (zFar - zNear);
		Result[2][3] = static_cast<T>(1);
		Result[3][2] = - (static_cast<T>(2) * zFar * zNear) / (zFar - zNear);
		return Result;

   void extractDataFromMatrices()
   {
      glm::mat4 const & viewMat = m_viewMat;
      glm::mat4 const & viewMatInv = glm::inverse( m_viewMat );
      //glm::vec3 camPos2 = glm::vec3(viewMatInv * glm::vec4( 0,0,0,1 ));
      glm::vec3 camPos2(viewMatInv[3][0], viewMatInv[3][1], viewMatInv[3][2]);
      //glm::vec3 camPos2( -viewMat[0][3], -viewMat[1][3], -viewMat[2][3] );
      glm::vec3 camUp2( viewMat[0][1], viewMat[1][1], viewMat[2][1] );
      glm::vec3 camRight2( viewMat[0][0], viewMat[1][0], viewMat[2][0] );
   }
   void updateViewMatrix()
   {
      // Also re-calculate the Right and Up vector
      Right = glm::normalize(glm::cross(Front, WorldUp));
      // Normalize the vectors, because their length gets closer to 0
      // the more you look up or down which results in slower movement.
      Up    = glm::normalize(glm::cross(Right, Front));

      m_viewMat = glm::lookAt(Position, Target, Up);
   }

   void updateProjectionMatrix()
   {
      m_projMat = glm::perspectiveFovLH(
                           glm::radians(Zoom),
                           (float)m_screenWidth, (float)m_screenHeight, .01f, 38000.0f);
   }
*/

var worldUp = [0, 1, 0];
var camPos = [100,200,100];
var camTarget = [110, 100, 140];

var camSide = [1, 0, 0];
var camUp = [0, 1, 0];
var camFront = [0, 0, 1];

var u_projMat = null;
var u_camMat = null;
var u_viewMat = null;
var u_mvp = null;

function logCamera( msg )
{
   const cx = camPos[0];
   const cy = camPos[1];
   const cz = camPos[2];
   const tx = camTarget[0];
   const ty = camTarget[1];
   const tz = camTarget[2];
   console.log(msg + "pos("+cx+","+cy+","+cz+"), eye("+tx+","+ty+","+tz+")");      
}

function initCamera()
{   
   u_camMat = new Float32Array(16);
}

function updateCamera( gl )
{   
   const w = gl.canvas.width;
   const h = gl.canvas.height;

   //console.log("updateCamera(" + w + "," + h + ")");
   u_projMat = m4_perspective(45 * (Math.PI / 180), w / h, 0.1, 3800.0);

   camFront = v3_normalize( v3_sub( camPos, camTarget ) );
   camSide = v3_normalize( v3_cross( worldUp, camFront ) );
   camUp = v3_cross( camFront, camSide );

   // u_camMat = m4_lookAtLH(camPos, camTarget, worldUp);
   u_camMat[0] = camSide[0]; 
   u_camMat[1] = camSide[1]; 
   u_camMat[2] = camSide[2]; 
   u_camMat[3] = 0; 
   u_camMat[4] = camUp[0]; 
   u_camMat[5] = camUp[1]; 
   u_camMat[6] = camUp[2]; 
   u_camMat[7] = 0;
   u_camMat[8] = camFront[0]; 
   u_camMat[9] = camFront[1]; 
   u_camMat[10]= camFront[2]; 
   u_camMat[11]= 0;
   //u_camMat[12]= -v3_dot(camSide, camPos);
   //u_camMat[13]= -v3_dot(camUp, camPos);
   //u_camMat[14]= -v3_dot(camFront, camPos);
   u_camMat[12]= camPos[0];
   u_camMat[13]= camPos[1];
   u_camMat[14]= camPos[2];
   u_camMat[15]= 1;

   u_viewMat = m4_inverse(u_camMat);

   u_mvp = m4_mul_m4(u_projMat, u_viewMat);
}

function moveCamera( speed ) 
{
   camDir = v3_normalize( v3_sub(camPos,camTarget) );
   camMoveStep = v3_mul_skalar(camDir,speed);
   camPos = v3_add(camPos, camMoveStep);
   camTarget = v3_add(camTarget, camMoveStep);
}

function strafeCamera( speed ) 
{
   camDir = v3_normalize( camSide );
   camMoveStep = v3_mul_skalar(camSide,speed);
   camPos = v3_add(camPos, camMoveStep);
   camTarget = v3_add(camTarget, camMoveStep);
}

function elevateCamera( speed ) 
{
   camDir = v3_normalize( camUp );
   camMoveStep = v3_mul_skalar(camDir,speed);
   camPos = v3_add(camPos, camMoveStep);
   camTarget = v3_add(camTarget, camMoveStep);
}



// WebGLShaderObject = compileShader( gl, "source text", gl.FRAMENT|gl.VERTEX) 

function compileShader(gl, shaderSource, shaderType) 
{
   //const errFn = opt_errorCallback || error;
   const shader = gl.createShader(shaderType);
   gl.shaderSource(shader, shaderSource);
   gl.compileShader(shader);

   const ok = gl.getShaderParameter(shader, gl.COMPILE_STATUS);
   if (!ok) 
   {
      const e = gl.getShaderInfoLog(shader);
      console.log('*** Error compiling shader \'' + shader + '\':' + e + `\n` + shaderSource.split('\n').map((l,i) => `${i + 1}: ${l}`).join('\n'));
      gl.deleteShader(shader);
      return null;
   }
   return shader;
}

// WebGLProgramObject = linkProgram( "shaderName", gl, WebGLShaderObject vs, WebGLShaderObject fs);

function linkProgram( name, gl, vs, fs) 
{
   const program = gl.createProgram();
   gl.attachShader(program, vs);
   gl.attachShader(program, fs);
   gl.linkProgram(program);

   const ok = gl.getProgramParameter(program, gl.LINK_STATUS);
   if (!ok) 
   {
      const e = gl.getProgramInfoLog(program);
      console.log('[Error] linkProgram(' + name + '): ' + e);
      gl.deleteProgram(program);
      return null;
   }
   return program;
}


function computeBoundingBox( positions )
{   
   var min_x = 1.e600;
   var min_y = 1.e600;
   var min_z = 1.e600;

   var max_x = -1.e600;
   var max_y = -1.e600;
   var max_z = -1.e600;

   const pointCount = positions.length;

   for ( var i = 0; i < pointCount/3; ++i )
   {
      const x = positions[ 3 * i + 0 ];
      const y = positions[ 3 * i + 1 ];
      const z = positions[ 3 * i + 2 ];
      min_x = Math.min( min_x, x );
      min_y = Math.min( min_y, y );
      min_z = Math.min( min_z, z );
      max_x = Math.max( max_x, x );
      max_y = Math.max( max_y, y );
      max_z = Math.max( max_z, z );
   }

   return [min_x, min_y, min_z, max_x, max_y, max_z ];
}

// =========================================================================
// First shader:
// =========================================================================

var g_UniformColor_shader = null;
var g_UniformColor_shader_u_color = -1;
var g_UniformColor_shader_u_mvp = -1;

var g_UniformColor_vbo_lines = null;

var vsUniformColor = `

   precision mediump float;
   attribute vec3       a_pos;      // in attrib layout( location = 0)  
   //attribute lowp vec4  a_color;    // in attrib layout( location = 1)  
   uniform   mat4       u_mvp;      // in 
   //varying   vec4       v_color;    // out

   void main() 
   {
      //vec4 color = a_color;                  
      //v_color = color / 255.0;              // out
      gl_Position = u_mvp * vec4(a_pos,1.0); // out
   }
`;

var fsUniformColor = `

   precision mediump float;
   //varying  vec4     v_color;      // in
   uniform vec4    u_color;      // in

   void main() 
   {
      gl_FragColor = u_color;       // out
   }
`;

function setUniformColor( color )
{   
	u_color = color;
}

function initShaderUniformColor( gl )
{   
   if ( !g_UniformColor_shader )
   {
      var vs = compileShader( gl, vsUniformColor, gl.VERTEX_SHADER );
      var fs = compileShader( gl, fsUniformColor, gl.FRAGMENT_SHADER );
      var shader = linkProgram( "uniformColor", gl, vs, fs );
      if ( shader )
      {
         g_UniformColor_shader = shader;
         g_UniformColor_shader_u_color = gl.getUniformLocation( shader, "u_color" ); 
         g_UniformColor_shader_u_mvp = gl.getUniformLocation( shader, "u_mvp" ); 
      }
      else
      {
         g_UniformColor_shader = null;
         g_UniformColor_shader_u_color = -1; 
         g_UniformColor_shader_u_mvp = -1; 
         console.log("Error, no g_UniformColor_shader created");
      }
   }

   if (!g_UniformColor_vbo_lines)
   {
      const vbo = gl.createBuffer();
      gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
      gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(linesPositions), gl.STATIC_DRAW);
      g_UniformColor_vbo_lines = vbo;
   }

}


function drawUniformColorLine3D( gl, x1, y1, z1, x2, y2, z2 )
{   
   if ( g_UniformColor_shader == null )
   {
      console.log("Error, no UniformColor shader for drawing.");
      return;
   }
   gl.useProgram( g_UniformColor_shader );
   gl.uniformMatrix4fv( g_UniformColor_shader_u_mvp, false, u_mvp );
   gl.uniform4fv( g_UniformColor_shader_u_color, u_color );

   const vboPos = gl.createBuffer();
   gl.bindBuffer(gl.ARRAY_BUFFER, vboPos);
   gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([x1,y1,z1,x2,y2,z2]), gl.STATIC_DRAW); // 6x 12 byte per vertex = 72 bytes;
   gl.enableVertexAttribArray(0);
   gl.vertexAttribPointer(0, 3, gl.FLOAT, gl.FALSE, 12, 0); // 12 byte per vertex.

   gl.drawArrays(gl.LINES, 0, 2); // 2x vertices = 1x line = 1x halbe hahn.
   gl.disableVertexAttribArray(0);
   gl.bindBuffer(gl.ARRAY_BUFFER, null);

   gl.deleteBuffer( vboPos );
}


function drawUniformColorLine3D( gl, A, B )
{   
   if ( g_UniformColor_shader == null )
   {
      console.log("Error, no UniformColor shader for drawing.");
      return;
   }
   gl.useProgram( g_UniformColor_shader );
   gl.uniformMatrix4fv( g_UniformColor_shader_u_mvp, false, u_mvp );
   gl.uniform4fv( g_UniformColor_shader_u_color, u_color );

   const x1 = A[0];
   const y1 = A[1];
   const z1 = A[2];
   const x2 = B[0];
   const y2 = B[1];
   const z2 = B[2];

   const vboPos = gl.createBuffer();
   gl.bindBuffer(gl.ARRAY_BUFFER, vboPos);
   gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([x1,y1,z1,x2,y2,z2]), gl.STATIC_DRAW); // 6x 12 byte per vertex = 72 bytes;
   gl.enableVertexAttribArray(0);
   gl.vertexAttribPointer(0, 3, gl.FLOAT, gl.FALSE, 12, 0); // 12 byte per vertex.

   gl.drawArrays(gl.LINES, 0, 2); // 2x vertices = 1x line = 1x halbe hahn.
   gl.disableVertexAttribArray(0);
   gl.bindBuffer(gl.ARRAY_BUFFER, null);

   gl.deleteBuffer( vboPos );
}


function drawUniformColorLineBox3D( gl, bbox )
{   
   const x1 = bbox[0];
   const y1 = bbox[1];
   const z1 = bbox[2];
   const x2 = bbox[3];
   const y2 = bbox[4];
   const z2 = bbox[5];
   const A = [x1,y1,z1];
   const B = [x1,y2,z1];
   const C = [x2,y2,z1];
   const D = [x2,y1,z1];
   const E = [x1,y1,z2];
   const F = [x1,y2,z2];
   const G = [x2,y2,z2];
   const H = [x2,y1,z2];
   drawUniformColorLine3D( gl, A, B );
   drawUniformColorLine3D( gl, B, C );
   drawUniformColorLine3D( gl, C, D );
   drawUniformColorLine3D( gl, D, A );

   drawUniformColorLine3D( gl, A, E );
   drawUniformColorLine3D( gl, B, F );
   drawUniformColorLine3D( gl, C, G );
   drawUniformColorLine3D( gl, D, H );

   drawUniformColorLine3D( gl, E, F );
   drawUniformColorLine3D( gl, F, G );
   drawUniformColorLine3D( gl, G, H );
   drawUniformColorLine3D( gl, H, E );
}

function drawUniformColorLines3D( gl )
{   
   if ( g_UniformColor_shader == null )
   {
      console.log("Error, no UniformColor shader for drawing.");
      return;
   }

   if ( g_UniformColor_vbo_lines == null )
   {
      console.log("Error, no UniformColor vbo for drawing.");
      return;
   }

   gl.useProgram        ( g_UniformColor_shader );
   gl.uniform4fv        ( g_UniformColor_shader_u_color, u_color );
   gl.uniformMatrix4fv  ( g_UniformColor_shader_u_mvp, false, u_mvp );

   gl.bindBuffer(gl.ARRAY_BUFFER, g_UniformColor_vbo_lines);
   gl.enableVertexAttribArray(0);
   gl.vertexAttribPointer(0, 3, gl.FLOAT, gl.FALSE, 12, 0); // 3x float = 12 byte per vertex.

   gl.drawArrays(gl.LINES, 0, linesCount);
   gl.disableVertexAttribArray(1);
   gl.disableVertexAttribArray(0);
   gl.bindBuffer(gl.ARRAY_BUFFER, null);
}

// =========================================================================
// Second shader:
// =========================================================================
/*
var g_AttributeColor_shader = null;
var g_AttributeColor_shader_u_mvp = -1;

var g_AttributeColor_vboLineStripPositions = null;
var g_AttributeColor_vboLineStripColors = null;

var vsAttributeColor = `

   precision mediump float;
   attribute vec3       a_pos;      // in attrib layout( location = 0)  
   attribute lowp vec4  a_color;    // in attrib layout( location = 1)  
   uniform   mat4       u_mvp;      // in 
   varying   vec4       v_color;    // out

   void main() 
   {
      vec4 color = a_color;                  
      v_color = color / 255.0;              // out
      gl_Position = u_mvp * vec4(a_pos,1.0); // out
   }
`;

var fsAttributeColor = `

   precision mediump float;
   varying  vec4     v_color;      // in
   //uniform vec4    u_color;      // in

   void main() 
   {
      gl_FragColor = v_color;       // out
   }
`;

function initShaderAttributeColor( gl )
{   
   if ( !g_AttributeColor_shader )
   {
      var vs = compileShader( gl, vsAttributeColor, gl.VERTEX_SHADER );
      var fs = compileShader( gl, fsAttributeColor, gl.FRAGMENT_SHADER );
      var shader = linkProgram( "AttributeColor", gl, vs, fs );
      if ( shader )
      {
         g_AttributeColor_shader = shader;
         g_AttributeColor_shader_u_mvp = gl.getUniformLocation( shader, "u_mvp" ); 
         console.log("Created AttributeColor shader");
      }
      else
      {
         g_AttributeColor_shader = null;
         g_AttributeColor_shader_u_mvp = -1; 
         console.log("Error, no AttributeColor shader");
      }
   }
}


function drawAttributeColorLine3D( gl, x1, y1, z1, x2, y2, z2, colorA, colorB, u_mvp )
{   
   if ( g_AttributeColor_shader == null )
   {
      console.log("Error, no shader ColorLine3D for drawing.");
      return;
   }
   gl.useProgram( g_AttributeColor_shader );
   gl.uniformMatrix4fv( g_AttributeColor_shader, false, u_mvp );

   const vboPos = gl.createBuffer();
   gl.bindBuffer(gl.ARRAY_BUFFER, vboPos);
   gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([x1,y1,z1,x2,y2,z2]), gl.STATIC_DRAW); // 6x 12 byte per vertex = 72 bytes;
   gl.enableVertexAttribArray(0);
   gl.vertexAttribPointer(0, 3, gl.FLOAT, gl.FALSE, 12, 0); // 12 byte per vertex.
   const vboColor = gl.createBuffer();
   gl.bindBuffer(gl.ARRAY_BUFFER, vboColor);
   gl.bufferData(gl.ARRAY_BUFFER, new Uint32Array([colorA,colorB]), gl.STATIC_DRAW); // 6x 12 byte per vertex = 72 bytes;
   gl.enableVertexAttribArray(1);
   gl.vertexAttribPointer(1, 4, gl.UNSIGNED_BYTE, gl.FALSE, 4, 0); // 4 byte per vertex.
   gl.drawArrays(gl.LINES, 0, 2); // 2 vertex/halbeHahn per line
   gl.disableVertexAttribArray(1);
   gl.disableVertexAttribArray(0);
   gl.bindBuffer(gl.ARRAY_BUFFER, null);
   gl.deleteBuffer( vboColor );
   gl.deleteBuffer( vboPos );
}


function drawAttributeColorLineStrip( gl, u_mvp )
{   
   if ( g_AttributeColor_shader == null )
   {
      console.log("Error, no AttributeColor shader for drawing.");
      return;
   }
   gl.useProgram( g_AttributeColor_shader );
   gl.uniformMatrix4fv( g_AttributeColor_shader_u_mvp, false, u_mvp );

   if (!g_AttributeColor_vboLineStripPositions)
   {
      const vbo = gl.createBuffer();
      gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
      gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(lineStripPositions), gl.STATIC_DRAW);
      g_AttributeColor_vboLineStripPositions = vbo;
   }
   else
   {
      gl.bindBuffer(gl.ARRAY_BUFFER, g_AttributeColor_vboLineStripPositions);
   }

   gl.enableVertexAttribArray(0);
   gl.vertexAttribPointer(0, 3, gl.FLOAT, gl.FALSE, 12, 0); // 3x float = 12 byte per vertex.

   if (!g_AttributeColor_vboLineStripColors)
   {
      const vbo = gl.createBuffer();
      gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
      gl.bufferData(gl.ARRAY_BUFFER, new Uint32Array(lineStripColors), gl.STATIC_DRAW);
      g_AttributeColor_vboLineStripColors = vbo;
   }
   else
   {
      gl.bindBuffer(gl.ARRAY_BUFFER, !g_AttributeColor_vboLineStripColors);
   }
   gl.enableVertexAttribArray(1);
   gl.vertexAttribPointer(1, 4, gl.UNSIGNED_BYTE, gl.FALSE, 4, 0); // 4x u8 RGBA

   gl.drawArrays(gl.LINES, 0, lineStripElementCount);
   gl.disableVertexAttribArray(1);
   gl.disableVertexAttribArray(0);
   gl.bindBuffer(gl.ARRAY_BUFFER, null);
   //gl.deleteBuffer( vboLineStripPositions );
}
*/
// =========================================================================
// Main loop: and event callbacks for mouse,keyboard,touch,etc...
// =========================================================================

var dragStart = [0,0];
var isDragging = true;

function main() 
{
   // Get A WebGL context
   // Either we have a canvas or container we insert a canvas inside.
   // If we don't find a container we use the body of the document.
   //var canvas = document.querySelector("#canvas");
   var canvas = document.getElementById('canvas');
   if (!canvas) 
   {
      console.log('Failed to retrieve the <canvas> element');
      return;
   }

   var isCanvas = (canvas instanceof HTMLCanvasElement);
   if (!isCanvas) 
   {
      console.log('canvas is not HTMLCanvasElement');
   }

   var gl = canvas.getContext("webgl", { alpha: false });
   if (!gl) 
   {
      console.log('No WebGL context found in HTMLCanvasElement!');
      return;
   }

   const gw = gl.canvas.width;
   const gh = gl.canvas.height;
   const w = gl.canvas.clientWidth;
   const h = gl.canvas.clientHeight;
   console.log("setupGL() :: w("+w+"), h("+h+"), gw("+gw+"), gh("+gh+")");

   initCamera();
   initShaderUniformColor( gl );
   //initShaderAttributeColor( gl );

   var bbox = computeBoundingBox( linesPositions );

   var then = 0;
   function render(time) 
   {
      var now = time * 0.001;
      var deltaTime = Math.min(0.1, now - then);
      then = now;

      // Resize <canvas> to <gl.canvas>
      if ( canvas.width !== gl.canvas.clientWidth )
      {
         //console.log("Set canvas width to " + canvas.width + " from " + gl.canvas.clientWidth );
         canvas.width = gl.canvas.clientWidth;
      }
      if ( canvas.height !== gl.canvas.clientHeight )
      {
         //console.log("Set canvas height to " + canvas.height + " from " + gl.canvas.clientHeight );
         canvas.height = gl.canvas.clientHeight;        
      }

      var w = gl.canvas.clientWidth;
      var h = gl.canvas.clientHeight;
      //var w = gl.canvas.width;
      //var h = gl.canvas.height;
      //console.log("render :: w(" + w + "), h(" + h + "), cw(" + cw + "), ch(" + ch + ")"); // (" + time + "), 

      // Clear screen
      gl.viewport(0,0,w,h);
      gl.clearColor(0.15,0.15,0.15,1);
      gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

      gl.enable(gl.CULL_FACE);
      gl.enable(gl.DEPTH_TEST);

      gl.enable(gl.BLEND);
      gl.blendEquation(gl.FUNC_ADD);
      gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA );

      updateCamera( gl );

      setUniformColor([1,1,1,0.01]);
      drawUniformColorLines3D( gl );

      gl.disable(gl.BLEND);
      setUniformColor([1,1,1,1]);
      drawUniformColorLineBox3D( gl, bbox );

      setUniformColor([1,0,0,1]);
      drawUniformColorLine3D( gl, [0,0,0], [100,0,0] );

      setUniformColor([0,1,0,1]);
      drawUniformColorLine3D( gl, [0,0,0], [0,100,0] );

      setUniformColor([0,0,1,1]);
      drawUniformColorLine3D( gl, [0,0,0], [0,0,100] );


      //update(deltaTime);
      //draw();

      requestAnimationFrame(render);
   }

   function onResizeObserver( event ) 
   {
      const w = event.width;
      const h = event.height;
      console.log("onResizeObserver(" + w + "," + h + ")");
      requestAnimationFrame(render);
   }

   const observer = new ResizeObserver(onResizeObserver);
   observer.observe(gl.canvas);

   requestAnimationFrame(render);

   function onMouseDown( event ) 
   {
      //event.preventDefault();
      const b = event.button;
      const x = event.clientX;
      const y = event.clientY;
      //console.log("onMouseDown(" + x + "," + y + "), Button(" + b + ")");
      if ( b == 0 )
      {
         if (!isDragging)
         {
            dragStart[0] = x;
            dragStart[1] = y;
            isDragging = true;
            console.log("dragStart(" + dragStart[0] + "," + dragStart[1] + ")");      
         }
      }
   }

   function onMouseUp( event )  
   {
      const b = event.button;      
      const x = event.clientX;
      const y = event.clientY;
      //console.log("onMouseUp(" + x + "," + y + "), Button(" + b + ")");
      if ( b == 0 )
      {
         if (isDragging)
         {
            var dx = x - dragStart[0];
            var dy = y - dragStart[1];
            console.log("dragEnd(" + x + "," + y + "), dragDelta(" + dx + "," + dy + ")");      
            isDragging = false;
            logCamera("Before: ");
            if ( dx != 0 ) strafeCamera( dx );
            if ( dy != 0 ) elevateCamera( dy );
            <!-- camPos[0] += dx; -->
            <!-- camPos[1] -= dy; -->
            <!-- camTarget[0] += dx; -->
            <!-- camTarget[1] -= dy; -->
            logCamera("After: ");
         }
      }

   }

   function onMouseClick( event )   
   {
      const x = event.clientX;
      const y = event.clientY;
      //console.log("onMouseClick(" + x + "," + y + ")");
   }

   function onMouseDoubleClick( event )   
   {
      const x = event.clientX;
      const y = event.clientY;
      //console.log("onMouseDoubleClick(" + x + "," + y + ")");
   }

   function onMouseMove( event ) 
   {
      const x = event.clientX;
      const y = event.clientY;
      //console.log("onMouseMove(" + x + "," + y + ")");
   }

   function onMouseWheel( event ) 
   {
      const d = event.deltaY > 0 ? -1 : 1;
      const x = event.clientX;
      const y = event.clientY;
      var s = "UP";
      if (d < 0) s = "DOWN";
      console.log("onMouseWheel(" + x + "," + y + "), Direction(" + s + ")");
      moveCamera( 25*d );
   }

   function onScroll( event ) 
   {
      const d = event.deltaY > 0 ? -1 : 1;
      const x = event.clientX;
      const y = event.clientY;
      var s = "UP";
      if (d < 0) s = "DOWN";
      console.log("onScroll(" + x + "," + y + "), Direction(" + s + ")");
   }

   function onChange( event ) 
   {
      const x = event.clientX;
      const y = event.clientY;
      console.log("onChange(" + x + "," + y + ")");
   }

   function onPointerDown( event ) 
   {
      const x = event.clientX;
      const y = event.clientY;
      //console.log("onPointerDown(" + x + "," + y + ")");
   }

   function onTouchStart( event ) 
   {
      const x = event.clientX;
      const y = event.clientY;
      console.log("onTouchStart(" + x + "," + y + ")");
   }

   function onTouchMove( event ) 
   {
      const x = event.clientX;
      const y = event.clientY;
      console.log("onTouchMove(" + x + "," + y + ")");
   }

   function onContextMenu( event ) 
   {
      const x = event.clientX;
      const y = event.clientY;
      //console.log("onContextMenu(" + x + "," + y + ")");
   }

   function onCanvasResize( event ) 
   {
      const w = event.width;
      const h = event.height;
      console.log("onCanvasResize(" + w + "," + h + ")");
      //requestAnimationFrame(render);
   }

   canvas.addEventListener( 'resize', onCanvasResize );
   document.addEventListener( 'change', onChange );
   document.addEventListener( 'mousemove', onMouseMove );
   document.addEventListener( 'scroll', onScroll );
   document.addEventListener( 'wheel', onMouseWheel );
   document.addEventListener( 'click', onMouseClick );
   document.addEventListener( 'dblclick', onMouseDoubleClick );
   document.addEventListener( 'mousedown', onMouseDown );
   document.addEventListener( 'mouseup', onMouseUp );
   document.addEventListener( 'pointerdown', onPointerDown );
	document.addEventListener( 'touchstart', onTouchStart );
	document.addEventListener( 'touchmove', onTouchMove );
	document.addEventListener( 'contextmenu', onContextMenu );
}


