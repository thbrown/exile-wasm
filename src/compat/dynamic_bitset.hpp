/*
 * dynamic_bitset.hpp
 * Compatibility layer for boost::dynamic_bitset
 *
 * For WebAssembly builds, provides std::vector<bool>-based implementation
 * For desktop builds, uses boost::dynamic_bitset
 */

#ifndef BOE_COMPAT_DYNAMIC_BITSET_HPP
#define BOE_COMPAT_DYNAMIC_BITSET_HPP

#ifdef __EMSCRIPTEN__
	// Web build: Use std::vector<bool> wrapper
	#include <vector>
	#include <cstddef>

	namespace boost {
		// Minimal dynamic_bitset compatible with std::vector<bool>
		template<typename Block = unsigned long>
		class dynamic_bitset {
		private:
			std::vector<bool> bits;

		public:
			// Constructors
			dynamic_bitset() = default;
			explicit dynamic_bitset(size_t num_bits, unsigned long value = 0)
				: bits(num_bits, false)
			{
				// Set bits based on value
				for(size_t i = 0; i < num_bits && i < sizeof(value) * 8; i++) {
					bits[i] = (value & (1UL << i)) != 0;
				}
			}

			// Size operations
			size_t size() const { return bits.size(); }
			void resize(size_t num_bits, bool value = false) {
				bits.resize(num_bits, value);
			}
			void clear() { bits.clear(); }
			bool empty() const { return bits.empty(); }

			// Element access
			bool operator[](size_t pos) const { return bits[pos]; }
			std::vector<bool>::reference operator[](size_t pos) { return bits[pos]; }

			// Bit operations
			dynamic_bitset& set(size_t pos, bool value = true) {
				if(pos < bits.size()) bits[pos] = value;
				return *this;
			}

			dynamic_bitset& reset(size_t pos) {
				if(pos < bits.size()) bits[pos] = false;
				return *this;
			}

			dynamic_bitset& flip(size_t pos) {
				if(pos < bits.size()) bits[pos] = !bits[pos];
				return *this;
			}

			// Comparison
			bool operator==(const dynamic_bitset& other) const {
				return bits == other.bits;
			}

			bool operator!=(const dynamic_bitset& other) const {
				return bits != other.bits;
			}
		};
	}

#else
	// Desktop build: Use actual Boost
	#include <boost/dynamic_bitset.hpp>
#endif

#endif // BOE_COMPAT_DYNAMIC_BITSET_HPP
