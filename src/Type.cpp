#include <algorithm>

#include "ilang/Type.hpp"

using namespace ilang;

TypeHandle createEncodedStringType(TypeData &data, StringEncoding encoding) noexcept{
	auto type = std::make_unique<Type>();
	type->base = data.stringType;

	switch(encoding){
		case StringEncoding::ascii: type->str = "AsciiString"; type->mangled = "sa8"; break;
		case StringEncoding::utf8:  type->str = "Utf8String"; type->mangled = "su8"; break;
		default: return nullptr;
	}

	return data.storage.emplace_back(std::move(type)).get();
}

TypeHandle createSizedNumberType(
	TypeData &data, TypeHandle base,
	const std::string &name, const std::string &mangledName,
	std::uint32_t numBits
) noexcept
{
	if(numBits == 0) return base;

	auto bitsStr = std::to_string(numBits);
	auto type = std::make_unique<Type>();

	type->base = base;
	type->str = name + bitsStr;
	type->mangled = mangledName + bitsStr;

	return data.storage.emplace_back(std::move(type)).get();
}

TypeHandle createNaturalType(TypeData &data, std::uint32_t numBits) noexcept{
	return createSizedNumberType(data, data.naturalType, "Natural", "n", numBits);
}

TypeHandle createIntegerType(TypeData &data, std::uint32_t numBits) noexcept{
	return createSizedNumberType(data, data.naturalType, "Integer", "i", numBits);
}

TypeHandle createRationalType(TypeData &data, std::uint32_t numBits) noexcept{
	return createSizedNumberType(data, data.naturalType, "Rational", "q", numBits);
}

TypeHandle createRealType(TypeData &data, std::uint32_t numBits) noexcept{
	return createSizedNumberType(data, data.naturalType, "Real", "r", numBits);
}

TypeResult type_result(TypeData &data, TypeHandle t) noexcept{
	std::sort(begin(data.storage), end(data.storage));
	return {std::move(data), t};
}

template<typename Container, typename Key>
TypeHandle findInnerType(const TypeData &data, TypeHandle base, const Container &cont, std::optional<Key> key) noexcept{
	if(key){
		auto res = cont.find(*key);
		if(res != end(cont))
			return res->second;

		return nullptr;
	}
	else
		return base;
}

template<typename Container>
TypeHandle findInnerNumberType(const TypeData &data, TypeHandle base, const Container &cont, std::uint32_t numBits) noexcept{
	return findInnerType(data, base, cont, numBits ? std::make_optional(numBits) : std::nullopt);
}

template<typename Comp>
auto getSortedTypes(const TypeData &data, Comp &&comp = std::less<>{}){
	std::vector<TypeHandle> types;
	types.reserve(data.storage.size());
	
	for(auto &&ptr : data.storage){
		types.emplace_back(ptr.get());
	}
	
	std::sort(begin(types), end(types), std::forward<Comp>(comp));
	return types;
}

TypeHandle findTypeByString(const TypeData &data, std::string_view str){
	auto typeStrCmp = [](TypeHandle lhs, TypeHandle rhs){ return lhs->str < rhs->str; };
	auto types = getSortedTypes(data, typeStrCmp);
	auto res = std::lower_bound(begin(types), end(types), typeStrCmp);
	if(res != end(types))
		return *res;
	
	return nullptr;
}

TypeHandle findTypeByMangled(const TypeData &data, std::string_view mangled){
	auto typeMangledCmp = [](TypeHandle lhs, TypeHandle rhs){ return lhs->mangled < rhs->mangled; };
	auto types = getSortedTypes(data, typeMangledCmp);
	auto res = std::lower_bound(begin(types), end(types), typeMangledCmp);
	if(res != end(types))
		return *res;
	
	return nullptr;
}

TypeHandle ilang::findUnitType(const TypeData &data) noexcept{
	return data.unitType;
}

