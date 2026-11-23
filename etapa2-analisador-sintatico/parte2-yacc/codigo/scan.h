#ifndef _SCAN_H_
#define _SCAN_H_

// lexema maior que isso e cortado
#define MAXTOKENLEN 100

extern char tokenString[MAXTOKENLEN + 1];

TokenType getToken(void);

#endif
