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
using Id = const Op*;

class Module {
  public:
    explicit Module(std::uint32_t version = spv::Version);
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
    void AddEntryPoint(spv::ExecutionModel execution_model, Id entry_point,
                       const std::string& name,
                       const std::vector<Id>& interfaces = {});

    /**
     * Adds an instruction to module's code
     * @param op Instruction to insert into code. Types and constants must not
     * be emitted.
     * @return Returns op.
     */
    Id Emit(Id op);

    /**
     * Adds a global variable
     * @param variable Global variable to add.
     * @return Returns variable.
     */
    Id AddGlobalVariable(Id variable);

    // Types

    /// Returns type void.
    Id OpTypeVoid();

    /// Returns type bool.
    Id OpTypeBool();

    /// Returns type integer.
    Id OpTypeInt(int width, bool is_signed);

    /// Returns type float.
    Id OpTypeFloat(int width);

    /// Returns type vector.
    Id OpTypeVector(Id component_type, int component_count);

    /// Returns type matrix.
    Id OpTypeMatrix(Id column_type, int column_count);

    /// Returns type image.
    Id OpTypeImage(Id sampled_type, spv::Dim dim, int depth, bool arrayed,
                   bool ms, int sampled, spv::ImageFormat image_format,
                   std::optional<spv::AccessQualifier> access_qualifier = {});

    /// Returns type sampler.
    Id OpTypeSampler();

    /// Returns type sampled image.
    Id OpTypeSampledImage(Id image_type);

    /// Returns type array.
    Id OpTypeArray(Id element_type, Id length);

    /// Returns type runtime array.
    Id OpTypeRuntimeArray(Id element_type);

    /// Returns type struct.
    Id OpTypeStruct(const std::vector<Id>& members = {});

    /// Returns type opaque.
    Id OpTypeOpaque(const std::string& name);

    /// Returns type pointer.
    Id OpTypePointer(spv::StorageClass storage_class, Id type);

    /// Returns type function.
    Id OpTypeFunction(Id return_type, const std::vector<Id>& arguments = {});

    /// Returns type event.
    Id OpTypeEvent();

    /// Returns type device event.
    Id OpTypeDeviceEvent();

    /// Returns type reserve id.
    Id OpTypeReserveId();

    /// Returns type queue.
    Id OpTypeQueue();

    /// Returns type pipe.
    Id OpTypePipe(spv::AccessQualifier access_qualifier);

    // Constant

    /// Returns a true scalar constant.
    Id ConstantTrue(Id result_type);

    /// Returns a false scalar constant.
    Id ConstantFalse(Id result_type);

    /// Returns a numeric scalar constant.
    Id Constant(Id result_type, const Literal& literal);

    /// Returns a numeric scalar constant.
    Id ConstantComposite(Id result_type, const std::vector<Id>& constituents);

    /// Returns a sampler constant.
    Id ConstantSampler(Id result_type,
                       spv::SamplerAddressingMode addressing_mode,
                       bool normalized, spv::SamplerFilterMode filter_mode);

    /// Returns a null constant value.
    Id ConstantNull(Id result_type);

    // Function

    /// Declares a function.
    Id OpFunction(Id result_type, spv::FunctionControlMask function_control,
                  Id function_type);

    /// Ends a function.
    Id OpFunctionEnd();

    /// Call a function.
    Id OpFunctionCall(Id result_type, Id function,
                      const std::vector<Id>& arguments = {});

    // Flow

    /// Declare a structured loop.
    Id OpLoopMerge(Id merge_block, Id continue_target,
                   spv::LoopControlMask loop_control,
                   const std::vector<Id>& literals = {});

    /// Declare a structured selection.
    Id OpSelectionMerge(Id merge_block,
                        spv::SelectionControlMask selection_control);

    /// The block label instruction: Any reference to a block is through this
    /// ref.
    Id OpLabel();

    /// Unconditional jump to label.
    Id OpBranch(Id target_label);

    /// If condition is true branch to true_label, otherwise branch to
    /// false_label.
    Id OpBranchConditional(Id condition, Id true_label, Id false_label,
                           std::uint32_t true_weight = 0,
                           std::uint32_t false_weight = 0);

    /// Returns with no value from a function with void return type.
    Id OpReturn();

    /// Return a value from a function.
    Id OpReturnValue(Id value);

