#include <stdio.h>
#include <stdarg.h>
#include "log/log.h"


void gLog(const char *format, const char *tag, ...) {
#ifdef gLogLevelInfo
    printf("INFO ");
    printf("[%s] ", tag);
    va_list args;
    va_start(args, tag);
    vprintf(format, args);
    va_end(args);
    printf("\n");
#endif
}

void gLogVerbose(const char *format, const char *tag, ...) {
#ifdef gLogLevelVerbose
    printf("VERBOSE ");
    printf("[%s] ", tag);
    va_list args;
    va_start(args, tag);
    vprintf(format, args);
    va_end(args);
    printf("\n");
#endif
}

void gLogError(const char *format, const char *tag, ...) {
#ifdef gLogLevelError
    printf("ERROR ");
    printf("[%s] ", tag);
    va_list args;
    va_start(args, tag);
    vprintf(format, args);
    va_end(args);
    printf("\n");
    #endif
}

void gLogWarning(const char *format, const char *tag, ...) {
#ifdef gLogLevelWarning
    printf("WARNING ");
    printf("[%s] ", tag);
    va_list args;
    va_start(args, tag);
    vprintf(format, args);
    va_end(args);
    printf("\n");
#endif
}

