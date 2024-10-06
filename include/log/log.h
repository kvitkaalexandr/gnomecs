#ifndef LOG_H
#define LOG_H

#define gLogLevelVerbose 0
#define gLogLevelInfo 1
#define gLogLevelWarning 2
#define gLogLevelError 3

void gLog(const char *format, const char *tag, ...);
void gLogVerbose(const char *format, const char *tag, ...);
void gLogError(const char *format, const char *tag, ...);
void gLogWarning(const char *format, const char *tag, ...);

#endif //LOG_H
