/*
 * SpookyHashV1.h
 * https://github.com/facebook/folly/blob/master/folly/hash/SpookyHashV1.h
 *
 *  Created on: Nov 16, 2018
 *      Student (MIG Virtual Developer): Tung Dang
 */

#ifndef FOLLY_HASH_SPOOKYHASHV1_H_
#define FOLLY_HASH_SPOOKYHASHV1_H_

#include <cstddef>
#include <cstdint>
#include <stdint.h>

namespace folly {
namespace hash {

class SpookyHashV1 {
public:
	static void Hash128 (
		// message to hash
		const void * message,
		// length of message in bytes
		size_t length,
		uint64_t *hash1,
		uint64_t *hash2
	);

	static uint64_t Hash64 (
		const void* message,
		size_t length,
		uint64_t seed)
	{
		uint64_t hash1 = seed;
		Hash128(message,length,&hash1,&seed);
		return hash1;
	}

	static uint32_t Hash32 (
		const void* message,
		size_t length,
		uint32_t seed)
	{
		uint64_t hash1 = seed, hash2 = seed;
		Hash128(message, length, &hash1, &hash2);
		return (uint32_t)hash1;
	}

	void Init(uint64_t seed1, uint64_t seed2);
	void Update(const void* message, size_t length);
	void Final(uint64_t *hash1, uint64_t *hash2);

	static inline uint64_t Rot64(uint64_t x, int k)
	{
		return (x << k) | (x >> (64 - k));
	}

	static inline void Mix(
	        const uint64_t *data,
	        uint64_t &s0, uint64_t &s1, uint64_t &s2, uint64_t &s3,
	        uint64_t &s4, uint64_t &s5, uint64_t &s6, uint64_t &s7,
	        uint64_t &s8, uint64_t &s9, uint64_t &s10,uint64_t &s11)
	{
	      s0 += data[0];   s2 ^= s10; s11 ^= s0;  s0 = Rot64(s0,11);   s11 += s1;
	      s1 += data[1];   s3 ^= s11; s0 ^= s1;   s1 = Rot64(s1,32);   s0 += s2;
	      s2 += data[2];   s4 ^= s0;  s1 ^= s2;   s2 = Rot64(s2,43);   s1 += s3;
	      s3 += data[3];   s5 ^= s1;  s2 ^= s3;   s3 = Rot64(s3,31);   s2 += s4;
	      s4 += data[4];   s6 ^= s2;  s3 ^= s4;   s4 = Rot64(s4,17);   s3 += s5;
	      s5 += data[5];   s7 ^= s3;  s4 ^= s5;   s5 = Rot64(s5,28);   s4 += s6;
	      s6 += data[6];   s8 ^= s4;  s5 ^= s6;   s6 = Rot64(s6,39);   s5 += s7;
	      s7 += data[7];   s9 ^= s5;  s6 ^= s7;   s7 = Rot64(s7,57);   s6 += s8;
	      s8 += data[8];   s10 ^= s6; s7 ^= s8;   s8 = Rot64(s8,55);   s7 += s9;
	      s9 += data[9];   s11 ^= s7; s8 ^= s9;   s9 = Rot64(s9,54);   s8 += s10;
	      s10 += data[10]; s0 ^= s8;  s9 ^= s10;  s10 = Rot64(s10,22); s9 += s11;
	      s11 += data[11]; s1 ^= s9;  s10 ^= s11; s11 = Rot64(s11,46); s10 += s0;
	}

