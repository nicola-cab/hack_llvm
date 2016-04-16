//
//  AST.hpp
//  Kaleidoscope-LLVM
//
//  this header containts all the abstract representation of the language (aka Abstract Sintax Tree or ParseTree)
//
//  Created by Nicola Cabiddu on 07/01/2016.
//  Copyright © 2016 Nicola Cabiddu. All rights reserved.
//

#ifndef ParseTree_h
#define ParseTree_h

#include <string>
#include <vector>
#include <memory>

#include "CodeGenerator.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"


namespace AST {
 
   ///
   /// @brief: base class to for all expression nodes
   ///
   class ExprAST
   {
      
   public:
      explicit ExprAST();
      virtual ~ExprAST() = default;
      virtual llvm::Value* codeGen() const = 0;
      
   protected:
      using CodeGen = std::unique_ptr<code_generator::CodeGenerator>;
      static CodeGen codeGen_;
   };
   
   ///
   /// @brief: class to represent numeric literal expressions (aka values)
   ///
   class NumberExprAST : public ExprAST
   {
      
   public:
      explicit NumberExprAST(double val);
      double getVal() const;
      llvm::Value* codeGen() const override;
      
   private:
      double val_;
   };
   
   ///
   /// @brief: class to represent variable expressions
   ///
   class VariableExprAST : public ExprAST
   {
      
   public:
      explicit VariableExprAST(const std::string& name );
      const std::string& getName() const;
      llvm::Value* codeGen() const override;
      
   private:
      std::string name_;
   };
   
   ///
   /// @brief: class to represent a function calls expression
   ///
   class CallExprAST : public ExprAST
   {
      using Args = std::vector<std::unique_ptr<ExprAST>>;
     
   public:
      explicit CallExprAST(const std::string& callee, Args args);
      const Args& getArgumentList() const;
      const std::string getCallee() const;
      llvm::Value* codeGen() const override;
      
   private:
      std::string callee_;
      Args args_;
   };
   
   ///
   ///@brief: class to represent a prototype of a function
   ///
   class PrototypeAST : public ExprAST
   {
      using Args = std::vector<std::string>;
      
   public:
      explicit PrototypeAST(std::string name,
                            Args args,
                            bool is_operator = false,
                            unsigned precedence = 0);
      
      const Args& getArgumentList() const;
      const std::string& getName() const;
      bool isUnary() const;
      bool isBinary() const;
      unsigned getBinaryPrecedence() const;
      char getOperatorName() const;
      
      llvm::Function* codeGen() const override;

   private:
      std::string name_;
      Args args_;
      bool is_operator_;
      unsigned precedence_;
   };
   
   ///
   ///@brief: representation function definition itself
   ///
   class FunctionAST : public ExprAST
   {
      using prototype_t = std::unique_ptr<PrototypeAST>;
      using body_t = std::unique_ptr<ExprAST>;
     
   public:
      explicit FunctionAST(prototype_t prototype, body_t body);
      const prototype_t& getPrototype() const;
      const body_t& getBody() const;
      llvm::Function* codeGen() const override;
      //void eval(llvm::Function* f); //add jit compilation for functions
      
   private:
      prototype_t prototype_;
      body_t body_;
   };
   
   ///
   /// @brief: representation control block if/then/else
   ///
   class IfExprAST : public ExprAST
   {
      using condion_t = std::unique_ptr<ExprAST>;
      using then_branch_t = std::unique_ptr<ExprAST>;
      using else_branch_t = std::unique_ptr<ExprAST>;
      
   public:
      explicit IfExprAST(condion_t c, then_branch_t t, else_branch_t e);
      const condion_t& getCondion() const;
      const then_branch_t& getThenBranch() const;
      const else_branch_t& getElseBranch() const;
      llvm::Value* codeGen() const override;
      
   private:
      std::unique_ptr<ExprAST> cond_, then_, else_;
      
   };
   
   ///
   /// @brief: representation for loop
   ///
   class ForExprAST : public ExprAST
   {
      using expression_t = std::unique_ptr<ExprAST>;
      
   public:
      explicit ForExprAST(std::string key,
                          expression_t start,
                          expression_t end,
                          expression_t step,
                          expression_t body);
      
      const std::string&  getKey() const;
      const expression_t& getStart() const;
      const expression_t& getEnd()   const;
      const expression_t& getStep()  const;
      const expression_t& getBody()  const;
      
      llvm::Value* codeGen() const override;
      
   private:
      std::string key_;
      expression_t start_, end_, step_, body_;
   };
   
   ///
   /// @brief: unary operator
   ///
   class UnaryExprAST : public ExprAST
   {
      using opcode_t = char;
      using operand_t = std::unique_ptr<ExprAST>;
      
   public:
      
      explicit UnaryExprAST(opcode_t opcode, operand_t operand);
      opcode_t getOpcode() const;
      const operand_t& getOperand() const;
      llvm::Value *codeGen() const override;
      
   private:
      opcode_t opcode_;
      operand_t operand_;
   };
   
   ///
   /// @brief: binary expression represention
   ///
   class BinaryExprAST : public ExprAST
   {
      using opcode_t = unsigned char;
      using operand_t = std::unique_ptr<ExprAST>;
      
   public:
      explicit BinaryExprAST(opcode_t opcode, operand_t lhs, operand_t rhs);
      opcode_t getOpcode() const;
      const operand_t& getLeftOperand() const;
      const operand_t& getRightOperand() const;
      llvm::Value* codeGen() const override;
      
   private:
      opcode_t opcode_;
      operand_t lhs_, rhs_;
      
   };
   
   ///
   /// @brief: var/in representation
   ///
   class VarExprAST : public ExprAST
   {
      using variable_names_t = std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>>;
      using expression_t = std::unique_ptr<ExprAST>;
      
   public:
      VarExprAST(variable_names_t varNames, expression_t body);
      const variable_names_t& getVarNames() const;
      const expression_t& getBody() const;
      llvm::Value *codeGen() const override;
      
   private:
      variable_names_t varNames_;
      expression_t body_;
   };

   
}

#endif /* ParseTree_h */
