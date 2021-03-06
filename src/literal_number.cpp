/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include "literal_number.h"
#include <cassert>

namespace Sirit {

LiteralNumber::LiteralNumber(std::type_index type) : type(type) {
    operand_type = OperandType::Number;
}

LiteralNumber::~LiteralNumber() = default;

void LiteralNumber::Fetch(Stream& stream) const {
    if (is_32) {
        stream.Write(static_cast<u32>(raw));
    } else {
        stream.Write(raw);
    }
}

u16 LiteralNumber::GetWordCount() const { return is_32 ? 1 : 2; }

bool LiteralNumber::operator==(const Operand& other) const {
    if (operand_type == other.GetType()) {
        const auto& o{dynamic_cast<const LiteralNumber&>(other)};
        return o.type == type && o.raw == raw;
    }
    return false;
}

} // namespace Sirit
