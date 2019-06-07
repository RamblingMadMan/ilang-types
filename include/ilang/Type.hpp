#ifndef ILANG_TYPE_HPP
#define ILANG_TYPE_HPP 1

#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include <optional>
#include <map>

/** \file */

namespace ilang{
	//! String encoding type
	enum class StringEncoding{
		ascii, utf8
	};

	//! Data type for type values
	struct Type{
		//! Base type of the type.
		const Type *base = nullptr;

		//! The type name as it would appear in code.
		std::string str;

		//! The type name as it would appear in binaries.
		std::string mangled;

		/**
		 * \brief Inner types of the type.
		 *
		 * This is empty for most types, but has special meaning otherwise.
		 * 
		 * For a function type, this is the list of parameter types (in-order)
		 * followed by the result type.
		 *
		 * For a sum type, this is the list of inner types.
		 * 
		 * For a product type, this is the list of inner types.
		 *
		 * For a list type, this will have a single element representing the
		 * list element type.
		 *
		 **/
		std::vector<const Type*> types;
	};

	//! \brief Used for type comparisons
	using TypeHandle = const Type*;

	/**
	 * \brief Data required for type calculations
	 *
	 * This should be treated as an opaque data type and
	 * only ever be used with the accompanying find and get functions:
	 * \code{.cpp}
	 * // possible insertions; always get a result
	 * TypeData data;
	 * 
	 * auto[typeData, int32_type] = getIntegerType(std::move(data), 32);
	 *
	 * // just a query; can get nullptr
	 * auto int_type = findIntegerType(typeData);
	 * \endcode
	 *
	 * TypeData exists this way to make state change explicit. This
	 * verbose expression of state is important for high-level interoperability
	 * with languages using the type system.
	 **/
	struct TypeData{
		TypeData();

		TypeHandle infinityType;
		TypeHandle typeType;
		TypeHandle unitType;
		TypeHandle stringType;
		TypeHandle numberType, complexType, imaginaryType, realType, rationalType, integerType, naturalType, booleanType;
		std::map<std::uint32_t, TypeHandle> sizedBooleanTypes;
		std::map<std::uint32_t, TypeHandle> sizedNaturalTypes;
		std::map<std::uint32_t, TypeHandle> sizedIntegerTypes;
		std::map<std::uint32_t, TypeHandle> sizedRationalTypes;
		std::map<std::uint32_t, TypeHandle> sizedImaginaryTypes;
		std::map<std::uint32_t, TypeHandle> sizedRealTypes;
		std::map<std::uint32_t, TypeHandle> sizedComplexTypes;
		std::map<StringEncoding, TypeHandle> encodedStringTypes;
		std::map<std::vector<TypeHandle>, TypeHandle> sumTypes;
		std::map<std::vector<TypeHandle>, TypeHandle> productTypes;
		std::vector<TypeHandle> partialTypes;
		std::vector<std::unique_ptr<Type>> storage;
	};

	/**
	 * \defgroup TypeCheckers Type checking functions
	 * \brief Functions for checking properties of types
	 * \{
	 **/

	//! Check if type is the infinity type
	bool isInfinityType(TypeHandle type) noexcept;

	//! Check if type is the type type
	bool isTypeType(TypeHandle type) noexcept;

	//! Check if type is the unit type
	bool isUnitType(TypeHandle type) noexcept;

	//! Check if type is a string type
	bool isStringType(TypeHandle type) noexcept;

	//! Check if type is a boolean type
	bool isBooleanType(TypeHandle type) noexcept;

	//! Check if type is a natural type
	bool isNaturalType(TypeHandle type) noexcept;

	//! Check if type is an integer type
	bool isIntegerType(TypeHandle type) noexcept;

	//! Check if type is a rational type
	bool isRationalType(TypeHandle type) noexcept;

	//! Check if type is a real type
	bool isRealType(TypeHandle type) noexcept;

	//! Check if type is an imaginary type
	bool isImaginaryType(TypeHandle type) noexcept;

	//! Check if type is a complex type
	bool isComplexType(TypeHandle type) noexcept;

	//! Check if type is of number type
	bool isNumberType(TypeHandle type) noexcept;
	
	//! Check if type is a function type
	bool isFunctionType(TypeHandle type) noexcept;
	
	//! Check if type is a partial type
	bool isPartialType(TypeHandle type) noexcept;

	/** \} */


	/**
	 * \defgroup TypeFinders Type finding functions
	 * \brief Functions for finding a type without modifying any state.
	 * \returns The \ref TypeHandle or nullptr if it could not be found.
	 * \{
	 **/

