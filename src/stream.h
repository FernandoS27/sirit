/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#pragma once

#include "common_types.h"
#include <string>
#include <vector>

namespace Sirit {

class Stream {
  public:
    explicit Stream(std::vector<u8>& bytes);
    ~Stream();

    void Write(std::string string);

    void Write(u64 value);

    void Write(u32 value);

    void Write(u16 value);

    void Write(u8 value);

  private:
    std::vector<u8>& bytes;
};

} // namespace Sirit
