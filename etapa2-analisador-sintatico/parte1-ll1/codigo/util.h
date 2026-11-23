#ifndef _UTIL_H_
#define _UTIL_H_

TreeNode *newNode(int symbol);
const char *productionString(int n);

void printToken(const Token *token);
void printTable(void);
void printStepHeader(void);
void printStep(int pos, const char *format, ...);
void printErrors(void);
void printTree(TreeNode *tree);

#endif
