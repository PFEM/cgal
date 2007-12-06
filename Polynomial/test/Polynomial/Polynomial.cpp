#include <iostream>
#include <CGAL/basic.h>
#include <CGAL/Testsuite/assert.h>

#include <CGAL/Arithmetic_kernel.h>
#include <CGAL/Polynomial.h>
#include <CGAL/Sqrt_extension.h>
#include <CGAL/Interval_nt.h>
#include <CGAL/_test_algebraic_structure.h>
#include <CGAL/_test_real_embeddable.h>

#include <CGAL/leda_coercion_traits.h>
#include <CGAL/CORE_coercion_traits.h>

#include <CGAL/Coercion_traits.h>

#define CGAL_DEFINE_TYPES_FROM_AK(AK)                           \
    typedef typename AK::Integer Integer;                       \
    typedef typename AK::Rational Rational;                     \
    typedef typename AK::Field_with_sqrt Field_with_sqrt;       \
    typedef CGAL::Polynomial<Integer> Poly_int1;                \
    typedef CGAL::Polynomial<Poly_int1> Poly_int2;              \
    typedef CGAL::Polynomial<Poly_int2> Poly_int3;              \
    typedef CGAL::Polynomial<Rational> Poly_rat1;               \
    typedef CGAL::Polynomial<Poly_rat1> Poly_rat2;              \
    typedef CGAL::Polynomial<Poly_rat2> Poly_rat3;             
   

// TODO: copied from number_type_utils.h
CGAL_BEGIN_NAMESPACE
template <class NT , class RT>
inline 
void convert_to(const NT& x, RT& r){ 
    typedef CGAL::Coercion_traits<NT,RT> CT;
    typedef typename CT::Coercion_type RET;
    BOOST_STATIC_ASSERT((::boost::is_same<RET,RT>::value));
    r = typename CT::Cast()(x);
}
CGAL_END_NAMESPACE

template <class NT>
void datastru() {
    typedef CGAL::Polynomial<NT> POLY;
    int i;

    // uninitialized construction
    POLY p;
    CGAL_test_assert( p.degree() == 0 );

    // construction from iterator range, assignment
    NT array[] = { NT(3), NT(1), NT(4), NT(1), NT(5), NT(9) };
    NT* const array_end = array + sizeof array / sizeof *array;
    POLY q(array, array_end);
    p = q;
    CGAL_test_assert( array_end - array == p.degree() + 1 );
    CGAL_test_assert( std::equal(p.begin(), p.end(), array) );

    // immediate construction, reduce(), copy construction, (in)equality
    POLY r(NT(3), NT(1), NT(4), NT(1), NT(5), NT(9), NT(0), NT(0));
    CGAL_test_assert( q.degree() == r.degree() );
    CGAL_test_assert( q == r );
    CGAL_test_assert( r == r );
    q = POLY(NT(2), NT(7), NT(1), NT(8), NT(1));
    CGAL_test_assert( q != r );
    POLY s(p);
    CGAL_test_assert( r == s );

    // selection of coefficients
    for (i = 0; i <= p.degree(); i++) {
        CGAL_test_assert( p[i] == array[i] );
    }
    CGAL_test_assert( i == array_end - array );
    CGAL_test_assert( p.lcoeff() == array_end[-1] );

    // reversal()
    CGAL_test_assert( reversal(q) == POLY(NT(1), NT(8), NT(1), NT(7), NT(2)) );

    // divide_by_x()
    r.divide_by_x();
    CGAL_test_assert( r.degree() == 4 );
    CGAL_test_assert( r == POLY(NT(1), NT(4), NT(1), NT(5), NT(9)) );

    // zero test
    CGAL_test_assert( !p.is_zero() );
    q = POLY(NT(0));
    CGAL_test_assert( q.is_zero() );
}

template <class NT> void signs(::CGAL::Tag_false);
template <class NT> void signs(::CGAL::Tag_true);
template <class NT> void division(CGAL::Integral_domain_without_division_tag);
template <class NT> void division(CGAL::Integral_domain_tag);

template <class NT>
void arithmetic() {
    typedef CGAL::Polynomial<NT> POLY;
    // evaluation
    POLY p(NT(6), NT(-5), NT(1));
    CGAL_test_assert( p.evaluate(NT(1)) == NT(2) );
    CGAL_test_assert( p.evaluate(NT(2)) == NT(0) );
    CGAL_test_assert( p.evaluate(NT(3)) == NT(0) );
    // TODO: No evaluate_absolute available for all NTs.
    //CGAL_test_assert( p.evaluate_absolute(NT(1)) == NT(12) );

    // diff
    POLY q(NT(3), NT(2), NT(-2));
    POLY z(NT(0));
    CGAL_test_assert( diff(z) == z );
    CGAL_test_assert( p.degree() == 2 );
    CGAL_test_assert( diff(p) == POLY(NT(-5), NT(2)) );
    CGAL_test_assert( p.degree() == 2 );
    CGAL_test_assert( q.degree() == 2 );
    q.diff();
    CGAL_test_assert( q == POLY(NT(2), NT(-4)) );

    // scale and translate
    p.scale_up(NT(4));
    CGAL_test_assert( p == POLY(NT(6), NT(-20), NT(16)) );
    CGAL_test_assert( scale_down(p, NT(2)) == POLY(NT(24), NT(-40), NT(16)) );
    CGAL_test_assert( p == POLY(NT(6), NT(-20), NT(16)) );
    CGAL_test_assert( translate_by_one(p) == translate(p, NT(1)) );
    p = POLY(NT(3), NT(2), NT(1));
    p.translate(NT(-2));
    CGAL_test_assert( p == POLY(NT(3), NT(-2), NT(1)) );

    // addition
    POLY r;
    p = POLY(NT(3), NT(2), NT(1), NT(1));
    q = POLY(NT(1), NT(2), NT(-1), NT(-1));
    r = p + q;
    CGAL_test_assert( r.degree() == 1 );
    CGAL_test_assert( r[0] == NT(4) );
    CGAL_test_assert( r[1] == NT(4) );
    q += p;
    CGAL_test_assert( q == r );
    q += NT(1);
    CGAL_test_assert( q == POLY(NT(5), NT(4)) );
    CGAL_test_assert( q == r + NT(1) );
    CGAL_test_assert( q == NT(1) + r );

    // subtraction
    p = POLY(NT(3), NT(2), NT(1), NT(1));
    q = POLY(NT(1), NT(2), NT(-1), NT(1));
    r = p - q;
    CGAL_test_assert( r.degree() == 2 );
    CGAL_test_assert( r == POLY(NT(2), NT(0), NT(2)) );
    r -= -q;
    CGAL_test_assert( r == p );
    r -= NT(2);
    CGAL_test_assert(  r == p - NT(2) );
    CGAL_test_assert( -r == NT(2) - p );

    // multiplication
    p = POLY(NT(1), NT(2), NT(3));
    q = POLY(NT(-2), NT(5), NT(2));
    r = p * q;
    CGAL_test_assert( r == POLY(NT(-2), NT(1), NT(6), NT(19), NT(6)) );
    q *= p;
    CGAL_test_assert( q == r );
    q *= NT(2);
    CGAL_test_assert( q == POLY(NT(-4), NT(2), NT(12), NT(38), NT(12)) );
    CGAL_test_assert( q == r * NT(2) );
    CGAL_test_assert( q == NT(2) * r );

    // sign etc., if applicable
    // TODO: Replaced Is_real_comparable with RET::Is_real_embeddable, OK?
    typedef typename CGAL::Real_embeddable_traits<NT>::Is_real_embeddable Embeddable;
    signs<NT>(Embeddable());

    // division, if applicable
    typedef typename CGAL::Algebraic_structure_traits<NT>::Algebraic_category Algebra_type;
    division<NT>(Algebra_type());
}

