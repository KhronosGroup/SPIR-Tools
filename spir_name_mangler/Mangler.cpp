//
//                     SPIR Tools
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//

#include "FunctionDescriptor.h"
#include "ParameterType.h"
#include "ManglingUtils.h"
#include <string>
#include <sstream>

namespace SPIR {

class MangleVisitor: public TypeVisitor {
public:

  MangleVisitor(std::stringstream& s): m_stream(s) {
  }

  void operator() (const ParamType* t) {
    t->accept(this);
  }

//
// Visit methods
//
  void visit(const PrimitiveType* t) {
    m_stream << mangledPrimitiveString(t->getPrimitive());
  }

  void visit(const PointerType* p) {
    m_stream << "P";
    for (unsigned int i = ATTR_QUALIFIER_FIRST; i <= ATTR_QUALIFIER_LAST; i++) {
      TypeAttributeEnum qualifier = (TypeAttributeEnum)i;
      if (p->hasQualifier(qualifier)) {
        m_stream << getMangledAttribute(qualifier);
      }
    }
    m_stream << getMangledAttribute((p->getAddressSpace()));
    p->getPointee()->accept(this);
  }

  void visit(const VectorType* v) {
    m_stream << "Dv" << v->getLength() << "_";
    v->getScalarType()->accept(this);
  }


  void visit(const UserDefinedType* pTy) {
    std::string name = pTy->toString();
    m_stream << name.size() << name;
  }

private:

  // Holds the mangled string representing the prototype of the function.
  std::stringstream& m_stream;
};

std::string mangle(const FunctionDescriptor& fd) {
  if (fd.isNull())
    return FunctionDescriptor::nullString();
  std::stringstream ret;
  ret << "_Z" << fd.name.length() << fd.name;
  MangleVisitor visitor(ret);
  for (unsigned int i=0; i < fd.parameters.size(); ++i) {
    fd.parameters[i]->accept(&visitor);
  }
  return ret.str();
}

} // End SPIR namespace