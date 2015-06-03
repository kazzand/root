// @(#)root/core/utils:$Id: SelectionRules.h 28529 2009-05-11 16:43:35Z pcanal $
// Author: Velislava Spasova September 2010

/*************************************************************************
 * Copyright (C) 1995-2011, Rene Brun, Fons Rademakers and al.           *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef R__SELECTIONRULES_H
#define R__SELECTIONRULES_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// SelectionRules                                                       //
//                                                                      //
// the class representing all selection rules                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <list>
#include "BaseSelectionRule.h"
#include "ClassSelectionRule.h"
#include "VariableSelectionRule.h"
#include "clang/AST/Decl.h"

#include "TMetaUtils.h"

namespace cling {
   class Interpreter;
}

namespace ROOT{
   namespace TMetaUtils {
      class TNormalizedCtxt;
   }
}

class SelectionRules {

public:
   enum ESelectionFileTypes { // type of selection file
      kSelectionXMLFile,
      kLinkdefFile,
      kNumSelectionFileTypes
   };

   SelectionRules(cling::Interpreter &interp,
                  ROOT::TMetaUtils::TNormalizedCtxt& normCtxt,
                  const std::vector<std::pair<std::string,std::string>>& namesForExclusion):
      fSelectionFileType(kNumSelectionFileTypes),
      fIsDeep(false),
      fHasFileNameRule(false),
      fRulesCounter(0),
      fNormCtxt(normCtxt),
      fInterp(interp) {
         long counter=1;
         for (auto& attrValPair : namesForExclusion){
            ClassSelectionRule csr(counter++, fInterp);
            csr.SetAttributeValue(attrValPair.first, attrValPair.second);
            csr.SetSelected(BaseSelectionRule::kNo);
            AddClassSelectionRule(csr);
            }
      }

   void AddClassSelectionRule(const ClassSelectionRule& classSel);
   bool HasClassSelectionRules() const { return !fClassSelectionRules.empty(); }
   const std::list<ClassSelectionRule>& GetClassSelectionRules() const {
      return fClassSelectionRules;
   }

   void AddFunctionSelectionRule(const FunctionSelectionRule& funcSel);
   bool HasFunctionSelectionRules() const {
      return !fFunctionSelectionRules.empty();
   }
   const std::list<FunctionSelectionRule>& GetFunctionSelectionRules() const {
      return fFunctionSelectionRules;
   }

   void AddVariableSelectionRule(const VariableSelectionRule& varSel);

   bool HasVariableSelectionRules() const {
      return !fVariableSelectionRules.empty();
   }
   const std::list<VariableSelectionRule>& GetVariableSelectionRules() const {
      return fVariableSelectionRules;
   }

   void AddEnumSelectionRule(const EnumSelectionRule& enumSel);
   bool HasEnumSelectionRules() const { return !fEnumSelectionRules.empty(); }
   const std::list<EnumSelectionRule>& GetEnumSelectionRules() const {
      return fEnumSelectionRules;
   }

   void PrintSelectionRules() const; // print all selection rules

   void ClearSelectionRules(); // clear all selection rules

   void SetHasFileNameRule(bool file_rule) { fHasFileNameRule = file_rule; }
   bool GetHasFileNameRule() const { return fHasFileNameRule; }

   void SetDeep(bool deep);
   bool GetDeep() const { return fIsDeep; }

   // These method are called from clr-scan and return true if the Decl selected, false otherwise
   //const BaseSelectionRule  *IsDeclSelected(clang::Decl* D) const;
   const ClassSelectionRule *IsDeclSelected(const clang::RecordDecl* D) const;
   const ClassSelectionRule *IsDeclSelected(const clang::TypedefNameDecl* D) const;
   const ClassSelectionRule *IsDeclSelected(const clang::NamespaceDecl* D) const;
   const BaseSelectionRule *IsDeclSelected(const clang::EnumDecl* D) const;
   const BaseSelectionRule *IsDeclSelected(const clang::VarDecl* D) const;
   const BaseSelectionRule *IsDeclSelected(const clang::FieldDecl* D) const;
   const BaseSelectionRule *IsDeclSelected(const clang::FunctionDecl* D) const;
   const BaseSelectionRule *IsDeclSelected(const clang::Decl* D) const;

   const ClassSelectionRule *IsClassSelected(const clang::Decl* D, const std::string& qual_name) const; // is the class selected
   const ClassSelectionRule *IsNamespaceSelected(const clang::Decl* D, const std::string& qual_name) const; // is the class selected

   // is the global function, variable, enum selected - the behavior is different for linkdef.h and selection.xml - that's why
   // we have two functions
   const BaseSelectionRule *IsVarSelected(const clang::VarDecl* D, const std::string& qual_name) const;
   const BaseSelectionRule *IsFunSelected(const clang::FunctionDecl* D, const std::string& qual_name) const;
   const BaseSelectionRule *IsEnumSelected(const clang::EnumDecl* D, const std::string& qual_name) const;
   const BaseSelectionRule *IsLinkdefVarSelected(const clang::VarDecl* D, const std::string& qual_name) const;
   const BaseSelectionRule *IsLinkdefFunSelected(const clang::FunctionDecl* D, const std::string& qual_name) const;
   const BaseSelectionRule *IsLinkdefEnumSelected(const clang::EnumDecl* D, const std::string& qual_name) const;

   // is member (field, method, enum) selected; the behavior for linkdef.h methods is different
   const BaseSelectionRule *IsMemberSelected(const clang::Decl* D, const std::string& str_name) const;
   const BaseSelectionRule *IsLinkdefMethodSelected(const clang::Decl* D, const std::string& qual_name) const;

   // Return the number of rules
    unsigned int Size() const{return fClassSelectionRules.size()+
                                     fFunctionSelectionRules.size()+
                                     fVariableSelectionRules.size()+
                                     fEnumSelectionRules.size();};

   // returns true if the parent is class or struct
   bool IsParentClass(const clang::Decl* D) const;

   // the same but returns also the parent name and qualified name
   bool IsParentClass(const clang::Decl* D, std::string& parent_name, std::string& parent_qual_name) const;

   // returns the parent name and qualified name
   bool GetParentName(const clang::Decl* D, std::string& parent_name, std::string& parent_qual_name) const;


   //bool getParent(clang::Decl* D, clang::Decl* parent); - this method would have saved a lot of efforts but it crashes
   // and I didn't understand why

   // gets the name and qualified name of the Decl
   bool GetDeclName(const clang::Decl* D, std::string& name, std::string& qual_name) const;

   // gets the qualname of the decl, no checks performed
   inline void GetDeclQualName(const clang::Decl* D, std::string& qual_name) const;

   // gets the function prototype if the Decl (if it is global function or method)
   bool GetFunctionPrototype(const clang::FunctionDecl* F, std::string& prototype) const;

   bool IsSelectionXMLFile() const {
      return fSelectionFileType == kSelectionXMLFile;
   }
   bool IsLinkdefFile() const {
      return fSelectionFileType == kLinkdefFile;
   }
   void SetSelectionFileType(ESelectionFileTypes fileType) {
      fSelectionFileType = fileType;
   }

   // returns true if all selection rules are used at least once
   bool AreAllSelectionRulesUsed() const;

   // Go through all the selections rules and lookup the name if any in the AST.
   // and force the instantiation of template if any are used in the rules.
   bool SearchNames(cling::Interpreter &interp);

   void FillCache(); // Fill the cache of all selection rules

private:
   std::list<ClassSelectionRule>    fClassSelectionRules;    // list of the class selection rules
   std::list<FunctionSelectionRule> fFunctionSelectionRules; // list of the global functions selection rules
   std::list<VariableSelectionRule> fVariableSelectionRules; // list of the global variables selection rules
   std::list<EnumSelectionRule>     fEnumSelectionRules;     // list of the enums selection rules

   ESelectionFileTypes fSelectionFileType;

   bool fIsDeep; // if --deep option passed from command line, this should be set to true
   bool fHasFileNameRule; // if we have a file name rule, this should be set to true
   long int fRulesCounter;

   ROOT::TMetaUtils::TNormalizedCtxt& fNormCtxt;
   cling::Interpreter &fInterp;

};

#endif