template <class NT> void signs(::CGAL::Tag_false) {
    // nothing to be done
}

template <class POLY>
void test_greater(POLY a, POLY b) {
    // check .compare()
    CGAL_test_assert( a.compare(b) == CGAL::LARGER );
    CGAL_test_assert( a.compare(a) == CGAL::EQUAL );
    CGAL_test_assert( b.compare(a) == CGAL::SMALLER );

    // check that all comparison operators reflect a > b
    CGAL_test_assert(   a >  b  );
    CGAL_test_assert( !(b >  a) );
    CGAL_test_assert( !(a >  a) );
    CGAL_test_assert(   b <  a  );
    CGAL_test_assert( !(a <  b) );
    CGAL_test_assert( !(a <  a) );
    CGAL_test_assert(   a >= b  );
    CGAL_test_assert( !(b >= a) );
    CGAL_test_assert(   a >= a  );
    CGAL_test_assert(   b <= a  );
    CGAL_test_assert( !(a <= b) );
    CGAL_test_assert(   a <= a  );
}

template <class NT>
void signs(::CGAL::Tag_true) {
    typedef CGAL::Polynomial<NT> POLY;

    POLY p(NT(6), NT(-5), NT(1));
    POLY pp(NT(6), NT(-5), NT(1));
    POLY q(NT(3), NT(2), NT(-2));
    POLY z(NT(0));

    // sign, abs
    CGAL_test_assert( z.sign() == CGAL::ZERO );
    CGAL_test_assert( p.sign() == CGAL::POSITIVE );
    CGAL_test_assert( p.abs() == p );
    CGAL_test_assert( q.sign() == CGAL::NEGATIVE );
    CGAL_test_assert( q.abs() == POLY(NT(-3), NT(-2), NT(2)) );

    // comparison operators
    test_greater(p + NT(1), p);
    test_greater(POLY(NT(1), NT(2), NT(3), NT(4)), p);
    
    // sign_at 
    q=POLY(NT(3),NT(2),NT(-1));
    CGAL_test_assert(q.sign_at(-2)==CGAL::NEGATIVE);
    CGAL_test_assert(q.sign_at(-1)==CGAL::ZERO);
    CGAL_test_assert(q.sign_at( 0)==CGAL::POSITIVE);
    CGAL_test_assert(q.sign_at( 3)==CGAL::ZERO);
    CGAL_test_assert(q.sign_at( 4)==CGAL::NEGATIVE);

    CGAL_test_assert(z.sign_at( 3)==CGAL::ZERO);
}

template <class NT>
void division(CGAL::Integral_domain_without_division_tag) {
    // nothing to be done
}

template <class NT>
void division(CGAL::Integral_domain_tag) {
    typedef CGAL::Polynomial<NT> POLY;

    // integral division (remainder zero)
    POLY p(NT(2), NT(4), NT(6)), q(NT(-2), NT(5), NT(2));
    POLY r(NT(-4), NT(2), NT(12), NT(38), NT(12)), s;
    CGAL_test_assert( q == r / p );
    r /= q;
    CGAL_test_assert( r == p );
    r /= NT(2);
    CGAL_test_assert( r == POLY(NT(1), NT(2), NT(3)) );
    CGAL_test_assert( NT(6) / POLY(NT(3)) == POLY(NT(2)) );

    // division with remainder
    p = POLY(NT(2), NT(-3), NT(1));    //  (x-1)(x-2)
    q = POLY(NT(24), NT(-14), NT(2));  // 2(x-3)(x-4)
    r = POLY(NT(-5), NT(1));           //       (x-5)
    s = p*q + r;

    POLY Q, R;
    POLY::euclidean_division(s, p, Q, R);  // works, since p.lcoeff() == 1
    CGAL_test_assert( Q == q );
    CGAL_test_assert( R == r );

    NT D;
    POLY::pseudo_division(s, q, Q, R, D);
    CGAL_test_assert( Q == D*p );
    CGAL_test_assert( R == D*r );

    // codecover 
    p = POLY(NT(2),NT(3));
    s = POLY(NT(2),NT(3),NT(4));
    POLY::euclidean_division(p, s, Q, R);  // works, since p.lcoeff() == 1
    CGAL_test_assert( Q == POLY(NT(0)) );
    CGAL_test_assert( R == p );

    POLY::pseudo_division(p, s, Q, R, D);  // works, since p.lcoeff() == 1
    CGAL_test_assert( Q == POLY(NT(0)) );
    CGAL_test_assert( R == D*p );
        
}

