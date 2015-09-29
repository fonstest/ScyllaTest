#define DEBUG_BUILD 1
#define INFO_BUILD  1
#define WARN_BUILD  1
#define ERROR_BUILD  1

#define DEBUG(fmt, ...) \
        do { if (DEBUG_BUILD) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __FUNCTION__, __VA_ARGS__); } while (0)

#define INFO(fmt, ...) \
            do { if (INFO_BUILD) fprintf(stderr,"[INFO] "fmt, __VA_ARGS__); } while (0)


#define WARN(fmt, ...) \
            do { if (WARN_BUILD) fprintf(stderr,"[WARNING] "fmt, __VA_ARGS__); } while (0)

#define ERROR(fmt, ...) \
            do { if (ERROR_BUILD) fprintf(stderr,"[ERROR] "fmt, __VA_ARGS__); } while (0)