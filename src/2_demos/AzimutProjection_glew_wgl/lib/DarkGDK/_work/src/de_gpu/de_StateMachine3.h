#pragma once

// ===========================================================================
// SM2 - H1 - History1 = StateMachine2 : Holds 2 states ( curr, last )
// ===========================================================================
template < typename T >
struct SM2
{
   T curr, last;
   SM2() { curr = last = T(0); }
   SM2( T const & t ) { curr = last = t; }
   SM2& operator= ( T const & t ) { curr = last = t; return *this; }
};

// ===========================================================================
// SM3 - H2 - History2 = StateMachine3 : Holds 3 states ( curr, last, start )
// ===========================================================================
template < typename T >
struct SM3
{
   T curr;
   T last;
   T init;
   SM3() { init = curr = last = T(0); }
   SM3( T const & t ) { init = curr = last = t; }
   SM3& operator= ( T const & t ) { init = curr = last = t; return *this; }
};