template <class NT>
void io() {
    typedef CGAL::Polynomial<NT> POLY;

    {
        // successful re-reading of output
        POLY p(NT(-3), NT(5), NT(0), NT(0), NT(-7), NT(9)), q;
        std::ostringstream os;
        os << p;
        std::istringstream is(os.str());
        is >> q;
        CGAL_test_assert( p == q );
    }{       
        std::ostringstream os;
        CGAL::set_pretty_mode(os);
        os << oformat(POLY(NT(3)));
        //std::cout <<os.str()<<std::endl;
        CGAL_test_assert( os.str() == "3" );
    }{       
        std::ostringstream os;
        CGAL::set_pretty_mode(os);
        os << oformat(POLY(NT(-3)));
        CGAL_test_assert( os.str() == "(-3)" );
    }{       
        std::ostringstream os;
        CGAL::set_pretty_mode(os);
        os << oformat(POLY(NT(-3)),CGAL::Parens_as_product_tag());
        CGAL_test_assert( os.str() == "(-3)" );
    }{       
        std::ostringstream os;
        CGAL::set_pretty_mode(os);
        os << oformat(POLY(NT(-3),NT(4)));
        if( CGAL::Polynomial_traits_d<POLY>::d == 1)
            CGAL_test_assert( os.str() == "4*x + (-3)" );
        else
            CGAL_test_assert( os.str() == "4*y + (-3)" );
    }{       
        std::ostringstream os;
        CGAL::set_pretty_mode(os);
        os << oformat(POLY(NT(-3),NT(4)), CGAL::Parens_as_product_tag());
        
        if( CGAL::Polynomial_traits_d<POLY>::d == 1)
            CGAL_test_assert( os.str() == "(4*x + (-3))" );
        else
            CGAL_test_assert( os.str() == "(4*y + (-3))" ); 
        
    }
    
}

template <class NT>
void canon(CGAL::Integral_domain_without_division_tag) {
    // dummy for cases where canonicalize_polynomial() does not apply
}

template <class NT>
void canon(CGAL::Unique_factorization_domain_tag) {
    typedef CGAL::Polynomial<NT>     UNPOLY;
    typedef CGAL::Polynomial<UNPOLY> BIPOLY;

    BIPOLY p(0), c;
    CGAL_test_assert(canonicalize_polynomial(p) == BIPOLY(0));

    p = BIPOLY(UNPOLY(NT(15)), UNPOLY(NT(-12), NT( 9)), UNPOLY(NT(0), NT(-6)));
    c = BIPOLY(UNPOLY(NT(-5)), UNPOLY(NT(  4), NT(-3)), UNPOLY(NT(0), NT( 2)));
    CGAL_test_assert(canonicalize_polynomial(p) == c);
}

template <class NT>
void canon(CGAL::Field_tag) {
    typedef CGAL::Polynomial<NT>     UNPOLY;
    typedef CGAL::Polynomial<UNPOLY> BIPOLY;

    BIPOLY p(0), c;
    CGAL_test_assert(canonicalize_polynomial(p) == BIPOLY(0));

    p = BIPOLY(UNPOLY(NT(15)), UNPOLY(NT(-12), NT( 9)), UNPOLY(NT(0), NT(-6)));
    c = BIPOLY(UNPOLY(NT(-5)/NT(2)), UNPOLY(NT(2), NT(-3)/NT(2)),
            UNPOLY(NT(0), NT(1)));
    CGAL_test_assert(canonicalize_polynomial(p) == c);
}

#ifdef NiX_POLY_USE_NT_TESTS
template <class NT>
void nt_tests(::CGAL::Tag_false) {
    typedef CGAL::Polynomial<NT> POLY;
    if ((volatile bool)0) { // test compilation only
        CGAL::test_number_type<POLY,
                              typename CGAL::Algebraic_structure_traits<POLY>::Algebric_structure_tag>();
    }
}

template <class NT>
void nt_tests(::CGAL::Tag_true) {
    typedef CGAL::Polynomial<NT> POLY;
    CGAL::test_real_comparable<POLY>();
    nt_tests<NT>(::CGAL::Tag_false());
}
#endif // NiX_POLY_USE_NT_TESTS

template <class NT>
void unigcdres(CGAL::Field_tag) {
    typedef CGAL::Polynomial<NT> POLY;

    // test univariate polynomial gcd and resultant over a field
    POLY f(NT(2), NT(7), NT(1), NT(8), NT(1), NT(8));
    POLY g(NT(3), NT(1), NT(4), NT(1), NT(5), NT(9));
    POLY h(NT(3), NT(4), NT(7), NT(7));
    f /= NT(3);
    g /= NT(5);
    h /= h.unit_part();
    POLY d;

    d = gcd(f, g);
    CGAL_test_assert( d == POLY(NT(1)) );
    CGAL_test_assert( prs_resultant(f, g) == NT(230664271L)/NT(759375L) ); // Maple

    POLY fh(f*h), gh(g*h);
    d = gcd(fh, gh);
    CGAL_test_assert( d == h );
    CGAL_test_assert( prs_resultant(fh, gh) == NT(0) );

    POLY a, b;
    d = gcdex(fh, gh, a, b);
    CGAL_test_assert( d == h );
    CGAL_test_assert( d == a*fh + b*gh );
}