    // Debug

    /// Assign a name string to a reference.
    /// @return target
    Id Name(Id target, const std::string& name);

    /// Assign a name string to a member of a structure type.
    /// @return type
    Id MemberName(Id type, std::uint32_t member, const std::string& name);

    /// Assign a Result <id> to a string for use by other debug instructions.
    Id String(const std::string& string);

    /// Add source-level location information
    Id OpLine(Id file, Literal line, Literal column);

    // Memory

    /// Allocate an object in memory, resulting in a copy to it.
    Id OpVariable(Id result_type, spv::StorageClass storage_class,
                  Id initializer = nullptr);

    /// Load through a pointer.
    Id OpLoad(Id result_type, Id pointer,
              std::optional<spv::MemoryAccessMask> memory_access = {});

    /// Store through a pointer.
    Id OpStore(Id pointer, Id object,
               std::optional<spv::MemoryAccessMask> memory_access = {});

    /// Create a pointer into a composite object that can be used with OpLoad
    /// and OpStore.
    Id OpAccessChain(Id result_type, Id base,
                     const std::vector<Id>& indexes = {});

    /// Make a copy of a composite object, while modifying one part of it.
    Id OpCompositeInsert(Id result_type, Id object, Id composite,
                         const std::vector<Literal>& indexes = {});

    /// Extract a part of a composite object.
    Id OpCompositeExtract(Id result_type, Id composite,
                          const std::vector<Literal>& indexes = {});

    /// Construct a new composite object from a set of constituent objects that
    /// will fully form it.
    Id OpCompositeConstruct(Id result_type, const std::vector<Id>& ids);

    // Annotation

    /// Add a decoration to target.
    Id Decorate(Id target, spv::Decoration decoration,
                const std::vector<Literal>& literals = {});

    Id MemberDecorate(Id structure_type, Literal member,
                      spv::Decoration decoration,
                      const std::vector<Literal>& literals = {});

    // Misc

    /// Make an intermediate object whose value is undefined.
    Id OpUndef(Id result_type);

    // Logical

    Id OpIsNan(Id result_type, Id operand);

    Id OpIsInf(Id result_type, Id operand);

    Id OpLogicalEqual(Id result_type, Id operand_1, Id operand_2);

    Id OpLogicalNotEqual(Id result_type, Id operand_1, Id operand_2);

    Id OpLogicalOr(Id result_type, Id operand_1, Id operand_2);

    Id OpLogicalAnd(Id result_type, Id operand_1, Id operand_2);

    /// Result is true if Operand is false. Result is false if Operand is true.
    Id OpLogicalNot(Id result_type, Id operand);

    Id OpSelect(Id result_type, Id condition, Id operand_1, Id operand_2);

    Id OpIEqual(Id result_type, Id operand_1, Id operand_2);

    Id OpINotEqual(Id result_type, Id operand_1, Id operand_2);

    Id OpUGreaterThan(Id result_type, Id operand_1, Id operand_2);

    Id OpSGreaterThan(Id result_type, Id operand_1, Id operand_2);

    Id OpUGreaterThanEqual(Id result_type, Id operand_1, Id operand_2);

    Id OpSGreaterThanEqual(Id result_type, Id operand_1, Id operand_2);

    Id OpULessThan(Id result_type, Id operand_1, Id operand_2);

    Id OpSLessThan(Id result_type, Id operand_1, Id operand_2);

    Id OpULessThanEqual(Id result_type, Id operand_1, Id operand_2);

    Id OpSLessThanEqual(Id result_type, Id operand_1, Id operand_2);

    Id OpFOrdEqual(Id result_type, Id operand_1, Id operand_2);

    Id OpFUnordEqual(Id result_type, Id operand_1, Id operand_2);

    Id OpFOrdNotEqual(Id result_type, Id operand_1, Id operand_2);

    Id OpFUnordNotEqual(Id result_type, Id operand_1, Id operand_2);

    Id OpFOrdLessThan(Id result_type, Id operand_1, Id operand_2);

    Id OpFUnordLessThan(Id result_type, Id operand_1, Id operand_2);

    Id OpFOrdGreaterThan(Id result_type, Id operand_1, Id operand_2);

    Id OpFUnordGreaterThan(Id result_type, Id operand_1, Id operand_2);

