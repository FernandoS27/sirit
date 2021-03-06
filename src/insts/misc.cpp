/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include "op.h"
#include "sirit/sirit.h"
#include <cassert>

namespace Sirit {

Id Module::OpUndef(Id result_type) {
    return AddCode(std::make_unique<Op>(spv::Op::OpUndef, bound++, result_type));
}

} // namespace Sirit