template <class NT>
void unigcdres(CGAL::Integral_domain_tag) {
    typedef CGAL::Polynomial<NT> POLY;

    // test univariate polynomial gcd and resultant over a non-field
    POLY f(NT(2), NT(7), NT(1), NT(8), NT(1), NT(8));
    POLY g(NT(3), NT(1), NT(4), NT(1), NT(5), NT(9));
    POLY h(NT(3), NT(4), NT(7), NT(7));
    POLY d;
    NT c(42);

    d = gcd((-c)*f, c*g);
    CGAL_test_assert( d == POLY(c) );
    CGAL_test_assert( prs_resultant(f, g) == NT(230664271L) ); // as computed by Maple

    POLY fh(f*h), gh(g*h);
    d = gcd((-c)*fh, c*gh);
    CGAL_test_assert( d == c*h );
    CGAL_test_assert( prs_resultant(fh, gh) == NT(0) );

    POLY a, b; NT v;
    d = pseudo_gcdex((-c)*fh, c*gh, a, b, v);
    CGAL_test_assert( d == c*h );
    CGAL_test_assert( v*d == (-c)*a*fh + c*b*gh );

    // Michael Kerber's example for the hgdelta_update() bug:
    // These polynomials cretate a situation where h does not divide g,
    // but h^(delta-1) divides g^delta (as predicted by subresultant theory).
    // TODO: Uncomment following code
    /*CGAL::Creator_1<int, NT> int2nt;
    static const int cp[] = { 196, 0, -140, 0, 69, 0, -8, 0, -4 },
                     cq[] = { -15156, 0, -58572, 0, -82309, 0, -27032, 0,
                         20190, 0, 1696, 0, -6697, 0, 1644, 0, 1664, 0, 256 };
    static const size_t np = sizeof cp / sizeof *cp,
                        nq = sizeof cq / sizeof *cq;
    POLY p(LiS::mapper(cp, int2nt), LiS::mapper(cp+np, int2nt)),
         q(LiS::mapper(cq, int2nt), LiS::mapper(cq+nq, int2nt));
    d = gcd(p,q);
    CGAL_test_assert ( d == POLY(1) );*/
}

template <class NT>
void bigcdres(CGAL::Field_tag) {
    typedef CGAL::Polynomial<NT>    POLY1;
    typedef CGAL::Polynomial<POLY1> POLY2;

    // test bivariate polynomial gcd and resultant over a field
    POLY2 f = POLY2(POLY1(NT(2), NT(7), NT(1)),
                    POLY1(NT(8), NT(1)), POLY1(NT(8)));
    POLY2 g = POLY2(POLY1(NT(3), NT(1), NT(4)),
                    POLY1(NT(1), NT(5)), POLY1(NT(9)));
    POLY2 h = POLY2(POLY1(NT(3), NT(4), NT(1)),
                    POLY1(NT(9), NT(7)), POLY1(NT(7)));
    f /= NT(3);
    g /= NT(5);
    h /= h.unit_part();
    POLY2 d;

    d = gcd(f, g);
    CGAL_test_assert( d == POLY2(1) );
    POLY1 r(NT(1444), NT(-1726), NT(3295), NT(-2501), NT(560));
    r /= NT(225);
    CGAL_test_assert( prs_resultant(f, g) == r ); // says Maple

    POLY2 fh(f*h), gh(g*h);
    d = gcd(fh, gh);
    CGAL_test_assert( d == h );
    CGAL_test_assert( prs_resultant(fh, gh) == POLY1(0) );
}

template <class NT>
void bigcdres(CGAL::Integral_domain_tag) {
    typedef CGAL::Polynomial<NT>    POLY1;
    typedef CGAL::Polynomial<POLY1> POLY2;

    // test bivariate polynomial gcd and resultant over a non-field
    POLY2 f = POLY2(POLY1(NT(2), NT(7), NT(1)),
                    POLY1(NT(8), NT(1)), POLY1(NT(8)));
    POLY2 g = POLY2(POLY1(NT(3), NT(1), NT(4)),
                    POLY1(NT(1), NT(5)), POLY1(NT(9)));
    POLY2 h = POLY2(POLY1(NT(3), NT(4), NT(1)),
                    POLY1(NT(9), NT(7)), POLY1(NT(7)));
    POLY2 c(42), d;

    d = gcd(-c*f, c*g);
    CGAL_test_assert( d == c );
    POLY1 r(NT(1444), NT(-1726), NT(3295), NT(-2501), NT(560));
    CGAL_test_assert( prs_resultant(f, g) == r ); // says Maple

    POLY2 fh(f*h), gh(g*h);
    d = gcd(-c*fh, c*gh);
    CGAL_test_assert( d == c*h );
    CGAL_test_assert( prs_resultant(fh, gh) == POLY1(0) );
}

