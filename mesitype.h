#pragma once

#include <string>
#include <ratio>

namespace Mesi {
/* Utility macro for applying another macro to all known units, for internal use only */
#define ALL_UNITS(op) op(m) op(s) op(kg) op(A) op(K) op(mol) op(cd)

	/**
	 * @brief Main class to store SI types
	 *
	 * @param T storage type parameter
	 * @param t_m_n, t_m_d number of dimensions of meters as a rational number, e.g., t_m_n/t_m_d == 2 => m^2
	 * @param t_s_* similar to t_m, but seconds
	 * @param t_kg_* similar to t_m, but kilograms
	 * @param t_A_* similar to t_m, but amperes
	 * @param t_K_* similar to t_m, but Kelvin
	 * @param t_mol_* similar to t_m, but moles
	 * @param t_cd_* similar to t_m, but candela
	 * @param t_pref_* defines a rational prefix, i.e. 1,1000 for 1/1000 == milli-
	 *
	 * This class is to enforce compile-time checking, and where possible,
	 * compile-time calculation of SI values using constexpr.
	 *
	 * Note: MESI_LITERAL_TYPE may be defined to set the storage type
	 * used by the operator literal overloads
	 *
	 * Note: Fractions must be reduced, i.e. the greatest common divisor
	 * of numerator and denominator must be 1. To prevent compile-time
	 * errors, use RationalType<>, which reduces fractions automatically.
	 *
	 * @author Jameson Thatcher (a.k.a. SirEel)
	 *
	 */
	template<typename T,
		intmax_t t_m_n, intmax_t t_m_d,
		intmax_t t_s_n, intmax_t t_s_d,
		intmax_t t_kg_n, intmax_t t_kg_d,
		intmax_t t_A_n, intmax_t t_A_d,
		intmax_t t_K_n, intmax_t t_K_d,
		intmax_t t_mol_n, intmax_t t_mol_d,
		intmax_t t_cd_n, intmax_t t_cd_d,
		intmax_t t_pref>
	struct RationalTypeReduced
	{
		using BaseType = T;
		using ScalarType = RationalTypeReduced<T, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, t_pref>;

		static_assert(std::ratio<t_m_n, t_m_d>::num == t_m_n, "The meter exponent fraction is not irreducible");
		static_assert(std::ratio<t_s_n, t_s_d>::num == t_s_n, "The second exponent fraction is not irreducible");
		static_assert(std::ratio<t_kg_n, t_kg_d>::num == t_kg_n, "The kilogram exponent fraction is not irreducible");
		static_assert(std::ratio<t_A_n, t_A_d>::num == t_A_n, "The ampere exponent fraction is not irreducible");
		static_assert(std::ratio<t_K_n, t_K_d>::num == t_K_n, "The Kelvin exponent fraction is not irreducible");
		static_assert(std::ratio<t_mol_n, t_mol_d>::num == t_mol_n, "The mole exponent fraction is not irreducible");
		static_assert(std::ratio<t_cd_n, t_cd_d>::num == t_cd_n, "The candela exponent fraction is not irreducible");

		T val;

		constexpr RationalTypeReduced()
		{}

		constexpr explicit RationalTypeReduced(T const in)
			:val(in)
		{}

		constexpr RationalTypeReduced(RationalTypeReduced const& in)
			:val(in.val)
		{}

		explicit operator T() const {
			return val;
		}
		
		template<intmax_t pref2>
		explicit constexpr operator RationalTypeReduced<T, t_m_n, t_m_d, t_s_n, t_s_d, t_kg_n, t_kg_d, t_A_n, t_A_d, t_K_n, t_K_d, t_mol_n, t_mol_d, t_cd_n, t_cd_d, pref2>() const {
			T nv = val;
			intmax_t pref_diff = t_pref - pref2;
			while(pref_diff > 0)
			{
				nv *= 10;
				pref_diff--;
			}
			while(pref_diff < 0)
			{
				nv /= 10;
				pref_diff++;
			}
			return RationalTypeReduced<T, t_m_n, t_m_d, t_s_n, t_s_d, t_kg_n, t_kg_d, t_A_n, t_A_d, t_K_n, t_K_d, t_mol_n, t_mol_d, t_cd_n, t_cd_d, pref2>(nv);
		}

