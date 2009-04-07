/*
 * Copyright (c) 1996-2007 Barton P. Miller
 * 
 * We provide the Paradyn Parallel Performance Tools (below
 * described as "Paradyn") on an AS IS basis, and do not warrant its
 * validity or performance.  We reserve the right to update, modify,
 * or discontinue this software at any time.  We shall have no
 * obligation to supply such updates or modifications or any other
 * form of support to you.
 * 
 * By your use of Paradyn, you understand and agree that we (or any
 * other person or entity with proprietary rights in Paradyn) are
 * under no obligation to provide either maintenance services,
 * update services, notices of latent defects, or correction of
 * defects for Paradyn.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/************************************************************************
 * $Id: Symbol.h,v 1.20 2008/11/03 15:19:24 jaw Exp $
 * Symbol.h: symbol table objects.
************************************************************************/

#if !defined(_Symbol_h_)
#define _Symbol_h_


/************************************************************************
 * header files.
************************************************************************/

#include "symutil.h"
#include "Collections.h"
#include "Type.h"

#include "Annotatable.h"
#include "Serialization.h"

#ifndef CASE_RETURN_STR
#define CASE_RETURN_STR(x) case x: return #x
#endif


#if 0
typedef struct {} symbol_file_name_a;
typedef struct {} symbol_version_names_a;
typedef struct {} symbol_variables_a;
typedef struct {} symbol_parameters_a;
#endif