    Id OpFOrdLessThanEqual(Id result_type, Id operand_1, Id operand_2);

    Id OpFUnordLessThanEqual(Id result_type, Id operand_1, Id operand_2);

    Id OpFOrdGreaterThanEqual(Id result_type, Id operand_1, Id operand_2);

    Id OpFUnordGreaterThanEqual(Id result_type, Id operand_1, Id operand_2);

    // Conversion

    Id OpConvertFToU(Id result_type, Id operand);

    Id OpConvertFToS(Id result_type, Id operand);

    Id OpConvertSToF(Id result_type, Id operand);

    Id OpConvertUToF(Id result_type, Id operand);

    Id OpUConvert(Id result_type, Id operand);

    Id OpSConvert(Id result_type, Id operand);

    Id OpFConvert(Id result_type, Id operand);

    Id OpQuantizeToF16(Id result_type, Id operand);

    /// Bit pattern-preserving type conversion.
    Id OpBitcast(Id result_type, Id operand);

    // Bit

    /// Shift the bits in Base right by the number of bits specified in Shift.
    /// The most-significant bits will be zero filled.
    Id OpShiftRightLogical(Id result_type, Id base, Id shift);

    /// Shift the bits in Base right by the number of bits specified in Shift.
    /// The most-significant bits will be filled with the sign bit from Base.
    Id OpShiftRightArithmetic(Id result_type, Id base, Id shift);

    /// Shift the bits in Base left by the number of bits specified in Shift.
    /// The least-significant bits will be zero filled.
    Id OpShiftLeftLogical(Id result_type, Id base, Id shift);

    /// Does a bitwise Or between operands 1 and 2.
    Id OpBitwiseOr(Id result_type, Id operand_1, Id operand_2);

    /// Does a bitwise Xor between operands 1 and 2.
    Id OpBitwiseXor(Id result_type, Id operand_1, Id operand_2);

    /// Result is 1 if both Operand 1 and Operand 2 are 1. Result is 0 if either
    /// Operand 1 or Operand 2 are 0.
    Id OpBitwiseAnd(Id result_type, Id operand_1, Id operand_2);

    /// Does a bitwise Not on the operand.
    Id OpNot(Id result_type, Id operand);

    Id OpBitFieldInsert(Id result_type, Id base, Id insert, Id offset,
                        Id count);

    Id OpBitFieldSExtract(Id result_type, Id base, Id offset, Id count);

    Id OpBitFieldUExtract(Id result_type, Id base, Id offset, Id count);

    Id OpBitReverse(Id result_type, Id base);

    Id OpBitCount(Id result_type, Id base);

    // Arithmetic

    /// Floating-point subtract of Operand from zero.
    Id OpSNegate(Id result_type, Id operand);

    /// Floating-point subtract of Operand from zero.
    Id OpFNegate(Id result_type, Id operand);

    /// Integer addition of Operand 1 and Operand 2.
    Id OpIAdd(Id result_type, Id operand_1, Id operand_2);

    /// Floating-point addition of Operand 1 and Operand 2.
    Id OpFAdd(Id result_type, Id operand_1, Id operand_2);

    /// Integer substraction of Operand 1 and Operand 2.
    Id OpISub(Id result_type, Id operand_1, Id operand_2);

    /// Floating-point substraction of Operand 1 and Operand 2.
    Id OpFSub(Id result_type, Id operand_1, Id operand_2);

    /// Integer multiplication of Operand 1 and Operand 2.
    Id OpIMul(Id result_type, Id operand_1, Id operand_2);

    /// Floating-point multiplication of Operand 1 and Operand 2.
    Id OpFMul(Id result_type, Id operand_1, Id operand_2);

    /// Unsigned-integer division of Operand 1 divided by Operand 2.
    Id OpUDiv(Id result_type, Id operand_1, Id operand_2);

    /// signed-integer division of Operand 1 divided by Operand 2.
    Id OpSDiv(Id result_type, Id operand_1, Id operand_2);

    /// Floating-point division of Operand 1 divided by Operand 2.
    Id OpFDiv(Id result_type, Id operand_1, Id operand_2);

    /// Unsigned modulo operation of Operand 1 modulo Operand 2.
    Id OpUMod(Id result_type, Id operand_1, Id operand_2);

