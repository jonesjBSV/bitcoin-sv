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

template<typename Stream> 
class StreamWrapper {
protected:
    Stream& stream;

public:
    explicit StreamWrapper(Stream& s) : stream(s) {}

    void write(const char* pch, size_t size) {
        stream.write(pch, size);
    }

    void read(char* pch, size_t size) {
        stream.read(pch, size);
    }
};

// Serialization wrapper for vectors and other containers
template<typename T>
class ContainerWrapper {
private:
    const T& container;

public:
    explicit ContainerWrapper(const T& c) : container(c) {}

    template<typename Stream>
    void Serialize(Stream& s) const {
        WriteCompactSize(s, container.size());
        for (const auto& item : container) {
            ::Serialize(s, item);
        }
    }
};

// Helper for compact size serialization
inline void WriteCompactSize(StreamWrapper& s, uint64_t nSize) {
    if (nSize < 253) {
        uint8_t chSize = nSize;
        s.write((char*)&chSize, 1);
    } else if (nSize <= std::numeric_limits<uint16_t>::max()) {
        uint8_t chSize = 253;
        uint16_t nSizeLE = htole16(nSize);
        s.write((char*)&chSize, 1);
        s.write((char*)&nSizeLE, 2);
    } else if (nSize <= std::numeric_limits<uint32_t>::max()) {
        uint8_t chSize = 254;
        uint32_t nSizeLE = htole32(nSize);
        s.write((char*)&chSize, 1);
        s.write((char*)&nSizeLE, 4);
    } else {
        uint8_t chSize = 255;
        uint64_t nSizeLE = htole64(nSize);
        s.write((char*)&chSize, 1);
        s.write((char*)&nSizeLE, 8);
    }
}

// Read compact size
template<typename Stream>
uint64_t ReadCompactSize(Stream& s) {
    uint8_t chSize;
    s.read((char*)&chSize, 1);
    uint64_t nSizeRet = 0;
    if (chSize < 253) {
        nSizeRet = chSize;
    } else if (chSize == 253) {
        uint16_t nSizeLE = 0;
        s.read((char*)&nSizeLE, 2);
        nSizeRet = le16toh(nSizeLE);
    } else if (chSize == 254) {
        uint32_t nSizeLE = 0;
        s.read((char*)&nSizeLE, 4);
        nSizeRet = le32toh(nSizeLE);
    } else {
        uint64_t nSizeLE = 0;
        s.read((char*)&nSizeLE, 8);
        nSizeRet = le64toh(nSizeLE);
    }
    return nSizeRet;
}

// Main serialization macro
#define ADD_SERIALIZE_METHODS \
    template<typename Stream> \
    void Serialize(Stream& s) const { \
        SerializationOp(s, true); \
    } \
    template<typename Stream> \
    void Unserialize(Stream& s) { \
        SerializationOp(s, false); \
    }

#define READWRITE(obj) (ser_action ? ::Serialize(s, obj) : ::Unserialize(s, obj))

// ... (keep rest of file) ...

#endif // BITCOIN_SERIALIZE_H