TypeHandle ilang::findStringType(const TypeData &data, std::optional<StringEncoding> encoding) noexcept{
	return findInnerType(data, data.stringType, data.encodedStringTypes, encoding);
}

TypeHandle ilang::findNaturalType(const TypeData &data, std::uint32_t numBits) noexcept{
	return findInnerNumberType(data, data.naturalType, data.sizedNaturalTypes, numBits);
}

TypeHandle ilang::findIntegerType(const TypeData &data, std::uint32_t numBits) noexcept{
	return findInnerNumberType(data, data.integerType, data.sizedIntegerTypes, numBits);
}

TypeHandle ilang::findRationalType(const TypeData &data, std::uint32_t numBits) noexcept{
	return findInnerNumberType(data, data.rationalType, data.sizedRationalTypes, numBits);
}

TypeHandle ilang::findRealType(const TypeData &data, std::uint32_t numBits) noexcept{
	return findInnerNumberType(data, data.realType, data.sizedRealTypes, numBits);
}

template<typename Container, typename Key, typename Create>
TypeResult getInnerType(
	TypeData &data, TypeHandle base,
	Container &&container, std::optional<Key> key,
	Create &&create
){
	auto res = findInnerType(data, base, container, key);
	if(res) return type_result(data, res);
	else return type_result(data, create(data, *key));
}

template<typename Container, typename Create>
TypeResult getInnerNumberType(
	TypeData &data, TypeHandle base,
	Container &&container, std::uint32_t numBits,
	Create &&create
){
	return getInnerType(
		data, base, container,
		numBits ? std::make_optional(numBits) : std::nullopt,
		std::forward<Create>(create)
	);
}

TypeResult ilang::getUnitType(TypeData data){
	return type_result(data, data.unitType);
}

TypeResult ilang::getNaturalType(TypeData data, std::uint32_t numBits){
	return getInnerNumberType(data, data.naturalType, data.sizedNaturalTypes, numBits, createNaturalType);
}

TypeResult ilang::getStringType(TypeData data, std::optional<StringEncoding> encoding){
	return getInnerType(data, data.stringType, data.encodedStringTypes, encoding, createEncodedStringType);
}

TypeResult ilang::getIntegerType(TypeData data, std::uint32_t numBits){
	return getInnerNumberType(data, data.integerType, data.sizedIntegerTypes, numBits, createIntegerType);
}

TypeResult ilang::getRationalType(TypeData data, std::uint32_t numBits){
	return getInnerNumberType(data, data.rationalType, data.sizedRationalTypes, numBits, createRationalType);
}

TypeResult ilang::getRealType(TypeData data, std::uint32_t numBits){
	return getInnerNumberType(data, data.realType, data.sizedRealTypes, numBits, createRealType);
}

TypeResult ilang::getPartialType(TypeData data){
	auto type = std::make_unique<Type>();
	auto id = std::to_string(data.partialTypes.size());
	type->str = "Unique" + id;
	type->mangled = "_" + id;
	auto &&typePtr = data.storage.emplace_back(std::move(type));
	return type_result(data, typePtr.get());
}

TypeData::TypeData(){
	auto unitT = new Type;
	auto strT = new Type;
	auto numT = new Type;
	auto natT = new Type;
	auto intT = new Type;
	auto ratioT = new Type;
	auto realT = new Type;

	unitT->str = "Unit";
	unitT->mangled = "u0";

	strT->str = "String";
	strT->mangled = "s?";

	numT->str = "Number";
	numT->mangled = "w?";

	natT->base = numT;
	natT->str = "Natural";
	natT->mangled = "n?";

	intT->base = numT;
	intT->str = "Integer";
	intT->mangled = "i?";

	ratioT->base = numT;
	ratioT->str = "Rational";
	ratioT->mangled = "q?";

	realT->base = numT;
	realT->str = "Real";
	realT->mangled = "r?";

	unitType = unitT;
	stringType = strT;
	numberType = numT;
	naturalType = natT;
	integerType = intT;
	rationalType = ratioT;
	realType = realT;
}
