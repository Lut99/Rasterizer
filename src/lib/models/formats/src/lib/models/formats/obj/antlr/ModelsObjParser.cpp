
// Generated from ..\..\src\lib\models\formats\obj\antlr\ModelsObj.g4 by ANTLR 4.9.2


#include "ModelsObjListener.h"

#include "ModelsObjParser.h"


using namespace antlrcpp;
using namespace antlr4;

ModelsObjParser::ModelsObjParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

ModelsObjParser::~ModelsObjParser() {
  delete _interpreter;
}

std::string ModelsObjParser::getGrammarFileName() const {
  return "ModelsObj.g4";
}

const std::vector<std::string>& ModelsObjParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& ModelsObjParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- ToplevelContext ------------------------------------------------------------------

ModelsObjParser::ToplevelContext::ToplevelContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* ModelsObjParser::ToplevelContext::ID() {
  return getToken(ModelsObjParser::ID, 0);
}


size_t ModelsObjParser::ToplevelContext::getRuleIndex() const {
  return ModelsObjParser::RuleToplevel;
}

void ModelsObjParser::ToplevelContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<ModelsObjListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterToplevel(this);
}

void ModelsObjParser::ToplevelContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<ModelsObjListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitToplevel(this);
}

ModelsObjParser::ToplevelContext* ModelsObjParser::toplevel() {
  ToplevelContext *_localctx = _tracker.createInstance<ToplevelContext>(_ctx, getState());
  enterRule(_localctx, 0, ModelsObjParser::RuleToplevel);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(2);
    match(ModelsObjParser::T__0);
    setState(3);
    match(ModelsObjParser::ID);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

// Static vars and initialization.
std::vector<dfa::DFA> ModelsObjParser::_decisionToDFA;
atn::PredictionContextCache ModelsObjParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN ModelsObjParser::_atn;
std::vector<uint16_t> ModelsObjParser::_serializedATN;

std::vector<std::string> ModelsObjParser::_ruleNames = {
  "toplevel"
};

std::vector<std::string> ModelsObjParser::_literalNames = {
  "", "'hello'"
};

std::vector<std::string> ModelsObjParser::_symbolicNames = {
  "", "", "ID", "WS"
};

dfa::Vocabulary ModelsObjParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> ModelsObjParser::_tokenNames;

ModelsObjParser::Initializer::Initializer() {
	for (size_t i = 0; i < _symbolicNames.size(); ++i) {
		std::string name = _vocabulary.getLiteralName(i);
		if (name.empty()) {
			name = _vocabulary.getSymbolicName(i);
		}

		if (name.empty()) {
			_tokenNames.push_back("<INVALID>");
		} else {
      _tokenNames.push_back(name);
    }
	}

  static const uint16_t serializedATNSegment0[] = {
    0x3, 0x608b, 0xa72a, 0x8133, 0xb9ed, 0x417c, 0x3be7, 0x7786, 0x5964, 
       0x3, 0x5, 0x8, 0x4, 0x2, 0x9, 0x2, 0x3, 0x2, 0x3, 0x2, 0x3, 0x2, 
       0x3, 0x2, 0x2, 0x2, 0x3, 0x2, 0x2, 0x2, 0x2, 0x6, 0x2, 0x4, 0x3, 
       0x2, 0x2, 0x2, 0x4, 0x5, 0x7, 0x3, 0x2, 0x2, 0x5, 0x6, 0x7, 0x4, 
       0x2, 0x2, 0x6, 0x3, 0x3, 0x2, 0x2, 0x2, 0x2, 
  };

  _serializedATN.insert(_serializedATN.end(), serializedATNSegment0,
    serializedATNSegment0 + sizeof(serializedATNSegment0) / sizeof(serializedATNSegment0[0]));


  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

ModelsObjParser::Initializer ModelsObjParser::_init;
