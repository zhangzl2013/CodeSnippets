#ifndef _COMMON_H_
#define _COMMON_H_

#define BUG_REPORT_EMAIL "zhangzl2013@126.com"
#define VERSION	"v0.1"

#define _WIN32_
#define _OPENSSL_
#define DEBUG
#ifdef DEBUG
#define DBGP(level, fmt, ...) fprintf(stderr, level "|%s| %s() [%d]: " fmt "\n" ,  \
								__FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)
#define DBG(fmt, ...)	    DBGP("[DEBUG] ", fmt, ## __VA_ARGS__)
#define ERR(fmt, ...)	    DBGP("[ERROR] ", fmt, ## __VA_ARGS__)
#define WARN(fmt, ...)      DBGP("[WARN]  ", fmt, ## __VA_ARGS__)
#define INFO(fmt, ...)      DBGP("[INFO]  ", fmt, ## __VA_ARGS__)
#else // !DEBUG
#define DBG(fmt, ...)       do { } while (0)
#define ERROR(fmt, ...)     fprintf(stderr, "[ERROR] " fmt "\n" ,## __VA_ARGS__)
#define WARN(fmt, ...)      fprintf(stderr, "[WARN]  " fmt "\n" ,## __VA_ARGS__)
#define INFO(fmt, ...)      fprintf(stderr, "[INFO]  " fmt "\n" ,## __VA_ARGS__)
#endif //DEBUG

#ifdef _WIN32_
#define INLINE __inline
//#pragma comment(linker, "/subsystem:/"windows/" /entry:/"mainCRTStartup/"")
#else
#define INLINE inline
#endif

#endif //_COMMON_H_