		/**
		 * getUnit will get a SI-style unit string for this class
		 */
		static std::string getUnit() {
			static std::string s_unitString("");
			if( s_unitString.size() > 0 )
				return s_unitString;

			if( t_pref != 0 )
			{
				s_unitString += " * 10^" + std::to_string(static_cast<long long>(t_pref)) + " ";
			}

#define DIM_TO_STRING(TP) if( t_##TP##_n == 1 && t_##TP##_d == 1 ) s_unitString += std::string(#TP) + " "; else if( t_##TP##_n != 0 && t_##TP##_d == 1) s_unitString += std::string(#TP) + "^" + std::to_string(static_cast<long long>(t_##TP##_n)) + " "; else if(t_##TP##_n != 0) s_unitString += std::string(#TP) + "^(" + std::to_string(static_cast<long long>(t_##TP##_n)) + "/" + std::to_string(static_cast<long long>(t_##TP##_d)) + ") ";
			ALL_UNITS(DIM_TO_STRING)
#undef DIM_TO_STRING
			
			s_unitString = s_unitString.substr(0, s_unitString.size() - 1);
			return s_unitString;
		}

		RationalTypeReduced& operator+=(
			RationalTypeReduced const& rhs
		) {
			return (*this) = (*this) + rhs;
		}

		RationalTypeReduced& operator-=(
			RationalTypeReduced const& rhs
		) {
			return (*this) = (*this) - rhs;
		}

		RationalTypeReduced& operator*=(T const& rhs) {
			return (*this) = (*this) * rhs;
		}

		RationalTypeReduced& operator/=(T const& rhs) {
			return (*this) = (*this) / rhs;
		}

		RationalTypeReduced& operator*=(ScalarType const& rhs) {
			return (*this) = (*this) * rhs;
		}

		RationalTypeReduced& operator/=(ScalarType const& rhs) {
			return (*this) = (*this) / rhs;
		}
	};

	template<typename T,
		intmax_t t_m_n, intmax_t t_m_d,
		intmax_t t_s_n, intmax_t t_s_d,
		intmax_t t_kg_n, intmax_t t_kg_d,
		intmax_t t_A_n, intmax_t t_A_d,
		intmax_t t_K_n, intmax_t t_K_d,
		intmax_t t_mol_n, intmax_t t_mol_d,
		intmax_t t_cd_n, intmax_t t_cd_d,
		intmax_t t_pref = 0> 
	using RationalType = RationalTypeReduced<T,
		std::ratio<t_m_n, t_m_d>::num, std::ratio<t_m_n, t_m_d>::den,
		std::ratio<t_s_n, t_s_d>::num, std::ratio<t_s_n, t_s_d>::den,
		std::ratio<t_kg_n, t_kg_d>::num, std::ratio<t_kg_n, t_kg_d>::den,
		std::ratio<t_A_n, t_A_d>::num, std::ratio<t_A_n, t_A_d>::den,
		std::ratio<t_K_n, t_K_d>::num, std::ratio<t_K_n, t_K_d>::den,
		std::ratio<t_mol_n, t_mol_d>::num, std::ratio<t_mol_n, t_mol_d>::den,
		std::ratio<t_cd_n, t_cd_d>::num, std::ratio<t_cd_n, t_cd_d>::den,
		t_pref>;

#define TYPE_A_FULL_PARAMS intmax_t t_m_n, intmax_t t_m_d, intmax_t t_s_n, intmax_t t_s_d, intmax_t t_kg_n, intmax_t t_kg_d, intmax_t t_A_n, intmax_t t_A_d, intmax_t t_K_n, intmax_t t_K_d, intmax_t t_mol_n, intmax_t t_mol_d, intmax_t t_cd_n, intmax_t t_cd_d, intmax_t t_pref
#define TYPE_A_PARAMS t_m_n, t_m_d, t_s_n, t_s_d, t_kg_n, t_kg_d, t_A_n, t_A_d, t_K_n, t_K_d, t_mol_n, t_mol_d, t_cd_n, t_cd_d, t_pref
#define TYPE_B_FULL_PARAMS intmax_t t_m_n2, intmax_t t_m_d2, intmax_t t_s_n2, intmax_t t_s_d2, intmax_t t_kg_n2, intmax_t t_kg_d2, intmax_t t_A_n2, intmax_t t_A_d2, intmax_t t_K_n2, intmax_t t_K_d2, intmax_t t_mol_n2, intmax_t t_mol_d2, intmax_t t_cd_n2, intmax_t t_cd_d2, intmax_t t_pref2
#define TYPE_B_PARAMS t_m_n2, t_m_d2, t_s_n2, t_s_d2, t_kg_n2, t_kg_d2, t_A_n2, t_A_d2, t_K_n2, t_K_d2, t_mol_n2, t_mol_d2, t_cd_n2, t_cd_d2, t_pref2
	/*
	 * Arithmatic operators for combining SI values.
	 */
	template<typename T, TYPE_A_FULL_PARAMS>
	constexpr auto operator+(
		RationalTypeReduced<T, TYPE_A_PARAMS> const& left,
		RationalTypeReduced<T, TYPE_A_PARAMS> const& right
	) {
		return RationalTypeReduced<T, TYPE_A_PARAMS>(left.val + right.val);
	}