template <class AT>
void psqff(){
// pseudo-square-free factorization (i.e. factorization by multiplicities)
    {
       typedef typename AT::Integer NT; //UFD domain
       typedef CGAL::Polynomial<NT> POLY;
       // square-free factorization (i.e. factorization by multiplicities)
       POLY p1(NT(3), NT(4), NT(1)), p3(NT(9), NT(5), NT(1));
       POLY p4(NT(2), NT(6), NT(1));
       POLY p = NT(5) * p1 * p3*p3*p3 * p4*p4*p4*p4;

       typedef std::vector<POLY> PVEC;
       typedef std::vector<int>  IVEC;
       PVEC fac;
       IVEC mul;
       std::back_insert_iterator<PVEC> fac_bi(fac);
       std::back_insert_iterator<IVEC> mul_bi(mul);

       int n;
       n = CGAL::filtered_square_free_factorization_utcf(p, fac_bi, mul_bi);

       CGAL_test_assert(n == 3);
       CGAL_test_assert((int) mul.size() == n);
       CGAL_test_assert((int) fac.size() == n);
       CGAL_test_assert(mul[0] == 1 && fac[0] == p1);
       CGAL_test_assert(mul[1] == 3 && fac[1] == p3);
       CGAL_test_assert(mul[2] == 4 && fac[2] == p4);
   }{
       typedef typename AT::Integer Integer; //UFD domain
       typedef CGAL::Sqrt_extension<Integer,Integer> NT;
       typedef CGAL::Polynomial<NT> POLY;
       // square-free factorization (i.e. factorization by multiplicities)
       POLY p1(NT(3), NT(4), NT(1)), p3(NT(9), NT(5), NT(1));
       POLY p4(NT(2), NT(6), NT(1));
       POLY p = NT(5) * p1 * p3*p3*p3 * p4*p4*p4*p4;

       typedef std::vector<POLY> PVEC;
       typedef std::vector<int>  IVEC;
       PVEC fac;
       IVEC mul;
       std::back_insert_iterator<PVEC> fac_bi(fac);
       std::back_insert_iterator<IVEC> mul_bi(mul);

       int n;
       n = CGAL::filtered_square_free_factorization_utcf(p, fac_bi, mul_bi);

       CGAL_test_assert(n == 3);
       CGAL_test_assert((int) mul.size() == n);
       CGAL_test_assert((int) fac.size() == n);
       CGAL_test_assert(mul[0] == 1 && fac[0] == p1);
       CGAL_test_assert(mul[1] == 3 && fac[1] == p3);
       CGAL_test_assert(mul[2] == 4 && fac[2] == p4);
    }{
       typedef typename AT::Integer Integer; //UFD domain
       typedef CGAL::Sqrt_extension<Integer,Integer> NT;
       typedef CGAL::Polynomial<NT> POLY;
       // square-free factorization (i.e. factorization by multiplicities)
       POLY p1(NT(3,4,7), NT(2,3,7), NT(3));
       POLY p3(NT(9,2,7), NT(5,11,7), NT(2));
       POLY p4(NT(2,8,7), NT(6,3,7), NT(5));
       POLY p = NT(5,3,7)*p1 * p3*p3*p3 * p4*p4*p4*p4;

       typedef std::vector<POLY> PVEC;
       typedef std::vector<int>  IVEC;
       PVEC fac;
       IVEC mul;
       std::back_insert_iterator<PVEC> fac_bi(fac);
       std::back_insert_iterator<IVEC> mul_bi(mul);

       int n;
       n = CGAL::filtered_square_free_factorization_utcf(p, fac_bi, mul_bi);

       CGAL_test_assert(n == 3);
       CGAL_test_assert((int) mul.size() == n);
       CGAL_test_assert((int) fac.size() == n);
       CGAL_test_assert(mul[0] == 1 && fac[0] == p1);
       CGAL_test_assert(mul[1] == 3 && fac[1] == p3);
       CGAL_test_assert(mul[2] == 4 && fac[2] == p4);
   }
}
template <class NT>
void sqff() {
    typedef CGAL::Polynomial<NT> POLY;

    // square-free factorization (i.e. factorization by multiplicities)
    POLY p1(NT(3), NT(4), NT(1));
    POLY p3(NT(9), NT(5), NT(1));
    POLY p4(NT(2), NT(6), NT(1));
    POLY p = NT(3) * p1 * p3*p3*p3 * p4*p4*p4*p4;

    typedef std::vector<POLY> PVEC;
    typedef std::vector<int>  IVEC;
    PVEC fac;
    IVEC mul;
    std::back_insert_iterator<PVEC> fac_bi(fac);
    std::back_insert_iterator<IVEC> mul_bi(mul);
    unsigned n;
    NT alpha;
    n = CGAL::filtered_square_free_factorization(p, fac_bi, mul_bi );

//    CGAL_test_assert(alpha == 3);
    CGAL_test_assert(n == 3);
    CGAL_test_assert(mul.size() == n);
    CGAL_test_assert(fac.size() == n);
    CGAL_test_assert(mul[0] == 1 && fac[0] == p1);
    CGAL_test_assert(mul[1] == 3 && fac[1] == p3);
    CGAL_test_assert(mul[2] == 4 && fac[2] == p4);
    
    /*p = POLY( NT(1), NT(-2), NT(1) );
    std::cerr << p << std::endl;
    fac.clear();
    mul.clear();
    fac_bi = std::back_insert_iterator<PVEC>(fac);
    mul_bi = std::back_insert_iterator<IVEC>(mul);
    std::cerr << CGAL::filtered_square_free_factorization( p, fac_bi, mul_bi ) << std::endl;
    std::cerr << fac[0] << std::endl;*/
    //std::cerr << fac[1] << std::endl;
    
    typedef CGAL::Polynomial< CGAL::Polynomial< NT > > BPOLY;
    typedef std::vector< BPOLY > BPVEC;
    BPOLY bp( POLY( NT(1), NT(-2), NT(1) ) );
//    std::cerr << bp << std::endl;
//    typename CGAL::Polynomial_traits_d< BPOLY >::Make_square_free make_square_free;
//    std::cerr << make_square_free( bp ) << std::endl;
//    std::cerr << "^^^^^^^^^^" << std::endl;
    BPVEC bfac;
    mul.clear();
    std::back_insert_iterator< BPVEC > bfac_bi( bfac );
    mul_bi = std::back_insert_iterator<IVEC>(mul);
//    std::cerr << CGAL::POLYNOMIAL::square_free_factorization( bp, bfac_bi, mul_bi ) << std::endl;
//    std::cerr << bfac[0] << std::endl;
    
}

template <class FNT>
void integr() {
    typedef CGAL::Polynomial<FNT> FPOLY;
    typedef typename CGAL::Fraction_traits<FPOLY>::Numerator_type IPOLY;
    typedef typename CGAL::Fraction_traits<FPOLY>::Denominator_type DENOM;
    typedef typename IPOLY::NT INT;

    // making polynomials integral and fractional
    FPOLY fp(FNT(1)/FNT(2), FNT(2)/FNT(3), FNT(3)/FNT(5));
    IPOLY ip; DENOM d;
    ip = integralize_polynomial(fp, d);
    CGAL_test_assert( d == DENOM(30) );
    CGAL_test_assert( ip == IPOLY(INT(15), INT(20), INT(18)) );
    CGAL_test_assert( fp == CGAL::fractionalize_polynomial<FPOLY>(ip, d) );
}

