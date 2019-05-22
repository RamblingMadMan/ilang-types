#include "ilang/Type.hpp"

using namespace ilang;

TypeHandle createEncodedStringType(TypeData &data, StringEncoding encoding){
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
)
{
	if(numBits == 0) return base;

	auto bitsStr = std::to_string(numBits);
	auto type = std::make_unique<Type>();

	type->base = base;
	type->str = name + bitsStr;
	type->mangled = mangledName + bitsStr;

	return data.storage.emplace_back(std::move(type)).get();
}

TypeHandle createNaturalType(TypeData &data, std::uint32_t numBits){
	return createSizedNumberType(data, data.naturalType, "Natural", "n", numBits);
}

TypeHandle createIntegerType(TypeData &data, std::uint32_t numBits){
	return createSizedNumberType(data, data.naturalType, "Integer", "i", numBits);
}

TypeHandle createRationalType(TypeData &data, std::uint32_t numBits){
	return createSizedNumberType(data, data.naturalType, "Rational", "q", numBits);
}

TypeHandle createRealType(TypeData &data, std::uint32_t numBits){
	return createSizedNumberType(data, data.naturalType, "Real", "r", numBits);
}

TypeResult type_result(TypeData &data, TypeHandle t){
	return {std::move(data), t};
}

template<typename Container, typename CreateFn>
TypeResult getInnerNumberType(
	TypeData &data, TypeHandle base,
	Container &&container, std::uint32_t numBits,
	CreateFn createFn
){
	if(numBits){
		auto res = container.find(numBits);
		if(res != end(container))
			return type_result(data, res->second);

		return type_result(data, createFn(data, numBits));
	}

	return type_result(data, base);
}

TypeResult ilang::getUnitType(TypeData data){
	return type_result(data, data.unitType);
}

TypeResult ilang::getStringType(TypeData data, std::optional<StringEncoding> encoding){
	if(encoding){
		auto res = data.encodedStringTypes.find(*encoding);
		if(res != end(data.encodedStringTypes))
			return type_result(data, res->second);

		return type_result(data, createEncodedStringType(data, *encoding));
	}
	else
		return type_result(data, data.stringType);
}

TypeResult ilang::getNaturalType(TypeData data, std::uint32_t numBits){
	return getInnerNumberType(data, data.naturalType, data.sizedNaturalTypes, numBits, createNaturalType);
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
