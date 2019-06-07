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

#define NUMBER_VALUE_TYPE(T, t, mangledSig)\
TypeHandle create##T##Type(TypeData &data, std::uint32_t numBits){\
	return createSizedNumberType(data, data.t##Type, #T, mangledSig, numBits);\
}\
TypeHandle ilang::find##T##Type(const TypeData &data, std::uint32_t numBits) noexcept{\
	return findInnerNumberType(data, data.t##Type, data.sized##T##Types, numBits);\
}\
TypeResult ilang::get##T##Type(TypeData data, std::uint32_t numBits){\
	return getInnerNumberType(data, data.t##Type, data.sized##T##Types, numBits, create##T##Type);\
}\
bool ilang::is##T##Type(TypeHandle type) noexcept{\
	return std::string_view(type->mangled).substr(0, 1) == mangledSig;\
}

#define BASE_TYPE(T, t, mangledStr)\
TypeHandle ilang::find##T##Type(const TypeData &data) noexcept{ return data.t##Type; }\
TypeResult ilang::get##T##Type(TypeData data){ return type_result(data, data.t##Type); }\
bool ilang::is##T##Type(TypeHandle type) noexcept{ return type->mangled == mangledStr; }

BASE_TYPE(Infinity, infinity, "??")
BASE_TYPE(Type, type, "t0")
BASE_TYPE(Unit, unit, "u0")
BASE_TYPE(Number, number, "w?")

NUMBER_VALUE_TYPE(Boolean, boolean, "b")
NUMBER_VALUE_TYPE(Natural, natural, "n")
NUMBER_VALUE_TYPE(Rational, rational, "q")
NUMBER_VALUE_TYPE(Real, real, "r")
NUMBER_VALUE_TYPE(Imaginary, imaginary, "i")
NUMBER_VALUE_TYPE(Complex, complex, "c")

bool ilang::isStringType(TypeHandle type) noexcept{
	return type->mangled[0] == 's';
}

bool ilang::isFunctionType(ilang::TypeHandle type) noexcept{
	return type->mangled[0] == 'f';
}

bool ilang::isPartialType(TypeHandle type) noexcept{
	return type->mangled[0] == '_';
}

template<typename Comp>
auto getSortedTypes(const TypeData &data, Comp &&comp = std::less<void>{}){
	std::vector<TypeHandle> types;
	types.reserve(data.storage.size());
	
	for(auto &&ptr : data.storage){
		types.emplace_back(ptr.get());
	}
	
	std::sort(begin(types), end(types), std::forward<Comp>(comp));
	return types;
}

TypeHandle findTypeByString(const TypeData &data, std::string_view str){
	auto types = getSortedTypes(data, [](auto lhs, auto rhs){ return lhs->str < rhs->str; });
	auto res = std::lower_bound(begin(types), end(types), str, [](TypeHandle lhs, auto rhs){ return lhs->str < rhs; });
	if(res != end(types))
		return *res;
	
	return nullptr;
}

TypeHandle findTypeByMangled(const TypeData &data, std::string_view mangled){
	auto types = getSortedTypes(data, [](auto lhs, auto rhs){ return lhs->mangled < rhs->mangled; });
	auto res = std::lower_bound(begin(types), end(types), mangled, [](TypeHandle lhs, auto rhs){ return lhs->mangled < rhs; });
	if(res != end(types))
		return *res;
	
	return nullptr;
}

/* TODO
TypeHandle ilang::commonType(TypeHandle type0, TypeHandle type1) noexcept{
	if(type0 == type1) return type0;
	else if(type0->base == type1->base) return type0->base;
}
*/

TypeHandle ilang::findStringType(const TypeData &data, std::optional<StringEncoding> encoding) noexcept{
	return findInnerType(data, data.stringType, data.encodedStringTypes, encoding);
}

TypeHandle findSumTypeInner(const TypeData &data, const std::vector<TypeHandle> &uniqueSortedInnerTypes) noexcept{
	return findInnerType(data, nullptr, data.sumTypes, std::make_optional(std::ref(uniqueSortedInnerTypes)));	
}