	template<typename T, TYPE_A_FULL_PARAMS>
	constexpr auto operator-(
		RationalTypeReduced<T, TYPE_A_PARAMS> const& left,
		RationalTypeReduced<T, TYPE_A_PARAMS> const& right
	) {
		return RationalTypeReduced<T, TYPE_A_PARAMS>(left.val - right.val);
	}

	template<typename T, TYPE_A_FULL_PARAMS, TYPE_B_FULL_PARAMS>
	constexpr auto operator*(
		RationalTypeReduced<T, TYPE_A_PARAMS> const& left,
		RationalTypeReduced<T, TYPE_B_PARAMS> const& right
	) {
#define ADD_FRAC(TP) using TP = std::ratio_add<std::ratio<t_##TP##_n, t_##TP##_d>, std::ratio<t_##TP##_n2, t_##TP##_d2>>;
		ALL_UNITS(ADD_FRAC)
#undef ADD_FRAC
		return RationalType<T, m::num, m::den, s::num, s::den, kg::num, kg::den, A::num, A::den, K::num, K::den, mol::num, mol::den, cd::num, cd::den, t_pref + t_pref2>(left.val * right.val);
	}

	template<typename T, TYPE_A_FULL_PARAMS, TYPE_B_FULL_PARAMS>
	constexpr auto operator/(
		RationalTypeReduced<T, TYPE_A_PARAMS> const& left,
		RationalTypeReduced<T, TYPE_B_PARAMS> const& right
	) {
#define SUB_FRAC(TP) using TP = std::ratio_subtract<std::ratio<t_##TP##_n, t_##TP##_d>, std::ratio<t_##TP##_n2, t_##TP##_d2>>;
		ALL_UNITS(SUB_FRAC)
	#undef SUB_FRAC
		return RationalType<T, m::num, m::den, s::num, s::den, kg::num, kg::den, A::num, A::den, K::num, K::den, mol::num, mol::den, cd::num, cd::den, t_pref - t_pref2>(left.val / right.val);
	}

	/*
	 * Unary operators, to help with literals (and general usage!)
	 */

	template<typename T, TYPE_A_FULL_PARAMS>
	constexpr auto operator-(
		RationalTypeReduced<T, TYPE_A_PARAMS> const& op
	) {
		return RationalTypeReduced<T, TYPE_A_PARAMS>(-op.val);
	}

	template<typename T, TYPE_A_FULL_PARAMS>
	constexpr auto operator+(
			RationalTypeReduced<T, TYPE_A_PARAMS> const& op
	) {
		return RationalTypeReduced<T, TYPE_A_PARAMS>(op);
	}

	/*
	 * Scalers by non-SI values (allows things like 2 * 3._m
	 */

	template<typename T, TYPE_A_FULL_PARAMS, typename S>
	constexpr auto operator*(
		RationalTypeReduced<T, TYPE_A_PARAMS> const& left,
		S const& right
	) {
		return RationalTypeReduced<T, TYPE_A_PARAMS>(left.val * right);
	}

	template<typename T, TYPE_A_FULL_PARAMS, typename S>
	constexpr auto operator*(
		S const & left,
		RationalTypeReduced<T, TYPE_A_PARAMS> const& right
	) {
		return right * left;
	}

