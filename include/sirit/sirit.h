/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <set>
#include <spirv/unified1/spirv.hpp11>
#include <variant>
#include <vector>

namespace Sirit {

constexpr std::uint32_t GENERATOR_MAGIC_NUMBER = 0;

class Op;
class Operand;

using Literal = std::variant<std::uint32_t, std::uint64_t, std::int32_t,
                             std::int64_t, float, double>;
using Ref = const Op*;

class Module {
  public:
    explicit Module();
    ~Module();

    /**
     * Assembles current module into a SPIR-V stream.
     * It can be called multiple times but it's recommended to copy code
     * externally.
     * @return A stream of bytes representing a SPIR-V module.
     */
    std::vector<std::uint8_t> Assemble() const;

    /// Adds a module capability.
    void AddCapability(spv::Capability capability);

    /// Sets module memory model.
    void SetMemoryModel(spv::AddressingModel addressing_model,
                        spv::MemoryModel memory_model);

    /// Adds an entry point.
    void AddEntryPoint(spv::ExecutionModel execution_model, Ref entry_point,
                       const std::string& name,
                       const std::vector<Ref>& interfaces = {});

    /**
     * Adds an instruction to module's code
     * @param op Instruction to insert into code. Types and constants must not
     * be emitted.
     * @return Returns op.
     */
    Ref Emit(Ref op);

    /**
     * Adds a global variable
     * @param variable Global variable to add.
     * @return Returns variable.
     */
    Ref AddGlobalVariable(Ref variable);

    // Types

    /// Returns type void.
    Ref OpTypeVoid();

    /// Returns type bool.
    Ref OpTypeBool();

    /// Returns type integer.
    Ref OpTypeInt(int width, bool is_signed);

    /// Returns type float.
    Ref OpTypeFloat(int width);

    /// Returns type vector.
    Ref OpTypeVector(Ref component_type, int component_count);

    /// Returns type matrix.
    Ref OpTypeMatrix(Ref column_type, int column_count);

    /// Returns type image.
    Ref OpTypeImage(Ref sampled_type, spv::Dim dim, int depth, bool arrayed,
                    bool ms, int sampled, spv::ImageFormat image_format,
                    std::optional<spv::AccessQualifier> access_qualifier = {});

    /// Returns type sampler.
    Ref OpTypeSampler();

    /// Returns type sampled image.
    Ref OpTypeSampledImage(Ref image_type);

    /// Returns type array.
    Ref OpTypeArray(Ref element_type, Ref length);

    /// Returns type runtime array.
    Ref OpTypeRuntimeArray(Ref element_type);

    /// Returns type struct.
    Ref OpTypeStruct(const std::vector<Ref>& members = {});

    /// Returns type opaque.
    Ref OpTypeOpaque(const std::string& name);

    /// Returns type pointer.
    Ref OpTypePointer(spv::StorageClass storage_class, Ref type);

    /// Returns type function.
    Ref OpTypeFunction(Ref return_type, const std::vector<Ref>& arguments = {});

    /// Returns type event.
    Ref OpTypeEvent();

    /// Returns type device event.
    Ref OpTypeDeviceEvent();

    /// Returns type reserve id.
    Ref OpTypeReserveId();

    /// Returns type queue.
    Ref OpTypeQueue();

    /// Returns type pipe.
    Ref OpTypePipe(spv::AccessQualifier access_qualifier);

    // Constant

    /// Returns a true scalar constant.
    Ref OpConstantTrue(Ref result_type);

    /// Returns a false scalar constant.
    Ref OpConstantFalse(Ref result_type);

    /// Returns a numeric scalar constant.
    Ref OpConstant(Ref result_type, const Literal& literal);

    /// Returns a numeric scalar constant.
    Ref OpConstantComposite(Ref result_type,
                            const std::vector<Ref>& constituents);

    /// Returns a sampler constant.
    Ref OpConstantSampler(Ref result_type,
                          spv::SamplerAddressingMode addressing_mode,
                          bool normalized, spv::SamplerFilterMode filter_mode);

    /// Returns a null constant value.
    Ref OpConstantNull(Ref result_type);

    // Function

    /// Declares a function.
    Ref OpFunction(Ref result_type, spv::FunctionControlMask function_control,
                   Ref function_type);

    /// Ends a function.
    Ref OpFunctionEnd();

    // Flow

    /// Declare a structured loop.
    Ref OpLoopMerge(Ref merge_block, Ref continue_target,
                    spv::LoopControlMask loop_control,
                    const std::vector<Ref>& literals = {});

    /// Declare a structured selection.
    Ref OpSelectionMerge(Ref merge_block,
                         spv::SelectionControlMask selection_control);

    /// The block label instruction: Any reference to a block is through this
    /// ref.
    Ref OpLabel();

    /// Unconditional jump to label.
    Ref OpBranch(Ref target_label);

    /// If condition is true branch to true_label, otherwise branch to
    /// false_label.
    Ref OpBranchConditional(Ref condition, Ref true_label, Ref false_label,
                            std::uint32_t true_weight = 0,
                            std::uint32_t false_weight = 0);

    /// Returns with no value from a function with void return type.
    Ref OpReturn();

    // Debug

    /// Assign a name string to a reference.
    /// @return target
    Ref OpName(Ref target, const std::string& name);

    // Memory

    /// Allocate an object in memory, resulting in a copy to it.
    Ref OpVariable(Ref result_type, spv::StorageClass storage_class,
                   Ref initializer = nullptr);

    /// Load through a pointer.
    Ref OpLoad(Ref result_type, Ref pointer,
               std::optional<spv::MemoryAccessMask> memory_access = {});

    /// Store through a pointer.
    Ref OpStore(Ref pointer, Ref object,
                std::optional<spv::MemoryAccessMask> memory_access = {});

    /// Create a pointer into a composite object that can be used with OpLoad
    /// and OpStore.
    Ref OpAccessChain(Ref result_type, Ref base,
                      const std::vector<Ref>& indexes = {});

    /// Make a copy of a composite object, while modifying one part of it.
    Ref OpCompositeInsert(Ref result_type, Ref object, Ref composite,
                          const std::vector<Literal>& indexes = {});

    // Annotation

    /// Add a decoration to target.
    Ref Decorate(Ref target, spv::Decoration decoration,
                 const std::vector<Literal>& literals = {});

    Ref MemberDecorate(Ref structure_type, Literal member,
                       spv::Decoration decoration,
                       const std::vector<Literal>& literals = {});

    // Misc

    /// Make an intermediate object whose value is undefined.
    Ref OpUndef(Ref result_type);

  private:
    Ref AddCode(Op* op);

    Ref AddCode(spv::Op opcode, std::optional<std::uint32_t> id = {});

    Ref AddDeclaration(Op* op);

    Ref AddAnnotation(Op* op);

    std::uint32_t bound{1};

    std::set<spv::Capability> capabilities;

    std::set<std::string> extensions;

    std::set<std::unique_ptr<Op>> ext_inst_import;

    spv::AddressingModel addressing_model{spv::AddressingModel::Logical};
    spv::MemoryModel memory_model{spv::MemoryModel::GLSL450};

    std::vector<std::unique_ptr<Op>> entry_points;

    std::vector<std::unique_ptr<Op>> execution_mode;

    std::vector<std::unique_ptr<Op>> debug;

    std::vector<std::unique_ptr<Op>> annotations;

    std::vector<std::unique_ptr<Op>> declarations;

    std::vector<Ref> global_variables;

    std::vector<Ref> code;

    std::vector<std::unique_ptr<Op>> code_store;
};

} // namespace Sirit
