/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2021. All rights reserved.
 */

#ifndef UNITTESTBOT_TYPES_H
#define UNITTESTBOT_TYPES_H

#include "AbstractType.h"
#include "Language.h"
#include "exceptions/NoSuchTypeException.h"
#include "utils/CollectionUtils.h"
#include "utils/ExecUtils.h"
#include "utils/StringUtils.h"

#include <clang/AST/Type.h>
#include <protobuf/util.pb.h>
#include <tsl/ordered_set.h>

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace types {
    using TypeName = std::string;
    using Kind = std::shared_ptr<AbstractType>;
    struct FunctionInfo;

    enum class PointerUsage;
    enum class ReferenceType;

    class Type {
    public:
        Type() = default;

        explicit Type(clang::QualType qualType, TypeName usedTypeName, const clang::SourceManager &sourceManager);

        /**
         * @return string representation of this type.
         */
        [[nodiscard]] TypeName typeName() const;

        /**
         * @return string representation of this type without qualifiers, references and arrays.
         */
        [[nodiscard]] TypeName baseType() const;

        /**
         * Returns string representation of this type that was actually used in source code.
         * @return typename that was used in code.
         */
        [[nodiscard]] TypeName usedType() const;

        /**
         * Returns vector that stores information about type:
         * pointers/arrays that were applied in the right order.
         * The last element is always of type SimpleType.
         * @return vector of kinds
         */
        [[nodiscard]] const std::vector<std::shared_ptr<AbstractType>> &kinds() const;

        /**
         *
         * @return if type is a pointer to function
         */
        [[nodiscard]] bool isPointerToFunction() const;

        /**
         *
         * @return if type is an array of pointers to function
         */
        [[nodiscard]] bool isArrayOfPointersToFunction() const;

        /**
         * Same as kinds(), but returns only elements that are PointerType and ArrayType.
         * @return vector of kinds
         */
        [[nodiscard]] std::vector<std::shared_ptr<AbstractType>> pointerArrayKinds() const;

        /**
         * @return Type object of a base type at the depth of 'depth'.
         */
        [[nodiscard]] Type baseTypeObj(size_t depth) const;

        /**
         * @return Type object of a base type.
         */
        [[nodiscard]] Type baseTypeObj() const;

        /**
         * @return String mType
         */
        [[nodiscard]] std::string mTypeName() const;


        /**
         * @return Type object where the first Kind is not a pointer, but an array
         */
        [[nodiscard]] Type arrayClone(PointerUsage usage, size_t pointerSize = 0) const;

        /**
         * @return Type object where first N kinds is not a pointer, but an array
         * where N is size of @pointerSizes
         * @pointerSizes contains sizes of arrays
         */
        [[nodiscard]] Type arrayCloneMultiDim(PointerUsage usage, std::vector<size_t> pointerSizes) const;

        /**
         * @return baseType if called on pointerObject, else return Type object,
         * where leading pointers are replaced to array with default sizes
         */
        [[nodiscard]] Type arrayCloneMultiDim(PointerUsage usage) const;

        /**
         * Checks whether given type is a pointer.
         * @return true if type is an object pointer, false otherwise.
         */
        [[nodiscard]] bool isArrayCandidate() const;

        /**
         * Checks whether given type is a pointer.
         * @return true if type is an object pointer, false otherwise.
         */
        [[nodiscard]] bool isObjectPointer() const;

        /**
         * Checks whether it is one dimension pointer.
         * @return true if type is a one dimension pointer, false otherwise.
         */
        [[nodiscard]] bool isOneDimensionPointer() const;

        /**
         * Checks whether given type is an array.
         * @return true if type is an array, false otherwise.
         */
        [[nodiscard]] bool isArray() const;

        /**
         * Checks whether given type is a simple type (not an array and not a pointer).
         * @return true if type is simple, false otherwise.
         */
        [[nodiscard]] bool isSimple() const;

        /**
         * Checks whether given type is an unnamed type.
         * @return true if type is unnamed, false otherwise.
         */
        [[nodiscard]] bool isUnnamed() const;

        /**
         * Checks whether given type is an lvalue reference type.
         * @return true if type is lvalue reference, false otherwise.
         */
        [[nodiscard]] bool isLValueReference() const;

        /**
         * Checks whether given type is an const qualified.
         * @return true if type is const, false otherwise.
         */
        [[nodiscard]] bool isConstQualified() const;

        /**
         * Checks whether given type is a pointer to pointer.
         * @return true if type is a pointer to pointer, false otherwise.
         */
        [[nodiscard]] bool isPointerToPointer() const;

        /**
         * Checks whether given type is a two dimensional pointer.
         * Example: int ** arr;
         * @return true if type is a pointer to pointer, false otherwise.
         */
        [[nodiscard]] bool isTwoDimensionalPointer() const;

        /**
         * Checks whether given type is a pointer to an array.
         * @return true if type is a pointer to an array, false otherwise.
         */
        [[nodiscard]] bool isPointerToArray() const;

        /**
         * Checks whether given type is one or multi dimensional pointer or array refer to const value.
         * @return true if type is refer to const value, false otherwise.
         */
        [[nodiscard]] bool isConstQualifiedValue() const;

        [[nodiscard]] bool isTypeContainsPointer() const;

        [[nodiscard]] bool isTypeContainsFunctionPointer() const;

        [[nodiscard]] int indexOfFirstPointerInTypeKinds() const;

        [[nodiscard]] std::vector<size_t> arraysSizes(PointerUsage usage) const;

        /**
         * Creates type from its name. Created type satisfies following:
         * typeName() == type && baseType() == type && usedType() == type && isSimple() == true;
         * @param type - name of type
         * @return Type wrapper for given typename.
         */
        static Type createSimpleTypeFromName(const TypeName &type, size_t pointersNum=0);

        /**
         * Replace current usedType with a new one.
         * @param type - name of type
         * @return
         */
        void replaceUsedType(const TypeName &newUsedType);

        /**
         * Creates type from its name. Created type satisfies following:
         * typeName() == "const " + type && baseType() == type && usedType() == typeName();
         * @param type - name of type
         * @return Type wrapper for given typename.
         */
        static Type createConstTypeFromName(const TypeName &type, size_t pointersNum=0);

        static Type createArray(const Type &type);

        static Type CStringType();

        static Type intType();

        static Type longlongType();

        static Type minimalScalarType();

        static Type minimalScalarPointerType(size_t pointersNum=1);

        size_t getDimension() const;

        bool maybeJustPointer() const;

        bool maybeReturnArray() const;

        size_t countReturnPointers(bool decrementIfArray = false) const;

        std::optional<uint64_t> getBaseTypeId() const;

        bool maybeArray = false;

        static const size_t symStdinSize = 64;
        static const std::string &getStdinParamName();
    private:

        explicit Type(const TypeName& type, size_t pointersNum=0);

        TypeName mType;
        TypeName mBaseType;
        TypeName mUsedType;
        std::vector<Kind> mKinds;
        size_t dimension;
        std::optional<uint64_t> mTypeId;
        std::optional<uint64_t> mBaseTypeId;

    public:
        uint64_t getId() const;

        static uint64_t getIdFromCanonicalType(clang::QualType canonicalType);
    };

    struct Field {
        types::Type type;
        std::string name;
        unsigned int size;
        // reassigned in structFields
        unsigned int offset = 0;
        enum AccessSpecifier {
            AS_pubic,
            AS_protected,
            AS_private,
            AS_none
        };
        AccessSpecifier accessSpecifier = AS_pubic;
    };

    struct TypeInfo {
        fs::path filePath;
        std::string name;
        std::string definition;
        uint64_t size;
        uint64_t alignment;
    };

    typedef std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> FPointerMap;

    struct StructInfo: TypeInfo {
        std::vector<Field> fields{};

        FPointerMap functionFields{};
        bool hasUnnamedFields;
    };

    struct UnionInfo: TypeInfo {
        std::vector<Field> fields{};
        bool hasUnnamedFields;
    };

    struct EnumInfo: TypeInfo {
        struct EnumEntry {
            std::string name;
            std::string value;
        };
        std::unordered_map<std::string, EnumEntry> valuesToEntries{};
        std::unordered_map<std::string, EnumEntry> namesToEntries{};

        std::optional<std::string> access;

        std::string getEntryName(std::string const& value, utbot::Language language);
    };

    struct TypeSupport {
        bool isSupported;
        std::string info;
    };


    using StructsMap = std::unordered_map<uint64_t, StructInfo>;
    using EnumsMap = std::unordered_map<uint64_t, EnumInfo>;
    using UnionsMap = std::unordered_map<uint64_t, UnionInfo>;

    struct TypeMaps {
        StructsMap structs;
        EnumsMap enums;
        UnionsMap unions;
    };

    // Looking for a better name
    enum class TypeKind { PRIMITIVE, STRUCT, OBJECT_POINTER,
            FUNCTION_POINTER, ARRAY, ENUM, UNION, UNKNOWN };

    enum class TypeUsage { PARAMETER, RETURN, ALL };
    enum class PointerUsage { PARAMETER, RETURN, KNOWN_SIZE, LAZY };

    class TypesHandler {
    public:
        struct SizeContext {
            uint64_t pointerSize = 8;
            uint64_t maximumAlignment = 16;
        };

        explicit TypesHandler(TypeMaps &types, SizeContext sizeContext)
            : typeMaps(types), sizeContext(sizeContext){};

        /**
         * This functions calculates size of a given type. For structs in it calculates sum of sizes of its fields,
         * ignoring alignment.
         * @return size of given type.
         */
        size_t typeSize(const types::Type &type) const;

        /**
         * Returns true if given type is an integer type, otherwise false.
         * IntegerType = short | int | long | long long | unsigned short
         *                | unsigned int | unsigned long | unsigned long long | intN_t | uintN_t
         * @return whether given type is IntegerType
         */
        static bool isIntegerType(const Type&);

        /**
         * Returns true if given type is a floating point type, otherwise false.
         * IntegerType = float | float_t | double | double_t
         * @return whether given type is FloatingPointType
         */
        static bool isFloatingPointType(const Type&);

        /**
         * Returns true if given type is a character type, otherwise false.
         * CharacterType = char | signed char | unsigned char
         * @return whether given type is CharacterType
         */
        static bool isCharacterType(const Type&);


        /**
         * Returns true if given type is a boolean type: 'bool' or '_Bool', otherwise false.
         * @return  whether given type is a boolean type.
         */
        static bool isBoolType(const Type&);


        /**
         * Returns true if given type is a primitive, otherwise false.
         * PrimitiveType = IntegerType | CharacterType
         * @return whether type is primitive
         */
        static bool isPrimitiveType(const Type&);


        /**
         * Checks whether given type is a pointer. For now, it doesn't handle
         * pointer to pointer situations. Examples for which this function returns true:
         * int *, int [] struct MyStruct *, struct MyStruct [];
         * @return  whether given type is a pointer
         */
        static bool isObjectPointerType(const Type&);

        /**
         * Returns true if given type is an array, otherwise false.
         * @return whether given type is an array
         */
        static bool isArrayType(const Type&);

        /**
         * Returns true if given type is an incomplete array (a C array with an unspecified size),
         * otherwise false.
         * @return whether given type is an incomplete array
         */
        static bool isIncompleteArrayType(const Type&);

        /**
         * Checks whether it is one dimension pointer.
         * @return true if type is a one dimension pointer, false otherwise.
         */
        static bool isOneDimensionPointer(const Type&);

        /**
         * Returns true if given type is a C style string, otherwise false.
         * @return whether given type is a C style string
         */
        static bool isCStringType(const Type&);

        /**
         * Returns true if given type is a C++ style string, otherwise false.
         * @return whether given type is a C++ style string
         */
        static bool isCppStringType(const Type&);

        /**
         * Returns true if given type is an unsigned type, otherwise false.
         * @return whether given type is an unsigned type
         */
        static bool isUnsignedType(const Type&);

        /**
         * Returns true if given type is a struct, otherwise false.
         * @return whether given type is a struct
         */
        bool isStruct(const Type&) const;


        /**
         * Returns true if given type is an enum, otherwise false.
         * @return whether given type is an enum
         */
        bool isEnum(const Type&) const;


        /**
         * Returns true if given type is a union, otherwise false.
         * @return whether given type is a union
         */
        bool isUnion(const Type&) const;


        /**
         * Returns true if given type is void, otherwise false.
         * @return whether given type is void
         */
        static bool isVoid(const Type&);

        /**
         * Returns true if given type is void, void*, void** etc, otherwise false.
         * @return whether void is base type
         */
        static bool baseTypeIsVoid(const Type &type);

        /**
         * Returns true if given type is a pointer to function, otherwise false.
         * @return whether given type is a pointer to function
         */
        static bool isPointerToFunction(const Type&);

        /**
         * Returns true if given type is an array of pointers to function, otherwise false.
         * @return whether given type is an array of pointers to function
         */
        static bool isArrayOfPointersToFunction(const Type&);

        /**
         * @return whether given type doesn't require to be made symbolic.
         */
        static bool omitMakeSymbolic(const Type&);

        /**
         * @return true if given type is required to be unchecked when it is returned.
         */
        static bool skipTypeInReturn(const Type&);

        /**
         * Returns kind of given type. In case type is unknown returns TypeKind::UNKNOWN.
         * @return kind of type
         */
        TypeKind getTypeKind(const Type&) const;

        /**
         * Returns default value for a given type. In case type is unknown returns .
         * @return Default value for a type
         */
        std::string getDefaultValueForType(const Type&, utbot::Language language) const;

        /**
         * Checks whether given type is supported.
         * @return TypeSupport structs that contains information regarding type support.
         */
        TypeSupport isSupportedType(const Type &type, TypeUsage usage = TypeUsage::ALL, int depth = 0) const;

        /**
         * Converts C _Bool type to C++ bool type.
         * @param type
         * @return
         */
        static std::string cBoolToCpp(const TypeName &type) ;

        /**
         * Returns StructInfo by given struct name.
         * For safe usage, please use isStruct(..) before calling getStructInfo(..).
         * @return StructInfo for given struct.
         */
        StructInfo getStructInfo(const Type&) const;

        /**
         * Returns EnumInfo bu given enum name.
         * Fir safe usage, please use isEnum(..) before calling getEnumInfo(..).
         * @return EnumInfo for given enum.
         */
        EnumInfo getEnumInfo(const Type&) const;

        /**
         * Returns UnionInfo by given union name.
         * For safe usage, please use isUnion(..) before calling getUnionInfo(..).
         * @return UnionInfo for given union.
         */
        UnionInfo getUnionInfo(const Type&) const;

        bool isStruct(uint64_t id) const;
        bool isEnum(uint64_t id) const;
        bool isUnion(uint64_t id) const;

        [[nodiscard]] StructInfo getStructInfo(uint64_t id) const;
        [[nodiscard]] EnumInfo getEnumInfo(uint64_t id) const;
        [[nodiscard]] UnionInfo getUnionInfo(uint64_t id) const;

        /**
         * Returns map of constraints for every supported primitive type, that might be used in
         * 'klee_prefer_cex' function.
         * @return map type -> constraints.
         */
        static std::unordered_map<TypeName, std::vector<std::string>> preferredConstraints() noexcept;

        size_t getPointerSize() const noexcept {
            return sizeContext.pointerSize;
        }

        uint64_t getMaximumAlignment() const noexcept {
            return sizeContext.maximumAlignment;
        }

        static size_t getElementsNumberInPointerMultiDim(PointerUsage usage, size_t def = 2) noexcept {
            switch (usage) {
            case PointerUsage::PARAMETER:
                return 2;
            case PointerUsage::RETURN:
                return 2;
            case PointerUsage::LAZY:
                return 1;
            case PointerUsage::KNOWN_SIZE:
                return def;
            }
        }

        static size_t
        getElementsNumberInPointerOneDim(PointerUsage usage,
                                         size_t def = types::Type::symStdinSize) noexcept {
            switch (usage) {
            case PointerUsage::PARAMETER:
                return 10;
            case PointerUsage::RETURN:
                return 1;
            case PointerUsage::LAZY:
                return 1;
            case PointerUsage::KNOWN_SIZE:
                return def;
            }
        }

        void setPointerSize(size_t size) noexcept {
            sizeContext.pointerSize = size;
        }

        types::Type getReturnTypeToCheck(const types::Type &returnType) const;

        static testsgen::ValidationType getIntegerValidationType(const types::Type &type);
    private:
        static bool isIntegerType(const TypeName&);
        static bool isFloatingPointType(const TypeName&);
        static bool isCharacterType(const TypeName&);
        static bool isBoolType(const TypeName&);
        static bool isVoid(const TypeName&);

        /**
         * Removes 'const' prefix or leaves the string as it is.
         */
        static std::string removeConstPrefix(const TypeName &type);

        /**
         * Checks whether given type has a 'const' modifier.
         * @return true if given type has a 'const' modifier, false otherwise.
         */
        static bool hasConstModifier(const TypeName &type);

        /**
         * Removes '*' from type if type has it, otherwise leaves it as it is.
         * @param type
         * @return type without '*'
         */
        static std::string removeArrayReference(TypeName type);

        /**
         * Removes '[]' from type if it's an array or leaves it as it is.
         * @param type
         * @return type without '[]'
         */
        static std::string removeArrayBrackets(TypeName type);

        struct IsSupportedTypeArguments {
            IsSupportedTypeArguments(TypeName typeName, TypeUsage usage);
            const TypeName typeName;
            types::TypeUsage usage;
            bool operator==(const IsSupportedTypeArguments & other) const;
        };
        struct IsSupportedTypeArgumentsHash {
            std::size_t operator()(const IsSupportedTypeArguments &args) const;
        };

    private:
        TypeMaps &typeMaps;
        SizeContext sizeContext;
        mutable tsl::ordered_set<TypeName> recursiveCheckStarted{};
        mutable std::unordered_map<IsSupportedTypeArguments,
                                   types::TypeSupport,
                                   IsSupportedTypeArgumentsHash>
            isSupportedTypeHash{};

        static std::unordered_map<TypeName, size_t> integerTypesToSizes() noexcept;
        static std::unordered_map<TypeName, size_t> floatingPointTypesToSizes() noexcept;
        static std::unordered_map<TypeName, size_t> characterTypesToSizes() noexcept;
        static std::unordered_map<TypeName, size_t> boolTypesToSizes() noexcept;

        template<typename T>
        bool typeIsInMap(uint64_t id, const std::unordered_map<uint64_t, T>& someMap) const {
            if (CollectionUtils::containsKey(someMap, id)) {
                return true;
            }
            return false;
        }

        template<typename T>
        T typeFromMap(uint64_t id, const std::unordered_map<uint64_t, T>& someMap) const {
            if (CollectionUtils::containsKey(someMap, id)) {
                return someMap.at(id);
            }
            throw NoSuchTypeException(StringUtils::stringFormat("Type with id=%llu can't be found.", id));
        }
    };

    static inline const std::string CONST_QUALIFIER = "const";
    static inline const std::string RESTRICT_QUALIFIER = "restrict";
    static inline const std::string VOLATILE_QUALIFIER = "volatile";

    static inline const std::vector<std::string> QUALIFIERS = { CONST_QUALIFIER, RESTRICT_QUALIFIER, VOLATILE_QUALIFIER };

    struct FunctionInfo {
        std::string name;
        bool isArray;
        types::Type returnType;

        struct FunctionParamInfo {
            types::Type type;
            std::string name;
        };
        std::vector<FunctionParamInfo> params;
    };

}

#endif //UNITTESTBOT_TYPES_H