	template<typename T, TYPE_A_FULL_PARAMS, typename S>
	constexpr auto operator/(
		RationalTypeReduced<T, TYPE_A_PARAMS> const& left,
		S const& right
	) {
		using Scalar = typename RationalTypeReduced<T, TYPE_A_PARAMS>::ScalarType;
		return left / Scalar(T(right));
	}

	template<typename T, TYPE_A_FULL_PARAMS, typename S>
	constexpr auto operator/(
		S const & left,
		RationalTypeReduced<T, TYPE_A_PARAMS> const& right
	) {
		using Scalar = typename RationalTypeReduced<T, TYPE_A_PARAMS>::ScalarType;
		return Scalar(T(left)) / right;
	}

	/*
	 * Comparison operators
	 */
	template<typename T, TYPE_A_FULL_PARAMS>
	constexpr bool operator==(
		RationalTypeReduced<T, TYPE_A_PARAMS> const& left,
		RationalTypeReduced<T, TYPE_A_PARAMS> const& right
	) {
		return left.val == right.val;
	}

	template<typename T, TYPE_A_FULL_PARAMS>
	constexpr bool operator<(
		RationalTypeReduced<T, TYPE_A_PARAMS> const& left,
		RationalTypeReduced<T, TYPE_A_PARAMS> const& right
	) {
		return left.val < right.val;
	}

	template<typename T, TYPE_A_FULL_PARAMS>
	constexpr bool operator!=(
		RationalTypeReduced<T, TYPE_A_PARAMS> const& left,
		RationalTypeReduced<T, TYPE_A_PARAMS> const& right
	) {
		return !(right == left);
	}

	template<typename T, TYPE_A_FULL_PARAMS>
	constexpr bool operator<=(
		RationalTypeReduced<T, TYPE_A_PARAMS> const& left,
		RationalTypeReduced<T, TYPE_A_PARAMS> const& right
	) {
		return left < right || left == right;
	}

	template<typename T, TYPE_A_FULL_PARAMS>
	constexpr bool operator>(
		RationalTypeReduced<T, TYPE_A_PARAMS> const& left,
		RationalTypeReduced<T, TYPE_A_PARAMS> const& right
	) {
		return right < left;
	}

	template<typename T, TYPE_A_FULL_PARAMS>
	constexpr bool operator>=(
		RationalTypeReduced<T, TYPE_A_PARAMS> const& left,
		RationalTypeReduced<T, TYPE_A_PARAMS> const& right
	) {
		return left > right || left == right;
	}

#undef TYPE_A_FULL_PARAMS
#undef TYPE_A_PARAMS
#undef TYPE_B_FULL_PARAMS
#undef TYPE_B_PARAMS
#undef ALL_UNITS

	/*
	 * Readable names for common types
	 */

	template<typename T, intmax_t t_m, intmax_t t_s, intmax_t t_kg, intmax_t t_A=0, intmax_t t_K=0, intmax_t t_mol=0, intmax_t t_cd=0, intmax_t t_pref = 0>
	using Type = RationalType<T, t_m, 1, t_s, 1, t_kg, 1, t_A, 1, t_K, 1, t_mol, 1, t_cd, 1, t_pref>;

#ifndef MESI_LITERAL_TYPE
#	define MESI_LITERAL_TYPE float
#endif

