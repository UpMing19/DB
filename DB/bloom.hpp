#pragma once

#include <stdio.h>
#include <cstdint>
#include <vector>
#include <array>
#include <math.h>

#include "MurmurHash.h"

using namespace std;

template <class Key>
class BloomFilter
{
private:
    __uint8_t m_numHashes;
    vector<bool> m_bits;

public:
    BloomFilter(uint64_t n, double fp)
    {
        double denom = 0.480453013918201; // (ln(2))^2
        double size = -1 * (double)n * (log(fp) / denom);

        m_bits = vector<bool>((int)size);

        double ln2 = 0.693147180559945;
        m_numHashes = (int)ceil((size / n) * ln2); // ln(2)
    }
    array<uint64_t, 2> hash(const Key *data, size_t len)
    {
        array<uint64_t, 2> hashValue;
        MurmurHash3_x64_128(data, (int)len, 0, hashValue.data());
        return hashValue;
    }
    uint64_t nthHash(uint32_t n, uint64_t hashA, uint64_t hashB, uint64_t filterSize)
    {
        return (hashA + n * hashB) % filterSize;
    }
    void add(const Key *data, size_t len)
    {
        auto hashValue = hash(data, len);
        for (int i = 0; i < m_numHashes; i++)
            m_bits[nthHash(i, hashValue[0], hashValue[1], m_bits.size())] = true;
    }
    bool mayContain(const Key *data, size_t len)
    {
        auto hashValue = hash(data, len);
        for (int i = 0; i < m_numHashes; i++)
            if (!m_bits[nthHash(i, hashValue[0], hashValue[1], m_bits.size())])
                return false;
        return true;
    }
};
