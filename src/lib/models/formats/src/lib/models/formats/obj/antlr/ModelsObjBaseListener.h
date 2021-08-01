
// Generated from ..\..\src\lib\models\formats\obj\antlr\ModelsObj.g4 by ANTLR 4.9.2

#pragma once


#include "antlr4-runtime.h"
#include "ModelsObjListener.h"


/**
 * This class provides an empty implementation of ModelsObjListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  ModelsObjBaseListener : public ModelsObjListener {
public:

  virtual void enterToplevel(ModelsObjParser::ToplevelContext * /*ctx*/) override { }
  virtual void exitToplevel(ModelsObjParser::ToplevelContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