TypeHandle ilang::findSumType(const TypeData &data, std::vector<TypeHandle> innerTypes) noexcept{
	std::sort(begin(innerTypes), end(innerTypes));
	innerTypes.erase(std::unique(begin(innerTypes), end(innerTypes)), end(innerTypes));
	return findSumTypeInner(data, innerTypes);
}

TypeHandle ilang::findProductType(const TypeData &data, const std::vector<TypeHandle> &innerTypes) noexcept{
	return findInnerType(data, nullptr, data.productTypes, std::make_optional(std::ref(innerTypes)));
}

TypeResult ilang::getStringType(TypeData data, std::optional<StringEncoding> encoding){
	return getInnerType(data, data.stringType, data.encodedStringTypes, encoding, createEncodedStringType);
}

TypeResult ilang::getPartialType(TypeData data){
	auto type = std::make_unique<Type>();
	auto id = std::to_string(data.partialTypes.size());
	type->str = "Unique" + id;
	type->mangled = "_" + id;
	auto &&typePtr = data.storage.emplace_back(std::move(type));
	return type_result(data, typePtr.get());
}

TypeResult ilang::getSumType(TypeData data, std::vector<TypeHandle> innerTypes){
	std::sort(begin(innerTypes), end(innerTypes));
	innerTypes.erase(std::unique(begin(innerTypes), end(innerTypes)), end(innerTypes));
	
	auto res = findSumTypeInner(data, innerTypes);
	if(res)
		return type_result(data, res);
	
	auto &&newType = data.storage.emplace_back(std::make_unique<Type>());
	
	newType->base = findInfinityType(data);
	newType->types = std::move(innerTypes);
	
	newType->mangled = "u" + std::to_string(innerTypes.size());
	newType->mangled += innerTypes[0]->mangled;
	
	newType->str = innerTypes[0]->str;
	
	for(std::size_t i = 1; i < innerTypes.size(); i++){
		newType->mangled += innerTypes[i]->mangled;
		newType->str += " | " + innerTypes[i]->str;
	}
	
	auto[it, good] = data.sumTypes.try_emplace(newType->types, newType.get());
	
	if(!good){
		// TODO: throw TypeError
	}
	
	return type_result(data, newType.get());
}

TypeResult ilang::getProductType(TypeData data, std::vector<TypeHandle> innerTypes){
	if(innerTypes.size() < 2){
		// TODO: throw TypeError
	}
	
	auto res = findProductType(data, innerTypes);
	if(res)
		return type_result(data, res);
	
	auto &&newType = data.storage.emplace_back(std::make_unique<Type>());
	
	newType->base = findInfinityType(data);
	newType->types = std::move(innerTypes);
	
	newType->mangled = "p" + std::to_string(innerTypes.size()) + innerTypes[0]->mangled;
	newType->str = innerTypes[0]->str;
	
	for(std::size_t i = 1; i < innerTypes.size(); i++){
		newType->mangled += innerTypes[i]->mangled;
		newType->str += " * " + innerTypes[i]->str;
	}
	
	auto[it, good] = data.productTypes.try_emplace(newType->types, newType.get());
	
	if(!good){
		// TODO: throw TypeError
	}
	
	return type_result(data, newType.get());
}

TypeData::TypeData(){
	auto newInfinityType = [this](){
		auto &&ptr = storage.emplace_back(std::make_unique<Type>());
		ptr->base = ptr.get();
		ptr->str = "Infinity";
		ptr->mangled = "??";
		return ptr.get();
	};

	auto newType = [this](auto str, auto mangled, auto base){
		auto &&ptr = storage.emplace_back(std::make_unique<Type>());
		ptr->base = base;
		ptr->str = std::move(str);
		ptr->mangled = std::move(mangled);
		return ptr.get();
	};
	
	infinityType = newInfinityType();
	typeType = newType("Type", "t?", infinityType);
	unitType = newType("Unit", "u0", infinityType);
	stringType = newType("String", "s?", infinityType);
	numberType = newType("Number", "w?", infinityType);
	complexType = newType("Complex", "c?", numberType);
	imaginaryType = newType("Imaginary", "i?", complexType);
	realType = newType("Real", "r?", complexType);
	rationalType = newType("Rational", "q?", realType);
	integerType = newType("Integer", "z?", rationalType);
	naturalType = newType("Natural", "n?", integerType);
	booleanType = newType("Boolean", "b?", naturalType);
}
