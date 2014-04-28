//
//                     SPIR Tools
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//

#ifndef __SPIR_ITERATORS_H__
#define __SPIR_ITERATORS_H__

#include <list>
#include <map>

namespace llvm {
class Value;
class Instruction;
class BasicBlock;
class Function;
class Module;
class MDNode;
}

using namespace llvm;

namespace SPIR {

struct ErrorCreator;

//
// Executor interfaces.
//

/// @brief Interface for executor on llvm value.
struct ValueExecutor {
  virtual void execute(const Value*) = 0;
};

/// @brief Interface for executor on llvm instruction.
struct InstructionExecutor {
  virtual void execute(const Instruction*) = 0;
};

/// @brief Interface for executor on llvm function.
struct FunctionExecutor {
  virtual void execute(const Function*) = 0;
};

/// @brief Interface for executor on llvm module.
struct ModuleExecutor {
  virtual void execute(const Module*) = 0;
};

/// @brief Interface for executor on llvm module.
struct MDNodeExecutor {
  virtual void execute(const MDNode*) = 0;
};

typedef std::list<ValueExecutor*> ValueExecutorList;
typedef std::list<InstructionExecutor*> InstructionExecutorList;
typedef std::list<FunctionExecutor*> FunctionExecutorList;
typedef std::list<ModuleExecutor*> ModuleExecutorList;
typedef std::list<MDNodeExecutor*> MDNodeExecutorList;

//
// Iterator classes.
//

struct BasicBlockIterator {
  /// @brief Constructor.
  /// @param IEL list of instruction executors.
  BasicBlockIterator(InstructionExecutorList& IEL) : m_iel(IEL) {
  }

  /// @brief Iterates over the instructions in a basic block
  ///        and execute all executors from the list on each instruction.
  /// @param Basic block to iterate over.
  void execute(const BasicBlock& BB);

private:
  /// @brief List of instruction executors.
  InstructionExecutorList& m_iel;
};

struct FunctionIterator {
  /// @brief Constructor.
  /// @param FEL list of function executors.
  /// @param BBI basic block iterator (optional).
  FunctionIterator(FunctionExecutorList& FEL, BasicBlockIterator *BBI = 0) :
    m_fel(FEL), m_bbi(BBI) {
  }

  /// @brief Iterates over the basic blocks in a function.
  /// @param F function to iterate over.
  void execute(const Function& F);

private:
  /// @brief List of function executors.
  FunctionExecutorList& m_fel;
  /// @brief Basic block iterator.
  BasicBlockIterator *m_bbi;
};

struct ModuleIterator {
  /// @brief Constructor.
  /// @param MEL list of module executors.
  /// @param FI function iterator (optional).
  ModuleIterator(ModuleExecutorList& MEL, FunctionIterator *FI = 0) :
    m_mel(MEL), m_fi(FI) {
  }

  /// @brief Iterates over the functions in a module.
  /// @param M module to iterate over.
  void execute(const Module& M);

private:
  /// @brief List of module executors.
  ModuleExecutorList& m_mel;
  /// @brief Function iterator.
  FunctionIterator *m_fi;
};

/// @brief Iterates over the metadata nodes.
struct MetaDataIterator {
  /// @brief Constructor.
  /// @param NEL list of metadata node executors.
  MetaDataIterator(MDNodeExecutorList& NEL) : m_nel(NEL) {
  }
  /// @brief Iterates over the operands of a metadata node.
  /// @param M module to iterate over.
  void execute(const MDNode& Node);

private:
  /// @brief List of Metadata node executors.
  MDNodeExecutorList& m_nel;
};


//
// Module data holder class.
//

struct DataHolder {
  DataHolder() :
    Is32Bit(true),
    HasDoubleFeature(false), HasImageFeature(false),
    HASFp16Extension(false) {
  }

  /// @brief Sizeof pointer indectaor
  bool Is32Bit;

  // Core Features

  /// @brief indicator for presence of cl_double core feature 
  bool HasDoubleFeature;

  /// @brief indicator for presence of cl_images core feature 
  bool HasImageFeature;

  // KHR Extensions

  /// @brief indicator for presence of cl_khr_fp16 KHR extension
  bool HASFp16Extension;
};

//
// Verify Executor classes.
//

struct VerifyCall : public InstructionExecutor {
  /// @brief Constructor.
  /// @param EH error holder.
  VerifyCall(ErrorCreator *EH) : ErrCreator(EH) {
  }

  /// @brief Verify that given instruction is not invalid call instruction.
  /// @param I instruction to verify.
  void execute(const Instruction *I);

private:
  ErrorCreator *ErrCreator;
};

struct VerifyBitcast : public InstructionExecutor {
  /// @brief Constructor.
  /// @param EH error holder.
  VerifyBitcast(ErrorCreator *EH) : ErrCreator(EH) {
  }

  /// @brief Verify that given instruction is not invalid bitcast instruction
  ///        and that it has no invalid bitcast constant expression operands.
  /// @param I instruction to verify.
  void execute(const Instruction *I);

private:
  ErrorCreator *ErrCreator;
};

struct VerifyInstructionType : public InstructionExecutor {
  /// @brief Constructor.
  /// @param EH error holder.
  /// @param D data holder.
  VerifyInstructionType(ErrorCreator *EH, DataHolder *D) :
    ErrCreator(EH), Data(D) {
  }

  /// @brief Verify that given instruction has a valid type.
  /// @param I instruction to verify.
  void execute(const Instruction *I);

private:
  ErrorCreator *ErrCreator;
  DataHolder *Data;
};

struct VerifyFunctionPrototype : public FunctionExecutor {
  /// @brief Constructor.
  /// @param EH error holder.
  /// @param D data holder.
  VerifyFunctionPrototype(ErrorCreator *EH, DataHolder *D) :
    ErrCreator(EH), Data(D) {
  }

