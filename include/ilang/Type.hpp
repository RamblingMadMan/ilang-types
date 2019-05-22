#ifndef ILANG_TYPE_HPP
#define ILANG_TYPE_HPP 1

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <map>

namespace ilang{
	//! String encoding type
	enum class StringEncoding{
		ascii, utf8
	};

	//! Type for type values
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

	//! Used to pass around types
	using TypeHandle = const Type*;

	/**
	 * \brief Type data required to make type calculations
	 *
	 * This should be treated as an Opaque data structure and
	 * only ever be used with the accompanying find and get functions:
	 * \code{.cpp}
	 * // possible insertions; always get a result
	 * TypeData data;
	 * TypeHandle result;
	 * std::tie(data, result) = get___Type(std::move(data), ...);
	 *
	 * // just a query; can get nullptr
	 * TypeHandle otherResult = find___Type(data, ...);
	 * \endcode
	 *
	 * TypeData exists this way to make state change explicit. This
	 * verbose expression of state is important for high-order interoperability
	 * with languages using this type system.
	 **/
	struct TypeData{
		TypeData();

		TypeHandle unitType;
		TypeHandle stringType;
		TypeHandle numberType, naturalType, integerType, rationalType, realType;
		std::map<std::uint32_t, TypeHandle> sizedNaturalTypes;
		std::map<std::uint32_t, TypeHandle> sizedIntegerTypes;
		std::map<std::uint32_t, TypeHandle> sizedRationalTypes;
		std::map<std::uint32_t, TypeHandle> sizedRealTypes;
		std::map<StringEncoding, TypeHandle> encodedStringTypes;
		std::map<std::vector<TypeHandle>, TypeHandle> sumTypes;
		std::map<std::vector<TypeHandle>, TypeHandle> productTypes;
		std::vector<TypeHandle> partialTypes;
		std::vector<std::unique_ptr<Type>> storage;
	};

	//! Result type of possibly state modifying type calculations
	using TypeResult = std::pair<TypeData, TypeHandle>;

	/****************************************
	 *
	 *     Type finding functions
	 *
	 ****************************************/

	//! find a type by name
	TypeHandle findTypeByString(const TypeData &data, std::string_view str);
	
	//! find a type by mangled name
	TypeHandle findTypeByMangled(const TypeData &data, std::string_view mangled);
	
	//! find the unit type
	TypeHandle findUnitType(const TypeData &data) noexcept;

	//! find a string type
	TypeHandle findStringType(const TypeData &data, std::optional<StringEncoding> encoding = std::nullopt) noexcept;

	//! find a natural type
	TypeHandle findNaturalType(const TypeData &data, std::uint32_t numBits = 0) noexcept;

	//! find an integer type
	TypeHandle findIntegerType(const TypeData &data, std::uint32_t numBits = 0) noexcept;
	
	//! find a rational type
	TypeHandle findRationalType(const TypeData &data, std::uint32_t numBits = 0) noexcept;
	
	//! find a real type
	TypeHandle findRealType(const TypeData &data, std::uint32_t numBits = 0) noexcept;


	/****************************************
	 *
	 *     Type getting functions
	 *     these create new types if no
	 *     result is found
	 *
	 ****************************************/

	//! Get the unit type
	TypeResult getUnitType(TypeData data);

	//! Get a string type
	TypeResult getStringType(TypeData data, std::optional<StringEncoding> encoding = std::nullopt);

	//! Get a natural type
	TypeResult getNaturalType(TypeData data, std::uint32_t numBits = 0);
	
	//! Get an integer type
	TypeResult getIntegerType(TypeData data, std::uint32_t numBits = 0);
	
	//! Get a rational type
	TypeResult getRationalType(TypeData data, std::uint32_t numBits = 0);
	
	//! Get a real type
	TypeResult getRealType(TypeData data, std::uint32_t numBits = 0);
	
	//! Get a unique incomplete type (used for unresolved expression types)
	TypeResult getPartialType(TypeData data);
}

// comparing type data structures
// WARNING: This is almost never what you want
//     type comparisons should almost always
//     be done with TypeHandle's
inline bool operator==(const ilang::Type &lhs, const ilang::Type &rhs){
	return (lhs.base == rhs.base) && (lhs.mangled == rhs.mangled);
}

#endif // !ILANG_TYPE_HPP