	static inline void EndPartial(
	        uint64_t &h0, uint64_t &h1, uint64_t &h2, uint64_t &h3,
	        uint64_t &h4, uint64_t &h5, uint64_t &h6, uint64_t &h7,
	        uint64_t &h8, uint64_t &h9, uint64_t &h10,uint64_t &h11)
    {
	        h11+= h1;    h2 ^= h11;   h1 = Rot64(h1,44);
	        h0 += h2;    h3 ^= h0;    h2 = Rot64(h2,15);
	        h1 += h3;    h4 ^= h1;    h3 = Rot64(h3,34);
	        h2 += h4;    h5 ^= h2;    h4 = Rot64(h4,21);
	        h3 += h5;    h6 ^= h3;    h5 = Rot64(h5,38);
	        h4 += h6;    h7 ^= h4;    h6 = Rot64(h6,33);
	        h5 += h7;    h8 ^= h5;    h7 = Rot64(h7,10);
	        h6 += h8;    h9 ^= h6;    h8 = Rot64(h8,13);
	        h7 += h9;    h10^= h7;    h9 = Rot64(h9,38);
	        h8 += h10;   h11^= h8;    h10= Rot64(h10,53);
	        h9 += h11;   h0 ^= h9;    h11= Rot64(h11,42);
	        h10+= h0;    h1 ^= h10;   h0 = Rot64(h0,54);
	}

	static inline void End(
	        uint64_t &h0, uint64_t &h1, uint64_t &h2, uint64_t &h3,
	        uint64_t &h4, uint64_t &h5, uint64_t &h6, uint64_t &h7,
	        uint64_t &h8, uint64_t &h9, uint64_t &h10,uint64_t &h11)
	{
	        EndPartial(h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
	        EndPartial(h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
	        EndPartial(h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
	}

	static inline void ShortMix(uint64_t &h0, uint64_t &h1,
	                                uint64_t &h2, uint64_t &h3)
	{
	        h2 = Rot64(h2,50);  h2 += h3;  h0 ^= h2;
	        h3 = Rot64(h3,52);  h3 += h0;  h1 ^= h3;
	        h0 = Rot64(h0,30);  h0 += h1;  h2 ^= h0;
	        h1 = Rot64(h1,41);  h1 += h2;  h3 ^= h1;
	        h2 = Rot64(h2,54);  h2 += h3;  h0 ^= h2;
	        h3 = Rot64(h3,48);  h3 += h0;  h1 ^= h3;
	        h0 = Rot64(h0,38);  h0 += h1;  h2 ^= h0;
	        h1 = Rot64(h1,37);  h1 += h2;  h3 ^= h1;
	        h2 = Rot64(h2,62);  h2 += h3;  h0 ^= h2;
	        h3 = Rot64(h3,34);  h3 += h0;  h1 ^= h3;
	        h0 = Rot64(h0,5);   h0 += h1;  h2 ^= h0;
	        h1 = Rot64(h1,36);  h1 += h2;  h3 ^= h1;
	}

	static inline void ShortEnd(uint64_t &h0, uint64_t &h1,
	                                uint64_t &h2, uint64_t &h3)
	{
	        h3 ^= h2;  h2 = Rot64(h2,15);  h3 += h2;
	        h0 ^= h3;  h3 = Rot64(h3,52);  h0 += h3;
	        h1 ^= h0;  h0 = Rot64(h0,26);  h1 += h0;
	        h2 ^= h1;  h1 = Rot64(h1,51);  h2 += h1;
	        h3 ^= h2;  h2 = Rot64(h2,28);  h3 += h2;
	        h0 ^= h3;  h3 = Rot64(h3,9);   h0 += h3;
	        h1 ^= h0;  h0 = Rot64(h0,47);  h1 += h0;
	        h2 ^= h1;  h1 = Rot64(h1,54);  h2 += h1;
	        h3 ^= h2;  h2 = Rot64(h2,32);  h3 += h2;
	        h0 ^= h3;  h3 = Rot64(h3,25);  h0 += h3;
	        h1 ^= h0;  h0 = Rot64(h0,63);  h1 += h0;
	}

private:
	static void Short (
	    const void *message,
		size_t length,
		uint64_t *hash1,
		uint64_t *hash2);

	static const size_t sc_numVars = 12;
	static const size_t sc_blockSize = sc_numVars*8;
	static const size_t sc_bufSize = 2 * sc_blockSize;
	static const uint64_t sc_const = 0xdeadbeefdeadbeefULL;

	uint64_t m_data[2*sc_numVars];
	uint64_t m_state[sc_numVars];
	size_t m_length;
	uint8_t m_remainder;
};
}
}

#endif /* FOLLY_HASH_SPOOKYHASHV1_H_ */
