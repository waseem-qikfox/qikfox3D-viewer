/** 
 * @file lluuid.h
 *
 * $LicenseInfo:firstyear=2000&license=viewerlgpl$
 * Second Life Viewer Source Code
 * Copyright (C) 2010, Linden Research, Inc.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License only.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
 * $/LicenseInfo$
 */

#ifndef LL_LLUUID_H
#define LL_LLUUID_H

#include <iostream>
#include <set>
#include <vector>
#include "stdtypes.h"
#include "llpreprocessor.h"
#include <boost/functional/hash.hpp>

class LLMutex;

const S32 UUID_BYTES = 16;
const S32 UUID_WORDS = 4;
const S32 UUID_STR_LENGTH = 37;	// actually wrong, should be 36 and use size below
const S32 UUID_STR_SIZE = 37;
const S32 UUID_BASE85_LENGTH = 21; // including the trailing NULL.

struct uuid_time_t {
	U32 high;
	U32 low;
		};

class LL_COMMON_API LLUUID
{
public:
	//
	// CREATORS
	//
	LLUUID();
	explicit LLUUID(const char *in_string); // Convert from string.
	explicit LLUUID(const std::string& in_string); // Convert from string.
	~LLUUID() = default;

	//
	// MANIPULATORS
	//
	void	generate();					// Generate a new UUID
	void	generate(const std::string& stream); //Generate a new UUID based on hash of input stream

	static LLUUID generateNewID(std::string stream = "");	//static version of above for use in initializer expressions such as constructor params, etc. 

	BOOL	set(const char *in_string, BOOL emit = TRUE);	// Convert from string, if emit is FALSE, do not emit warnings
	BOOL	set(const std::string& in_string, BOOL emit = TRUE);	// Convert from string, if emit is FALSE, do not emit warnings
	void	setNull();					// Faster than setting to LLUUID::null.

    S32     cmpTime(uuid_time_t *t1, uuid_time_t *t2);
	static void    getSystemTime(uuid_time_t *timestamp);
	void    getCurrentTime(uuid_time_t *timestamp);

	//
	// ACCESSORS
	//
	BOOL	isNull() const;			// Faster than comparing to LLUUID::null.
	BOOL	notNull() const;		// Faster than comparing to LLUUID::null.
	// JC: This is dangerous.  It allows UUIDs to be cast automatically
	// to integers, among other things.  Use isNull() or notNull().
	//		operator bool() const;

	// JC: These must return real bool's (not BOOLs) or else use of the STL
	// will generate bool-to-int performance warnings.
	bool	operator==(const LLUUID &rhs) const;
	bool	operator!=(const LLUUID &rhs) const;
	bool	operator<(const LLUUID &rhs) const;
	bool	operator>(const LLUUID &rhs) const;

	// xor functions. Useful since any two random uuids xored together
	// will yield a determinate third random unique id that can be
	// used as a key in a single uuid that represents 2.
	const LLUUID& operator^=(const LLUUID& rhs);
	LLUUID operator^(const LLUUID& rhs) const;

	// similar to functions above, but not invertible
	// yields a third random UUID that can be reproduced from the two inputs
	// but which, given the result and one of the inputs can't be used to
	// deduce the other input
	LLUUID combine(const LLUUID& other) const;
	void combine(const LLUUID& other, LLUUID& result) const;  

	friend LL_COMMON_API std::ostream&	 operator<<(std::ostream& s, const LLUUID &uuid);
	friend LL_COMMON_API std::istream&	 operator>>(std::istream& s, LLUUID &uuid);

	void toString(char *out) const;		// Does not allocate memory, needs 36 characters (including \0)
	void toString(std::string& out) const;
	void toCompressedString(char *out) const;	// Does not allocate memory, needs 17 characters (including \0)
	void toCompressedString(std::string& out) const;
	