template <class NT>
void basic_tests() {
    // tests that should work for all number types
    typedef CGAL::Polynomial<NT> POLY;
    datastru<NT>();
    datastru<POLY>();
    arithmetic<NT>();
    arithmetic<POLY>();
    io<NT>();
    io<POLY>();
    canon<NT>(typename CGAL::Algebraic_structure_traits<NT  >::Algebraic_category());
#ifdef NiX_POLY_USE_NT_TESTS
    // TODO: Replaced Is_real_comparable with RET::Is_real_embeddable, OK?
    nt_tests<NT  >(typename CGAL::Real_embeddable_traits<NT  >::Is_real_embeddable());
    // TODO: Replaced Is_real_comparable with RET::Is_real_embeddable, OK?
    nt_tests<POLY>(typename CGAL::Real_embeddable_traits<POLY>::Is_real_embeddable());
#endif // NiX_POLY_USE_NT_TESTS
}

template <class NT>
void exact_tests() {
    // tests that probably don't work for inexact or bounded number types
    unigcdres<NT>(typename CGAL::Algebraic_structure_traits<NT>::Algebraic_category());
    bigcdres<NT>(typename CGAL::Algebraic_structure_traits<NT>::Algebraic_category());
    sqff<NT>();
}

template < class Rational >
void test_coefficients_to() {
    typedef CGAL::Polynomial< int >       Poly_i1;
    typedef CGAL::Polynomial< Poly_i1 >   Poly_i2;
    typedef CGAL::Polynomial< Poly_i2 >   Poly_i3;
    
    typedef CGAL::Polynomial< Rational >  Poly_rat1;
    typedef CGAL::Polynomial< Poly_rat1 > Poly_rat2;
    typedef CGAL::Polynomial< Poly_rat2 > Poly_rat3;
    
    // univariate
    {
        Poly_i1 p = Poly_i1(-3,4,5,-9,0,2);
        
        Poly_rat1 r = Poly_rat1(Rational(-3),Rational(4),Rational(5),
                                Rational(-9),Rational(0),Rational(2));
        Poly_rat1 tmp;
        CGAL::convert_to(p,tmp); 
        CGAL_test_assert(tmp == r);                
    }
    
    // bivariate
    {
        Poly_i1 p1 = Poly_i1( 3,-1, 2);
        Poly_i1 p2 = Poly_i1(-5, 2,-4);
        Poly_i2 p = Poly_i2(p1, p2);
        
        Poly_rat1 r1 = Poly_rat1(Rational( 3),Rational(-1),Rational( 2));
        Poly_rat1 r2 = Poly_rat1(Rational(-5),Rational( 2),Rational(-4));
        Poly_rat2 r = Poly_rat2(r1,r2);
        
        Poly_rat2 tmp;
        CGAL::convert_to(p,tmp); 
        CGAL_test_assert(tmp == r);     
    }

    // trivariate
    {
        Poly_i1 p11 = Poly_i1( 7,-6, 9);
        Poly_i1 p12 = Poly_i1(-1, 3);
        Poly_i2 p1  = Poly_i2(p11, p12);
        
        Poly_i1 p21 = Poly_i1(-17,  16, -19);
        Poly_i1 p22 = Poly_i1( 11, -13);
        Poly_i2 p2  = Poly_i2(p21, p22);
        
        Poly_i3 p   = Poly_i3(p1,p2);
 
        Poly_rat1 r11 = Poly_rat1(Rational( 7),Rational(-6),Rational( 9));
        Poly_rat1 r12 = Poly_rat1(Rational(-1),Rational( 3));
        Poly_rat2 r1  = Poly_rat2(r11, r12);
        
        Poly_rat1 r21 = Poly_rat1(Rational(-17),Rational( 16),Rational(-19));
        Poly_rat1 r22 = Poly_rat1(Rational( 11),Rational(-13));
        Poly_rat2 r2  = Poly_rat2(r21, r22);
        
        Poly_rat3 r   = Poly_rat3(r1,r2);
        
        Poly_rat3 tmp;
        CGAL::convert_to(p,tmp); 
        CGAL_test_assert(tmp == r);
    }
}

template <class AT>
void test_evaluate(){
    CGAL_DEFINE_TYPES_FROM_AK(AT);
    { 
        Poly_int1 P(3,0,2);
        Integer x(2);        
        CGAL_test_assert(P.evaluate(x)==Integer(11));
    }{    
        Poly_rat1 P(3,0,2);
        Integer x(2);
        CGAL_test_assert(P.evaluate(x)==Rational(11));
    }{    
        Poly_int1 P(3,0,2);
        Rational x(2);
        CGAL_test_assert(P.evaluate(x)==Rational(11));
    }{ 
        Poly_int1 P(3,0,2);
        CGAL::Interval_nt<true> x(2);    
        CGAL_test_assert(P.evaluate(x).inf()<=11);
        CGAL_test_assert(P.evaluate(x).sup()>=11);
    }{
        CGAL::Polynomial< CGAL::Interval_nt<true> > P(3,0,2);
        CGAL::Interval_nt<true> x(2);
        CGAL_test_assert(P.evaluate(x).inf()<=11);
        CGAL_test_assert(P.evaluate(x).sup()>=11);
    }{
        CGAL::Polynomial< CGAL::Interval_nt<true> > P(3,0,2);
        Integer x(2);
        CGAL_test_assert(P.evaluate(x).inf()<=11);
        CGAL_test_assert(P.evaluate(x).sup()>=11);
        }
}

template <class AT>
void test_evaluate_homogeneous(){
    CGAL_DEFINE_TYPES_FROM_AK(AT);
    { 
        Poly_int1 P(3,0,2);
        Integer u(2);
        Integer v(1);
        CGAL_test_assert(P.evaluate_homogeneous(u,v)==Integer(11));
    }{ 
        Poly_rat1 P(2,-1,3);
        Rational u(3);
        Rational v(5);
        CGAL_test_assert(P.evaluate_homogeneous(u,v)==Rational(62));
        CGAL_test_assert(P.evaluate(u/v)==Rational(62)/Rational(25));
    }
}

