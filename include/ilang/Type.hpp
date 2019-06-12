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
		TypeHandle partialType;
		TypeHandle typeType;
		TypeHandle unitType;
		TypeHandle stringType;
		TypeHandle numberType, complexType, imaginaryType, realType, rationalType, integerType, naturalType, booleanType;
		TypeHandle functionType;
		std::map<std::uint32_t, TypeHandle> sizedBooleanTypes;
		std::map<std::uint32_t, TypeHandle> sizedNaturalTypes;
		std::map<std::uint32_t, TypeHandle> sizedIntegerTypes;
		std::map<std::uint32_t, TypeHandle> sizedRationalTypes;
		std::map<std::uint32_t, TypeHandle> sizedImaginaryTypes;
		std::map<std::uint32_t, TypeHandle> sizedRealTypes;
		std::map<std::uint32_t, TypeHandle> sizedComplexTypes;
		std::map<StringEncoding, TypeHandle> encodedStringTypes;
		std::map<std::vector<TypeHandle>, std::map<TypeHandle, TypeHandle>> functionTypes;
		std::map<std::vector<TypeHandle>, TypeHandle> sumTypes;
		std::map<std::vector<TypeHandle>, TypeHandle> productTypes;
		std::vector<TypeHandle> partialTypes;
		std::vector<std::unique_ptr<Type>> storage;
	};

	/**
	 * \defgroup RefinementCheckers Type refinement checking
	 * \brief Functions for checking refinement of types
	 * \{
	 **/

	bool hasBaseType(TypeHandle type, TypeHandle baseType) noexcept;

	bool isRootType(TypeHandle type) noexcept;
	bool isRefinedType(TypeHandle type) noexcept;
	bool isValueType(TypeHandle type) noexcept;
	bool isCompoundType(TypeHandle type) noexcept;

	/** \} */

	/**
	 * \defgroup RootTypeCheckers Root type checking
	 * \brief Functions for checking root types
	 * \{
	 **/

	bool isUnitType(TypeHandle type, const TypeData &data) noexcept;
	bool isTypeType(TypeHandle type, const TypeData &data) noexcept;
	bool isPartialType(TypeHandle type, const TypeData &data) noexcept;
	bool isFunctionType(TypeHandle type, const TypeData &data) noexcept;
	bool isNumberType(TypeHandle type, const TypeData &data) noexcept;
	bool isStringType(TypeHandle type, const TypeData &data) noexcept;

	/** \} */

	/**
	 * \defgroup NumberTypeCheckers Number type checking
	 * \brief Functions for checking numeric types
	 * \{
	 **/

	bool isComplexType(TypeHandle type, const TypeData &data) noexcept;
	bool isImaginaryType(TypeHandle type, const TypeData &data) noexcept;
	bool isRealType(TypeHandle type, const TypeData &data) noexcept;
	bool isRationalType(TypeHandle type, const TypeData &data) noexcept;
	bool isIntegerType(TypeHandle type, const TypeData &data) noexcept;
	bool isNaturalType(TypeHandle type, const TypeData &data) noexcept;
	bool isBooleanType(TypeHandle type, const TypeData &data) noexcept;

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

	//! Find the most-refined common type
	TypeHandle findCommonType(TypeHandle type0, TypeHandle type1) noexcept;
	
	//! Find the infinity type
	TypeHandle findInfinityType(const TypeData &data) noexcept;

	//! Find the type type
	TypeHandle findTypeType(const TypeData &data) noexcept;
	
	//! Find the unit type
	TypeHandle findUnitType(const TypeData &data) noexcept;

	//! Find a partial type
	TypeHandle findPartialType(const TypeData &data, std::optional<std::uint32_t> id = std::nullopt) noexcept;

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

	//! Find function base type
	TypeHandle findFunctionType(const TypeData &data) noexcept;

	//! Find a function type
	TypeHandle findFunctionType(const TypeData &data, const std::vector<TypeHandle> &params, TypeHandle result) noexcept;
	
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
	
	//! Get a unique partial (incomplete) type
	TypeResult getPartialType(TypeData data);

	//! Get a function type
	TypeResult getFunctionType(TypeData data, std::vector<TypeHandle> args, TypeHandle ret);
	
	//! Get a sum type
	TypeResult getSumType(TypeData data, std::vector<TypeHandle> innerTypes = {});

	//! Get a product type
	TypeResult getProductType(TypeData data, std::vector<TypeHandle> innerTypes = {});
	
	/** \} */
}

#endif // !ILANG_TYPE_HPP
