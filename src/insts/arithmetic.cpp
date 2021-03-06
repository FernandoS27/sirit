/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include "common_types.h"
#include "op.h"
#include "sirit/sirit.h"
#include <memory>

namespace Sirit {

#define DEFINE_UNARY(funcname, opcode)                                         \
    Id Module::funcname(Id result_type, Id operand) {                          \
        auto op{std::make_unique<Op>(opcode, bound++, result_type)};           \
        op->Add(operand);                                                      \
        return AddCode(std::move(op));                                         \
    }

#define DEFINE_BINARY(funcname, opcode)                                        \
    Id Module::funcname(Id result_type, Id operand_1, Id operand_2) {          \
        auto op{std::make_unique<Op>(opcode, bound++, result_type)};           \
        op->Add(operand_1);                                                    \
        op->Add(operand_2);                                                    \
        return AddCode(std::move(op));                                         \
    }

DEFINE_UNARY(OpFNegate, spv::Op::OpFNegate);

DEFINE_BINARY(OpUDiv, spv::Op::OpUDiv)
DEFINE_BINARY(OpUMod, spv::Op::OpUMod)
DEFINE_BINARY(OpFDiv, spv::Op::OpFDiv)
DEFINE_BINARY(OpIAdd, spv::Op::OpIAdd)

} // namespace Sirit