template <class NT>
void flat_iterator_tests() {
    typedef CGAL::Polynomial<NT> POLY1;
    typedef CGAL::Polynomial<POLY1> POLY2;
    typedef CGAL::Polynomial<POLY2> POLY3;

    POLY1
        p1(NT( 1), NT( 2), NT( 3)), 
        p2(NT( 4), NT( 5), NT( 6)),
        p3(NT( 7), NT( 8), NT( 9)),
        p4(NT(10), NT(11), NT(12)),
        p5(NT(13), NT(14), NT(15)),
        p6(NT(16), NT(17), NT(18)),
        p7(NT(19), NT(20), NT(21)),
        p8(NT(22), NT(23), NT(24)),
        p9(NT(25), NT(26), NT(27));

    POLY2
        q1(p1, p2, p3),
        q2(p4, p5, p6),
        q3(p7, p8, p9);

    POLY3 r(q1, q2, q3);

    typedef CGAL::Polynomial_traits_d<POLY1> PT1;
    typedef CGAL::Polynomial_traits_d<POLY2> PT2;
    typedef CGAL::Polynomial_traits_d<POLY3> PT3;

    int i;
    typename PT1::Innermost_coefficient_iterator it1;
    typename PT1::Innermost_coefficient_begin begin1;
    typename PT1::Innermost_coefficient_end   end1;
    typename PT2::Innermost_coefficient_iterator it2;
    typename PT2::Innermost_coefficient_begin begin2;
    typename PT2::Innermost_coefficient_end   end2;
    typename PT3::Innermost_coefficient_iterator it3;
    typename PT3::Innermost_coefficient_begin begin3;
    typename PT3::Innermost_coefficient_end   end3;

    for (i = 1, it1 = begin1(p1); i <= 3; ++i, ++it1)
        CGAL_test_assert(*it1 == i);
    CGAL_test_assert(it1 == end1(p1));
    for (i = 1, it2 = begin2(q1); i <= 9; ++i, ++it2)
        CGAL_test_assert(*it2 == i);
    CGAL_test_assert(it2 == end2(q1));
    for (i = 1, it3 = begin3(r); i <= 27; ++i, ++it3)
        CGAL_test_assert(*it3 == i);
    CGAL_test_assert(it3 == end3(r));
}

void test_total_degree(){
    typedef CGAL::Polynomial<int>    Poly_1;
    typedef CGAL::Polynomial<Poly_1> Poly_2;
    
    CGAL_test_assert(CGAL::total_degree(5) == 0);
    
    CGAL_test_assert(CGAL::total_degree(Poly_1(0))      == Poly_1(0).degree());
    CGAL_test_assert(CGAL::total_degree(Poly_1(1))      == Poly_1(1).degree());      
    CGAL_test_assert(CGAL::total_degree(Poly_1(1,1))    == Poly_1(1,1).degree());
    CGAL_test_assert(CGAL::total_degree(Poly_1(0,0,0,1))== Poly_1(0,0,0,1).degree());
    
    CGAL_test_assert(CGAL::total_degree(Poly_2(0))      ==  0);
    CGAL_test_assert(CGAL::total_degree(Poly_2(1))      ==  0);
    CGAL_test_assert(CGAL::total_degree(Poly_2(Poly_1(1),Poly_1(1))) ==  1);
    CGAL_test_assert(CGAL::total_degree(Poly_2(0,0,0,1))==  3);
    
    CGAL_test_assert(CGAL::total_degree(Poly_2(Poly_1(1,1),Poly_1(1)))  == 1);
    CGAL_test_assert(CGAL::total_degree(Poly_2(0,0,1))==  2);
    CGAL_test_assert(CGAL::total_degree(Poly_2(Poly_1(0),Poly_1(0),Poly_1(1)))== 2);
    CGAL_test_assert(CGAL::total_degree(Poly_2(Poly_1(0),Poly_1(0),Poly_1(1,1)))== 3);
    CGAL_test_assert(CGAL::total_degree(Poly_2(Poly_1(1),Poly_1(0),Poly_1(1,1)))== 3);
    CGAL_test_assert(CGAL::total_degree(Poly_2(Poly_1(1),Poly_1(1),Poly_1(1,1)))== 3);
    CGAL_test_assert(CGAL::total_degree(Poly_2(Poly_1(1),Poly_1(1,1,1,1),Poly_1(1,1)))== 4);
}

template<class AT>
void test_scalar_factor_traits(){
    
    {   
        typedef typename AT::Integer Integer;
        typedef CGAL::Polynomial<Integer> Polynomial; 
        typedef CGAL::Scalar_factor_traits<Polynomial> SFT;
        typedef typename AT::Integer Scalar;
        typedef typename SFT::Scalar Scalar_;
        BOOST_STATIC_ASSERT((::boost::is_same<Scalar_, Scalar>::value));
        
        typename SFT::Scalar_factor sfac;
        
        CGAL_test_assert(sfac(Polynomial(0))==Scalar(0));
        CGAL_test_assert(sfac(Polynomial(9))==Scalar(9));
        CGAL_test_assert(sfac(Polynomial(9,15,30))==Scalar(3));
        
        CGAL_test_assert(sfac(Polynomial(0), Integer(0))==Scalar(0));
        CGAL_test_assert(sfac(Polynomial(0), Integer(1))==Scalar(1));
        CGAL_test_assert(sfac(Polynomial(0), Integer(2))==Scalar(2));
        
        CGAL_test_assert(sfac(Polynomial(9), Integer(0))==Scalar(9));
        CGAL_test_assert(sfac(Polynomial(9), Integer(6))==Scalar(3));
        
        CGAL_test_assert(sfac(Polynomial(15,0 ,30) , Integer(9))==Scalar(3));
        CGAL_test_assert(sfac(Polynomial(0 ,15,30) , Integer(9))==Scalar(3));
        CGAL_test_assert(sfac(Polynomial(18,15,30) , Integer(0))==Scalar(3));
    }{
        typedef typename AT::Integer Integer;
        typedef CGAL::Sqrt_extension<Integer,Integer> EXT_1;
        typedef CGAL::Polynomial<EXT_1       > Poly_1_ext_1;
        typedef CGAL::Polynomial<Poly_1_ext_1> Poly_2_ext_1;
        typedef CGAL::Scalar_factor_traits<Poly_2_ext_1> SFT;
        typedef typename AT::Integer Scalar;
        typedef typename SFT::Scalar Scalar_;
        BOOST_STATIC_ASSERT((::boost::is_same<Scalar_, Scalar>::value));
            
        typename SFT::Scalar_factor sfac;

        CGAL_test_assert(sfac(Poly_2_ext_1( ))==Integer(0));
        CGAL_test_assert(sfac(Poly_2_ext_1(1))==Integer(1));
        CGAL_test_assert(sfac(Poly_2_ext_1(2))==Integer(2));
        
        EXT_1 a(18,27,456); 
        EXT_1 b( 0,15,456);
        Poly_2_ext_1 p(Poly_1_ext_1(a,b),Poly_1_ext_1(b,a));
        
        CGAL_test_assert(sfac(p)==Integer(3));
  }
}


