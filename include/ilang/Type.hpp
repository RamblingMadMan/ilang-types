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
	 * should be only ever be used in the following way:
	 * \code{.cpp}
	 * TypeData data;
	 * TypeHandle result;
	 * std::tie(data, result) = get_Type(std::move(data), ...);
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
		std::map<std::vector<TypeHandle>, TypeHandle> 
		std::vector<std::unique_ptr<Type>> storage;
	};

	//! Result type of all type calculations
	using TypeResult = std::pair<TypeData, TypeHandle>;

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
}

// comparing type data structures
// WARNING: This is almost never what you want
//     type comparisons should almost always
//     be done with TypeHandle's
inline bool operator==(const ilang::Type &lhs, const ilang::Type &rhs){
	return (lhs.base == rhs.base) && (lhs.mangled == rhs.mangled);
}

#endif // !ILANG_TYPE_HPP