  /// @brief Verify that given function has valid prototype.
  /// @param F function to verify.
  void execute(const Function *F);

private:
  ErrorCreator *ErrCreator;
  DataHolder *Data;
};


struct VerifyTripleAndDataLayout : public ModuleExecutor {
  /// @brief Constructor.
  /// @param EH error holder.
  VerifyTripleAndDataLayout(ErrorCreator *EH, DataHolder *D) :
    ErrCreator(EH), Data(D) {
  }

  /// @brief Verify that given module has valid triple.
  /// @param M module to verify.
  void execute(const Module *M);

private:
  ErrorCreator *ErrCreator;
  DataHolder *Data;
};

struct VerifyMetadataArgAddrSpace : public MDNodeExecutor {
  /// @brief Constructor.
  /// @param EH error holder.
  /// @param F the function metadata arg base type is describing.
  VerifyMetadataArgAddrSpace(ErrorCreator *EH, Function *F) :
    ErrCreator(EH), Func(F), WasFound(false) {
  }

  /// @brief Verify that given metadata node is valid arg type metadata.
  /// @param Node metadata node to verify.
  void execute(const MDNode *Node);

  bool found() {
    return WasFound;
  }

private:
  ErrorCreator *ErrCreator;
  Function *Func;
  bool WasFound;
};

struct VerifyMetadataArgType : public MDNodeExecutor {
  /// @brief Constructor.
  /// @param EH error holder.
  VerifyMetadataArgType(ErrorCreator *EH) : ErrCreator(EH), WasFound(false) {
  }

  /// @brief Verify that given metadata node is valid arg type metadata.
  /// @param Node metadata node to verify.
  void execute(const MDNode *Node);

  bool found() {
    return WasFound;
  }

private:
  ErrorCreator *ErrCreator;
  bool WasFound;
};

struct VerifyMetadataArgBaseType : public MDNodeExecutor {
  /// @brief Constructor.
  /// @param EH error holder.
  /// @param F the function metadata arg base type is describing.
  VerifyMetadataArgBaseType(ErrorCreator *EH, Function *F, DataHolder *D) :
    ErrCreator(EH), Func(F), Data(D), WasFound(false) {
  }

  /// @brief Verify that given metadata node is valid arg base type metadata.
  /// @param Node metadata node to verify.
  void execute(const MDNode *Node);

  bool found() {
    return WasFound;
  }

private:
  ErrorCreator *ErrCreator;
  Function *Func;
  DataHolder *Data;
  bool WasFound;
};

typedef std::map<const Function*, const MDNode*> FunctionToMDNodeMap;
struct VerifyMetadataKernel : public MDNodeExecutor {
  /// @brief Constructor.
  /// @param EH error holder.
  VerifyMetadataKernel(ErrorCreator *EH,
    DataHolder *D, FunctionToMDNodeMap& Map) :
    ErrCreator(EH), Data(D), FoundMap(Map) {
  }

  /// @brief Verify that given metadata node is valid arg type metadata.
  /// @param Node metadata node to verify.
  void execute(const MDNode *Node);

private:
  ErrorCreator *ErrCreator;
  DataHolder *Data;
  FunctionToMDNodeMap& FoundMap;
};

struct VerifyMetadataKernels : public ModuleExecutor {
  /// @brief Constructor.
  /// @param EH error holder.
  /// @param D data holder.
  VerifyMetadataKernels(ErrorCreator *EH, DataHolder *D) :
    ErrCreator(EH), Data(D) {
  }

  void execute(const Module *M);

private:
  ErrorCreator *ErrCreator;
  DataHolder *Data;
};

struct VerifyMetadataVersions : public ModuleExecutor {

  typedef enum {
    VERSION_OCL,
    VERSION_SPIR,

    OPENCL_VERISON_NUM
  } OPENCL_VERSION_TYPE;

  /// @brief Constructor.
  /// @param EH error holder.
  VerifyMetadataVersions(ErrorCreator *EH, OPENCL_VERSION_TYPE VTy) :
    ErrCreator(EH), VType(VTy) {
  }

  void execute(const Module *M);

private:
  ErrorCreator *ErrCreator;
  OPENCL_VERSION_TYPE VType;
};

struct VerifyMetadataCoreFeatures : public ModuleExecutor {
  /// @brief Constructor.
  /// @param EH error holder.
  /// @param D data holder.s
  VerifyMetadataCoreFeatures(ErrorCreator *EH, DataHolder *D) :
    ErrCreator(EH), Data(D) {
  }

  void execute(const Module *M);

private:
  ErrorCreator *ErrCreator;
  DataHolder *Data;
};

struct VerifyMetadataKHRExtensions : public ModuleExecutor {
  /// @brief Constructor.
  /// @param EH error holder.
  /// @param D data holder.
  VerifyMetadataKHRExtensions(ErrorCreator *EH, DataHolder *D) :
    ErrCreator(EH), Data(D) {
  }

  void execute(const Module *M);

private:
  ErrorCreator *ErrCreator;
  DataHolder *Data;
};

struct VerifyMetadataCompilerOptions : public ModuleExecutor {
  /// @brief Constructor.
  /// @param EH error holder.
  /// @param D data holder.
  VerifyMetadataCompilerOptions(ErrorCreator *EH, DataHolder *D) :
    ErrCreator(EH), Data(D) {
  }

  void execute(const Module *M);

private:
  ErrorCreator *ErrCreator;
  DataHolder *Data;
};

} // End SPIR namespace

#endif // __SPIR_ITERATORS_H__
