
#define PRIO_ERROR		1
#define PRIO_CRITICAL	2
#define PRIO_HIGH		4
#define PRIO_INFO		8
#define PRIO_VERBOSE	16
#define PRIO_VVERBOSE	32
#define PRIO_VVVERBOSE	64

#define LOG(p, s, ...) LOG_h(p, __FILE__, __LINE__, s, ##__VA_ARGS__);

void LOG_h(int priority, char *file, int line, char *str, ...);
