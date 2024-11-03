#ifndef BITCOIN_SERIALIZE_H
#define BITCOIN_SERIALIZE_H

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <ios>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

// Forward declarations
class CScript;
class uint256;

// Serialization/deserialization operations
enum class CSerializeAction {
    Read,
    Write
};

// Helper templates for serialization
template<typename T>
inline void Serialize(std::ostream& os, const T& obj);

template<typename T>
inline void Unserialize(std::istream& is, T& obj);

// Base serialization operations
#define READWRITE(obj) \
    (ser_action.ForRead() ? ::Unserialize(s, obj) : ::Serialize(s, obj))

#define VARINT(obj) \
    REF(WrapVarInt(obj))

#define COMPACTSIZE(obj) \
    REF(CompactSizeFormatter<decltype(obj)>(obj))

#define FLATDATA(obj) \
    REF(CFlatData((char*)&(obj), (char*)&(obj) + sizeof(obj)))

// Main serialization macro
#define ADD_SERIALIZE_METHODS \
    template<typename Stream> \
    void Serialize(Stream& s) const { \
        const_cast<std::remove_const_t<std::remove_reference_t<decltype(*this)>>*>(this) \
            ->SerializationOp(s, CSerializeAction::Write); \
    } \
    template<typename Stream> \
    void Unserialize(Stream& s) { \
        SerializationOp(s, CSerializeAction::Read); \
    } \
protected: \
    template<typename Stream, typename Operation> \
    void SerializationOp(Stream& s, Operation ser_action)

// Helper classes
template<typename T>
class WrapVarInt {
    T& value;
public:
    explicit WrapVarInt(T& v) : value(v) {}
    template<typename Stream>
    void Serialize(Stream& s) const;
    template<typename Stream>
    void Unserialize(Stream& s);
};

template<typename T>
class CompactSizeFormatter {
    T& value;
public:
    explicit CompactSizeFormatter(T& v) : value(v) {}
    template<typename Stream>
    void Serialize(Stream& s) const;
    template<typename Stream>
    void Unserialize(Stream& s);
};

class CFlatData {
    char* ptr;
    size_t len;
public:
    CFlatData(char* p, size_t l) : ptr(p), len(l) {}
    template<typename Stream>
    void Serialize(Stream& s) const;
    template<typename Stream>
    void Unserialize(Stream& s);
};

#endif // BITCOIN_SERIALIZE_H