    /// Signed modulo operation of Operand 1 modulo Operand 2.
    Id OpSMod(Id result_type, Id operand_1, Id operand_2);

    /// Floating-point modulo operation of Operand 1 modulo Operand 2.
    Id OpFMod(Id result_type, Id operand_1, Id operand_2);

    /// Signed reminder operation of Operand 1 modulo Operand 2.
    Id OpSRem(Id result_type, Id operand_1, Id operand_2);

    /// Floating-point reminder operation of Operand 1 modulo Operand 2.
    Id OpFRem(Id result_type, Id operand_1, Id operand_2);

    // Extensions

    /// Execute an instruction in an imported set of extended instructions.
    Id OpExtInst(Id result_type, Id set, std::uint32_t instruction,
                 const std::vector<Id>& operands);

    /// Result is x if x >= 0; otherwise result is -x.
    Id OpFAbs(Id result_type, Id x);

    Id OpSAbs(Id result_type, Id x);

    Id OpRound(Id result_type, Id x);

    Id OpRoundEven(Id result_type, Id x);

    Id OpTrunc(Id result_type, Id x);

    Id OpFSign(Id result_type, Id x);

    Id OpSSign(Id result_type, Id x);

    Id OpFloor(Id result_type, Id x);

    Id OpCeil(Id result_type, Id x);

    Id OpFract(Id result_type, Id x);

    Id OpSin(Id result_type, Id x);

    Id OpCos(Id result_type, Id x);

    Id OpAsin(Id result_type, Id x);

    Id OpAcos(Id result_type, Id x);

    Id OpPow(Id result_type, Id x, Id y);

    Id OpExp(Id result_type, Id x);

    Id OpLog(Id result_type, Id x);

    Id OpExp2(Id result_type, Id x);

    Id OpLog2(Id result_type, Id x);

    Id OpSqrt(Id result_type, Id x);

    Id OpInverseSqrt(Id result_type, Id x);

    Id OpFMin(Id result_type, Id x, Id y);

    Id OpUMin(Id result_type, Id x, Id y);

    Id OpSMin(Id result_type, Id x, Id y);

    Id OpFMax(Id result_type, Id x, Id y);

    Id OpUMax(Id result_type, Id x, Id y);

    Id OpSMax(Id result_type, Id x, Id y);

    Id OpFClamp(Id result_type, Id x, Id minVal, Id maxVal);

    Id OpUClamp(Id result_type, Id x, Id minVal, Id maxVal);

    Id OpSClamp(Id result_type, Id x, Id minVal, Id maxVal);

    Id OpFma(Id result_type, Id a, Id b, Id c);

    Id OpPackHalf2x16(Id result_type, Id v);

    Id OpUnpackHalf2x16(Id result_type, Id v);

    Id OpFindILsb(Id result_type, Id Value);

    Id OpFindSMsb(Id result_type, Id Value);

    Id OpFindUMsb(Id result_type, Id Value);

    Id OpInterpolateAtCentroid(Id result_type, Id interpolant);

    Id OpInterpolateAtSample(Id result_type, Id interpolant, Id sample);

    Id OpInterpolateAtOffset(Id result_type, Id interpolant, Id offset);

  private:
    Id AddCode(std::unique_ptr<Op> op);

    Id AddCode(spv::Op opcode, std::optional<std::uint32_t> id = {});

    Id AddDeclaration(std::unique_ptr<Op> op);

    void AddAnnotation(std::unique_ptr<Op> op);

    Id GetGLSLstd450();

    const std::uint32_t version;

    std::uint32_t bound{1};

    std::set<spv::Capability> capabilities;
    std::unique_ptr<Op> glsl_std_450;
    std::set<std::unique_ptr<Op>> ext_inst_import;

    spv::AddressingModel addressing_model{spv::AddressingModel::Logical};
    spv::MemoryModel memory_model{spv::MemoryModel::GLSL450};

    std::vector<std::unique_ptr<Op>> entry_points;
    std::vector<std::unique_ptr<Op>> execution_mode;
    std::vector<std::unique_ptr<Op>> debug;
    std::vector<std::unique_ptr<Op>> annotations;
    std::vector<std::unique_ptr<Op>> declarations;

    std::vector<Id> global_variables;

    std::vector<Id> code;

    std::vector<std::unique_ptr<Op>> code_store;
};

} // namespace Sirit