	//! Find a type by name
	TypeHandle findTypeByString(const TypeData &data, std::string_view str);
	
	//! Find a type by mangled name
	TypeHandle findTypeByMangled(const TypeData &data, std::string_view mangled);

	// TODO: Find the most-refined common type
	// TypeHandle commonType(TypeHandle type0, TypeHandle type1) noexcept;
	
	//! Find the infinity type
	TypeHandle findInfinityType(const TypeData &data) noexcept;

	//! Find the type type
	TypeHandle findTypeType(const TypeData &data) noexcept;
	
	//! Find the unit type
	TypeHandle findUnitType(const TypeData &data) noexcept;

	//! Find a string type
	TypeHandle findStringType(const TypeData &data, std::optional<StringEncoding> encoding = std::nullopt) noexcept;

	//! Find a boolean type
	TypeHandle findBooleanType(const TypeData &data, std::uint32_t numBits = 0) noexcept;

	//! Find a natural type
	TypeHandle findNaturalType(const TypeData &data, std::uint32_t numBits = 0) noexcept;

	//! Find an integer type
	TypeHandle findIntegerType(const TypeData &data, std::uint32_t numBits = 0) noexcept;
	
	//! Find a rational type
	TypeHandle findRationalType(const TypeData &data, std::uint32_t numBits = 0) noexcept;
	
	//! Find a real type
	TypeHandle findRealType(const TypeData &data, std::uint32_t numBits = 0) noexcept;

	//! Find an imaginary type
	TypeHandle findImaginaryType(const TypeData &data, std::uint32_t numBits = 0) noexcept;

	//! Find a complex type
	TypeHandle findComplexType(const TypeData &data, std::uint32_t numBits = 0) noexcept;

	//! Find the number type
	TypeHandle findNumberType(const TypeData &data) noexcept;
	
	//! Find a sum type
	TypeHandle findSumType(const TypeData &data, std::vector<TypeHandle> innerTypes) noexcept;
	
	//! Find a product type
	TypeHandle findProductType(const TypeData &data, const std::vector<TypeHandle> &innerTypes) noexcept;
	
	/** \} */


	/**
	 * \defgroup TypeGetters Type getting functions
	 * \brief Functions for getting a type, otherwise creating it.
	 * \returns Pair of the \ref TypeData and resulting \ref TypeHandle (in that order)
	 * \{
	 **/

	//! \brief Result type of possibly state modifying type calculations
	using TypeResult = std::pair<TypeData, TypeHandle>;

	//! Get the infinity type
	TypeResult getInfinityType(TypeData data);

	//! Get the type type
	TypeResult getTypeType(TypeData data);
	
	//! Get the unit type
	TypeResult getUnitType(TypeData data);

	//! Get a string type
	TypeResult getStringType(TypeData data, std::optional<StringEncoding> encoding = std::nullopt);

	//! Get a boolean type
	TypeResult getBooleanType(TypeData data, std::uint32_t numBits = 0);

	//! Get a natural type
	TypeResult getNaturalType(TypeData data, std::uint32_t numBits = 0);
	
	//! Get an integer type
	TypeResult getIntegerType(TypeData data, std::uint32_t numBits = 0);
	
	//! Get a rational type
	TypeResult getRationalType(TypeData data, std::uint32_t numBits = 0);
	
	//! Get an imaginary type
	TypeResult getImaginaryType(TypeData data, std::uint32_t numBits = 0);

	//! Get a real type
	TypeResult getRealType(TypeData data, std::uint32_t numBits = 0);

	//! Get a complex type
	TypeResult getComplexType(TypeData data, std::uint32_t numBits = 0);

	//! Get the number type
	TypeResult getNumberType(TypeData data);
	
	//! Get a unique incomplete type (used for unresolved expressions and partial typing)
	TypeResult getPartialType(TypeData data);
	
	//! Get a sum type
	TypeResult getSumType(TypeData data, std::vector<TypeHandle> innerTypes);

	//! Get a product type
	TypeResult getProductType(TypeData data, std::vector<TypeHandle> innerTypes);
	
	/** \} */
}

// comparing type data structures
// WARNING: This is almost never what you want
//     type comparisons should almost always
//     be done with TypeHandle's
inline bool operator==(const ilang::Type &lhs, const ilang::Type &rhs){
	return (lhs.base == rhs.base) && (lhs.mangled == rhs.mangled);
}

#endif // !ILANG_TYPE_HPP
