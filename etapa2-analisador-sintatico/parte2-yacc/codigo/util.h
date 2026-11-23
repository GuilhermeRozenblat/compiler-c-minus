#ifndef _UTIL_H_
#define _UTIL_H_

void printToken(TokenType token, const char *tokenString);
void printSource(void);

char *copyString(const char *s);
TreeNode *newNode(NodeKind kind);
TreeNode *newDeclNode(NodeKind kind, const char *type, const char *name);
TreeNode *newOpNode(const char *op, TreeNode *left, TreeNode *right);
TreeNode *addSibling(TreeNode *list, TreeNode *t);
void printTree(TreeNode *tree);

void printSyntaxError(const char *message);

void printStep(const char *format, ...);
void printShift(void);
void printReceive(TokenType token);
void printReduce(const char *lhs, const char *rhs, int noLookahead);

#endif
