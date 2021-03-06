/*
Copyright (C) 2015 Ruslan Kabatsayev <b7.10110111@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef INSTRUCTION_20150908_H
#define INSTRUCTION_20150908_H

#include "Formatter.h"
#include "Operand.h"
#include <capstone/capstone.h>
#include <cstdint>
#include <string>

class QString;

namespace CapstoneEDB {

enum class Architecture {
	ARCH_X86, 
	ARCH_AMD64, 
	ARCH_ARM32, 
	ARCH_ARM64
};

bool init(Architecture arch);
bool isX86_64();

class Instruction;
class Formatter;

class Instruction {
	friend class Formatter;
	friend class Operand;

public:
	static constexpr std::size_t MAX_SIZE = 15;

public:
	Instruction(const void *first, const void *end, uint64_t rva) noexcept;
	Instruction(const Instruction &)            = delete;
	Instruction &operator=(const Instruction &) = delete;
	Instruction(Instruction &&);
	Instruction &operator=(Instruction &&);
	~Instruction();

public:
	bool valid() const {
		return insn_;
	}
	
	explicit operator bool() const {
		return valid();
	}

public:
	int operation() const             { return insn_ ? insn_->id                   : 0;             }
	std::size_t operand_count() const { return insn_ ? insn_->detail->x86.op_count : 0;             }
	std::size_t byte_size() const     { return insn_ ? insn_->size                 : 1;             }
	uint64_t rva() const              { return insn_ ? insn_->address              : rva_;          }
	std::string mnemonic() const      { return insn_ ? insn_->mnemonic             : std::string(); }
	const uint8_t *bytes() const      { return insn_ ? insn_->bytes                : &byte0_;       }

public:
	Operand operator[](size_t n) const;
	Operand operand(size_t n) const;

public:
	const cs_insn *native() const {
		return insn_;
	}

public:
	cs_insn *operator->()             { return insn_; }
	const cs_insn *operator->() const { return insn_; }

public:
	void swap(Instruction &other);

public:
	enum ConditionCode : uint8_t {
		CC_UNCONDITIONAL = 0x10, // value must be higher than 0xF
		CC_CXZ,
		CC_ECXZ,
		CC_RCXZ,

		CC_B   = 2,
		CC_C   = CC_B,
		CC_E   = 4,
		CC_Z   = CC_E,
		CC_NA  = 6,
		CC_BE  = CC_NA,
		CC_S   = 8,
		CC_P   = 0xA,
		CC_PE  = CC_P,
		CC_L   = 0xC,
		CC_NGE = CC_L,
		CC_LE  = 0xE,
		CC_NG  = CC_LE,

		CC_NB  = CC_B | 1,
		CC_AE  = CC_NB,
		CC_NE  = CC_E | 1,
		CC_NZ  = CC_NE,
		CC_A   = CC_NA | 1,
		CC_NBE = CC_A,
		CC_NS  = CC_S | 1,
		CC_NP  = CC_P | 1,
		CC_PO  = CC_NP,
		CC_NL  = CC_L | 1,
		CC_GE  = CC_NL,
		CC_NLE = CC_LE | 1,
		CC_G   = CC_NLE
	};
	
	ConditionCode condition_code() const;

private:
	cs_insn *insn_;
	
	// we have our own copies of this data so we can give something meaningful
	// even during a failed disassembly
	uint8_t  byte0_ = 0;
	uint64_t rva_   = 0;
};

}

#include "Inspection.h"

#endif
