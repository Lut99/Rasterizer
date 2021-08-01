
// Generated from ..\..\src\lib\models\formats\obj\antlr\ModelsObj.g4 by ANTLR 4.9.2

#pragma once


#include "antlr4-runtime.h"
#include "ModelsObjParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by ModelsObjParser.
 */
class  ModelsObjListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterToplevel(ModelsObjParser::ToplevelContext *ctx) = 0;
  virtual void exitToplevel(ModelsObjParser::ToplevelContext *ctx) = 0;


};