	// last 4 chars for quick ref - Very lightweight, no nul-term added - provide your own, ensure min 4 bytes.
# define hexnybl(N) (N)>9?((N)-10)+'a':(N)+'0'
	inline char * toShortString(char *out) const
	{
		// LL_PROFILE_ZONE_SCOPED;
		out[0] = hexnybl(mData[14]>>4);
		out[1] = hexnybl(mData[14]&15);
		out[2] = hexnybl(mData[15]>>4);
		out[3] = hexnybl(mData[15]&15);
		return out;
	}
	// full uuid - Much lighterweight than default, no allocation, or nul-term added - provide your own, ensure min 36 bytes.
	inline char * toStringFast(char *out) const
	{
		// LL_PROFILE_ZONE_SCOPED;
		out[0]  = hexnybl(mData[0]>>4);
		out[1]  = hexnybl(mData[0]&15);
		out[2]  = hexnybl(mData[1]>>4);
		out[3]  = hexnybl(mData[1]&15);
		out[4]  = hexnybl(mData[2]>>4);
		out[5]  = hexnybl(mData[2]&15);
		out[6]  = hexnybl(mData[3]>>4);
		out[7]  = hexnybl(mData[3]&15);
		out[8]  = '-';
		out[9]  = hexnybl(mData[4]>>4);
		out[10] = hexnybl(mData[4]&15);
		out[11] = hexnybl(mData[5]>>4);
		out[12] = hexnybl(mData[5]&15);
		out[13] = '-';
		out[14] = hexnybl(mData[6]>>4);
		out[15] = hexnybl(mData[6]&15);
		out[16] = hexnybl(mData[7]>>4);
		out[17] = hexnybl(mData[7]&15);
		out[18] = '-';
		out[19] = hexnybl(mData[8]>>4);
		out[20] = hexnybl(mData[8]&15);
		out[21] = hexnybl(mData[9]>>4);
		out[22] = hexnybl(mData[9]&15);
		out[23] = '-';
		out[24] = hexnybl(mData[10]>>4);
		out[25] = hexnybl(mData[10]&15);
		out[26] = hexnybl(mData[11]>>4);
		out[27] = hexnybl(mData[11]&15);
		out[28] = hexnybl(mData[12]>>4);
		out[29] = hexnybl(mData[12]&15);
		out[30] = hexnybl(mData[13]>>4);
		out[31] = hexnybl(mData[13]&15);
		out[32] = hexnybl(mData[14]>>4);
		out[33] = hexnybl(mData[14]&15);
		out[34] = hexnybl(mData[15]>>4);
		out[35] = hexnybl(mData[15]&15);
		return out;
	}


	std::string asString() const;
	std::string getString() const;

	U16 getCRC16() const;
	U32 getCRC32() const;

	static BOOL validate(const std::string& in_string); // Validate that the UUID string is legal.

	static const LLUUID null;
	static LLMutex * mMutex;

	static U32 getRandomSeed();
	static S32 getNodeID(unsigned char * node_id);

	static BOOL parseUUID(const std::string& buf, LLUUID* value);

	U8 mData[UUID_BYTES];
};
static_assert(std::is_trivially_copyable<LLUUID>::value, "LLUUID must be trivial copy");
static_assert(std::is_trivially_move_assignable<LLUUID>::value, "LLUUID must be trivial move");
static_assert(std::is_standard_layout<LLUUID>::value, "LLUUID must be a standard layout type");

typedef std::vector<LLUUID> uuid_vec_t;
typedef std::set<LLUUID> uuid_set_t;
// <FS:Beq> cleanupDeadObject spam avoidance
typedef std::multiset<LLUUID> uuid_multiset_t;

// Helper structure for ordering lluuids in stl containers.  eg:
// std::map<LLUUID, LLWidget*, lluuid_less> widget_map;
//
// (isn't this the default behavior anyway? I think we could
// everywhere replace these with uuid_set_t, but someone should
// verify.)
struct lluuid_less
{
	bool operator()(const LLUUID& lhs, const LLUUID& rhs) const
	{
		return (lhs < rhs) ? true : false;
	}
};

typedef std::set<LLUUID, lluuid_less> uuid_list_t;
/*
 * Sub-classes for keeping transaction IDs and asset IDs
 * straight.
 */
typedef LLUUID LLAssetID;

class LL_COMMON_API LLTransactionID : public LLUUID
{
public:
	LLTransactionID() : LLUUID() { }
	
	static const LLTransactionID tnull;
	LLAssetID makeAssetID(const LLUUID& session) const;
};

// Generate a hash of an LLUUID object using the boost hash templates. 

// <FS:ND> GCC 4.9 does not like the specialization in form of boost::hash but rather wants a namespace
// template <>
// struct boost::hash<LLUUID>
namespace boost { template <> struct hash<LLUUID>
// </FS:ND>
{
    typedef LLUUID argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& s) const
    {
        result_type seed(0);

        for (S32 i = 0; i < UUID_BYTES; ++i)
        {
            boost::hash_combine(seed, s.mData[i]);
        }

        return seed;
    }
};
} // <FS:ND/> close namespace

// <FS:Ansariel> UUID hash calculation
struct FSUUIDHash
{
	inline size_t operator() (const LLUUID& id) const
	{
		return *reinterpret_cast<const size_t*>(id.mData);
	}
};
// </FS:Ansariel> UUID hash calculation

// Adapt boost hash to std hash
namespace std
{
    template<> struct hash<LLUUID>
    {
        std::size_t operator()(LLUUID const& s) const noexcept
        {
            return boost::hash<LLUUID>()(s);
        }
    };
}
#endif