	using Scalar    = Type<MESI_LITERAL_TYPE, 0, 0, 0>;
	using Meters    = Type<MESI_LITERAL_TYPE, 1, 0, 0>;
	using Seconds   = Type<MESI_LITERAL_TYPE, 0, 1, 0>;
	using Kilograms = Type<MESI_LITERAL_TYPE, 0, 0, 1>;
	using Amperes   = Type<MESI_LITERAL_TYPE, 0, 0, 0, 1>;
	using Kelvin    = Type<MESI_LITERAL_TYPE, 0, 0, 0, 0, 1>;
	using Moles     = Type<MESI_LITERAL_TYPE, 0, 0, 0, 0, 0, 1>;
	using Candela   = Type<MESI_LITERAL_TYPE, 0, 0, 0, 0, 0, 0, 1>;
	using Newtons   = decltype(Meters{} * Kilograms{} / Seconds{} / Seconds{});
	using NewtonsSq = decltype(Newtons{} * Newtons{});
	using MetersSq  = decltype(Meters{} * Meters{});
	using MetersCu  = decltype(Meters{} * MetersSq{});
	using SecondsSq = decltype(Seconds{} * Seconds{});
	using KilogramsSq = decltype(Kilograms{} * Kilograms{});
	using Hertz     = decltype(Scalar{} / Seconds{});
	using Pascals   = decltype(Newtons{} / MetersSq{});
	using Joules    = decltype(Newtons{} * Meters{});
	using Watts     = decltype(Joules{} / Seconds{});
	using Coulombs  = decltype(Amperes{} * Seconds{});
	using Volts     = decltype(Watts{} / Amperes{});
	using Farads    = decltype(Coulombs{} / Volts{});
	using Ohms      = decltype(Volts{} / Amperes{});
	using Siemens   = decltype(Amperes{} / Volts{});
	using Webers    = decltype(Volts{} * Seconds{});
	using Tesla     = decltype(Webers{} / MetersSq{});
	using Henry     = decltype(Webers{} / Amperes{});
	using Kilo      = Type<MESI_LITERAL_TYPE, 0, 0, 0, 0, 0, 0, 0, 3>;
	using Mega      = decltype(Kilo{} * Kilo{});
	using Giga      = decltype(Mega{} * Kilo{});
	using Tera      = decltype(Giga{} * Kilo{});
	using Peta      = decltype(Tera{} * Kilo{});
	using Exa       = decltype(Peta{} * Kilo{});
	using Zetta     = decltype(Exa{} * Kilo{});
	using Yota      = decltype(Zetta{} * Kilo{});
	using Milli     = decltype(Scalar{} / Kilo{});
	using Micro     = decltype(Milli{} / Kilo{});
	using Nano      = decltype(Micro{} / Kilo{});
	using Pico      = decltype(Nano{} / Kilo{});
	using Femto     = decltype(Pico{} / Kilo{});
	using Atto      = decltype(Femto{} / Kilo{});
	using Zepto     = decltype(Atto{} / Kilo{});
	using Yocto     = decltype(Zepto{} / Kilo{});

	namespace Literals {
	/*
	 * Literal operators, to allow quick creation of basic types
	 * Note that this defaults to the type set below, if no other is set
	 * before calling!
	 *
	 * These are all lowercase, as identifiers beginning with
	 * _[A-Z] are reserved.
	 */
#define LITERAL_TYPE(T, SUFFIX) \
			constexpr auto operator "" SUFFIX(long double arg) { return T(arg); } \
			constexpr auto operator "" SUFFIX(unsigned long long arg) { return T(arg); }

		LITERAL_TYPE(Mesi::Meters, _m)
		LITERAL_TYPE(Mesi::MetersSq, _m2)
		LITERAL_TYPE(Mesi::MetersCu, _m3)
		LITERAL_TYPE(Mesi::Seconds, _s)
		LITERAL_TYPE(Mesi::SecondsSq, _s2)
		LITERAL_TYPE(Mesi::Kilograms, _kg)
		LITERAL_TYPE(Mesi::KilogramsSq, _kg2)
		LITERAL_TYPE(Mesi::Newtons, _n)
		LITERAL_TYPE(Mesi::NewtonsSq, _n2)
		LITERAL_TYPE(Mesi::Hertz, _hz)
		LITERAL_TYPE(Mesi::Amperes, _a)
		LITERAL_TYPE(Mesi::Kelvin, _k)
		LITERAL_TYPE(Mesi::Moles, _mol)
		LITERAL_TYPE(Mesi::Candela, _cd)
		LITERAL_TYPE(Mesi::Pascals, _pa)
		LITERAL_TYPE(Mesi::Joules, _j)
		LITERAL_TYPE(Mesi::Watts, _w)
		LITERAL_TYPE(Mesi::Coulombs, _c)
		LITERAL_TYPE(Mesi::Volts, _v)
		LITERAL_TYPE(Mesi::Farads, _f)
		LITERAL_TYPE(Mesi::Ohms, _ohm)
		LITERAL_TYPE(Mesi::Siemens, _siemens)
		LITERAL_TYPE(Mesi::Webers, _wb)
		LITERAL_TYPE(Mesi::Tesla, _t)
		LITERAL_TYPE(Mesi::Henry, _h)
#undef LITERAL_TYPE
	}
}
