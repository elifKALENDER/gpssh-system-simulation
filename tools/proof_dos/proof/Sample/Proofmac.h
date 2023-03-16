#define PCREATE(class, id) \
        { \
        PUTTIME; \
        fprintf(trace_file, "Create %s %ld\n", class, id); \
        }

#define PDESTROY(id) \
        { \
        PUTTIME; \
        fprintf(trace_file, "Destroy %ld\n", id); \
        }

#define PLACEAT(id, x, y) \
        { \
        PUTTIME; \
        fprintf(trace_file, "Place %ld at %8.2f %8.2f\n", id, x, y); \
        }

#define PLACEON(id, path) \
        { \
        PUTTIME; \
        fprintf(trace_file, "Place %ld on %s\n", id, path); \
        }

#define PUTTIME \
        if (time > last_time_written) \
                { \
                fprintf(trace_file, "Time %8.2f\n", time); \
                last_time_written = time; \
                }

#define PWRITE(msg, contents) \
        { \
        PUTTIME; \
        fprintf(trace_file, "Write %s %s\n"); \
        }

#define SETCOLOR(id, color) \
        { \
        PUTTIME; \
        fprintf(trace_file, "Set %ld color %s\n", id, color); \
        }


