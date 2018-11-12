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

DEFINE_UNARY(OpConvertFToU, spv::Op::OpConvertFToU)
DEFINE_UNARY(OpConvertFToS, spv::Op::OpConvertFToS)
DEFINE_UNARY(OpConvertSToF, spv::Op::OpConvertSToF)
DEFINE_UNARY(OpConvertUToF, spv::Op::OpConvertUToF)
DEFINE_UNARY(OpUConvert, spv::Op::OpUConvert)
DEFINE_UNARY(OpSConvert, spv::Op::OpSConvert)
DEFINE_UNARY(OpFConvert, spv::Op::OpFConvert)
DEFINE_UNARY(OpQuantizeToF16, spv::Op::OpQuantizeToF16)
DEFINE_UNARY(OpBitcast, spv::Op::OpBitcast)

} // namespace Sirit
