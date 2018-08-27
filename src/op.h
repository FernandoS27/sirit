/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#pragma once

#include "sirit/sirit.h"
#include "common_types.h"
#include "operand.h"
#include "stream.h"

namespace Sirit {

class Op : public Operand {
public:
    explicit Op(spv::Op opcode, u32 id = UINT32_MAX, const Op* result_type = nullptr);
    ~Op();

    virtual void Fetch(Stream& stream) const;
    virtual u16 GetWordCount() const;

    virtual bool operator==(const Operand& other) const;

    void Write(Stream& stream) const;

    void Add(Operand* operand);

    void Add(const Operand* operand);

    void Add(u32 integer);

    void Add(const std::string& string);

    void Add(const std::vector<const Op*>& ids);

private:
    u16 WordCount() const;

    spv::Op opcode;

    const Op* result_type;

    u32 id;

    std::vector<const Operand*> operands;

    std::vector<std::unique_ptr<Operand>> operand_store;
};

} // namespace Sirit