/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include "literal_string.h"
#include "common_types.h"
#include <string>

namespace Sirit {

LiteralString::LiteralString(const std::string& string) : string(string) {
    operand_type = OperandType::String;
}

LiteralString::~LiteralString() = default;

void LiteralString::Fetch(Stream& stream) const {
    for (std::size_t i = 0; i < string.size(); i++) {
        stream.Write(static_cast<u8>(string[i]));
    }
    for (std::size_t i = 0; i < 4 - (string.size() % 4); i++) {
        stream.Write(static_cast<u8>(0));
    }
}

u16 LiteralString::GetWordCount() const {
    return static_cast<u16>(string.size() / 4 + 1);
}

bool LiteralString::operator==(const Operand& other) const {
    if (operand_type == other.GetType()) {
        return dynamic_cast<const LiteralString&>(other).string == string;
    }
    return false;
}

} // namespace Sirit
