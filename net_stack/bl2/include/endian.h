#ifndef _ENDIAN_H_
#define _ENDIAN_H_

#define LITTLE_ENDIAN	1234
#define BYTE_ORDER	LITTLE_ENDIAN

#define __le16_to_cpu(x) ((__force __u16)(__le16)(x))


#define __bswap_32(x) \
     ((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >>  8) |\
      (((x) & 0x0000ff00u) <<  8) | (((x) & 0x000000ffu) << 24))

#define __bswap_16(x)	((unsigned short int)((((x) >> 8) & 0xffu) | (((x) & 0xffu) << 8)))

// #define __le32_to_cpu(x) ((__force __u32)(__le32)(x))
// #define __le64_to_cpu(x) ((__force __u64)(__le64)(x))
// #define __cpu_to_le16(x) ((__force __le16)(__u16)(x))
// #define __cpu_to_le32(x) ((__force __le32)(__u32)(x))
// #define __cpu_to_le64(x) ((__force __le64)(__u64)(x))
// #define __be16_to_cpu(x) __swab16((__force __u16)(__be16)(x))
// #define __be32_to_cpu(x) __swab32((__force __u32)(__be32)(x))
// #define __be64_to_cpu(x) __swab64((__force __u64)(__be64)(x))
// #define __cpu_to_be16(x) ((__force __be16)__swab16(x))
// #define __cpu_to_be32(x) ((__force __be32)__swab32(x))
// #define __cpu_to_be64(x) ((__force __be64)__swab64(x))


#endif
