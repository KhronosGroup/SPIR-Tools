//
//                     SPIR Tools
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//

#include "ParameterType.h"
#include "ManglingUtils.h"
#include <assert.h>
#include <cctype>
#include <sstream>

namespace SPIR {
  //
  // Primitive Type
  //

  PrimitiveType::PrimitiveType(TypePrimitiveEnum primitive) :
   ParamType(TYPE_ID_PRIMITIVE), m_primitive(primitive) {
  }

  void PrimitiveType::accept(TypeVisitor* visitor) const {
    visitor->visit(this);
  }

  std::string PrimitiveType::toString() const {
    assert( (m_primitive >= PRIMITIVE_FIRST
      && m_primitive <= PRIMITIVE_LAST) && "illegal primitive");
    std::stringstream myName;
    myName << readablePrimitiveString(m_primitive);
    return myName.str();
  }

  bool PrimitiveType::equals(const ParamType* type) const {
    const PrimitiveType* p = SPIR::dyn_cast<PrimitiveType>(type);
    return p && (m_primitive == p->m_primitive);
  }


  //
  // Pointer Type
  //

  PointerType::PointerType(const RefParamType type) :
    ParamType(TYPE_ID_POINTER), m_pType(type) {
    for (unsigned int i = ATTR_QUALIFIER_FIRST; i <= ATTR_QUALIFIER_LAST; i++) {
      setQualifier((TypeAttributeEnum)i, false);
    }
    m_address_space = ATTR_PRIVATE;
  }

  void PointerType::accept(TypeVisitor* visitor) const {
    visitor->visit(this);
  }

  void PointerType::setAddressSpace(TypeAttributeEnum attr) {
    if (attr < ATTR_ADDR_SPACE_FIRST || attr > ATTR_ADDR_SPACE_LAST) {
      return;
    }
    m_address_space = attr;
  }

  const TypeAttributeEnum PointerType::getAddressSpace() const {
    return m_address_space;
  }

  void PointerType::setQualifier(TypeAttributeEnum qual, bool enabled) {
    if (qual < ATTR_QUALIFIER_FIRST || qual > ATTR_QUALIFIER_LAST) {
      return;
    }
    m_qualifiers[qual - ATTR_QUALIFIER_FIRST] = enabled;
  }

  bool PointerType::hasQualifier(TypeAttributeEnum qual) const {
    if (qual < ATTR_QUALIFIER_FIRST || qual > ATTR_QUALIFIER_LAST) {
      return false;
    }
    return m_qualifiers[qual - ATTR_QUALIFIER_FIRST];
  }

  std::string PointerType::toString() const {
    std::stringstream myName;
    for (unsigned int i = ATTR_QUALIFIER_FIRST; i <= ATTR_QUALIFIER_LAST; i++) {
      TypeAttributeEnum qual = (TypeAttributeEnum)i;
      if (hasQualifier(qual)) {
        myName << getReadableAttribute(qual) << " ";
      }
    }
    myName << getReadableAttribute(TypeAttributeEnum(m_address_space)) << " ";
    myName << getPointee()->toString() << " *";
    return myName.str();
  }

  bool PointerType::equals(const ParamType* type) const {
    const PointerType* p = SPIR::dyn_cast<PointerType>(type);
    if (!p) {
      return false;
    }
    if (getAddressSpace() != p->getAddressSpace()) {
      return false;
    }
    for (unsigned int i = ATTR_QUALIFIER_FIRST; i <= ATTR_QUALIFIER_LAST; i++) {
      TypeAttributeEnum qual = (TypeAttributeEnum)i;
      if (hasQualifier(qual) != p->hasQualifier(qual)) {
        return false;
      }
    }
    return (*getPointee()).equals(&*(p->getPointee()));
  }

  //
  // Vector Type
  //

  VectorType::VectorType(const RefParamType type, int len) :
    ParamType(TYPE_ID_VECTOR), m_pType(type), m_len(len) {
  }

  void VectorType::accept(TypeVisitor* visitor) const {
    visitor->visit(this);
  }

  std::string VectorType::toString() const {
    std::stringstream myName;
    myName << getScalarType()->toString();
    myName << m_len;
    return myName.str();
  }

  bool VectorType::equals(const ParamType* type) const {
    const VectorType* pVec = SPIR::dyn_cast<VectorType>(type);
    return pVec && (m_len == pVec->m_len) &&
      (*getScalarType()).equals(&*(pVec->getScalarType()));
  }

  //
  // User Defined Type
  //
  UserDefinedType::UserDefinedType(const std::string& name):
    ParamType(TYPE_ID_STRUCTURE), m_name(name) {
  }

  void UserDefinedType::accept(TypeVisitor* visitor) const {
    visitor->visit(this);
  }

  std::string UserDefinedType::toString() const {
    std::stringstream myName;
    myName << m_name;
    return myName.str();
  }

  bool UserDefinedType::equals(const ParamType* pType) const {
    const UserDefinedType* pTy = SPIR::dyn_cast<UserDefinedType>(pType);
    return pTy && (m_name == pTy->m_name);
  }


  //
  // Static enums
  //
  const TypeEnum PrimitiveType::enumTy    = TYPE_ID_PRIMITIVE;
  const TypeEnum PointerType::enumTy      = TYPE_ID_POINTER;
  const TypeEnum VectorType::enumTy       = TYPE_ID_VECTOR;
  const TypeEnum UserDefinedType::enumTy  = TYPE_ID_STRUCTURE;

} // End SPIR namespace