template <class AT>
void test_AT(){
    basic_tests<int>();
    basic_tests<double>();
    // TODO: The flat_iterator_test leads to compile errors. Need to be fixed
    flat_iterator_tests<int>();
        
    {
        typedef typename AT::Integer Integer;
        typedef typename CGAL::Polynomial<Integer> Polynomial;
        typedef CGAL::Unique_factorization_domain_tag Tag;
        typedef CGAL::Tag_true Is_exact;
        CGAL::test_algebraic_structure<Polynomial,Tag, Is_exact>();
        basic_tests<Integer>();
        exact_tests<Integer>();
    }{
        typedef typename AT::Rational Rational;
        typedef typename CGAL::Polynomial<Rational> Polynomial;
        typedef CGAL::Euclidean_ring_tag Tag;
        typedef CGAL::Tag_true Is_exact;
        //can't use this test for Polynomials 
        //CGAL::test_algebraic_structure<Polynomial,Tag, Is_exact>();
        basic_tests<Rational>();
        exact_tests<Rational>();
        integr<Rational>(); 
    }

    // TODO: This also leads to trouble with flattening. Needs to be fixed
    psqff<AT>();
    test_evaluate<AT>();
    test_evaluate_homogeneous<AT>();
    
    test_total_degree();
    //test_scalar_factor_traits<AT>();
}

int main() {
  
  
// The MODULAR_TRAITS specializations for Polynomial
// =========================================================================
#ifdef CGAL_USE_LEDA    
/*    {
        typedef CGAL::Polynomial<leda::integer> TESTT;
        typedef CGAL::Modular MOD;
        MOD::set_current_prime(7);
        typedef CGAL::Modular_traits<TESTT> MOD_traits;
        typedef MOD_traits::Modular_NT MOD_NT;
        MOD_traits::Modular_image MOD_image;
    
        //definition of MOD::NT
        CGAL_test_assert((::boost::is_same<TESTT,
                  MOD_traits::NT>::value));
        // Is_modularizable
        CGAL_test_assert( 
            (::boost::is_same<::CGAL::Tag_true,
             MOD_traits::Is_modularizable>::value)
            );

        CGAL_test_assert(MOD_image(TESTT(21)) == MOD_NT(0));   
        CGAL_test_assert(MOD_image(TESTT(22)) == MOD_NT(1));
        CGAL_test_assert(MOD_image(TESTT(777777722)) == MOD_NT(1));
        
        TESTT p(12,2,4,21),q(5,2,4);
        CGAL_test_assert(MOD_image(p)==MOD_image(q));
        p=TESTT(7,21,14,-28);
        CGAL_test_assert(MOD_image(p)==MOD_NT(0));
    }*/

    // NT_Traits<POLY>::to_Interval
/*    {  
        typedef leda_integer NT;
        typedef CGAL::Interval Interval;
        typedef CGAL::Polynomial<NT> POLY;
        typedef CGAL::Polynomial<Interval> IPOLY;
        typedef CGAL::Real_embeddable_traits<POLY>::To_interval::result_type RESULTTYPE;
        CGAL::Real_embeddable_traits<POLY>::To_interval to_Interval;
        
        BOOST_STATIC_ASSERT((::boost::is_same<IPOLY,RESULTTYPE>::value));
        POLY p(NT(6), NT(-5), NT(1));
        IPOLY ip=to_Interval(p);
        CGAL_test_assert(CGAL::in(12.0, ip.evaluate(Interval(-1))));
        CGAL_test_assert(CGAL::in(6.0 , ip.evaluate(Interval(0))));
        CGAL_test_assert(CGAL::in(2.0 , ip.evaluate(Interval(1))));
        CGAL_test_assert(CGAL::in(0.0 , ip.evaluate(Interval(2))));
        CGAL_test_assert(CGAL::in(0.0 , ip.evaluate(Interval(3))));
    }
    {   //POLY.evaluate(Interval)
        typedef leda::integer NT;
        typedef CGAL::Polynomial<NT> POLY;
        typedef CGAL::Interval Interval;
       
        POLY p(NT(6), NT(-5), NT(1));
        CGAL_test_assert(CGAL::in(12.0, p.evaluate(Interval(-1))));
        CGAL_test_assert(CGAL::in(6.0 , p.evaluate(Interval(0))));
        CGAL_test_assert(CGAL::in(2.0 , p.evaluate(Interval(1))));
        CGAL_test_assert(CGAL::in(0.0 , p.evaluate(Interval(2))));
        CGAL_test_assert(CGAL::in(0.0 , p.evaluate(Interval(3))));
        CGAL_test_assert(CGAL::in(12.0, p.evaluate_absolute(Interval(1))));
        CGAL_test_assert(CGAL::in(12.0, p.evaluate_absolute(Interval(-1))));
        CGAL_test_assert(CGAL::in(6.0 , p.evaluate_absolute(Interval(0))));        
    }*/
    // coefficients_to
//    test_coefficients_to<leda::rational>();
#endif // CGAL_USE_LEDA        
 

#ifdef CGAL_USE_LEDA
    {        
        typedef CGAL::LEDA_arithmetic_kernel AT;
        test_AT<AT>();
    }
#endif
#ifdef CGAL_USE_CORE
    {    
        typedef CGAL::CORE_arithmetic_kernel AT;
        test_AT<AT>();
    }
#endif

    return 0;
}


