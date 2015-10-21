//===--------------------------- Mangler.cpp -----------------------------===//
//
//                              SPIR Tools
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===---------------------------------------------------------------------===//

#include "FunctionDescriptor.h"
#include "ManglingUtils.h"
#include "NameMangleAPI.h"
#include "ParameterType.h"
#include <assert.h>
#include <map>
#include <string>
#include <sstream>

namespace SPIR {

class MangleVisitor : public TypeVisitor {
public:
  MangleVisitor(SPIRversion ver, std::stringstream &s)
      : TypeVisitor(ver), m_stream(s), m_nextSeqID(0) {}

  //
  // Visit methods
  //
  MangleError visit(const PrimitiveType *t) {
    m_stream << mangledPrimitiveString(t->getPrimitive());
    return MANGLE_SUCCESS;
  }

  MangleError visit(const PointerType *p) {
    if (mangleSubstitution(reinterpret_cast<uintptr_t>(p)))
      return MANGLE_SUCCESS;
    m_stream << "P";
    for (unsigned int i = ATTR_QUALIFIER_FIRST; i <= ATTR_QUALIFIER_LAST; i++) {
      TypeAttributeEnum qualifier = (TypeAttributeEnum)i;
      if (p->hasQualifier(qualifier)) {
        m_stream << getMangledAttribute(qualifier);
      }
    }
    m_stream << getMangledAttribute((p->getAddressSpace()));
    addSubstitution(reinterpret_cast<uintptr_t>(p));
    return p->getPointee()->accept(this);
  }

  MangleError visit(const VectorType *v) {
    m_stream << "Dv" << v->getLength() << "_";
    return v->getScalarType()->accept(this);
  }

  MangleError visit(const AtomicType *p) {
    m_stream << "U"
             << "7_Atomic";
    return p->getBaseType()->accept(this);
  }

  MangleError visit(const BlockType *p) {
    m_stream << "U"
             << "13block_pointerFv";
    for (unsigned int i = 0; i < p->getNumOfParams(); ++i) {
      MangleError err = p->getParam(i)->accept(this);
      if (err != MANGLE_SUCCESS) {
        return err;
      }
    }
    m_stream << "E";
    return MANGLE_SUCCESS;
  }

  MangleError visit(const UserDefinedType *pTy) {
    if (mangleSubstitution(reinterpret_cast<uintptr_t>(pTy)))
      return MANGLE_SUCCESS;
    std::string name = pTy->toString();
    m_stream << name.size() << name;
    addSubstitution(reinterpret_cast<uintptr_t>(pTy));
    return MANGLE_SUCCESS;
  }

private:
  bool mangleSubstitution(uintptr_t id);
  void addSubstitution(uintptr_t id);

  // Next substitution sequence number.
  unsigned m_nextSeqID;


  // Maps substitutable parameter to its m_substitutions sequence number.
  std::map<uintptr_t, unsigned> m_substitutions;

  // Holds the mangled string representing the prototype of the function.
  std::stringstream &m_stream;
};

void MangleVisitor::addSubstitution(uintptr_t id) {
  assert(!m_substitutions.count(id) && "Substitution already exists!");
  m_substitutions[id] = m_nextSeqID++;
}

bool MangleVisitor::mangleSubstitution(uintptr_t id) {
  std::map<uintptr_t, unsigned>::iterator I = m_substitutions.find(id);
  if (I == m_substitutions.end())
    return false;

  unsigned SeqID = I->second;

  const size_t BufferSize = 10;
  char Buffer[BufferSize];
  char *BufferPtr = Buffer + BufferSize;

  if (SeqID == 0)
    *--BufferPtr = '0';

  while (SeqID) {
    assert(BufferPtr > Buffer && "Buffer overflow!");

    char c = static_cast<char>(SeqID % 36);

    *--BufferPtr = (c < 10 ? '0' + c : 'A' + c - 10);
    SeqID /= 36;
  }

  m_stream << 'S' << std::string(BufferPtr, Buffer + BufferSize - BufferPtr)
           << '_';

  return true;
}

//
// NameMangler
//
NameMangler::NameMangler(SPIRversion version) : m_spir_version(version) {}

MangleError NameMangler::mangle(const FunctionDescriptor &fd,
                                std::string &mangledName) {
  if (fd.isNull()) {
    mangledName.assign(FunctionDescriptor::nullString());
    return MANGLE_NULL_FUNC_DESCRIPTOR;
  }
  std::stringstream ret;
  ret << "_Z" << fd.name.length() << fd.name;
  MangleVisitor visitor(m_spir_version, ret);
  for (unsigned int i = 0; i < fd.parameters.size(); ++i) {
    MangleError err = fd.parameters[i]->accept(&visitor);
    if (err == MANGLE_TYPE_NOT_SUPPORTED) {
      mangledName.assign("Type ");
      mangledName.append(fd.parameters[i]->toString());
      mangledName.append(" is not supported in ");
      std::string ver = getSPIRVersionAsString(m_spir_version);
      mangledName.append(ver);
      return err;
    }
  }
  mangledName.assign(ret.str());
  return MANGLE_SUCCESS;
}

} // End SPIR namespace