namespace Dyninst{
namespace SymtabAPI{

class Module;
class typeCommon;
class localVarCollection;
class Region;
class Aggregate;
class Function;
class Variable;

/************************************************************************
 * class Symbol
************************************************************************/

class Symbol : public Serializable,
               public AnnotatableSparse 
{
   friend class typeCommon;
   friend class Symtab;
   friend class AObject;
   friend class Object;

   friend std::string parseStabString(Module *, int linenum, char *, int, 
         typeCommon *);

   public:

   enum SymbolType {
      ST_UNKNOWN,
      ST_FUNCTION,
      ST_OBJECT,
      ST_MODULE,
      ST_SECTION,
      ST_NOTYPE
   };

   static const char *symbolType2Str(SymbolType t);

   enum SymbolLinkage {
      SL_UNKNOWN,
      SL_GLOBAL,
      SL_LOCAL,
      SL_WEAK
   };

   static const char *symbolLinkage2Str(SymbolLinkage t);

   enum SymbolTag {
      TAG_UNKNOWN,
      TAG_USER,
      TAG_LIBRARY,
      TAG_INTERNAL
   };

   static const char *symbolTag2Str(SymbolTag t);

   enum SymbolVisibility {
       SV_UNKNOWN,
       SV_DEFAULT,
       SV_INTERNAL,
       SV_HIDDEN,
       SV_PROTECTED
   };
   static const char *symbolVisibility2Str(SymbolVisibility t);

   SYMTAB_EXPORT Symbol () :
       module_(NULL),
       type_(ST_NOTYPE),
       linkage_(SL_UNKNOWN),
       visibility_(SV_UNKNOWN),
       offset_(0),
       region_(NULL),
       size_(0),
       upPtr_(NULL),
       isDynamic_(false),
       isAbsolute_(false),
       aggregate_(NULL),
       mangledName_(Symbol::emptyString),
       prettyName_(Symbol::emptyString),
       typedName_(Symbol::emptyString),
       tag_(TAG_UNKNOWN) ,
       index_(-1) {}

   SYMTAB_EXPORT static Symbol *magicEmitElfSymbol();

   SYMTAB_EXPORT Symbol (const std::string name,
                         SymbolType t,
                         SymbolLinkage l,
                         SymbolVisibility v,
                         Offset o,
                         Module *module = NULL, 
                         Region *r = NULL, 
                         unsigned s = 0,
                         bool d = false,
                         bool a = false,
			 int index=-1):
       module_(module),
       type_(t),
       linkage_(l),
       visibility_(v),
       offset_(o),
       region_(r),
       size_(s),
       upPtr_(NULL),
       isDynamic_(d),
       isAbsolute_(a),
       aggregate_(NULL),
       mangledName_(name),
       prettyName_(name),
       typedName_(name),
       tag_(TAG_UNKNOWN),
       index_(index){

   }

   SYMTAB_EXPORT ~Symbol();

   SYMTAB_EXPORT bool          operator== (const Symbol &) const;

   /***********************************************************
     Name Output Functions
    ***********************************************************/		
   SYMTAB_EXPORT const std::string&      getMangledName () const;
   SYMTAB_EXPORT const std::string&	 getPrettyName() const;
   SYMTAB_EXPORT const std::string&      getTypedName() const;

   SYMTAB_EXPORT Module *getModule() const { return module_; } 
   SYMTAB_EXPORT SymbolType getType () const { return type_; }
   SYMTAB_EXPORT SymbolLinkage getLinkage () const { return linkage_; }
   SYMTAB_EXPORT Offset getOffset() const { return offset_; }
   SYMTAB_EXPORT Offset getPtrOffset() const { return ptr_offset_; }
   SYMTAB_EXPORT unsigned getSize() const { return size_; }
   SYMTAB_EXPORT Region *getRegion() const { return region_; }

   SYMTAB_EXPORT bool isInDynSymtab() const { return isDynamic_; }
   SYMTAB_EXPORT bool isInSymtab() const { return isInDynSymtab(); }
   SYMTAB_EXPORT bool isAbsolute() const { return isAbsolute_; }

   SYMTAB_EXPORT bool              isFunction()            const;
   SYMTAB_EXPORT bool              setFunction(Function * func);
   SYMTAB_EXPORT Function *        getFunction()           const;

   SYMTAB_EXPORT bool              isVariable()            const;
   SYMTAB_EXPORT bool              setVariable(Variable *var);
   SYMTAB_EXPORT Variable *        getVariable()           const;

   SYMTAB_EXPORT SymbolVisibility getVisibility() const { return visibility_; }

   SYMTAB_EXPORT int getIndex() const { return index_; }
   SYMTAB_EXPORT bool setIndex(int index) { index_ = index; return true; }

   //////////////// Modification
   SYMTAB_EXPORT bool setOffset (Offset newOffset);
   SYMTAB_EXPORT bool setPtrOffset (Offset newOffset);
   SYMTAB_EXPORT bool setSize(unsigned ns);
   SYMTAB_EXPORT bool setModule(Module *mod);

   SYMTAB_EXPORT bool setMangledName(std::string &name); 
   SYMTAB_EXPORT bool setPrettyName(std::string &name); 
   SYMTAB_EXPORT bool setTypedName(std::string &name); 

   SYMTAB_EXPORT bool setSymbolType(SymbolType sType);

   SYMTAB_EXPORT SymbolTag            tag ()               const;
   SYMTAB_EXPORT bool  setDynamic(bool d) { isDynamic_ = d; return true;}
   SYMTAB_EXPORT bool  setAbsolute(bool a) { isAbsolute_ = a; return true; }

   SYMTAB_EXPORT bool  setVersionFileName(std::string &fileName);
   SYMTAB_EXPORT bool  setVersions(std::vector<std::string> &vers);
   SYMTAB_EXPORT bool  setVersionNum(unsigned verNum);

   SYMTAB_EXPORT bool  getVersionFileName(std::string &fileName);
   SYMTAB_EXPORT bool  getVersions(std::vector<std::string> *&vers);
   SYMTAB_EXPORT bool  getVersionNum(unsigned &verNum);

   friend
      std::ostream& operator<< (std::ostream &os, const Symbol &s);

   //////////// DEPRECATED
   SYMTAB_EXPORT Region		   *getSec ()      	   const { return getRegion(); }
   SYMTAB_EXPORT Offset            getAddr ()              const { return getOffset(); }
   SYMTAB_EXPORT const std::string&getModuleName ()        const;
   SYMTAB_EXPORT const std::string &getName() const { return getMangledName(); }
   SYMTAB_EXPORT bool setAddr (Offset newAddr) { return setOffset(newAddr); }
   SYMTAB_EXPORT bool	setModuleName(std::string) { return false; }


   public:
   static std::string emptyString;


   private:

   Module*       module_;
   SymbolType    type_;
   SymbolLinkage linkage_;
   SymbolVisibility visibility_;
   Offset        offset_;
   Offset        ptr_offset_;  // Function descriptor offset.  Not available on all platforms.
   Region*       region_;
   unsigned      size_;  // size of this symbol. This is NOT available on all platforms.
   void *upPtr_;

   bool          isDynamic_;
   bool          isAbsolute_;

   Aggregate *   aggregate_; // Pointer to Function or Variable container, if appropriate.

   std::string mangledName_;
   std::string prettyName_;
   std::string typedName_;

   SymbolTag     tag_;
   int 	index_;
   //int           framePtrRegNum_;
   //Type          *retType_;
   // Module Objects are created in Symtab and not in Object.
   // So we need a way to store the name of the module in 
   // which the symbol is present.
   //std::string moduleName_;  
   //std::string fileName_;

#if !defined (USE_ANNOTATIONS)
   std::vector<std::string> verNames_;
#endif

   public:
   SYMTAB_EXPORT void serialize(SerializerBase *, const char *tag = "Symbol");
};

#if 0
inline
Symbol::Symbol(unsigned)
    : //name_("*bad-symbol*"), module_("*bad-module*"),
    module_(NULL), type_(ST_UNKNOWN), linkage_(SL_UNKNOWN), addr_(0), sec_(NULL), size_(0), 
    isInDynsymtab_(false), isInSymtab_(true), isAbsolute_(false), tag_(TAG_UNKNOWN),
    retType_(NULL), moduleName_("")
   //vars_(NULL), params_(NULL) 
{
}
#endif

class LookupInterface 
{
   public:
      SYMTAB_EXPORT LookupInterface();
      SYMTAB_EXPORT virtual bool getAllSymbolsByType(std::vector<Symbol *> &ret,
            Symbol::SymbolType sType) = 0;
      SYMTAB_EXPORT virtual bool findSymbolByType(std::vector<Symbol *> &ret,
            const std::string name,
            Symbol::SymbolType sType,
            bool isMangled = false,
            bool isRegex = false,
            bool checkCase = false) = 0;
      SYMTAB_EXPORT virtual bool findType(Type *&type, std::string name) = 0;
      SYMTAB_EXPORT virtual bool findVariableType(Type *&type, std::string name)= 0;

      SYMTAB_EXPORT virtual ~LookupInterface();
};


}//namespace SymtabAPI
}//namespace Dyninst

#endif /* !defined(_Symbol_h_) */
